/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gst_surface_src.h"
#include <gst/video/video.h>
#include <sync_fence.h>
#include "gst_consumer_surface_pool.h"
#include "gst_consumer_surface_allocator.h"
#include "media_errors.h"
#include "surface_buffer.h"
#include "buffer_type_meta.h"
#include "scope_guard.h"
#include "display_type.h"
#include "param_wrapper.h"
#include "media_log.h"

#define gst_surface_src_parent_class parent_class
using namespace OHOS;
namespace {
    constexpr guint DEFAULT_SURFACE_QUEUE_SIZE = 4;
    constexpr int32_t DEFAULT_SURFACE_SIZE = 1024 * 1024;
    constexpr int32_t DEFAULT_VIDEO_WIDTH = 1920;
    constexpr int32_t DEFAULT_VIDEO_HEIGHT = 1080;
    constexpr uint32_t STRIDE_ALIGN = 8;
}

GST_DEBUG_CATEGORY_STATIC(gst_surface_src_debug_category);
#define GST_CAT_DEFAULT gst_surface_src_debug_category

static GstStaticPadTemplate gst_src_template =
GST_STATIC_PAD_TEMPLATE("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS(GST_VIDEO_CAPS_MAKE(GST_VIDEO_FORMATS_ALL)));

enum {
    PROP_0,
    PROP_SURFACE,
    PROP_SURFACE_STRIDE,
    PROP_SUSPEND,
    PROP_REPEAT,
    PROP_MAX_FRAME_RATE,
    PROP_NOTIFY_EOS,
};

G_DEFINE_TYPE(GstSurfaceSrc, gst_surface_src, GST_TYPE_MEM_SRC);

static void gst_surface_src_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void gst_surface_src_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static GstStateChangeReturn gst_surface_src_change_state(GstElement *element, GstStateChange transition);
static gboolean gst_surface_src_create_surface(GstSurfaceSrc *surfacesrc);
static GstBufferPool *gst_surface_src_create_pool(GstMemSrc *memsrc);
static gboolean gst_surface_src_init_pool(GstSurfaceSrc *surfacesrc);
static void gst_surface_src_destroy_surface(GstSurfaceSrc *src);
static void gst_surface_src_destroy_pool(GstSurfaceSrc *src);
static gboolean gst_surface_src_decide_allocation(GstBaseSrc *basesrc, GstQuery *query);
static GstFlowReturn gst_surface_src_fill(GstBaseSrc *src, guint64 offset, guint size, GstBuffer *buf);
static void gst_surface_src_init_surface(GstSurfaceSrc *src);
static gboolean gst_surface_src_send_event(GstElement *element, GstEvent *event);
static void gst_surface_mem_src_dump_buffer(GstBaseSrc *self, GstBuffer *buffer);
static void gst_surface_mem_src_dump_from_sys_param(GstSurfaceSrc *src);

static void gst_surface_src_class_init(GstSurfaceSrcClass *klass)
{
    g_return_if_fail(klass != nullptr);
    GObjectClass *gobject_class = reinterpret_cast<GObjectClass*>(klass);
    GstElementClass *gstelement_class = reinterpret_cast<GstElementClass*>(klass);
    GstBaseSrcClass *gstbasesrc_class = reinterpret_cast<GstBaseSrcClass*>(klass);
    GstMemSrcClass *gstmemsrc_class = reinterpret_cast<GstMemSrcClass*>(klass);
    GST_DEBUG_CATEGORY_INIT(gst_surface_src_debug_category, "surfacepoolsrc", 0, "surface pool src base class");
    gobject_class->get_property = gst_surface_src_get_property;
    gobject_class->set_property = gst_surface_src_set_property;

    g_object_class_install_property(gobject_class, PROP_SURFACE,
        g_param_spec_pointer("surface", "Surface", "Surface for buffer",
            (GParamFlags)(G_PARAM_READABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_SURFACE_STRIDE,
        g_param_spec_uint("surface-stride", "surface stride",
            "surface buffer stride", 0, G_MAXINT32, STRIDE_ALIGN,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_SUSPEND,
        g_param_spec_boolean("suspend", "Suspend surface", "Suspend surface",
            FALSE, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_REPEAT,
        g_param_spec_uint("repeat", "Repeat frame", "Repeat previous frame after given milliseconds",
            0, G_MAXUINT32, 0, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_MAX_FRAME_RATE,
        g_param_spec_uint("max-framerate", "Max frame rate", "Max frame rate",
            0, G_MAXUINT32, 0, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_NOTIFY_EOS,
        g_param_spec_boolean("notify-eos", "notify eos", "Need notify eos",
            FALSE, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    gstelement_class->change_state = gst_surface_src_change_state;
    gstelement_class->send_event = gst_surface_src_send_event;
    gstbasesrc_class->fill = gst_surface_src_fill;
    gstbasesrc_class->decide_allocation = gst_surface_src_decide_allocation;
    gstmemsrc_class->create_pool = gst_surface_src_create_pool;
    gst_element_class_set_static_metadata(gstelement_class,
        "surface mem source", "Source/Surface/Pool",
        "Retrieve frame from surface buffer queue with raw data", "OpenHarmony");

    gst_element_class_add_static_pad_template(gstelement_class, &gst_src_template);
}

static void gst_surface_src_init(GstSurfaceSrc *surfacesrc)
{
    GST_DEBUG_OBJECT(surfacesrc, "Init, id = %d", static_cast<int32_t>(FAKE_POINTER(surfacesrc)));
    g_return_if_fail(surfacesrc != nullptr);
    surfacesrc->pool = nullptr;
    surfacesrc->stride = STRIDE_ALIGN;
    surfacesrc->need_flush = FALSE;
    surfacesrc->flushing = FALSE;
    gst_surface_mem_src_dump_from_sys_param(surfacesrc);
}

static void gst_surface_src_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    GstSurfaceSrc *src = GST_SURFACE_SRC(object);
    g_return_if_fail(src != nullptr);
    g_return_if_fail(value != nullptr);
    (void)pspec;
    switch (prop_id) {
        case PROP_SURFACE:
            g_value_set_pointer(value, src->producerSurface.GetRefPtr());
            break;
        case PROP_SURFACE_STRIDE:
            g_value_set_uint(value, src->stride);
            break;
        default:
            break;
    }
}

static void gst_surface_src_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    GstSurfaceSrc *src = GST_SURFACE_SRC(object);
    g_return_if_fail(src != nullptr && value != nullptr);
    (void)pspec;
    switch (prop_id) {
        case PROP_SURFACE_STRIDE:
            src->stride = g_value_get_uint(value);
            if (src->stride > INT32_MAX) {
                src->stride = STRIDE_ALIGN;
            }
            break;
        case PROP_SUSPEND:
            g_return_if_fail(src->pool != nullptr);
            g_object_set(src->pool, "suspend", g_value_get_boolean(value), nullptr);
            break;
        case PROP_REPEAT:
            g_return_if_fail(src->pool != nullptr);
            g_object_set(src->pool, "repeat", g_value_get_uint(value), nullptr);
            break;
        case PROP_MAX_FRAME_RATE:
            g_return_if_fail(src->pool != nullptr);
            g_object_set(src->pool, "max-framerate", g_value_get_uint(value), nullptr);
            break;
        case PROP_NOTIFY_EOS:
            g_return_if_fail(src->pool != nullptr);
            g_object_set(src->pool, "notify-eos", g_value_get_boolean(value), nullptr);
            break;
        default:
            break;
    }
}

static GstFlowReturn gst_surface_src_fill(GstBaseSrc *src, guint64 offset, guint size, GstBuffer *buf)
{
    (void)src;
    (void)offset;
    (void)size;
    gst_surface_mem_src_dump_buffer(src, buf);

    GstBufferTypeMeta *meta = gst_buffer_get_buffer_type_meta(buf);
    if (meta != nullptr && (meta->bufferFlag & BUFFER_FLAG_EOS)) {
        GST_DEBUG_OBJECT(src, "EOS buffer");
        return GST_FLOW_EOS;
    }
    return GST_FLOW_OK;
}

static GstStateChangeReturn gst_surface_src_change_state(GstElement *element, GstStateChange transition)
{
    GstSurfaceSrc *surfacesrc = GST_SURFACE_SRC(element);
    g_return_val_if_fail(surfacesrc != nullptr, GST_STATE_CHANGE_FAILURE);
    switch (transition) {
        case GST_STATE_CHANGE_NULL_TO_READY:
            g_return_val_if_fail(gst_surface_src_create_surface(surfacesrc) == TRUE, GST_STATE_CHANGE_FAILURE);
            g_return_val_if_fail(gst_surface_src_init_pool(surfacesrc) == TRUE, GST_STATE_CHANGE_FAILURE);
            break;
        case GST_STATE_CHANGE_READY_TO_PAUSED:
            gst_surface_src_init_surface(surfacesrc);
            break;
        default:
            break;
    }
    GstStateChangeReturn ret = GST_ELEMENT_CLASS(parent_class)->change_state(element, transition);

    switch (transition) {
        case GST_STATE_CHANGE_READY_TO_NULL:
            gst_surface_src_destroy_pool(surfacesrc);
            gst_surface_src_destroy_surface(surfacesrc);
            if (surfacesrc->dump.enable_dump == TRUE) {
                if (surfacesrc->dump.dump_file != nullptr) {
                    fclose(surfacesrc->dump.dump_file);
                    surfacesrc->dump.dump_file = nullptr;
                }
            }
            GST_OBJECT_LOCK(surfacesrc);
            surfacesrc->need_flush = FALSE;
            GST_OBJECT_UNLOCK(surfacesrc);
            break;
        default:
            break;
    }

    return ret;
}

static gboolean gst_surface_src_create_surface(GstSurfaceSrc *surfacesrc)
{
    g_return_val_if_fail(surfacesrc != nullptr, FALSE);
    sptr<Surface> consumerSurface = Surface::CreateSurfaceAsConsumer();
    g_return_val_if_fail(consumerSurface != nullptr, FALSE);
    sptr<IBufferProducer> producer = consumerSurface->GetProducer();
    g_return_val_if_fail(producer != nullptr, FALSE);
    sptr<Surface> producerSurface = Surface::CreateSurfaceAsProducer(producer);
    g_return_val_if_fail(producerSurface != nullptr, FALSE);
    surfacesrc->consumerSurface = consumerSurface;
    surfacesrc->producerSurface = producerSurface;

    SurfaceError ret = surfacesrc->consumerSurface->SetQueueSize(DEFAULT_SURFACE_QUEUE_SIZE);
    if (ret != SURFACE_ERROR_OK) {
        GST_WARNING_OBJECT(surfacesrc, "set queue size fail");
    }
    GST_DEBUG_OBJECT(surfacesrc, "create surface");
    return TRUE;
}

static gboolean gst_surface_src_send_event(GstElement *element, GstEvent *event)
{
    GstSurfaceSrc *surfacesrc = GST_SURFACE_SRC(element);
    g_return_val_if_fail(surfacesrc != nullptr, FALSE);
    g_return_val_if_fail(event != nullptr, FALSE);
    GST_INFO_OBJECT(surfacesrc, "New event %s", GST_EVENT_TYPE_NAME(event));

    switch (GST_EVENT_TYPE(event)) {
        case GST_EVENT_FLUSH_START:
            if (surfacesrc->need_flush == FALSE) {
                GST_INFO_OBJECT(surfacesrc, "No need flushing");
                surfacesrc->flushing = FALSE;
                return TRUE;
            }
            surfacesrc->flushing = TRUE;
            break;
        case GST_EVENT_FLUSH_STOP:
            if (surfacesrc->flushing == FALSE) {
                GST_INFO_OBJECT(surfacesrc, "No flush start");
                return TRUE;
            }
            surfacesrc->flushing = FALSE;
            break;
        default:
            break;
    }

    return GST_ELEMENT_CLASS(parent_class)->send_event(element, event);
}

static GstBufferPool *gst_surface_src_create_pool(GstMemSrc *memsrc)
{
    (void)memsrc;
    return gst_consumer_surface_pool_new();
}

static gboolean gst_surface_src_init_pool(GstSurfaceSrc *surfacesrc)
{
    GstMemSrcClass *memsrcclass = GST_MEM_SRC_GET_CLASS(surfacesrc);
    g_return_val_if_fail(memsrcclass != nullptr, FALSE);
    GstAllocationParams params;
    GstBufferPool *pool = nullptr;
    gst_allocation_params_init(&params);

    if (memsrcclass->create_pool) {
        GstMemSrc *memsrc = GST_MEM_SRC(surfacesrc);
        pool = memsrcclass->create_pool(memsrc);
    }

    g_return_val_if_fail(pool != nullptr, FALSE);
    ON_SCOPE_EXIT(0) { gst_object_unref(pool); };
    GstAllocator *allocator = gst_consumer_surface_allocator_new();
    g_return_val_if_fail(allocator != nullptr, FALSE);
    ON_SCOPE_EXIT(1) { gst_object_unref(allocator); };
    gst_consumer_surface_pool_set_surface(pool, surfacesrc->consumerSurface);
    gst_consumer_surface_allocator_set_surface(allocator, surfacesrc->consumerSurface);
    // init pool config
    GstStructure *config = gst_buffer_pool_get_config(pool);
    gst_buffer_pool_config_set_allocator(config, allocator, &params);
    g_return_val_if_fail(gst_buffer_pool_set_config(pool, config) != TRUE, FALSE);
    surfacesrc->pool = pool;
    CANCEL_SCOPE_EXIT_GUARD(0);
    GST_DEBUG_OBJECT(surfacesrc, "create surface pool");
    return TRUE;
}

static void gst_surface_src_destroy_pool(GstSurfaceSrc *src)
{
    gst_object_unref(src->pool);
    src->pool = nullptr;
}

static void gst_surface_src_destroy_surface(GstSurfaceSrc *src)
{
    src->consumerSurface = nullptr;
    src->producerSurface = nullptr;
}

static void gst_surface_src_init_surface(GstSurfaceSrc *src)
{
    g_return_if_fail(src != nullptr && src->consumerSurface != nullptr);
    // The internal function do not need judge whether it is empty
    GstMemSrc *memsrc = GST_MEM_SRC(src);
    sptr<Surface> surface = src->consumerSurface;
    guint width = DEFAULT_VIDEO_WIDTH;
    guint height = DEFAULT_VIDEO_HEIGHT;
    GST_OBJECT_LOCK(memsrc);
    if (memsrc->caps != nullptr) {
        GstVideoInfo info;
        gst_video_info_init(&info);
        gst_video_info_from_caps(&info, memsrc->caps);
        width = static_cast<guint>(info.width);
        height = static_cast<guint>(info.height);
    }
    GST_OBJECT_UNLOCK(memsrc);
    SurfaceError ret = surface->SetUserData("video_width", std::to_string(width));
    if (ret != SURFACE_ERROR_OK) {
        GST_WARNING_OBJECT(src, "Set video width fail");
    }
    ret = surface->SetUserData("video_height", std::to_string(height));
    if (ret != SURFACE_ERROR_OK) {
        GST_WARNING_OBJECT(src, "Set video height fail");
    }
    ret = surface->SetUserData("surface_size", std::to_string(DEFAULT_SURFACE_SIZE));
    if (ret != SURFACE_ERROR_OK) {
        GST_WARNING_OBJECT(src, "Set surface size fail");
    }
    ret = surface->SetDefaultWidthAndHeight(width, height);
    if (ret != SURFACE_ERROR_OK) {
        GST_WARNING_OBJECT(src, "Set surface width and height fail");
    }
}

static gboolean gst_surface_src_get_pool(GstSurfaceSrc *surfacesrc, GstQuery *query, GstCaps *outcaps)
{
    g_return_val_if_fail(surfacesrc != nullptr && query != nullptr && surfacesrc->consumerSurface != nullptr, FALSE);
    if (surfacesrc->pool == nullptr) {
        return FALSE;
    }
    GstMemSrc *memsrc = GST_MEM_SRC(surfacesrc);
    guint min_buf = surfacesrc->consumerSurface->GetQueueSize();
    guint max_buf = min_buf;
    memsrc->buffer_num = max_buf;
    gboolean is_video = gst_query_find_allocation_meta(query, GST_VIDEO_META_API_TYPE, nullptr);
    if (is_video) {
        // when video need update size
        GstVideoInfo info;
        gst_video_info_init(&info);
        gst_video_info_from_caps(&info, outcaps);
        if (info.size != 0) {
            memsrc->buffer_size = info.size;
        }
    }
    GST_INFO_OBJECT(surfacesrc, "update buffer num %u", memsrc->buffer_num);
    GstStructure *config = gst_buffer_pool_get_config(surfacesrc->pool);
    if (is_video) {
        gst_buffer_pool_config_add_option(config, GST_BUFFER_POOL_OPTION_VIDEO_META);
    }
    gst_buffer_pool_config_set_params(config, outcaps, memsrc->buffer_size, min_buf, max_buf);
    if (gst_buffer_pool_set_config(surfacesrc->pool, config) != TRUE) {
        GST_WARNING_OBJECT(surfacesrc, "set config failed");
    }

    if (gst_query_get_n_allocation_pools(query) > 0) {
        gst_query_set_nth_allocation_pool(query, 0, surfacesrc->pool, memsrc->buffer_num, min_buf, max_buf);
    } else {
        gst_query_add_allocation_pool(query, surfacesrc->pool, memsrc->buffer_num, min_buf, max_buf);
    }

    GST_DEBUG_OBJECT(surfacesrc, "set surface pool success");
    return TRUE;
}

static gboolean gst_surface_src_decide_allocation(GstBaseSrc *basesrc, GstQuery *query)
{
    GstSurfaceSrc *surfacesrc = GST_SURFACE_SRC(basesrc);
    g_return_val_if_fail(basesrc != nullptr && query != nullptr, FALSE);
    GST_OBJECT_LOCK(surfacesrc);
    surfacesrc->need_flush = TRUE;
    GST_OBJECT_UNLOCK(surfacesrc);
    GstCaps *outcaps = nullptr;
    GstBufferPool *pool = nullptr;
    guint size = 0;
    guint min_buf = DEFAULT_SURFACE_QUEUE_SIZE;
    guint max_buf = DEFAULT_SURFACE_QUEUE_SIZE;

    // get caps and save to video info
    gst_query_parse_allocation(query, &outcaps, nullptr);
    // get pool and pool info from down stream
    if (gst_query_get_n_allocation_pools(query) > 0) {
        gst_query_parse_nth_allocation_pool(query, 0, &pool, &size, &min_buf, &max_buf);
    }
    auto caps_struct = gst_caps_get_structure(outcaps, 0);
    auto mediaType = gst_structure_get_name(caps_struct);
    gboolean isVideo = g_str_has_prefix(mediaType, "video/");
    if (isVideo) {
        gst_query_add_allocation_meta(query, GST_VIDEO_META_API_TYPE, nullptr);
    }
    GstVideoInfo info;
    gst_video_info_init(&info);
    gst_video_info_from_caps(&info, outcaps);
    size = info.size;

    // we use our own pool
    if (pool != nullptr) {
        gst_query_set_nth_allocation_pool(query, 0, nullptr, 0, 0, 0);
        gst_object_unref(pool);
        pool = nullptr;
    }
    if (gst_surface_src_get_pool(surfacesrc, query, outcaps)) {
        return TRUE;
    }
    return FALSE;
}

static void gst_surface_mem_src_dump_buffer(GstBaseSrc *self, GstBuffer *buffer)
{
    g_return_if_fail(self != nullptr);
    GstSurfaceSrc *src = GST_SURFACE_SRC(self);
    g_return_if_fail(src != nullptr);
    g_return_if_fail(buffer != nullptr);

    if (src->dump.enable_dump == FALSE) {
        return;
    }

    GST_DEBUG_OBJECT(src, "Dump surface src buffer");
    std::string input_dump_file = "/data/media/surface-in-" +
        std::to_string(static_cast<int32_t>(FAKE_POINTER(self))) + ".yuv";
    if (src->dump.dump_file == nullptr) {
        src->dump.dump_file = fopen(input_dump_file.c_str(), "wb+");
    }
    if (src->dump.dump_file == nullptr) {
        GST_ERROR_OBJECT(self, "open file failed");
        return;
    }
    GstMapInfo info = GST_MAP_INFO_INIT;
    g_return_if_fail(gst_buffer_map(buffer, &info, GST_MAP_READ));
    (void)fwrite(info.data, info.size, 1, src->dump.dump_file);
    (void)fflush(src->dump.dump_file);
    gst_buffer_unmap(buffer, &info);
}

static void gst_surface_mem_src_dump_from_sys_param(GstSurfaceSrc *src)
{
    std::string dump_enable;
    src->dump.enable_dump = FALSE;
    int32_t res = OHOS::system::GetStringParameter("sys.media.dump.surfacesrc.enable", dump_enable, "");
    if (res != 0 || dump_enable.empty()) {
        GST_DEBUG_OBJECT(src, "sys.media.dump.surfacesrc.enable");
        return;
    }
    GST_DEBUG_OBJECT(src, "sys.media.dump.surfacesrc.enable=%s", dump_enable.c_str());

    if (dump_enable == "true") {
        src->dump.enable_dump = TRUE;
    }
}