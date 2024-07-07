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

#include "gst_venc_base.h"
#include <vector>
#include "buffer_type_meta.h"
#include "scope_guard.h"
#include "securec.h"
#include "gst_codec_video_common.h"

using namespace OHOS;
using namespace OHOS::Media;
GST_DEBUG_CATEGORY_STATIC(gst_venc_base_debug_category);
#define GST_CAT_DEFAULT gst_venc_base_debug_category
#define gst_venc_base_parent_class parent_class
#define GST_VENC_BASE_SUPPORTED_FORMATS "{ NV21 }"
#define GST_VENC_BITRATE_DEFAULT (0xffffffff)
#define GST_VENC_ALIGN_DEFAULT 16

static void gst_venc_base_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void gst_venc_base_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static gboolean gst_venc_base_open(GstVideoEncoder *encoder);
static gboolean gst_venc_base_close(GstVideoEncoder *encoder);
static gboolean gst_venc_base_start(GstVideoEncoder *encoder);
static gboolean gst_venc_base_stop(GstVideoEncoder *encoder);
static gboolean gst_venc_base_set_format(GstVideoEncoder *encoder, GstVideoCodecState *state);
static gboolean gst_venc_base_flush(GstVideoEncoder *encoder);
static GstFlowReturn gst_venc_base_handle_frame(GstVideoEncoder *encoder, GstVideoCodecFrame *frame);
static void gst_venc_base_finalize(GObject *object);
static GstFlowReturn gst_venc_base_finish(GstVideoEncoder *encoder);
static void gst_venc_base_loop(GstVencBase *self);
static void gst_venc_base_pause_loop(GstVencBase *self);
static gboolean gst_venc_base_event(GstVideoEncoder *encoder, GstEvent *event);
static gboolean gst_venc_base_decide_allocation(GstVideoEncoder *encoder, GstQuery *query);
static gboolean gst_venc_base_propose_allocation(GstVideoEncoder *encoder, GstQuery *query);
static gboolean gst_codec_return_is_ok(const GstVencBase *encoder, gint ret,
    const char *error_name, gboolean need_report);
static GstStateChangeReturn gst_venc_base_change_state(GstElement *element, GstStateChange transition);
static void gst_venc_base_init_config(GObjectClass *gobject_class);

enum {
    PROP_0,
    PROP_BITRATE,
    PROP_REQUEST_I_FRAME,
    PROP_VENDOR,
    PROP_SURFACE_ENABLE,
    PROP_I_FRAME_INTERVAL,
    PROP_BITRATE_MODE,
    PROP_CODEC_QUALITY,
    PROP_I_FRAME_INTERVAL_NEW,
    PROP_CODEC_PROFILE,
};

G_DEFINE_ABSTRACT_TYPE(GstVencBase, gst_venc_base, GST_TYPE_VIDEO_ENCODER);

