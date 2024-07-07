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

#include "gst_surface_mem_sink.h"
#include <cinttypes>
#include "surface.h"
#include "gst_producer_surface_pool.h"
#include "buffer_type_meta.h"
#include "media_log.h"
#include "param_wrapper.h"
#include "scope_guard.h"
#include "media_dfx.h"
#include "av_common.h"

using namespace OHOS;
using namespace OHOS::Media;

struct _GstSurfaceMemSinkPrivate {
    OHOS::sptr<OHOS::Surface> surface;
    GstProducerSurfacePool *pool;
    guint rotation;
};

enum {
    PROP_0,
    PROP_SURFACE,
    PROP_SURFACE_POOL,
    PROP_CACHE_BUFFERS_NUM,
    PROP_PERFORMANCE_MODE,
    PROP_VIDEO_SCALE_TYPE,
    PROP_VIDEO_ROTATION,
};

static GstStaticPadTemplate g_sinktemplate = GST_STATIC_PAD_TEMPLATE("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

static void gst_surface_mem_sink_dispose(GObject *obj);
static void gst_surface_mem_sink_finalize(GObject *obj);
static void gst_surface_mem_sink_set_property(GObject *object, guint propId, const GValue *value, GParamSpec *pspec);
static void gst_surface_mem_sink_get_property(GObject *object, guint propId, GValue *value, GParamSpec *pspec);
static gboolean gst_surface_mem_sink_do_propose_allocation(GstMemSink *memsink, GstQuery *query);
static GstFlowReturn gst_surface_mem_sink_do_app_render(GstMemSink *memsink, GstBuffer *buffer, bool is_preroll);
static void gst_surface_mem_sink_dump_from_sys_param(GstSurfaceMemSink *self);
static void gst_surface_mem_sink_dump_buffer(GstSurfaceMemSink *self, GstBuffer *buffer);
static GstStateChangeReturn gst_surface_mem_sink_change_state(GstElement *element, GstStateChange transition);
static gboolean gst_surface_mem_sink_event(GstBaseSink *bsink, GstEvent *event);
static GstFlowReturn gst_surface_mem_sink_subclass_do_app_render(GstSurfaceMemSink *sink,
    GstBuffer *buffer, bool is_preroll);

#define gst_surface_mem_sink_parent_class parent_class
G_DEFINE_TYPE_WITH_CODE(GstSurfaceMemSink, gst_surface_mem_sink,
                        GST_TYPE_MEM_SINK, G_ADD_PRIVATE(GstSurfaceMemSink));

GST_DEBUG_CATEGORY_STATIC(gst_surface_mem_sink_debug_category);
#define GST_CAT_DEFAULT gst_surface_mem_sink_debug_category

static void gst_surface_mem_sink_class_init(GstSurfaceMemSinkClass *klass)
{
    g_return_if_fail(klass != nullptr);

    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GstMemSinkClass *mem_sink_class = GST_MEM_SINK_CLASS(klass);
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);
    GstBaseSinkClass *base_sink_class = GST_BASE_SINK_CLASS(klass);

    gst_element_class_add_static_pad_template(element_class, &g_sinktemplate);

    gobject_class->dispose = gst_surface_mem_sink_dispose;
    gobject_class->finalize = gst_surface_mem_sink_finalize;
    gobject_class->set_property = gst_surface_mem_sink_set_property;
    gobject_class->get_property = gst_surface_mem_sink_get_property;
    element_class->change_state = gst_surface_mem_sink_change_state;

    gst_element_class_set_static_metadata(element_class,
        "SurfaceMemSink", "Sink/Video",
        "Output to surface buffer and allow the application to get access to the surface buffer",
        "OpenHarmony");

    g_object_class_install_property(gobject_class, PROP_SURFACE,
        g_param_spec_pointer("surface", "Surface",
            "Surface for rendering output",
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_SURFACE_POOL,
        g_param_spec_pointer("surface-pool", "Surface Pool",
            "Surface pool for rendering output buffers",
            (GParamFlags)(G_PARAM_READABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_CACHE_BUFFERS_NUM,
        g_param_spec_uint("cache-buffers-num", "Cache Buffers Num",
            "The cache buffers num in pool",
            0, G_MAXUINT, 0, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_PERFORMANCE_MODE,
        g_param_spec_boolean("performance-mode", "performance mode", "performance mode",
            FALSE, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_VIDEO_SCALE_TYPE,
        g_param_spec_uint("video-scale-type", "Video Scale Type",
            "Set video scale type for graphic",
            0, G_MAXUINT, 0, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_VIDEO_ROTATION,
        g_param_spec_uint("video-rotation", "Video Rotation",
            "Set video rotation for surface",
            0, G_MAXUINT, 0, (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    mem_sink_class->do_propose_allocation = gst_surface_mem_sink_do_propose_allocation;
    mem_sink_class->do_app_render = gst_surface_mem_sink_do_app_render;
    base_sink_class->event = gst_surface_mem_sink_event;
    klass->do_app_render = gst_surface_mem_sink_subclass_do_app_render;
    GST_DEBUG_CATEGORY_INIT(gst_surface_mem_sink_debug_category, "surfacesink", 0, "surfacesink class");
}

static void gst_surface_mem_sink_init(GstSurfaceMemSink *sink)
{
    g_return_if_fail(sink != nullptr);
    GST_DEBUG_OBJECT(sink, "Init, id = %d", static_cast<int32_t>(FAKE_POINTER(sink)));

    auto priv = reinterpret_cast<GstSurfaceMemSinkPrivate *>(gst_surface_mem_sink_get_instance_private(sink));
    g_return_if_fail(priv != nullptr);
    sink->priv = priv;
    sink->priv->surface = nullptr;
    sink->priv->pool = GST_PRODUCER_SURFACE_POOL_CAST(gst_producer_surface_pool_new());
    sink->priv->rotation = 0;
    sink->prerollBuffer = nullptr;
    sink->firstRenderFrame = TRUE;
    sink->preInitPool = FALSE;
    sink->dump.enable_dump = FALSE;
    sink->dump.dump_file = nullptr;
    sink->performanceMode = FALSE;
    sink->lastRate = 0;
    sink->renderCnt = 0;
    GstMemSink *memSink = GST_MEM_SINK_CAST(sink);
    memSink->max_pool_capacity = SURFACE_MAX_QUEUE_SIZE;
    gst_surface_mem_sink_dump_from_sys_param(sink);
}

static void gst_surface_mem_sink_dispose(GObject *obj)
{
    g_return_if_fail(obj != nullptr);

    GstSurfaceMemSink *surface_sink = GST_SURFACE_MEM_SINK_CAST(obj);
    GstSurfaceMemSinkPrivate *priv = surface_sink->priv;
    g_return_if_fail(priv != nullptr);

    GST_OBJECT_LOCK(surface_sink);
    priv->surface = nullptr;
    if (priv->pool != nullptr) {
        gst_object_unref(priv->pool);
        priv->pool = nullptr;
    }
    GST_OBJECT_UNLOCK(surface_sink);

    G_OBJECT_CLASS(parent_class)->dispose(obj);
}

static void gst_surface_mem_sink_finalize(GObject *obj)
{
    g_return_if_fail(obj != nullptr);
    GstSurfaceMemSink *surface_sink = GST_SURFACE_MEM_SINK_CAST(obj);
    gst_caps_unref(surface_sink->caps);
    G_OBJECT_CLASS(parent_class)->finalize(obj);
}

static GraphicTransformType gst_surface_mem_sink_get_rotation(guint rotation)
{
    // We gets the rotation direction clockwise, but Surface needs it counterclockwise
    switch (rotation) {
        case VIDEO_ROTATION_90: {
            return GRAPHIC_ROTATE_270;
        }
        case VIDEO_ROTATION_180: {
            return GRAPHIC_ROTATE_180;
        }
        case VIDEO_ROTATION_270: {
            return GRAPHIC_ROTATE_90;
        }
        default:
            return GRAPHIC_ROTATE_NONE;
    }
}

static void gst_surface_mem_sink_set_property(GObject *object, guint propId, const GValue *value, GParamSpec *pspec)
{
    g_return_if_fail(object != nullptr && value != nullptr);

    GstSurfaceMemSink *surface_sink = GST_SURFACE_MEM_SINK_CAST(object);
    GstSurfaceMemSinkPrivate *priv = surface_sink->priv;
    g_return_if_fail(priv != nullptr);

    switch (propId) {
        case PROP_SURFACE: {
            gpointer surface = g_value_get_pointer(value);
            g_return_if_fail(surface != nullptr);
            OHOS::sptr<OHOS::Surface> surface_ref = reinterpret_cast<OHOS::Surface *>(surface);
            GST_OBJECT_LOCK(surface_sink);
            priv->surface = surface_ref;
            gst_producer_surface_pool_set_surface(priv->pool, surface_ref);
            GST_OBJECT_UNLOCK(surface_sink);
            break;
        }
        case PROP_CACHE_BUFFERS_NUM: {
            guint cache_buffer_num = g_value_get_uint(value);
            g_object_set(G_OBJECT(priv->pool), "cache-buffers-num", cache_buffer_num, nullptr);
            break;
        }
        case PROP_PERFORMANCE_MODE:
            surface_sink->performanceMode = g_value_get_boolean(value);
            break;

        case PROP_VIDEO_SCALE_TYPE: {
            guint video_scale_type = g_value_get_uint(value);
            g_object_set(G_OBJECT(priv->pool), "video-scale-type", video_scale_type, nullptr);
            break;
        }
        case PROP_VIDEO_ROTATION: {
            priv->rotation = g_value_get_uint(value);
            break;
        }
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propId, pspec);
            break;
    }
}

static void gst_surface_mem_sink_get_property(GObject *object, guint propId, GValue *value, GParamSpec *pspec)
{
    g_return_if_fail(object != nullptr);

    GstSurfaceMemSink *surface_sink = GST_SURFACE_MEM_SINK_CAST(object);
    GstSurfaceMemSinkPrivate *priv = surface_sink->priv;
    g_return_if_fail(priv != nullptr);

    switch (propId) {
        case PROP_SURFACE: {
            GST_OBJECT_LOCK(surface_sink);
            g_return_if_fail(priv->surface != nullptr);
            g_value_set_pointer(value, priv->surface.GetRefPtr());
            GST_OBJECT_UNLOCK(surface_sink);
            break;
        }
        case PROP_SURFACE_POOL: {
            GST_OBJECT_LOCK(surface_sink);
            ON_SCOPE_EXIT(0) { GST_OBJECT_UNLOCK(surface_sink); };
            g_value_set_pointer(value, priv->pool);
            GstSurfaceAllocator *allocator = gst_surface_allocator_new();
            g_return_if_fail(allocator != nullptr);
            ON_SCOPE_EXIT(1) { gst_object_unref(allocator); };
            GstStructure *config = gst_buffer_pool_get_config(GST_BUFFER_POOL(priv->pool));
            g_return_if_fail(config != nullptr);
            gst_buffer_pool_config_set_allocator(config, GST_ALLOCATOR_CAST(allocator), nullptr);
            (void)gst_buffer_pool_set_config(GST_BUFFER_POOL(priv->pool), config);
            surface_sink->preInitPool = TRUE;
            break;
        }
        case PROP_VIDEO_ROTATION: {
            GST_OBJECT_LOCK(surface_sink);
            g_value_set_uint(value, priv->rotation);
            GST_OBJECT_UNLOCK(surface_sink);
            break;
        }
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propId, pspec);
            break;
    }
}

static gboolean gst_surface_mem_sink_drop_frame_check(GstSurfaceMemSink *surface_sink)
{
    GstBaseSink *gst_base_sink = GST_BASE_SINK(surface_sink);
    static guint dropFrameArray[] = {0, 1, 0, 1, 0, 0, 1};

    guint rate = (guint)(gst_base_sink->segment.rate * 100);   // 100 : rate double convert to uint
    if (surface_sink->lastRate != rate) {
        surface_sink->lastRate = rate;
        surface_sink->renderCnt = 0;
    }
    surface_sink->renderCnt++;

    if ((rate == 125 && surface_sink->renderCnt % 5 == 0) ||  // 125 : 1.25 * 100 && 5 : drop frame inteval
        (rate == 175 && dropFrameArray[surface_sink->renderCnt % 7]) ||  // 175 : 1.75 * 100 && 7 : drop frame inteval
        (rate == 200 && surface_sink->renderCnt % 2 == 0)) {  // 200 : 2.00 * 100 && 2 : drop frame inteval
        return FALSE;
    }

    return TRUE;
}

static GstFlowReturn gst_surface_mem_sink_subclass_do_app_render(GstSurfaceMemSink *sink,
    GstBuffer *buffer, bool is_preroll)
{
    (void)sink;
    (void)buffer;
    (void)is_preroll;

    return GST_FLOW_OK;
}

static gboolean gst_surface_mem_sink_need_flush(GstSurfaceMemSink *surface_sink, GstBuffer *buffer, bool is_preroll)
{
    gboolean needFlush = TRUE;
    if (is_preroll) {
        surface_sink->prerollBuffer = buffer;
    } else {
        if (surface_sink->prerollBuffer == buffer) {
            // if it's paused, then play, this buffer is render by preroll
            surface_sink->prerollBuffer = nullptr;
            needFlush = FALSE;
        }
    }
    return needFlush;
}

static GstFlowReturn gst_surface_do_render_buffer(GstMemSink *memsink, GstBuffer *buffer, bool is_preroll)
{
    g_return_val_if_fail(memsink != nullptr && buffer != nullptr, GST_FLOW_ERROR);
    GstSurfaceMemSink *surface_sink = GST_SURFACE_MEM_SINK_CAST(memsink);
    g_return_val_if_fail(surface_sink != nullptr, GST_FLOW_ERROR);
    GstSurfaceMemSinkPrivate *priv = surface_sink->priv;
    g_return_val_if_fail(priv != nullptr, GST_FLOW_ERROR);

    for (guint i = 0; i < gst_buffer_n_memory(buffer); i++) {
        GstMemory *memory = gst_buffer_peek_memory(buffer, i);
        if (!gst_is_surface_memory(memory)) {
            GST_WARNING_OBJECT(surface_sink, "not surface buffer !, 0x%06" PRIXPTR, FAKE_POINTER(memory));
            continue;
        }

        GstSurfaceMemory *surface_mem = reinterpret_cast<GstSurfaceMemory *>(memory);

        gboolean needFlush = gst_surface_mem_sink_need_flush(surface_sink, buffer, is_preroll);
        if (needFlush) {
            // surface do not support timestamp is 0.
            GST_BUFFER_PTS(buffer) = GST_BUFFER_PTS(buffer) == 0 ? 1 : GST_BUFFER_PTS(buffer);
            OHOS::BufferFlushConfig flushConfig = {
                { 0, 0, surface_mem->buf->GetWidth(), surface_mem->buf->GetHeight() }, GST_BUFFER_PTS(buffer)
            };
            gst_surface_mem_sink_dump_buffer(surface_sink, buffer);
            {
                MediaTrace trace("Surface::FlushBuffer");
                surface_mem->need_render = TRUE;
                OHOS::SurfaceError ret = priv->surface->FlushBuffer(surface_mem->buf, surface_mem->fence, flushConfig);
                if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK) {
                    surface_mem->need_render = FALSE;
                    GST_ERROR_OBJECT(surface_sink, "flush buffer to surface failed, %d", ret);
                }
            }
        }
    }
    return GST_FLOW_OK;
}

static GstFlowReturn gst_surface_mem_sink_do_app_render(GstMemSink *memsink, GstBuffer *buffer, bool is_preroll)
{
    g_return_val_if_fail(memsink != nullptr && buffer != nullptr, GST_FLOW_ERROR);
    GstSurfaceMemSink *surface_sink = GST_SURFACE_MEM_SINK_CAST(memsink);
    g_return_val_if_fail(surface_sink != nullptr, GST_FLOW_ERROR);

    GstSurfaceMemSinkClass *surface_sink_class = GST_SURFACE_MEM_SINK_GET_CLASS(surface_sink);
    GST_OBJECT_LOCK(surface_sink);

    if (gst_surface_mem_sink_drop_frame_check(surface_sink) == FALSE) {
        GST_OBJECT_UNLOCK(surface_sink);
        GST_DEBUG_OBJECT(surface_sink, "user set rate, drop same frame");
        return GST_FLOW_OK;
    }

    /* Make sure set rotation information for each video, avoid the rotation
     * information of the previous video affecting the subsequent video
     */
    if (surface_sink->firstRenderFrame) {
        GST_DEBUG_OBJECT(surface_sink, "set rotation: %u", surface_sink->priv->rotation);
        if (surface_sink->priv->surface) {
            MediaTrace trace("Surface::SetTransform");
            (void)surface_sink->priv->surface->SetTransform(
                gst_surface_mem_sink_get_rotation(surface_sink->priv->rotation));
        }
    }

    if (surface_sink->firstRenderFrame && is_preroll) {
        GST_DEBUG_OBJECT(surface_sink, "first render frame");
        surface_sink->firstRenderFrame = FALSE;
        GST_OBJECT_UNLOCK(surface_sink);
        return GST_FLOW_OK;
    }

    (void)gst_surface_do_render_buffer(memsink, buffer, is_preroll);

    GST_OBJECT_UNLOCK(surface_sink);

    if (surface_sink_class->do_app_render != nullptr) {
        (void)surface_sink_class->do_app_render(surface_sink, buffer, is_preroll);
    }
    GST_DEBUG_OBJECT(surface_sink, "End gst_surface_mem_sink_do_app_render");
    return GST_FLOW_OK;
}

static gboolean gst_surface_mem_sink_do_propose_allocation(GstMemSink *memsink, GstQuery *query)
{
    g_return_val_if_fail(memsink != nullptr && query != nullptr, FALSE);
    GstSurfaceMemSink *surface_sink = GST_SURFACE_MEM_SINK_CAST(memsink);
    g_return_val_if_fail(surface_sink != nullptr, FALSE);

    GstCaps *caps = nullptr;
    gboolean needPool = FALSE;
    gst_query_parse_allocation(query, &caps, &needPool);

    if (!needPool) {
        GST_ERROR_OBJECT(surface_sink, "no need buffer pool, unexpected!");
        return FALSE;
    }
    if (surface_sink->performanceMode && surface_sink->preInitPool) {
        GST_INFO_OBJECT(surface_sink, "pool pre init");
        surface_sink->preInitPool = FALSE;
        return TRUE;
    }

    GST_OBJECT_LOCK(surface_sink);
    ON_SCOPE_EXIT(0) { GST_OBJECT_UNLOCK(surface_sink); };

    guint size = 0;
    guint minBuffers = 0;
    guint maxBuffers = 0;
    gst_query_parse_nth_allocation_pool(query, 0, nullptr, &size, &minBuffers, &maxBuffers);
    if (maxBuffers == 0) {
        GST_INFO_OBJECT(surface_sink, "correct the maxbuffer from %u to %u", maxBuffers, memsink->max_pool_capacity);
        maxBuffers = memsink->max_pool_capacity;
    }
    GST_DEBUG("maxBuffers is: %u", maxBuffers);

    GstProducerSurfacePool *pool = surface_sink->priv->pool;
    g_return_val_if_fail(pool != nullptr, FALSE);
    g_return_val_if_fail(gst_buffer_pool_set_active(GST_BUFFER_POOL(pool), FALSE), FALSE);

    GstVideoInfo info;
    GST_DEBUG("begin gst_video_info_from_caps");
    gboolean ret = gst_video_info_from_caps(&info, caps);
    g_return_val_if_fail(ret, FALSE);
    gst_query_add_allocation_pool(query, GST_BUFFER_POOL_CAST(pool), info.size, minBuffers, maxBuffers);

    GstSurfaceAllocator *allocator = gst_surface_allocator_new();
    g_return_val_if_fail(allocator != nullptr, FALSE);
    ON_SCOPE_EXIT(1) { gst_object_unref(allocator); };
    GstStructure *params = gst_structure_new("mem", "memtype", G_TYPE_STRING, "surface", nullptr);
    gst_query_add_allocation_param(query, GST_ALLOCATOR_CAST(allocator), nullptr);
    gst_query_add_allocation_meta(query, GST_VIDEO_META_API_TYPE, nullptr);
    gst_query_add_allocation_meta(query, GST_BUFFER_TYPE_META_API_TYPE, params);
    gst_structure_free(params);

    GstStructure *config = gst_buffer_pool_get_config(GST_BUFFER_POOL_CAST(pool));
    g_return_val_if_fail(config != nullptr, FALSE);

    gst_buffer_pool_config_set_params(config, caps, info.size, minBuffers, maxBuffers);
    gst_buffer_pool_config_set_allocator(config, GST_ALLOCATOR_CAST(allocator), nullptr);
    // set config will take ownership of the config, we dont need to free it.
    ret = gst_buffer_pool_set_config(GST_BUFFER_POOL_CAST(pool), config);

    return ret;
}

void gst_surface_mem_sink_dump_from_sys_param(GstSurfaceMemSink *self)
{
    std::string dump_enable;
    self->dump.enable_dump = FALSE;
    int32_t res = OHOS::system::GetStringParameter("sys.media.dump.frame.enable", dump_enable, "");
    if (res != 0 || dump_enable.empty()) {
        GST_DEBUG_OBJECT(self, "sys.media.dump.frame.enable");
        return;
    }
    GST_DEBUG_OBJECT(self, "sys.media.dump.frame.enable=%s", dump_enable.c_str());

    if (dump_enable == "true") {
        self->dump.enable_dump = TRUE;
    }
}

static gboolean gst_surface_mem_sink_event(GstBaseSink *bsink, GstEvent *event)
{
    GstSurfaceMemSink *surface_mem_sink = GST_SURFACE_MEM_SINK(bsink);
    g_return_val_if_fail(surface_mem_sink != nullptr, FALSE);
    g_return_val_if_fail(event != nullptr, FALSE);

    GST_DEBUG_OBJECT(surface_mem_sink, "event->type %d", event->type);
    switch (event->type) {
        case GST_EVENT_CAPS: {
            GstCaps *caps;
            gst_event_parse_caps(event, &caps);
            surface_mem_sink->caps = caps;
            gst_caps_ref(surface_mem_sink->caps);
            break;
        }
        default:
            break;
    }
    return GST_BASE_SINK_CLASS(parent_class)->event(bsink, event);
}

static void gst_surface_mem_sink_dump_buffer(GstSurfaceMemSink *self, GstBuffer *buffer)
{
    g_return_if_fail(self != nullptr);
    g_return_if_fail(buffer != nullptr);
    if (self->dump.enable_dump == FALSE) {
        return;
    }
    GST_DEBUG_OBJECT(self, "Dump yuv buffer");

    if (self->dump.dump_file == nullptr) {
        GST_ERROR_OBJECT(self, "file not opened");
        return;
    }

    GstVideoMeta *video_meta = gst_buffer_get_video_meta(buffer);
    g_return_if_fail(video_meta != nullptr);
    GstStructure *struc = gst_caps_get_structure(self->caps, 0);
    g_return_if_fail(struc != nullptr);
    const gchar *format = gst_structure_get_string(struc, "format");
    g_return_if_fail(format != nullptr);

    GstMapInfo info = GST_MAP_INFO_INIT;
    g_return_if_fail(gst_buffer_map(buffer, &info, GST_MAP_READ));

    guint stride_width = 0;
    guint stride_height = 0;
    guint stride_size = info.size;
    if (g_str_equal(format, "NV12") || g_str_equal(format, "NV21")) {
        stride_width = video_meta->stride[0];
        if (stride_width != 0) {
            stride_height = video_meta->offset[1] / stride_width;
            if (stride_height % 32 != 0) { // 32 : 高对齐
                stride_height = ((stride_height / 32) + 1) * 32; // 32 : 高对齐
            }
            stride_size = (stride_width * stride_height * 3) / 2;  // 3 2 : NV12和NV21 size比例大小
        }
    }
    GST_DEBUG_OBJECT(self, "format %s, stride width %u, stride height %u,"
        "stride_size %u, info.size %" G_GSIZE_FORMAT,
        format, stride_width, stride_height, stride_size, info.size);
    if (stride_size > info.size) {
        stride_size = info.size;
    }
    (void)fwrite(info.data, stride_size, 1, self->dump.dump_file);
    (void)fflush(self->dump.dump_file);
    gst_buffer_unmap(buffer, &info);
}

static GstStateChangeReturn gst_surface_mem_sink_change_state(GstElement *element, GstStateChange transition)
{
    g_return_val_if_fail(element != nullptr, GST_STATE_CHANGE_FAILURE);
    GstSurfaceMemSink *self = GST_SURFACE_MEM_SINK(element);

    GST_DEBUG_OBJECT(element, "change state %d", transition);
    switch (transition) {
        case GST_STATE_CHANGE_READY_TO_PAUSED:
            if (self->dump.enable_dump == TRUE) {
                std::string dump_file = "/data/media/dump-" +
                    std::to_string(static_cast<int32_t>(FAKE_POINTER(element))) + ".yuv";
                if (self->dump.dump_file == nullptr) {
                    self->dump.dump_file = fopen(dump_file.c_str(), "wb+");
                }
            }
            break;
        case GST_STATE_CHANGE_PAUSED_TO_READY:
            if (self->dump.enable_dump == TRUE) {
                if (self->dump.dump_file != nullptr) {
                    fclose(self->dump.dump_file);
                    self->dump.dump_file = nullptr;
                }
            }
            break;
        default:
            break;
    }
    return GST_ELEMENT_CLASS(parent_class)->change_state(element, transition);
}