static void gst_venc_base_class_init(GstVencBaseClass *klass)
{
    GST_DEBUG_OBJECT(klass, "Class init");
    g_return_if_fail(klass != nullptr);
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);
    GstVideoEncoderClass *video_encoder_class = GST_VIDEO_ENCODER_CLASS(klass);
    GST_DEBUG_CATEGORY_INIT(gst_venc_base_debug_category, "vencbase", 0, "video encoder base class");
    gobject_class->set_property = gst_venc_base_set_property;
    gobject_class->get_property = gst_venc_base_get_property;
    gobject_class->finalize = gst_venc_base_finalize;
    video_encoder_class->open = gst_venc_base_open;
    video_encoder_class->close = gst_venc_base_close;
    video_encoder_class->start = gst_venc_base_start;
    video_encoder_class->stop = gst_venc_base_stop;
    video_encoder_class->flush = gst_venc_base_flush;
    video_encoder_class->set_format = gst_venc_base_set_format;
    video_encoder_class->handle_frame = gst_venc_base_handle_frame;
    video_encoder_class->finish = gst_venc_base_finish;
    video_encoder_class->sink_event = gst_venc_base_event;
    video_encoder_class->decide_allocation = gst_venc_base_decide_allocation;
    video_encoder_class->propose_allocation = gst_venc_base_propose_allocation;
    element_class->change_state = gst_venc_base_change_state;

    g_object_class_install_property(gobject_class, PROP_BITRATE,
        g_param_spec_uint("bitrate", "Bitrate", "Bitrate in bits per second",
        0, G_MAXUINT, GST_VENC_BITRATE_DEFAULT,
        (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | GST_PARAM_MUTABLE_PLAYING)));

    g_object_class_install_property(gobject_class, PROP_REQUEST_I_FRAME,
        g_param_spec_uint("req-i-frame", "Request I frame", "Request I frame for video encoder",
            0, G_MAXUINT32, 0, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_I_FRAME_INTERVAL,
        g_param_spec_uint("i-frame-interval", "I frame interval", "Set i frame interval for video encoder",
            0, G_MAXUINT32, 0, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_VENDOR,
        g_param_spec_pointer("vendor", "Vendor property", "Vendor property",
            (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    gst_venc_base_init_config(gobject_class);
    const gchar *sink_caps_string = GST_VIDEO_CAPS_MAKE(GST_VENC_BASE_SUPPORTED_FORMATS);
    GstCaps *sink_caps = gst_caps_from_string(sink_caps_string);
    if (sink_caps != nullptr) {
        GstPadTemplate *sink_templ = gst_pad_template_new("sink", GST_PAD_SINK, GST_PAD_ALWAYS, sink_caps);
        gst_element_class_add_pad_template(element_class, sink_templ);
        gst_caps_unref(sink_caps);
    }
}

static void gst_venc_base_init_config(GObjectClass *gobject_class)
{
    g_object_class_install_property(gobject_class, PROP_SURFACE_ENABLE,
        g_param_spec_boolean("enable-surface", "Enable Surface", "The input mem is surface buffer",
        FALSE, (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_BITRATE_MODE,
        g_param_spec_int("bitrate-mode", "Bitrate mode", "bitrate mode for video encoder",
            0, G_MAXINT32, 0, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_CODEC_QUALITY,
        g_param_spec_int("codec-quality", "Codec quality", "Codec quality for video encoder",
            0, G_MAXINT32, 0, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    // this interval is operate by codec
    g_object_class_install_property(gobject_class, PROP_I_FRAME_INTERVAL_NEW,
        g_param_spec_int("i-frame-interval-new", "I frame interval new", "I frame interval for video encoder",
            0, G_MAXINT32, 0, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_CODEC_PROFILE,
        g_param_spec_int("codec-profile", "Codec profile", "Codec profile for video encoder",
            0, G_MAXINT32, 0, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));
}

static void gst_venc_base_set_property_next(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    (void)pspec;
    GstVencBase *self = GST_VENC_BASE(object);
    g_return_if_fail(value != nullptr);
    g_return_if_fail(self != nullptr);
    switch (property_id) {
        case PROP_BITRATE_MODE:
            self->bitrate_mode = g_value_get_int(value);
            break;
        case PROP_CODEC_QUALITY:
            self->codec_quality = g_value_get_int(value);
            break;
        case PROP_I_FRAME_INTERVAL_NEW:
            self->i_frame_interval_new = g_value_get_int(value);
            break;
        case PROP_CODEC_PROFILE:
            self->codec_profile = g_value_get_int(value);
            break;
        default:
            break;
    }
}

static void gst_venc_base_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GST_DEBUG_OBJECT(object, "Set Property");
    GstVencBase *self = GST_VENC_BASE(object);
    g_return_if_fail(value != nullptr);
    g_return_if_fail(self != nullptr);
    gint ret = GST_CODEC_OK;
    switch (property_id) {
        case PROP_BITRATE: {
            GST_INFO_OBJECT(object, "Set dynamic bitrate");
            GST_OBJECT_LOCK(self);
            self->bitrate = g_value_get_uint(value);
            if (self->encoder != nullptr) {
                ret = self->encoder->SetParameter(GST_DYNAMIC_BITRATE, GST_ELEMENT(self));
            }
            GST_OBJECT_UNLOCK(self);
            g_return_if_fail(ret == GST_CODEC_OK);
            break;
        }
        case PROP_REQUEST_I_FRAME: {
            GST_INFO_OBJECT(object, "Request I frame");
            if (self->encoder != nullptr) {
                g_return_if_fail(self->encoder->SetParameter(GST_REQUEST_I_FRAME, GST_ELEMENT(self)) == GST_CODEC_OK);
            }
            break;
        }
        case PROP_I_FRAME_INTERVAL: {
            self->i_frame_interval = g_value_get_uint(value);
            GST_INFO_OBJECT(object, "Set i frame interval %u for video encoder", self->i_frame_interval);
            break;
        }
        case PROP_VENDOR: {
            GST_INFO_OBJECT(object, "Set vendor property");
            if (self->encoder != nullptr) {
                g_return_if_fail(self->encoder->SetParameter(GST_VENDOR, GST_ELEMENT(self)) == GST_CODEC_OK);
            }
            break;
        }
        case PROP_SURFACE_ENABLE: {
            GST_OBJECT_LOCK(self);
            gboolean enable = g_value_get_boolean(value);
            self->memtype = enable ? GST_MEMTYPE_SURFACE : self->memtype;
            GST_OBJECT_UNLOCK(self);
            break;
        }
        default:
            break;
    }
    gst_venc_base_set_property_next(object, property_id, value, pspec);
}

static void gst_venc_base_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    (void)pspec;
    GST_DEBUG_OBJECT(object, "Get Property");
    g_return_if_fail(object != nullptr);
    GstVencBase *self = GST_VENC_BASE(object);
    switch (property_id) {
        case PROP_BITRATE:
            GST_OBJECT_LOCK(self);
            g_value_set_uint(value, self->bitrate);
            GST_OBJECT_UNLOCK(self);
            break;
        default: {
            break;
        }
    }
}

static void gst_venc_base_init(GstVencBase *self)
{
    GST_DEBUG_OBJECT(self, "Init");
    g_return_if_fail(self != nullptr);
    g_return_if_fail(GST_VIDEO_ENCODER_SINK_PAD(self) != nullptr);
    GST_PAD_SET_ACCEPT_TEMPLATE(GST_VIDEO_ENCODER_SINK_PAD(self));

    g_mutex_init(&self->lock);
    g_mutex_init(&self->drain_lock);
    g_cond_init(&self->drain_cond);
    self->draining = FALSE;
    self->flushing = FALSE;
    self->prepared = FALSE;
    self->width = 0;
    self->height = 0;
    self->frame_rate = 0;
    (void)memset_s(&self->input, sizeof(GstVencBasePort), 0, sizeof(GstVencBasePort));
    (void)memset_s(&self->output, sizeof(GstVencBasePort), 0, sizeof(GstVencBasePort));
    self->memtype = GST_MEMTYPE_INVALID;
    self->bitrate = 0;
    self->input.frame_cnt = 0;
    self->input.first_frame_time = 0;
    self->input.last_frame_time = 0;
    self->output.frame_cnt = 0;
    self->output.first_frame_time = 0;
    self->output.last_frame_time = 0;
    self->coding_outbuf_cnt = 0;
    self->first_in_frame = TRUE;
    self->first_out_frame = TRUE;
    self->last_pts = GST_CLOCK_TIME_NONE;
    self->first_frame_pts = GST_CLOCK_TIME_NONE;
    self->i_frame_interval = 0;
    self->flushing_stopping = FALSE;
    self->encoder_start = FALSE;
    self->bitrate_mode = -1;
    self->codec_quality = -1;
    self->i_frame_interval_new = 1000; // 1000ms I frame once
    self->codec_profile = -1;
    self->codec_level = -1;
}

static void gst_venc_base_finalize(GObject *object)
{
    GST_DEBUG_OBJECT(object, "Finalize");
    g_return_if_fail(object != nullptr);
    GstVencBase *self = GST_VENC_BASE(object);
    gst_object_unref(self->inpool);
    self->inpool = nullptr;
    gst_object_unref(self->outpool);
    self->outpool = nullptr;
    gst_object_unref(self->input.allocator);
    self->input.allocator = nullptr;
    gst_object_unref(self->output.allocator);
    self->output.allocator = nullptr;
    gst_video_codec_state_unref(self->input_state);
    self->input_state = nullptr;
    gst_object_unref(self->output_state);
    self->output_state = nullptr;
    g_mutex_clear(&self->drain_lock);
    g_cond_clear(&self->drain_cond);
    g_mutex_clear(&self->lock);
    self->input.av_shmem_pool = nullptr;
    self->output.av_shmem_pool = nullptr;
    std::vector<GstVideoFormat> tempVec;
    tempVec.swap(self->formats);
    self->encoder = nullptr;
    G_OBJECT_CLASS(parent_class)->finalize(object);
}

static gboolean gst_venc_base_open(GstVideoEncoder *encoder)
{
    GST_DEBUG_OBJECT(encoder, "Open");
    g_return_val_if_fail(encoder != nullptr, FALSE);
    GstVencBase *self = GST_VENC_BASE(encoder);
    GstVencBaseClass *base_class = GST_VENC_BASE_GET_CLASS(self);
    g_return_val_if_fail(base_class != nullptr && base_class->create_codec != nullptr, FALSE);
    self->encoder = base_class->create_codec(reinterpret_cast<GstElementClass*>(base_class));
    g_return_val_if_fail(self->encoder != nullptr, FALSE);
    return TRUE;
}

static gboolean gst_venc_base_is_flushing(GstVencBase *self)
{
    g_return_val_if_fail(self != nullptr, FALSE);
    GST_OBJECT_LOCK(self);
    gboolean flushing = self->flushing;
    GST_OBJECT_UNLOCK(self);
    GST_DEBUG_OBJECT(self, "Flushing %d", flushing);
    return flushing;
}

static void gst_venc_base_set_flushing(GstVencBase *self, const gboolean flushing)
{
    GST_DEBUG_OBJECT(self, "Set flushing %d", flushing);
    g_return_if_fail(self != nullptr);
    GST_OBJECT_LOCK(self);
    self->flushing = flushing;
    GST_OBJECT_UNLOCK(self);
}

static gboolean gst_venc_base_close(GstVideoEncoder *encoder)
{
    GST_DEBUG_OBJECT(encoder, "Close");
    g_return_val_if_fail(encoder != nullptr, FALSE);
    GstVencBase *self = GST_VENC_BASE(encoder);
    g_return_val_if_fail(self->encoder != nullptr, FALSE);
    self->encoder->Deinit();
    self->encoder = nullptr;

    return TRUE;
}

static gboolean gst_venc_base_start(GstVideoEncoder *encoder)
{
    GST_DEBUG_OBJECT(encoder, "Start");
    GstVencBase *self = GST_VENC_BASE(encoder);
    self->input.frame_cnt = 0;
    self->input.first_frame_time = 0;
    self->input.last_frame_time = 0;
    self->output.frame_cnt = 0;
    self->output.first_frame_time = 0;
    self->output.last_frame_time = 0;
    self->first_out_frame = TRUE;
    return TRUE;
}

static GstBufferPool *gst_venc_base_pool_ref(GstBufferPool *pool)
{
    g_return_val_if_fail(pool != nullptr, nullptr);
    return reinterpret_cast<GstBufferPool*>(gst_object_ref(reinterpret_cast<gpointer>(pool)));
}

static void gst_venc_base_pool_unref(GstBufferPool *pool)
{
    g_return_if_fail(pool != nullptr);
    gst_object_unref(reinterpret_cast<gpointer>(pool));
}

static gboolean gst_venc_base_stop(GstVideoEncoder *encoder)
{
    g_return_val_if_fail(encoder != nullptr, FALSE);
    GstVencBase *self = GST_VENC_BASE(encoder);
    g_return_val_if_fail(self->encoder != nullptr, FALSE);
    GST_DEBUG_OBJECT(self, "Stop encoder start");

    g_mutex_lock(&self->drain_lock);
    self->draining = FALSE;
    g_cond_broadcast(&self->drain_cond);
    g_mutex_unlock(&self->drain_lock);

    gint ret = self->encoder->Stop();
    (void)gst_codec_return_is_ok(self, ret, "Stop", TRUE);
    if (self->input_state) {
        gst_video_codec_state_unref(self->input_state);
    }
    if (self->output_state) {
        gst_video_codec_state_unref(self->output_state);
    }
    self->input_state = nullptr;
    self->output_state = nullptr;
    gst_pad_stop_task(GST_VIDEO_ENCODER_SRC_PAD(encoder));
    ret = self->encoder->FreeInputBuffers();
    (void)gst_codec_return_is_ok(self, ret, "FreeInput", TRUE);
    ret = self->encoder->FreeOutputBuffers();
    (void)gst_codec_return_is_ok(self, ret, "FreeOutput", TRUE);
    if (self->inpool) {
        GST_DEBUG_OBJECT(self, "Input buffer ref count %u", (reinterpret_cast<GObject*>(self->inpool)->ref_count));
        gst_venc_base_pool_unref(self->inpool);
        self->inpool = nullptr;
    }
    if (self->outpool) {
        GST_DEBUG_OBJECT(self, "Output buffer ref count %u", (reinterpret_cast<GObject*>(self->outpool)->ref_count));
        gst_venc_base_pool_unref(self->outpool);
        self->outpool = nullptr;
    }
    self->prepared = FALSE;
    self->encoder_start = FALSE;
    GST_DEBUG_OBJECT(self, "Stop encoder end");

    return TRUE;
}

static gboolean gst_codec_return_is_ok(const GstVencBase *encoder, gint ret,
    const char *error_name, gboolean need_report)
{
    if (ret == GST_CODEC_OK) {
        return TRUE;
    }
    if (need_report) {
        GST_ELEMENT_ERROR(encoder, STREAM, ENCODE, ("hardware encoder error!"), ("%s", error_name));
    } else {
        GST_ERROR_OBJECT(encoder, "Hardware encoder error %s", error_name);
    }
    return FALSE;
}

static gboolean gst_venc_base_flush(GstVideoEncoder *encoder)
{
    GstVencBase *self = GST_VENC_BASE(encoder);
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(self->encoder != nullptr, FALSE);
    GST_DEBUG_OBJECT(self, "Flush start");

    if (!self->flushing_stopping) {
        gst_venc_base_set_flushing(self, TRUE);
        gint ret = self->encoder->Flush(GST_CODEC_ALL);
        (void)gst_codec_return_is_ok(self, ret, "flush", FALSE);
        gst_venc_base_set_flushing(self, FALSE);
    }

    GST_DEBUG_OBJECT(self, "Flush end");

    return TRUE;
}

static gboolean gst_venc_base_allocate_surface_in_buffers(GstVencBase *self)
{
    GST_DEBUG_OBJECT(self, "Allocate input buffers start");
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(self->encoder != nullptr, FALSE);
    std::vector<GstBuffer*> buffers;
    for (guint i = 0; i < self->input.buffer_cnt; ++i) {
        buffers.push_back(nullptr);
    }
    gint ret = self->encoder->UseInputBuffers(buffers);
    buffers.clear();
    return gst_codec_return_is_ok(self, ret, "Usebuffer", TRUE);
}

static gboolean gst_venc_base_init_surface_mem(GstVencBase *self, GstQuery *query, gboolean update_pool)
{
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(self->encoder != nullptr, FALSE);
    gint ret = self->encoder->SetParameter(GST_VIDEO_SURFACE_INIT, GST_ELEMENT(self));
    g_return_val_if_fail(gst_codec_return_is_ok(self, ret, "GST_VIDEO_SURFACE_INIT", TRUE), FALSE);
    if (update_pool) {
        gst_query_set_nth_allocation_pool(query, 0, nullptr, 0, self->input.buffer_cnt, self->input.buffer_cnt);
    } else {
        gst_query_add_allocation_pool(query, nullptr, 0, self->input.buffer_cnt, self->input.buffer_cnt);
    }
    return gst_venc_base_allocate_surface_in_buffers(self);
}

static gboolean gst_venc_base_set_outstate(GstVencBase *self)
{
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(GST_VIDEO_ENCODER_SRC_PAD(self) != nullptr, FALSE);
    GstVideoEncoder *encoder = GST_VIDEO_ENCODER(self);
    GstVencBaseClass *klass = GST_VENC_BASE_GET_CLASS(self);

    GST_DEBUG_OBJECT(self, "Setting output state: format %s, width %d, height %d",
        gst_video_format_to_string(self->format), self->width, self->height);
    GstCaps *caps = klass->get_caps(self, self->input_state);
    GstVideoCodecState *out_state = gst_video_encoder_set_output_state(encoder, caps, self->input_state);
    GST_DEBUG_OBJECT(self, "Out_state ref %d", out_state->ref_count);
    out_state->info.interlace_mode = GST_VIDEO_INTERLACE_MODE_PROGRESSIVE;
    out_state->info.chroma_site = GST_VIDEO_CHROMA_SITE_NONE;
    gst_video_codec_state_unref(out_state);
    return TRUE;
}

static gboolean gst_venc_base_negotiate(GstVencBase *self)
{
    GST_DEBUG_OBJECT(self, "Negotiate");
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(GST_VIDEO_ENCODER_SRC_PAD(self) != nullptr, FALSE);
    g_return_val_if_fail(gst_venc_base_set_outstate(self), FALSE);
    g_return_val_if_fail(gst_video_encoder_negotiate(GST_VIDEO_ENCODER(self)), FALSE);
    GST_WARNING_OBJECT(self, "KPI-TRACE-VENC: negotiate end");
    return TRUE;
}

static gboolean gst_venc_base_allocate_in_buffers(GstVencBase *self)
{
    GST_DEBUG_OBJECT(self, "Allocate input buffers start");
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(self->encoder != nullptr, FALSE);
    g_return_val_if_fail(self->inpool != nullptr, FALSE);
    std::vector<GstBuffer*> buffers;
    GstBufferPool *pool = reinterpret_cast<GstBufferPool*>(gst_object_ref(self->inpool));
    ON_SCOPE_EXIT(0) { gst_object_unref(pool); };
    for (guint i = 0; i < self->input.buffer_cnt; ++i) {
        GST_DEBUG_OBJECT(self, "Input buffer index %u", i);
        GstBuffer *buffer = nullptr;
        GstFlowReturn flow_ret = gst_buffer_pool_acquire_buffer(pool, &buffer, nullptr);
        if (flow_ret != GST_FLOW_OK || buffer == nullptr) {
            GST_WARNING_OBJECT(self, "Input buffer is nullptr");
            gst_buffer_unref(buffer);
            continue;
        }
        buffers.push_back(buffer);
    }
    gint ret = self->encoder->UseInputBuffers(buffers);
    for (auto buffer : buffers) {
        gst_buffer_unref(buffer);
    }
    buffers.clear();
    return gst_codec_return_is_ok(self, ret, "Usebuffer", TRUE);
}

static gboolean gst_venc_base_update_out_port_def(GstVencBase *self)
{
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(self->encoder != nullptr, FALSE);
    gint ret = self->encoder->GetParameter(GST_VIDEO_OUTPUT_COMMON, GST_ELEMENT(self));
    GST_INFO_OBJECT(self, "output params is min buffer count %u, buffer count %u, buffer size is %u",
        self->output.min_buffer_cnt, self->output.buffer_cnt, self->output.buffer_size);
    g_return_val_if_fail(ret == GST_CODEC_OK, FALSE);
    self->output.buffer_size = self->input.buffer_size;
    ret = self->encoder->SetParameter(GST_VIDEO_OUTPUT_COMMON, GST_ELEMENT(self));
    g_return_val_if_fail(ret == GST_CODEC_OK, FALSE);
    return TRUE;
}

static gboolean gst_venc_base_allocate_out_buffers(GstVencBase *self)
{
    GST_DEBUG_OBJECT(self, "Allocate output buffers start");
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(self->encoder != nullptr, FALSE);
    g_return_val_if_fail(self->outpool != nullptr, FALSE);
    std::vector<GstBuffer*> buffers;
    self->coding_outbuf_cnt = self->output.buffer_cnt;
    GstBufferPool *pool = reinterpret_cast<GstBufferPool*>(gst_object_ref(self->outpool));
    ON_SCOPE_EXIT(0) { gst_object_unref(pool); };
    for (guint i = 0; i < self->output.buffer_cnt; ++i) {
        GST_DEBUG_OBJECT(self, "Output buffer index %u", i);
        GstBuffer *buffer = nullptr;
        GstFlowReturn flow_ret = gst_buffer_pool_acquire_buffer(pool, &buffer, nullptr);
        if (flow_ret != GST_FLOW_OK || buffer == nullptr) {
            GST_WARNING_OBJECT(self, "Output buffer is nullptr");
            gst_buffer_unref(buffer);
            continue;
        }
        buffers.push_back(buffer);
    }
    gint ret = self->encoder->UseOutputBuffers(buffers);
    for (auto buffer : buffers) {
        gst_buffer_unref(buffer);
    }
    g_return_val_if_fail(gst_codec_return_is_ok(self, ret, "usebuffer", TRUE), FALSE);
    return TRUE;
}

static gboolean gst_venc_base_prepare(GstVencBase *self)
{
    GST_DEBUG_OBJECT(self, "Prepare");
    g_return_val_if_fail(self != nullptr, FALSE);
    // Negotiate with downstream and get format
    g_return_val_if_fail(gst_venc_base_negotiate(self), FALSE);
    // To allocate output memory, we need to give the size
    g_return_val_if_fail(gst_venc_base_allocate_out_buffers(self), FALSE);

    return TRUE;
}

static void gst_venc_debug_input_time(GstVencBase *self)
{
    if (self->input.first_frame_time == 0) {
        self->input.first_frame_time = g_get_monotonic_time();
    } else {
        self->input.last_frame_time = g_get_monotonic_time();
    }
    self->input.frame_cnt++;
    gint64 time_interval = self->input.last_frame_time - self->input.first_frame_time;
    gint64 frame_cnt = self->input.frame_cnt - 1;
    if (frame_cnt > 0) {
        gint64 time_every_frame = time_interval / frame_cnt;
        GST_DEBUG_OBJECT(self, "Encoder Input Time interval %" G_GINT64_FORMAT " us, frame count %" G_GINT64_FORMAT
        " ,every frame time %" G_GINT64_FORMAT " us, frame rate %.9f", time_interval, self->input.frame_cnt,
        time_every_frame, static_cast<double>(G_TIME_SPAN_SECOND) / static_cast<double>(time_every_frame));
    }
}

static void gst_venc_debug_output_time(GstVencBase *self)
{
    if (self->output.first_frame_time == 0) {
        self->output.first_frame_time = g_get_monotonic_time();
    } else {
        self->output.last_frame_time = g_get_monotonic_time();
    }
    self->output.frame_cnt++;
    gint64 time_interval = self->output.last_frame_time - self->output.first_frame_time;
    gint64 frame_cnt = self->output.frame_cnt - 1;
    if (frame_cnt > 0) {
        gint64 time_every_frame = time_interval / frame_cnt;
        GST_DEBUG_OBJECT(self, "Encoder Output Time interval %" G_GINT64_FORMAT " us, frame count %" G_GINT64_FORMAT
        " ,every frame time %" G_GINT64_FORMAT " us, frame rate %.9f", time_interval, self->output.frame_cnt,
        time_every_frame, static_cast<double>(G_TIME_SPAN_SECOND) / static_cast<double>(time_every_frame));
    }
}

static GstStateChangeReturn gst_venc_base_change_state(GstElement *element, GstStateChange transition)
{
    g_return_val_if_fail(element != nullptr, GST_STATE_CHANGE_FAILURE);
    GstVencBase *self = GST_VENC_BASE(element);

    GST_DEBUG_OBJECT(element, "change state %d", transition);
    switch (transition) {
        case GST_STATE_CHANGE_PAUSED_TO_READY:
            GST_WARNING_OBJECT(self, "KPI-TRACE-VENC: stop start");

            GST_VIDEO_ENCODER_STREAM_LOCK(self);
            if (self->encoder != nullptr) {
                (void)self->encoder->Flush(GST_CODEC_ALL);
            }
            gst_venc_base_set_flushing(self, TRUE);
            GST_VIDEO_ENCODER_STREAM_UNLOCK(self);
            break;
        case GST_STATE_CHANGE_READY_TO_NULL:
            GST_WARNING_OBJECT(self, "KPI-TRACE-VENC: close start");
            break;
        default:
            break;
    }

    GstStateChangeReturn ret = GST_ELEMENT_CLASS(parent_class)->change_state(element, transition);

    switch (transition) {
        case GST_STATE_CHANGE_PAUSED_TO_READY:
            GST_WARNING_OBJECT(self, "KPI-TRACE-VENC: stop end");
            break;
        case GST_STATE_CHANGE_READY_TO_NULL:
            GST_WARNING_OBJECT(self, "KPI-TRACE-VENC: close end");
            break;
        default:
            break;
    }
    return ret;
}

static void gst_venc_base_handle_frame_after(GstVencBase *self, GstVideoCodecFrame *frame)
{
    g_return_if_fail(frame != nullptr);
    g_return_if_fail(self != nullptr);
    gst_buffer_unref(frame->input_buffer);
    frame->input_buffer = nullptr;
    self->last_pts = frame->pts;
    if (self->first_in_frame) {
        self->first_in_frame = FALSE;
        self->first_frame_pts = frame->pts;
    }
}

static GstFlowReturn gst_venc_base_handle_frame(GstVideoEncoder *encoder, GstVideoCodecFrame *frame)
{
    GST_DEBUG_OBJECT(encoder, "Handle frame");
    ON_SCOPE_EXIT(0) { gst_video_codec_frame_unref(frame); };
    g_return_val_if_fail(frame != nullptr, GST_FLOW_ERROR);
    GstVencBase *self = GST_VENC_BASE(encoder);
    g_return_val_if_fail(self != nullptr && self->encoder != nullptr, GST_FLOW_ERROR);
    if (self->first_in_frame) {
        GST_WARNING_OBJECT(self, "KPI-TRACE-VENC: first in frame");
    }
    if (gst_venc_base_is_flushing(self)) {
        return GST_FLOW_FLUSHING;
    }
    if (!self->prepared) {
        if (!gst_venc_base_prepare(self)) {
            GST_WARNING_OBJECT(self, "hdi video dec enable failed");
            return GST_FLOW_ERROR;
        }
        self->prepared = TRUE;
    }
    GstPad *pad = GST_VIDEO_ENCODER_SRC_PAD(self);
    if (!self->encoder_start) {
        gint ret = self->encoder->Start();
        g_return_val_if_fail(gst_codec_return_is_ok(self, ret, "start", TRUE), GST_FLOW_ERROR);
        self->encoder_start = TRUE;
        GST_WARNING_OBJECT(self, "KPI-TRACE-VENC: start end");
    }
    if (gst_pad_get_task_state(pad) != GST_TASK_STARTED &&
        gst_pad_start_task(pad, (GstTaskFunction)gst_venc_base_loop, encoder, nullptr) != TRUE) {
        return GST_FLOW_ERROR;
    }
    GST_VIDEO_ENCODER_STREAM_UNLOCK(self);
    gst_venc_debug_input_time(self);
    if (self->i_frame_interval != 0 && self->input.frame_cnt % (gint64)self->i_frame_interval == 0) {
        (void)self->encoder->SetParameter(GST_REQUEST_I_FRAME, GST_ELEMENT(self));
    }
    // buffer ref no give to encoder
    gint codec_ret = self->encoder->PushInputBuffer(frame->input_buffer);
    GST_VIDEO_ENCODER_STREAM_LOCK(self);
    GstFlowReturn ret = GST_FLOW_OK;
    switch (codec_ret) {
        case GST_CODEC_OK:
            break;
        case GST_CODEC_FLUSH:
            GST_DEBUG_OBJECT(self, "Codec flushing");
            ret = GST_FLOW_FLUSHING;
            break;
        default:
            GST_ELEMENT_WARNING(self, STREAM, ENCODE, ("Hardware encoder error!"), ("pull"));
            ret = GST_FLOW_ERROR;
    }
    gst_venc_base_handle_frame_after(self, frame);
    return ret;
}

static GstFlowReturn gst_venc_base_finish_output_buffer(GstVencBase *self, GstBuffer *buffer)
{
    GST_DEBUG_OBJECT(self, "Finish output buffer start");
    g_return_val_if_fail(self != nullptr, GST_FLOW_ERROR);
    g_return_val_if_fail(buffer != nullptr, GST_FLOW_ERROR);
    GstFlowReturn flow_ret = GST_FLOW_OK;
    gst_venc_debug_output_time(self);

    GstVideoCodecFrame *frame = gst_video_encoder_get_oldest_frame(GST_VIDEO_ENCODER(self));
    if (frame != nullptr) {
        frame->output_buffer = buffer;
        flow_ret = gst_video_encoder_finish_frame(GST_VIDEO_ENCODER(self), frame);
    } else {
        GST_DEBUG_OBJECT(self, "No frame available");
        GST_BUFFER_PTS(buffer) = self->last_pts;
        flow_ret = gst_pad_push(GST_VIDEO_ENCODER_SRC_PAD(self), buffer);
    }
    GST_DEBUG_OBJECT(self, "Finish output buffer end");
    return flow_ret;
}

static GstFlowReturn gst_venc_base_codec_eos(GstVencBase *self)
{
    GST_DEBUG_OBJECT(self, "Eos");
    g_return_val_if_fail(self != nullptr, GST_FLOW_ERROR);
    g_return_val_if_fail(GST_VIDEO_ENCODER_SRC_PAD(self) != nullptr, GST_FLOW_ERROR);
    g_mutex_lock(&self->drain_lock);
    if (self->draining) {
        GST_DEBUG_OBJECT(self, "Drained");
        self->draining = FALSE;
        g_cond_broadcast(&self->drain_cond);
    }
    g_mutex_unlock(&self->drain_lock);
    gst_venc_base_pause_loop(self);
    return GST_FLOW_OK;
}

static void gst_venc_base_pause_loop(GstVencBase *self)
{
    GST_DEBUG_OBJECT(self, "Pause loop start");
    g_return_if_fail(self != nullptr);
    g_mutex_lock(&self->drain_lock);
    if (self->draining) {
        self->draining = FALSE;
        g_cond_broadcast(&self->drain_cond);
    }
    g_mutex_unlock(&self->drain_lock);
    gst_pad_pause_task(GST_VIDEO_ENCODER_SRC_PAD(self));
    GST_DEBUG_OBJECT(self, "pause loop end");
}

static gboolean gst_venc_base_push_out_buffers(GstVencBase *self)
{
    GstBuffer *buffer = nullptr;
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(self->encoder != nullptr, FALSE);
    GstBufferPool *pool = gst_venc_base_pool_ref(self->outpool);
    ON_SCOPE_EXIT(0) { gst_venc_base_pool_unref(pool); };
    GstFlowReturn flow = GST_FLOW_OK;
    gint codec_ret = GST_CODEC_OK;
    GstBufferPoolAcquireParams params;
    g_return_val_if_fail(memset_s(&params, sizeof(params), 0, sizeof(params)) == EOK, FALSE);
    if (self->coding_outbuf_cnt != 0) {
        params.flags = GST_BUFFER_POOL_ACQUIRE_FLAG_DONTWAIT;
    }
    while (flow == GST_FLOW_OK) {
        flow = gst_buffer_pool_acquire_buffer(pool, &buffer, &params);
        if (flow == GST_FLOW_OK) {
            g_return_val_if_fail(buffer != nullptr, FALSE);
            // buffer ref give to encoder
            codec_ret = self->encoder->PushOutputBuffer(buffer);
            g_return_val_if_fail(gst_codec_return_is_ok(self, codec_ret, "push buffer", TRUE), FALSE);
            self->coding_outbuf_cnt++;
            params.flags = GST_BUFFER_POOL_ACQUIRE_FLAG_DONTWAIT;
        }
    }
    g_return_val_if_fail(flow == GST_FLOW_EOS, FALSE);
    return TRUE;
}

static void gst_venc_base_loop(GstVencBase *self)
{
    g_return_if_fail(self != nullptr);
    g_return_if_fail(self->encoder != nullptr);
    GstBuffer *gst_buffer = nullptr;
    if (gst_venc_base_push_out_buffers(self) != TRUE) {
        gst_venc_base_pause_loop(self);
        return;
    }
    GST_DEBUG_OBJECT(self, "coding buffers %u", self->coding_outbuf_cnt);
    gint codec_ret = self->encoder->PullOutputBuffer(&gst_buffer);
    gint flow_ret = GST_FLOW_OK;
    switch (codec_ret) {
        case GST_CODEC_OK:
            self->coding_outbuf_cnt--;
            flow_ret = gst_venc_base_finish_output_buffer(self, gst_buffer);
            break;
        case GST_CODEC_EOS:
            self->coding_outbuf_cnt--;
            flow_ret = gst_venc_base_codec_eos(self);
            break;
        case GST_CODEC_FLUSH:
            flow_ret = GST_FLOW_FLUSHING;
            break;
        case GST_CODEC_ERROR:
            GST_ELEMENT_WARNING(self, STREAM, ENCODE, ("Hardware encoder error!"), ("pull"));
            flow_ret = GST_FLOW_ERROR;
            break;
        default:
            GST_ERROR_OBJECT(self, "Unknown error");
            flow_ret = GST_FLOW_ERROR;
    }
    GST_DEBUG_OBJECT(self, "Flow_ret %d", flow_ret);
    switch (flow_ret) {
        case GST_FLOW_OK:
            return;
        case GST_FLOW_FLUSHING:
            GST_DEBUG_OBJECT(self, "Flushing");
            break;
        case GST_FLOW_EOS:
            GST_DEBUG_OBJECT(self, "Eos");
            gst_pad_push_event(GST_VIDEO_ENCODER_SRC_PAD(self), gst_event_new_eos());
            break;
        default:
            gst_pad_push_event(GST_VIDEO_ENCODER_SRC_PAD(self), gst_event_new_eos());
    }
    gst_venc_base_pause_loop(self);
}

static gboolean gst_venc_base_set_format(GstVideoEncoder *encoder, GstVideoCodecState *state)
{
    GstVencBase *self = GST_VENC_BASE(encoder);
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(state != nullptr, FALSE);
    g_return_val_if_fail(self->encoder != nullptr, FALSE);
    GstVideoInfo *info = &state->info;
    GstCaps *caps = state->caps;
    g_return_val_if_fail(info != nullptr, FALSE);
    gboolean is_format_change = FALSE;
    gint ret = self->encoder->GetParameter(GST_VIDEO_INPUT_COMMON, GST_ELEMENT(self));
    g_return_val_if_fail(ret == GST_CODEC_OK, FALSE);
    GST_INFO_OBJECT(self, "input params is min buffer count %u, buffer count %u, buffer size is %u",
        self->input.min_buffer_cnt, self->input.buffer_cnt, self->input.buffer_size);

    GST_DEBUG_OBJECT(self, "Setting new caps");

    GstStructure *structure = gst_caps_get_structure(caps, 0);
    const gchar *format_str = gst_structure_get_string(structure, "format");
    GST_DEBUG_OBJECT(self, "gst_venc_base_set_format format=%s", format_str);
    self->format = gst_video_format_from_string(format_str);

    is_format_change = is_format_change || self->width != info->width;
    is_format_change = is_format_change || self->height != GST_VIDEO_INFO_FIELD_HEIGHT(info);
    is_format_change = is_format_change || (self->frame_rate != info->fps_n && info->fps_n != 0);

    if (is_format_change) {
        self->width = info->width;
        self->height = GST_VIDEO_INFO_FIELD_HEIGHT(info);
        self->frame_rate  = info->fps_n;
        self->nslice_height = self->height;
        self->nstride = info->stride[0];
        self->input.buffer_size = info->size;
        GST_DEBUG_OBJECT(self, "width %d height %d frame_rate %d nslice_height %d nstride %d buffer_size %u",
            self->width, self->height, self->frame_rate, self->nslice_height, self->nstride, self->input.buffer_size);
    }

    GST_DEBUG_OBJECT(self, "Setting inport definition");
    ret = self->encoder->SetParameter(GST_VIDEO_INPUT_COMMON, GST_ELEMENT(self));
    g_return_val_if_fail(ret == GST_CODEC_OK, FALSE);
    ret = self->encoder->SetParameter(GST_VIDEO_FORMAT, GST_ELEMENT(self));
    g_return_val_if_fail(ret == GST_CODEC_OK, FALSE);
    ret = self->encoder->SetParameter(GST_VIDEO_ENCODER_CONFIG, GST_ELEMENT(self));
    g_return_val_if_fail(ret == GST_CODEC_OK, FALSE);
    if (self->input_state != nullptr) {
        gst_video_codec_state_unref(self->input_state);
    }
    self->input_state = gst_video_codec_state_ref(state);
    return gst_codec_return_is_ok(self, ret, "setparam", TRUE);
}

static GstFlowReturn gst_venc_base_finish(GstVideoEncoder *encoder)
{
    GstVencBase *self = GST_VENC_BASE(encoder);
    g_return_val_if_fail(self != nullptr, GST_FLOW_ERROR);
    g_return_val_if_fail(self->encoder != nullptr, GST_FLOW_ERROR);
    GST_DEBUG_OBJECT(self, "Finish codec start");
    GstPad *pad = GST_VIDEO_ENCODER_SRC_PAD(self);
    if (gst_pad_get_task_state(pad) != GST_TASK_STARTED) {
        GST_DEBUG_OBJECT(self, "venc not start yet");
        return GST_FLOW_OK;
    }
    GST_VIDEO_ENCODER_STREAM_UNLOCK(self);
    g_mutex_lock(&self->drain_lock);
    self->draining = TRUE;
    gint ret = self->encoder->PushInputBuffer(nullptr);
    if (ret != GST_CODEC_OK) {
        GST_ERROR_OBJECT(self, "Failed to push input buffer for draining: %d", ret);
        g_mutex_unlock(&self->drain_lock);
        GST_VIDEO_ENCODER_STREAM_LOCK(self);
        return GST_FLOW_ERROR;
    }
    GST_DEBUG_OBJECT(self, "Waiting until codec is drained");
    gint64 wait_until = g_get_monotonic_time() + G_TIME_SPAN_SECOND;
    if (!g_cond_wait_until(&self->drain_cond, &self->drain_lock, wait_until)) {
        GST_ERROR_OBJECT(self, "Drain timed out");
    } else {
        GST_DEBUG_OBJECT(self, "Finish hdi end");
    }
    g_mutex_unlock(&self->drain_lock);
    GST_VIDEO_ENCODER_STREAM_LOCK(self);

    return GST_FLOW_OK;
}

static gboolean gst_venc_base_event(GstVideoEncoder *encoder, GstEvent *event)
{
    g_return_val_if_fail(encoder != nullptr, FALSE);
    g_return_val_if_fail(event != nullptr, FALSE);
    g_return_val_if_fail(GST_VIDEO_ENCODER_CLASS(parent_class) != nullptr, FALSE);
    GstVencBase *self = GST_VENC_BASE(encoder);
    GST_DEBUG_OBJECT(self, "gst_venc_base_sink_event, type=%s", GST_EVENT_TYPE_NAME(event));

    gboolean ret = TRUE;
    switch (GST_EVENT_TYPE(event)) {
        case GST_EVENT_FLUSH_START:
            GST_WARNING_OBJECT(self, "KPI-TRACE-VENC: flush start");
            if (self->encoder != nullptr) {
                (void)self->encoder->Flush(GST_CODEC_ALL);
            }
            gst_venc_base_set_flushing(self, TRUE);
            self->encoder_start = FALSE;
            break;
        case GST_EVENT_FLUSH_STOP:
            self->flushing_stopping = TRUE;
            self->encoder_start = FALSE;
            ret = GST_VIDEO_ENCODER_CLASS(parent_class)->sink_event(encoder, event);
            gst_venc_base_set_flushing(self, FALSE);
            self->flushing_stopping = FALSE;
            GST_WARNING_OBJECT(self, "KPI-TRACE-VENC: flush stop");
            return ret;
        default:
            break;
    }

    ret = GST_VIDEO_ENCODER_CLASS(parent_class)->sink_event(encoder, event);
    return ret;
}

static GstBufferPool *gst_venc_base_new_shmem_pool(GstVencBase *self, GstCaps *caps, guint size,
    guint buffer_cnt, gboolean is_input)
{
    GST_DEBUG_OBJECT(self, "New pool");
    g_return_val_if_fail(self != nullptr, nullptr);
    g_return_val_if_fail(caps != nullptr, nullptr);

    GstShMemPool *pool = gst_shmem_pool_new();
    GstAllocationParams allocParams;
    gst_allocation_params_init(&allocParams);
    g_return_val_if_fail(pool != nullptr, nullptr);
    ON_SCOPE_EXIT(0) { gst_object_unref(pool); };
    std::shared_ptr<OHOS::Media::AVSharedMemoryPool> av_shmem_pool =
                std::make_shared<OHOS::Media::AVSharedMemoryPool>("venc");
    GstShMemAllocator *allocator = gst_shmem_allocator_new();
    g_return_val_if_fail(allocator != nullptr, nullptr);
    ON_SCOPE_EXIT(1) { gst_object_unref(allocator); };
    (void)gst_shmem_pool_set_avshmempool(pool, av_shmem_pool);
    (void)gst_shmem_allocator_set_pool(allocator, av_shmem_pool);
    GstStructure *config = gst_buffer_pool_get_config(GST_BUFFER_POOL_CAST(pool));
    g_return_val_if_fail(config != nullptr, nullptr);
    gst_buffer_pool_config_set_allocator(config, GST_ALLOCATOR_CAST(allocator), &allocParams);
    gst_buffer_pool_config_set_params(config, caps, size, buffer_cnt, buffer_cnt);
    g_return_val_if_fail(gst_buffer_pool_set_config(GST_BUFFER_POOL_CAST(pool), config), nullptr);
    CANCEL_SCOPE_EXIT_GUARD(0);
    CANCEL_SCOPE_EXIT_GUARD(1);
    if (is_input) {
        self->input.av_shmem_pool = av_shmem_pool;
        if (self->input.allocator != nullptr) {
            gst_object_unref(self->input.allocator);
        }
        self->input.allocator = allocator;
    } else {
        self->output.av_shmem_pool = av_shmem_pool;
        if (self->output.allocator != nullptr) {
            gst_object_unref(self->output.allocator);
        }
        self->output.allocator = allocator;
    }
    return GST_BUFFER_POOL(pool);
}

static void gst_venc_base_update_pool(GstVencBase *self, GstBufferPool **pool, GstCaps *caps, gint size, guint buf_cnt)
{
    GST_DEBUG_OBJECT(self, "Update pool");
    g_return_if_fail(*pool != nullptr);
    ON_SCOPE_EXIT(0) { gst_object_unref(*pool); *pool = nullptr; };
    GstStructure *config = gst_buffer_pool_get_config(*pool);
    g_return_if_fail(config != nullptr);
    gst_buffer_pool_config_set_params(config, caps, size, buf_cnt, buf_cnt);
    if (self->memtype == GST_MEMTYPE_SURFACE) {
        gst_structure_set(config, "usage", G_TYPE_UINT64, self->usage, nullptr);
    }
    g_return_if_fail(gst_buffer_pool_set_config(*pool, config));
    CANCEL_SCOPE_EXIT_GUARD(0);
}

static gboolean gst_venc_base_decide_allocation(GstVideoEncoder *encoder, GstQuery *query)
{
    GST_DEBUG_OBJECT(encoder, "Decide allocation");
    g_return_val_if_fail(encoder != nullptr, FALSE);
    GstCaps *outcaps = nullptr;
    GstBufferPool *pool = nullptr;
    guint size = 0;
    guint min_buf = 0;
    guint max_buf = 0;
    GstVencBase *self = GST_VENC_BASE(encoder);
    gst_query_parse_allocation(query, &outcaps, nullptr);

    GstAllocationParams params;
    guint index = 0;
    gst_allocation_params_init(&params);
    guint pool_num = gst_query_get_n_allocation_pools(query);
    if (pool_num > 0) {
        gst_query_parse_nth_allocation_pool(query, 0, &pool, &size, &min_buf, &max_buf);
        GST_DEBUG_OBJECT(encoder, "Get bufferpool num %u", pool_num);
        if (!gst_query_find_allocation_meta(query, GST_BUFFER_TYPE_META_API_TYPE, &index)) {
            GST_INFO_OBJECT(encoder, "no meta api");
            gst_object_unref(pool);
            pool = nullptr;
        }
    } else {
        pool = nullptr;
    }

    g_return_val_if_fail(gst_venc_base_update_out_port_def(self), FALSE);
    if (pool == nullptr) {
        pool = gst_venc_base_new_shmem_pool(self, outcaps, self->output.buffer_size, self->output.buffer_cnt, FALSE);
    } else {
        gst_venc_base_update_pool(self, &pool, outcaps, self->output.buffer_size, self->output.buffer_cnt);
    }
    g_return_val_if_fail(pool != nullptr, FALSE);
    GST_DEBUG_OBJECT(encoder, "Pool ref %d", (reinterpret_cast<GObject*>(pool)->ref_count));
    if (self->outpool) {
        GST_DEBUG_OBJECT(encoder, "change buffer pool");
        gst_object_unref(self->outpool);
    }
    self->outpool = pool;
    gst_buffer_pool_set_active(pool, TRUE);
    return TRUE;
}

static gboolean gst_venc_base_propose_allocation(GstVideoEncoder *encoder, GstQuery *query)
{
    GST_DEBUG_OBJECT(encoder, "Propose allocation");
    g_return_val_if_fail(encoder != nullptr, FALSE);
    g_return_val_if_fail(query != nullptr, FALSE);
    GstVencBase *self = GST_VENC_BASE(encoder);
    GstCaps *incaps = nullptr;
    GstVideoInfo vinfo;
    GstBufferPool *pool = nullptr;
    guint size = 0;
    gboolean update_pool = FALSE;
    guint pool_num = gst_query_get_n_allocation_pools(query);
    if (pool_num > 0) {
        update_pool = TRUE;
    }
    gst_query_add_allocation_meta(query, GST_VIDEO_META_API_TYPE, nullptr);
    gst_query_add_allocation_meta(query, GST_BUFFER_TYPE_META_API_TYPE, nullptr);
    if (self->memtype == GST_MEMTYPE_SURFACE) {
        GST_INFO_OBJECT(encoder, "It is surface mem");
        return gst_venc_base_init_surface_mem(self, query, update_pool);
    }
    gst_video_info_init(&vinfo);
    gst_query_parse_allocation(query, &incaps, nullptr);
    if (incaps != nullptr) {
        gst_video_info_from_caps(&vinfo, incaps);
    }
    size = vinfo.size;
    pool = gst_venc_base_new_shmem_pool(self, incaps, size, self->input.buffer_cnt, TRUE);
    g_return_val_if_fail(pool != nullptr, FALSE);
    if (update_pool) {
        gst_query_set_nth_allocation_pool(query, 0, pool, size, self->input.buffer_cnt, self->input.buffer_cnt);
    } else {
        gst_query_add_allocation_pool(query, pool, size, self->input.buffer_cnt, self->input.buffer_cnt);
    }
    GST_DEBUG_OBJECT(encoder, "Pool ref %d", (reinterpret_cast<GObject*>(pool)->ref_count));
    if (self->inpool) {
        GST_INFO_OBJECT(encoder, "Change buffer pool");
        gst_object_unref(self->inpool);
    }
    self->inpool = pool;
    gst_buffer_pool_set_active(pool, TRUE);
    g_return_val_if_fail(gst_venc_base_allocate_in_buffers(self), FALSE);
    return TRUE;
}