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

#include "gst_vdec_base.h"
#include <vector>
#include <iostream>
#include "securec.h"
#include "buffer_type_meta.h"
#include "scope_guard.h"
#include "gst_codec_video_common.h"
#include "param_wrapper.h"
#include "media_dfx.h"
#include "media_log.h"

using namespace OHOS;
using namespace OHOS::Media;
GST_DEBUG_CATEGORY_STATIC(gst_vdec_base_debug_category);
#define GST_CAT_DEFAULT gst_vdec_base_debug_category
#define gst_vdec_base_parent_class parent_class
#define GST_VDEC_BASE_SUPPORTED_FORMATS "{ NV12, NV21 }"
#define DEFAULT_MAX_QUEUE_SIZE 30
#define DEFAULT_WIDTH 1920
#define DEFAULT_HEIGHT 1080
#define DEFAULT_SEEK_FRAME_RATE 1000
#define BLOCKING_ACQUIRE_BUFFER_THRESHOLD 5
#define DRAIN_TIME_OUT_MIN (G_TIME_SPAN_SECOND * 2)
#define DRAIN_TIME_OUT_MAX (G_TIME_SPAN_SECOND * 5)
#define DRAIN_TIME_OUT_BY_FRAMERATE(rate) (static_cast<gint64>(G_TIME_SPAN_SECOND * 30 / (rate)))

static void gst_vdec_base_class_install_property(GObjectClass *gobject_class);
static void gst_vdec_base_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static gboolean gst_vdec_base_open(GstVideoDecoder *decoder);
static gboolean gst_vdec_base_close(GstVideoDecoder *decoder);
static gboolean gst_vdec_base_start(GstVideoDecoder *decoder);
static gboolean gst_vdec_base_stop(GstVideoDecoder *decoder);
static gboolean gst_vdec_base_set_format(GstVideoDecoder *decoder, GstVideoCodecState *state);
static gboolean gst_vdec_base_flush(GstVideoDecoder *decoder);
static GstFlowReturn gst_vdec_base_handle_frame(GstVideoDecoder *decoder, GstVideoCodecFrame *frame);
static GstFlowReturn gst_vdec_base_drain(GstVideoDecoder *decoder);
static void gst_vdec_base_finalize(GObject *object);
static GstFlowReturn gst_vdec_base_finish(GstVideoDecoder *decoder);
static void gst_vdec_base_loop(GstVdecBase *self);
static void gst_vdec_base_pause_loop(GstVdecBase *self);
static gboolean gst_vdec_base_event(GstVideoDecoder *decoder, GstEvent *event);
static gboolean gst_vdec_base_decide_allocation(GstVideoDecoder *decoder, GstQuery *query);
static gboolean gst_vdec_base_propose_allocation(GstVideoDecoder *decoder, GstQuery *query);
static gboolean gst_vdec_base_check_allocate_input(GstVdecBase *self);
static gboolean gst_codec_return_is_ok(const GstVdecBase *decoder, gint ret,
    const char *error_name, gboolean need_report);
static GstStateChangeReturn gst_vdec_base_change_state(GstElement *element, GstStateChange transition);
static gboolean gst_vdec_base_update_out_port_def(GstVdecBase *self, guint *size);
static void gst_vdec_base_update_out_pool(GstVdecBase *self, GstBufferPool **pool, GstCaps *outcaps, gint size);
static void gst_vdec_base_post_resolution_changed_message(GstVdecBase *self);

enum {
    PROP_0,
    PROP_VENDOR,
    PROP_SURFACE_POOL,
    PROP_SINK_CAPS,
    PROP_PERFORMANCE_MODE,
    PROP_ENABLE_SLICE_CAT,
    PROP_SEEK,
    PROP_PLAYER_MODE,
};

enum {
    SIGNAL_CAPS_FIX_ERROR,
    SIGNAL_LAST
};

static guint signals[SIGNAL_LAST] = { 0, };

G_DEFINE_ABSTRACT_TYPE(GstVdecBase, gst_vdec_base, GST_TYPE_VIDEO_DECODER);

static void gst_vdec_base_class_init(GstVdecBaseClass *klass)
{
    GST_DEBUG_OBJECT(klass, "Class init");
    g_return_if_fail(klass != nullptr);
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);
    GstVideoDecoderClass *video_decoder_class = GST_VIDEO_DECODER_CLASS(klass);
    GST_DEBUG_CATEGORY_INIT (gst_vdec_base_debug_category, "vdecbase", 0, "video decoder base class");
    gobject_class->set_property = gst_vdec_base_set_property;
    gobject_class->finalize = gst_vdec_base_finalize;
    video_decoder_class->open = gst_vdec_base_open;
    video_decoder_class->close = gst_vdec_base_close;
    video_decoder_class->start = gst_vdec_base_start;
    video_decoder_class->stop = gst_vdec_base_stop;
    video_decoder_class->flush = gst_vdec_base_flush;
    video_decoder_class->set_format = gst_vdec_base_set_format;
    video_decoder_class->handle_frame = gst_vdec_base_handle_frame;
    video_decoder_class->finish = gst_vdec_base_finish;
    video_decoder_class->sink_event = gst_vdec_base_event;
    video_decoder_class->drain = gst_vdec_base_drain;
    video_decoder_class->decide_allocation = gst_vdec_base_decide_allocation;
    video_decoder_class->propose_allocation = gst_vdec_base_propose_allocation;
    element_class->change_state = gst_vdec_base_change_state;

    gst_vdec_base_class_install_property(gobject_class);

    signals[SIGNAL_CAPS_FIX_ERROR] =
        g_signal_new ("caps-fix-error", G_TYPE_FROM_CLASS (klass),
        G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL, G_TYPE_NONE, 0, G_TYPE_NONE);

    const gchar *src_caps_string = GST_VIDEO_CAPS_MAKE(GST_VDEC_BASE_SUPPORTED_FORMATS);
    GST_DEBUG_OBJECT(klass, "Pad template caps %s", src_caps_string);

    GstCaps *src_caps = gst_caps_from_string(src_caps_string);
    if (src_caps != nullptr) {
        GstPadTemplate *src_templ = gst_pad_template_new("src", GST_PAD_SRC, GST_PAD_ALWAYS, src_caps);
        gst_element_class_add_pad_template(element_class, src_templ);
        gst_caps_unref(src_caps);
    }
}

static void gst_vdec_base_class_install_property(GObjectClass *gobject_class)
{
    g_object_class_install_property(gobject_class, PROP_VENDOR,
        g_param_spec_pointer("vendor", "Vendor property", "Vendor property",
            (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_SURFACE_POOL,
        g_param_spec_pointer("surface-pool", "Surface pool", "Surface pool",
            (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_SINK_CAPS,
        g_param_spec_boxed("sink-caps", "Sink Caps",
            "The caps use with surface pool", GST_TYPE_CAPS,
            (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_PERFORMANCE_MODE,
        g_param_spec_boolean("performance-mode", "performance mode", "performance mode",
            FALSE, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_ENABLE_SLICE_CAT,
        g_param_spec_boolean("enable-slice-cat", "enable slice cat", "enable slice cat",
            FALSE, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_PLAYER_MODE,
        g_param_spec_boolean("player-mode", "player mode", "player mode",
            FALSE, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_SEEK,
        g_param_spec_boolean("seeking", "Seeking", "Whether the decoder is in seek",
            FALSE, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));
}

static void gst_vdec_base_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    (void)pspec;
    g_return_if_fail(object != nullptr && value != nullptr);
    GstVdecBase *self = GST_VDEC_BASE(object);
    gint ret = GST_CODEC_OK;

    switch (prop_id) {
        case PROP_VENDOR:
            GST_INFO_OBJECT(object, "Set vendor property");
            break;
        case PROP_SURFACE_POOL:
            if (self->outpool != nullptr) {
                gst_object_unref(self->outpool);
            }
            self->outpool = static_cast<GstBufferPool *>(g_value_get_pointer(value));
            gst_object_ref(self->outpool);
            break;
        case PROP_SINK_CAPS: {
            const GstCaps *caps = gst_value_get_caps(value);
            if (self->sink_caps != nullptr) {
                gst_caps_unref(self->sink_caps);
            }
            self->sink_caps = gst_caps_copy(caps);
            break;
        }
        case PROP_PERFORMANCE_MODE:
            self->performance_mode = g_value_get_boolean(value);
            break;
        case PROP_ENABLE_SLICE_CAT:
            self->enable_slice_cat = g_value_get_boolean(value);
            break;
        case PROP_SEEK:
            GST_OBJECT_LOCK(self);
            if (self->decoder != nullptr) {
                self->seek_frame_rate = g_value_get_boolean(value) ? DEFAULT_SEEK_FRAME_RATE : self->frame_rate;
                ret = self->decoder->SetParameter(GST_DYNAMIC_FRAME_RATE, GST_ELEMENT(self));
            }
            GST_OBJECT_UNLOCK(self);
            g_return_if_fail(ret == GST_CODEC_OK);
            break;
        case PROP_PLAYER_MODE:
            self->player_mode = g_value_get_boolean(value);
            break;
        default:
            break;
    }
}

static void gst_vdec_base_check_input_need_copy(GstVdecBase *self)
{
    GstVdecBaseClass *base_class = GST_VDEC_BASE_GET_CLASS(self);
    g_return_if_fail(base_class != nullptr);
    if (base_class->input_need_copy != nullptr) {
        self->input_need_ashmem = base_class->input_need_copy();
    }
}

static void gst_vdec_base_property_init(GstVdecBase *self)
{
    g_mutex_init(&self->lock);

    g_mutex_init(&self->drain_lock);
    g_cond_init(&self->drain_cond);
    self->draining = FALSE;
    self->flushing = FALSE;
    self->prepared = FALSE;
    self->width = DEFAULT_WIDTH;
    self->height = DEFAULT_HEIGHT;
    self->frame_rate = 0;
    self->seek_frame_rate = 0;
    self->memtype = GST_MEMTYPE_INVALID;
    (void)memset_s(&self->input, sizeof(GstVdecBasePort), 0, sizeof(GstVdecBasePort));
    (void)memset_s(&self->output, sizeof(GstVdecBasePort), 0, sizeof(GstVdecBasePort));
    self->coding_outbuf_cnt = 0;
    self->input_state = nullptr;
    self->output_state = nullptr;
    self->last_pts = GST_CLOCK_TIME_NONE;
    self->flushing_stoping = FALSE;
    self->decoder_start = FALSE;
    self->stride = 0;
    self->stride_height = 0;
    self->real_stride = 0;
    self->real_stride_height = 0;
    (void)memset_s(&self->rect, sizeof(DisplayRect), 0, sizeof(DisplayRect));
    self->inpool = nullptr;
    self->outpool = nullptr;
    self->out_buffer_max_cnt = DEFAULT_MAX_QUEUE_SIZE;
    self->pre_init_pool = FALSE;
    self->performance_mode = FALSE;
    self->enable_slice_cat = FALSE;
    self->resolution_changed = FALSE;
    self->input_need_ashmem = FALSE;
    self->has_set_format = FALSE;
    self->player_mode = FALSE;
}

static void gst_vdec_base_init(GstVdecBase *self)
{
    GST_DEBUG_OBJECT(self, "Init, id = %d", static_cast<int32_t>(FAKE_POINTER(self)));
    g_return_if_fail(self != nullptr && GST_VIDEO_DECODER_SINK_PAD(self) != nullptr);
    // The upstreamer must after parser.
    gst_video_decoder_set_packetized(GST_VIDEO_DECODER(self), TRUE);
    // Use accept caps from default.
    gst_video_decoder_set_use_default_pad_acceptcaps(GST_VIDEO_DECODER_CAST (self), TRUE);
    GST_PAD_SET_ACCEPT_TEMPLATE(GST_VIDEO_DECODER_SINK_PAD(self));

    gst_vdec_base_property_init(self);
    self->input.allocator = gst_shmem_allocator_new();
    self->output.allocator = gst_shmem_allocator_new();
    gst_allocation_params_init(&self->input.allocParams);
    gst_allocation_params_init(&self->output.allocParams);
    self->input.frame_cnt = 0;
    self->input.first_frame_time = 0;
    self->input.last_frame_time = 0;
    self->input.enable_dump = FALSE;
    self->input.dump_file = nullptr;
    self->output.frame_cnt = 0;
    self->output.first_frame_time = 0;
    self->output.last_frame_time = 0;
    self->output.enable_dump = FALSE;
    self->output.dump_file = nullptr;
    self->input.first_frame = TRUE;
    self->output.first_frame = TRUE;
}

static void gst_vdec_base_finalize(GObject *object)
{
    GST_DEBUG_OBJECT(object, "Finalize");
    g_return_if_fail(object != nullptr);
    GstVdecBase *self = GST_VDEC_BASE(object);
    g_mutex_clear(&self->drain_lock);
    g_cond_clear(&self->drain_cond);
    g_mutex_clear(&self->lock);
    if (self->input.allocator) {
        gst_object_unref(self->input.allocator);
        self->input.allocator = nullptr;
    }
    if (self->output.allocator) {
        gst_object_unref(self->output.allocator);
        self->output.allocator = nullptr;
    }
    if (self->inpool) {
        gst_object_unref(self->inpool);
        self->inpool = nullptr;
    }
    if (self->outpool) {
        gst_buffer_pool_set_active(self->outpool, FALSE);
        gst_object_unref(self->outpool);
        self->outpool = nullptr;
    }
    if (self->sink_caps) {
        gst_caps_unref(self->sink_caps);
        self->sink_caps = nullptr;
    }

    std::list<GstClockTime> tempList;
    tempList.swap(self->pts_list);
    std::vector<GstVideoFormat> tempVec;
    tempVec.swap(self->formats);

    self->input.av_shmem_pool = nullptr;
    self->output.av_shmem_pool = nullptr;
    self->decoder = nullptr;
    G_OBJECT_CLASS(parent_class)->finalize(object);
}

static gboolean gst_vdec_base_open(GstVideoDecoder *decoder)
{
    GST_DEBUG_OBJECT(decoder, "Open");
    g_return_val_if_fail(decoder != nullptr, FALSE);
    GstVdecBase *self = GST_VDEC_BASE(decoder);
    GstVdecBaseClass *base_class = GST_VDEC_BASE_GET_CLASS(self);
    g_return_val_if_fail(base_class != nullptr && base_class->create_codec != nullptr, FALSE);
    self->decoder = base_class->create_codec(reinterpret_cast<GstElementClass*>(base_class));
    g_return_val_if_fail(self->decoder != nullptr, FALSE);
    gst_vdec_base_check_input_need_copy(self);
    return TRUE;
}

static gboolean gst_vdec_base_is_flushing(GstVdecBase *self)
{
    g_return_val_if_fail(self != nullptr, FALSE);
    GST_OBJECT_LOCK(self);
    gboolean flushing = self->flushing;
    GST_OBJECT_UNLOCK(self);
    GST_DEBUG_OBJECT(self, "Flushing %d", flushing);
    return flushing;
}

static void gst_vdec_base_set_flushing(GstVdecBase *self, const gboolean flushing)
{
    GST_DEBUG_OBJECT(self, "Set flushing %d", flushing);
    g_return_if_fail(self != nullptr);
    GST_OBJECT_LOCK(self);
    self->flushing = flushing;
    GST_OBJECT_UNLOCK(self);
}

static GstStateChangeReturn gst_vdec_base_change_state(GstElement *element, GstStateChange transition)
{
    g_return_val_if_fail(element != nullptr, GST_STATE_CHANGE_FAILURE);
    GstVdecBase *self = GST_VDEC_BASE(element);

    GST_DEBUG_OBJECT(element, "change state %d", transition);
    switch (transition) {
        case GST_STATE_CHANGE_PAUSED_TO_READY:
            GST_WARNING_OBJECT(self, "KPI-TRACE-VDEC: stop start");
            gst_buffer_pool_set_active(self->outpool, FALSE);
            GST_VIDEO_DECODER_STREAM_LOCK(self);
            if (self->decoder != nullptr) {
                (void)self->decoder->Flush(GST_CODEC_ALL);
            }
            gst_vdec_base_set_flushing(self, TRUE);

            GST_VIDEO_DECODER_STREAM_UNLOCK(self);
            break;
        case GST_STATE_CHANGE_READY_TO_NULL:
            GST_WARNING_OBJECT(self, "KPI-TRACE-VDEC: close start");
            break;
        default:
            break;
    }

    GstStateChangeReturn ret = GST_ELEMENT_CLASS(parent_class)->change_state(element, transition);

    switch (transition) {
        case GST_STATE_CHANGE_PAUSED_TO_READY:
            GST_WARNING_OBJECT(self, "KPI-TRACE-VDEC: stop end");
            break;
        case GST_STATE_CHANGE_READY_TO_NULL:
            GST_WARNING_OBJECT(self, "KPI-TRACE-VDEC: close end");
            break;
        default:
            break;
    }
    return ret;
}

static gboolean gst_vdec_base_close(GstVideoDecoder *decoder)
{
    GST_DEBUG_OBJECT(decoder, "Close");
    g_return_val_if_fail(decoder != nullptr, FALSE);
    GstVdecBase *self = GST_VDEC_BASE(decoder);
    g_return_val_if_fail(self->decoder != nullptr, FALSE);
    self->decoder->Deinit();
    self->decoder = nullptr;
    return TRUE;
}

void gst_vdec_base_dump_from_sys_param(GstVdecBase *self)
{
    std::string dump_vdec;
    self->input.enable_dump = FALSE;
    self->output.enable_dump = FALSE;
    int32_t res = OHOS::system::GetStringParameter("sys.media.dump.codec.vdec", dump_vdec, "");
    if (res != 0 || dump_vdec.empty()) {
        GST_DEBUG_OBJECT(self, "sys.media.dump.codec.vdec");
        return;
    }
    GST_DEBUG_OBJECT(self, "sys.media.dump.codec.vdec=%s", dump_vdec.c_str());

    if (dump_vdec == "INPUT" || dump_vdec == "ALL") {
        self->input.enable_dump = TRUE;
    }
    if (dump_vdec == "OUTPUT" || dump_vdec == "ALL") {
        self->output.enable_dump = TRUE;
    }
}

static gboolean gst_vdec_base_start(GstVideoDecoder *decoder)
{
    GST_DEBUG_OBJECT(decoder, "Start");
    GstVdecBase *self = GST_VDEC_BASE(decoder);
    g_return_val_if_fail(self != nullptr, FALSE);
    self->input.frame_cnt = 0;
    self->input.first_frame_time = 0;
    self->input.last_frame_time = 0;
    self->output.frame_cnt = 0;
    self->output.first_frame_time = 0;
    self->output.last_frame_time = 0;
    std::list<GstClockTime> empty;
    self->pts_list.swap(empty);
    self->last_pts = GST_CLOCK_TIME_NONE;
    gst_vdec_base_dump_from_sys_param(self);
    return TRUE;
}

static void gst_vdec_base_stop_after(GstVdecBase *self)
{
    g_return_if_fail(self != nullptr);
    self->prepared = FALSE;
    self->input.first_frame = TRUE;
    self->output.first_frame = TRUE;
    self->decoder_start = FALSE;
    self->pre_init_pool = FALSE;
    self->performance_mode = FALSE;
    self->resolution_changed = FALSE;
    self->has_set_format = FALSE;
    gst_vdec_base_set_flushing(self, FALSE);
    if (self->input.dump_file != nullptr) {
        fclose(self->input.dump_file);
        self->input.dump_file = nullptr;
    }
    if (self->output.dump_file != nullptr) {
        fclose(self->output.dump_file);
        self->output.dump_file = nullptr;
    }
    if (self->sink_caps) {
        gst_caps_unref(self->sink_caps);
        self->sink_caps = nullptr;
    }
}

static gboolean gst_vdec_base_stop(GstVideoDecoder *decoder)
{
    GstVdecBase *self = GST_VDEC_BASE(decoder);
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(self->decoder != nullptr, FALSE);
    GST_DEBUG_OBJECT(self, "Stop decoder start");

    g_mutex_lock(&self->drain_lock);
    self->draining = FALSE;
    g_cond_broadcast(&self->drain_cond);
    g_mutex_unlock(&self->drain_lock);

    gint ret = self->decoder->Stop();
    (void)gst_codec_return_is_ok(self, ret, "Stop", TRUE);
    if (self->input_state) {
        gst_video_codec_state_unref(self->input_state);
    }
    if (self->output_state) {
        gst_video_codec_state_unref(self->output_state);
    }
    self->input_state = nullptr;
    self->output_state = nullptr;

    gst_pad_stop_task(GST_VIDEO_DECODER_SRC_PAD(decoder));
    ret = self->decoder->FreeInputBuffers();
    (void)gst_codec_return_is_ok(self, ret, "FreeInput", TRUE);
    ret = self->decoder->FreeOutputBuffers();
    (void)gst_codec_return_is_ok(self, ret, "FreeOutput", TRUE);
    if (self->inpool) {
        gst_object_unref(self->inpool);
        self->inpool = nullptr;
    }
    if (self->outpool) {
        gst_buffer_pool_set_active(self->outpool, FALSE);
        gst_object_unref(self->outpool);
        self->outpool = nullptr;
    }
    gst_vdec_base_stop_after(self);
    GST_DEBUG_OBJECT(self, "Stop decoder end");
    return TRUE;
}

static gboolean gst_codec_return_is_ok(const GstVdecBase *decoder, gint ret,
    const char *error_name, gboolean need_report)
{
    if (ret == GST_CODEC_OK) {
        return TRUE;
    }
    if (need_report) {
        GST_ELEMENT_ERROR(decoder, STREAM, DECODE, ("hardware decoder error!"), ("%s", error_name));
    } else {
        GST_ERROR_OBJECT(decoder, "hardware decoder error %s", error_name);
    }
    return FALSE;
}

static gboolean gst_vdec_base_flush(GstVideoDecoder *decoder)
{
    GstVdecBase *self = GST_VDEC_BASE(decoder);
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(self->decoder != nullptr, FALSE);
    GST_DEBUG_OBJECT(self, "Flush start");

    if (!self->flushing_stoping) {
        gst_vdec_base_set_flushing(self, TRUE);
        gint ret = self->decoder->Flush(GST_CODEC_ALL);
        (void)gst_codec_return_is_ok(self, ret, "flush", FALSE);
        gst_vdec_base_set_flushing(self, FALSE);
    }

    GST_DEBUG_OBJECT(self, "Flush end");
    return TRUE;
}

static gboolean update_caps_format(GstVdecBase *self, GstCaps *caps)
{
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(self->decoder != nullptr, FALSE);
    gint ret = self->decoder->GetParameter(GST_VIDEO_FORMAT, GST_ELEMENT(self));
    (void)gst_codec_return_is_ok(self, ret, "flush", FALSE);
    GST_DEBUG_OBJECT(self, "update_caps_format");

    g_return_val_if_fail(gst_codec_return_is_ok(self, ret, "flush", FALSE), FALSE);
    GValue value_list = G_VALUE_INIT;
    GValue value = G_VALUE_INIT;

    if (!self->formats.empty()) {
        g_value_init(&value_list, GST_TYPE_LIST);
        g_value_init(&value, G_TYPE_STRING);
        for (auto format : self->formats) {
            if (format == GST_VIDEO_FORMAT_UNKNOWN) {
                continue;
            }
            g_value_set_string(&value, gst_video_format_to_string(format));
            gst_value_list_append_value(&value_list, &value);
        }
        gst_caps_set_value(caps, "format", &value_list);
        g_value_unset(&value);
        g_value_unset(&value_list);
    }

    return TRUE;
}

static gboolean get_memtype(GstVdecBase *self, const GstStructure *structure)
{
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(self->decoder != nullptr, FALSE);
    g_return_val_if_fail(structure != nullptr, FALSE);
    const gchar *memtype = gst_structure_get_string(structure, "memtype");
    g_return_val_if_fail(memtype != nullptr, FALSE);

    if (strcmp("surface", memtype) == 0) {
        self->memtype = GST_MEMTYPE_SURFACE;
    } else if (strcmp("avshmem", memtype) == 0) {
        self->memtype = GST_MEMTYPE_AVSHMEM;
    }

    if (self->memtype == GST_MEMTYPE_SURFACE) {
        gint ret = self->decoder->SetParameter(GST_VIDEO_SURFACE_INIT, GST_ELEMENT(self));
        g_return_val_if_fail(gst_codec_return_is_ok(self, ret, "GST_VIDEO_SURFACE_INIT", TRUE), FALSE);
    }

    return TRUE;
}

static gboolean gst_vdec_base_negotiate_format(GstVdecBase *self)
{
    GST_DEBUG_OBJECT(self, "Negotiate format");
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(self->decoder != nullptr, FALSE);
    g_return_val_if_fail(GST_VIDEO_DECODER_SRC_PAD(self) != nullptr, FALSE);

    GST_DEBUG_OBJECT(self, "Trying to negotiate a video format with downstream");
    if (self->performance_mode && self->pre_init_pool) {
        return TRUE;
    }
    GstCaps *templ_caps = gst_pad_get_pad_template_caps(GST_VIDEO_DECODER_SRC_PAD(self));
    g_return_val_if_fail(templ_caps != nullptr, FALSE);

    (void)update_caps_format(self, templ_caps);
    GstCaps *intersection = gst_pad_peer_query_caps(GST_VIDEO_DECODER_SRC_PAD(self), templ_caps);
    gst_caps_unref(templ_caps);
    g_return_val_if_fail(intersection != nullptr, FALSE);
    // We need unref at end.
    ON_SCOPE_EXIT(0) { gst_caps_unref(intersection); };
    if (gst_caps_is_empty(intersection)) {
        GST_ERROR_OBJECT(self, "negotiate failed, empty caps");
        return FALSE;
    }

    intersection = gst_caps_truncate(intersection);
    intersection = gst_caps_fixate(intersection);

    GstStructure *structure = gst_caps_get_structure(intersection, 0);
    const gchar *format_str = gst_structure_get_string(structure, "format");
    g_return_val_if_fail(format_str != nullptr, FALSE);

    GstVideoFormat format = gst_video_format_from_string(format_str);
    g_return_val_if_fail(format != GST_VIDEO_FORMAT_UNKNOWN, FALSE);

    self->format = format;
    gint ret = self->decoder->SetParameter(GST_VIDEO_FORMAT, GST_ELEMENT(self));
    g_return_val_if_fail(gst_codec_return_is_ok(self, ret, "setparameter", TRUE), FALSE);
    return TRUE;
}

static gboolean gst_vdec_base_set_outstate(GstVdecBase *self)
{
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(GST_VIDEO_DECODER_SRC_PAD(self) != nullptr, FALSE);
    GstVideoDecoder *decoder = GST_VIDEO_DECODER(self);

    GST_DEBUG_OBJECT(self, "Setting output state: format %s, width %u, height %u",
        gst_video_format_to_string(self->format), self->width, self->height);
    GstVideoCodecState *out_state = gst_video_decoder_set_output_state(GST_VIDEO_DECODER(self),
        self->format, self->width, self->height, self->input_state);
    g_return_val_if_fail(out_state != nullptr, FALSE);
    if (self->output_state != nullptr) {
        gst_video_codec_state_unref(self->output_state);
    }
    self->output_state = out_state;
    if (GST_VIDEO_INFO_MULTIVIEW_MODE(&out_state->info) == GST_VIDEO_MULTIVIEW_MODE_NONE) {
        GST_VIDEO_INFO_MULTIVIEW_MODE(&out_state->info) = GST_VIDEO_MULTIVIEW_MODE_MONO;
        GST_VIDEO_INFO_MULTIVIEW_FLAGS(&out_state->info) = GST_VIDEO_MULTIVIEW_FLAGS_NONE;
    }
    if (out_state->caps == nullptr) {
        out_state->caps = gst_video_info_to_caps(&out_state->info);
    }
    if (out_state->allocation_caps == nullptr) {
        out_state->allocation_caps = gst_caps_ref(out_state->caps);
    }
    g_return_val_if_fail(gst_pad_set_caps(decoder->srcpad, out_state->caps), FALSE);
    return TRUE;
}

static void gst_vdec_base_remove_reconfig_flag(GstVideoDecoder *decoder)
{
    if (gst_pad_check_reconfigure(decoder->srcpad)) {
        GST_INFO_OBJECT(decoder, "remove reconfig flag");
    }
}

static gboolean gst_vdec_base_negotiate(GstVdecBase *self)
{
    MediaTrace::TraceBegin("VdecBace::Negotiate", FAKE_POINTER(self));
    GST_DEBUG_OBJECT(self, "Negotiate");
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(GST_VIDEO_DECODER_SRC_PAD(self) != nullptr, FALSE);
    g_return_val_if_fail(gst_vdec_base_negotiate_format(self), FALSE);
    g_return_val_if_fail(gst_vdec_base_set_outstate(self), FALSE);
    g_return_val_if_fail(gst_video_decoder_negotiate(GST_VIDEO_DECODER(self)), FALSE);
    gst_vdec_base_remove_reconfig_flag(GST_VIDEO_DECODER(self));
    GST_WARNING_OBJECT(self, "KPI-TRACE-VDEC: negotiate end");
    MediaTrace::TraceEnd("VdecBace::Negotiate", FAKE_POINTER(self));
    return TRUE;
}

static gboolean gst_vdec_base_allocate_in_buffers(GstVdecBase *self)
{
    GST_DEBUG_OBJECT(self, "Allocate input buffers");
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(self->decoder != nullptr, FALSE);
    g_return_val_if_fail(self->inpool != nullptr, FALSE);
    std::vector<GstBuffer*> buffers;
    GstBufferPool *pool = reinterpret_cast<GstBufferPool*>(gst_object_ref(self->inpool));
    ON_SCOPE_EXIT(0) { gst_object_unref(pool); };
    for (guint i = 0; i < self->input.buffer_cnt; ++i) {
        GST_DEBUG_OBJECT(self, "Allocate Buffer %u", i);
        GstBuffer *buffer = nullptr;
        GstFlowReturn flow_ret = gst_buffer_pool_acquire_buffer(pool, &buffer, nullptr);
        if (flow_ret != GST_FLOW_OK || buffer == nullptr) {
            GST_WARNING_OBJECT(self, "Acquire buffer is nullptr");
            gst_buffer_unref(buffer);
            continue;
        }
        buffers.push_back(buffer);
    }
    GST_DEBUG_OBJECT(self, "Use input buffers start");
    // no give ref to decoder
    gint ret = self->decoder->UseInputBuffers(buffers);
    // Return buffers to pool
    for (auto buffer : buffers) {
        gst_buffer_unref(buffer);
    }
    buffers.clear();
    GST_DEBUG_OBJECT(self, "Use input buffers end");
    return gst_codec_return_is_ok(self, ret, "usebuffer", TRUE);
}

static gboolean gst_vdec_base_update_out_port_def(GstVdecBase *self, guint *size)
{
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(self->decoder != nullptr, FALSE);
    g_return_val_if_fail(size != nullptr, FALSE);
    gint ret = self->decoder->GetParameter(GST_VIDEO_OUTPUT_COMMON, GST_ELEMENT(self));
    g_return_val_if_fail(ret == GST_CODEC_OK, FALSE);
    if (self->output.min_buffer_cnt > self->out_buffer_max_cnt) {
        GST_ERROR_OBJECT(self, "min buffer %u > max buffer %u", self->output.min_buffer_cnt, self->out_buffer_max_cnt);
        return FALSE;
    }
    if (self->output.buffer_cnt > self->out_buffer_max_cnt) {
        self->output.buffer_cnt = self->out_buffer_max_cnt;
    }
    ret = self->decoder->SetParameter(GST_VIDEO_OUTPUT_COMMON, GST_ELEMENT(self));
    g_return_val_if_fail(ret == GST_CODEC_OK, FALSE);
    ret = self->decoder->GetParameter(GST_VIDEO_OUTPUT_COMMON, GST_ELEMENT(self));
    g_return_val_if_fail(ret == GST_CODEC_OK, FALSE);
    GST_INFO_OBJECT(self, "output params is min buffer count %u, buffer count %u, buffer size is %u",
        self->output.min_buffer_cnt, self->output.buffer_cnt, self->output.buffer_size);
    *size = self->output.buffer_size > *size ? self->output.buffer_size : *size;
    self->out_buffer_cnt = self->output.buffer_cnt;
    return TRUE;
}

static gboolean gst_vdec_base_allocate_out_buffers(GstVdecBase *self)
{
    GST_DEBUG_OBJECT(self, "Allocate output buffers");
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(self->decoder != nullptr, FALSE);
    std::vector<GstBuffer*> buffers;
    self->coding_outbuf_cnt = self->out_buffer_cnt;
    {
        MediaTrace trace("VdecBase::AllocateOutPutBuffer");
        for (guint i = 0; i < self->out_buffer_cnt; ++i) {
            GST_DEBUG_OBJECT(self, "Allocate output buffer %u", i);
            GstBuffer *buffer = gst_video_decoder_allocate_output_buffer(GST_VIDEO_DECODER(self));
            if (buffer == nullptr) {
                GST_WARNING_OBJECT(self, "Allocate buffer is nullptr");
                continue;
            }
            buffers.push_back(buffer);
        }
    }

    gint ret;
    {
        MediaTrace trace("VdecBase::UseOutputBuffers");
        // give buffer ref to decoder
        ret = self->decoder->UseOutputBuffers(buffers);
    }
    g_return_val_if_fail(gst_codec_return_is_ok(self, ret, "usebuffer", TRUE), FALSE);
    return TRUE;
}

static gboolean gst_vdec_base_prepare(GstVdecBase *self)
{
    GST_DEBUG_OBJECT(self, "Prepare");
    g_return_val_if_fail(self != nullptr, FALSE);

    // Negotiate with downstream and get format
    g_return_val_if_fail(gst_vdec_base_negotiate(self), FALSE);

    if (!gst_vdec_base_is_flushing(self)) {
        // Check allocate input buffer already
        gst_vdec_base_check_allocate_input(self);

        // To allocate output memory, we need to give the size
        g_return_val_if_fail(gst_vdec_base_allocate_out_buffers(self), FALSE);
    }

    return TRUE;
}

static void gst_vdec_base_clean_all_frames(GstVideoDecoder *decoder)
{
    GST_DEBUG_OBJECT(decoder, "Clean all frames");
    g_return_if_fail(decoder != nullptr);
    GList *frame_head = gst_video_decoder_get_frames(decoder);
    g_return_if_fail(frame_head != nullptr);
    for (GList *frame = frame_head; frame != nullptr; frame = frame->next) {
        GstVideoCodecFrame *tmp = reinterpret_cast<GstVideoCodecFrame*>(frame->data);
        if (tmp == nullptr) {
            continue;
        }
        gst_video_decoder_release_frame(decoder, tmp);
    }
    g_list_free(frame_head);
}

static void gst_vdec_debug_input_time(GstVdecBase *self)
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
        GST_DEBUG_OBJECT(self, "Decoder Input Time interval %" G_GINT64_FORMAT " us, frame count %" G_GINT64_FORMAT
        " ,every frame time %" G_GINT64_FORMAT " us, frame rate %.9f", time_interval, self->input.frame_cnt,
        time_every_frame, static_cast<double>(G_TIME_SPAN_SECOND) / static_cast<double>(time_every_frame));
    }
}

static void gst_vdec_debug_output_time(GstVdecBase *self)
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
        GST_DEBUG_OBJECT(self, "Decoder Output Time interval %" G_GINT64_FORMAT " us, frame count %" G_GINT64_FORMAT
        " ,every frame time %" G_GINT64_FORMAT " us, frame rate %.9f", time_interval, self->output.frame_cnt,
        time_every_frame, static_cast<double>(G_TIME_SPAN_SECOND) / static_cast<double>(time_every_frame));
    }
}

static void gst_vdec_base_dump_input_buffer(GstVdecBase *self, GstBuffer *buffer)
{
    g_return_if_fail(self != nullptr);
    g_return_if_fail(buffer != nullptr);
    if (self->input.enable_dump == FALSE) {
        return;
    }
    GST_DEBUG_OBJECT(self, "Dump input buffer");
    std::string input_dump_file = "/data/media/vdecbase-in-" +
        std::to_string(static_cast<int32_t>(FAKE_POINTER(self))) + ".es";
    if (self->input.dump_file == nullptr) {
        self->input.dump_file = fopen(input_dump_file.c_str(), "wb+");
    }
    if (self->input.dump_file == nullptr) {
        GST_ERROR_OBJECT(self, "open file failed");
        return;
    }
    GstMapInfo info = GST_MAP_INFO_INIT;
    g_return_if_fail(gst_buffer_map(buffer, &info, GST_MAP_READ));
    (void)fwrite(info.data, info.size, 1, self->input.dump_file);
    (void)fflush(self->input.dump_file);
    gst_buffer_unmap(buffer, &info);
}

static void gst_vdec_base_dump_output_buffer(GstVdecBase *self, GstBuffer *buffer)
{
    g_return_if_fail(self != nullptr);
    g_return_if_fail(buffer != nullptr);
    if (self->output.enable_dump == FALSE) {
        return;
    }
    GST_DEBUG_OBJECT(self, "Dump output buffer");
    std::string output_dump_file = "/data/media/vdecbase-out-" +
        std::to_string(static_cast<int32_t>(FAKE_POINTER(self))) + ".yuv";
    if (self->output.dump_file == nullptr) {
        self->output.dump_file = fopen(output_dump_file.c_str(), "wb+");
    }
    if (self->output.dump_file == nullptr) {
        GST_DEBUG_OBJECT(self, "open file failed");
        return;
    }
    GstMapInfo info = GST_MAP_INFO_INIT;
    g_return_if_fail(gst_buffer_map(buffer, &info, GST_MAP_READ));
    guint stride = (guint)(self->real_stride == 0 ? self->stride : self->real_stride);
    guint stride_height = (guint)(self->real_stride_height == 0 ? self->stride_height : self->real_stride_height);
    stride = stride == 0 ? self->width : stride;
    stride_height = stride_height == 0 ? self->height : stride_height;
    // yuv size
    guint size = stride * stride_height * 3 / 2;
    if (size > info.size) {
        size = info.size;
        GST_WARNING_OBJECT(self, "dump size %u and buffer size %u", size, (guint)info.size);
    }
    (void)fwrite(info.data, size, 1, self->output.dump_file);
    (void)fflush(self->output.dump_file);
    gst_buffer_unmap(buffer, &info);
}

static void gst_vdec_base_input_frame_pts_to_list(GstVdecBase *self, GstVideoCodecFrame *frame)
{
    GST_DEBUG_OBJECT(self, "Input frame pts %" G_GUINT64_FORMAT, frame->pts);
    g_mutex_lock(&self->lock);
    if (frame->pts == GST_CLOCK_TIME_NONE) {
        g_mutex_unlock(&self->lock);
        return;
    }
    if (self->pts_list.empty() || frame->pts > self->pts_list.back()) {
        self->pts_list.push_back(frame->pts);
        self->last_pts = frame->pts;
        g_mutex_unlock(&self->lock);
        return;
    }
    for (auto iter = self->pts_list.begin(); iter != self->pts_list.end(); ++iter) {
        if ((*iter) == frame->pts) {
            break;
        }
        if ((*iter) > frame->pts) {
            self->pts_list.insert(iter, frame->pts);
            break;
        }
    }
    g_mutex_unlock(&self->lock);
}

static int32_t gst_vdec_base_push_input_buffer_with_copy(GstVdecBase *self, GstBuffer *src_buffer)
{
    GstBuffer* dts_buffer;
    GstBufferPool *pool = reinterpret_cast<GstBufferPool *>(gst_object_ref(self->inpool));
    ON_SCOPE_EXIT(0) { gst_object_unref(pool); };
    GstFlowReturn flow_ret = gst_buffer_pool_acquire_buffer(pool, &dts_buffer, nullptr);
    ON_SCOPE_EXIT(1) { gst_buffer_unref(dts_buffer); };
    if (flow_ret != GST_FLOW_OK || dts_buffer == nullptr) {
        GST_ERROR_OBJECT(self, "Acquire buffer is nullptr");
        return GST_CODEC_FLUSH;
    }
    GstMapInfo dts_map = GST_MAP_INFO_INIT;
    if (gst_buffer_map(dts_buffer, &dts_map, GST_MAP_WRITE) != TRUE) {
        return GST_CODEC_ERROR;
    }
    ON_SCOPE_EXIT(2) { gst_buffer_unmap(dts_buffer, &dts_map); };
    GstMapInfo src_map = GST_MAP_INFO_INIT;
    if (gst_buffer_map(src_buffer, &src_map, GST_MAP_READ) != TRUE) {
        return GST_CODEC_ERROR;
    }
    ON_SCOPE_EXIT(3) { gst_buffer_unmap(src_buffer, &src_map); };

    auto ret = memcpy_s(dts_map.data, dts_map.size, src_map.data, src_map.size);
    g_return_val_if_fail(ret == EOK, GST_CODEC_ERROR);
    GstBufferTypeMeta *meta = gst_buffer_get_buffer_type_meta(dts_buffer);
    g_return_val_if_fail(meta != nullptr, GST_CODEC_ERROR);
    meta->length = src_map.size;
    CANCEL_SCOPE_EXIT_GUARD(2);
    CANCEL_SCOPE_EXIT_GUARD(3);
    gst_buffer_unmap(dts_buffer, &dts_map);
    gst_buffer_unmap(src_buffer, &src_map);
    gst_buffer_set_size(dts_buffer, meta->length);
    gst_vdec_base_dump_input_buffer(self, dts_buffer);
    gint codec_ret = self->decoder->PushInputBuffer(dts_buffer);
    return codec_ret;
}

static gboolean gst_vdec_check_ashmem_buffer(GstBuffer *buffer)
{
    g_return_val_if_fail(buffer != nullptr, FALSE);
    GstMemory *memory = gst_buffer_peek_memory(buffer, 0);
    return gst_is_shmem_memory(memory);
}

static GstFlowReturn gst_vdec_base_push_input_buffer(GstVideoDecoder *decoder, GstVideoCodecFrame *frame)
{
    GstVdecBase *self = GST_VDEC_BASE(decoder);
    gst_vdec_debug_input_time(self);
    gst_vdec_base_input_frame_pts_to_list(self, frame);
    GstVdecBaseClass *kclass = GST_VDEC_BASE_GET_CLASS(self);
    GstBuffer *buf = nullptr;
    if (kclass->handle_slice_buffer != nullptr && self->enable_slice_cat == true) {
        bool ready_push_slice_buffer = false;
        GstBuffer *cat_buffer = kclass->handle_slice_buffer(self, frame->input_buffer, ready_push_slice_buffer, false);
        if (cat_buffer != nullptr && ready_push_slice_buffer == true) {
            buf = cat_buffer;
        }
    } else {
        buf = frame->input_buffer;
        gst_buffer_ref(buf);
    }

    if (buf == nullptr) {
        GST_DEBUG_OBJECT(self, "buffer is null");
        return GST_FLOW_OK;
    }

    GST_VIDEO_DECODER_STREAM_UNLOCK(self);

    gint codec_ret = GST_CODEC_OK;
    if (!gst_vdec_check_ashmem_buffer(buf) && self->input_need_ashmem) {
        codec_ret = gst_vdec_base_push_input_buffer_with_copy(self, buf);
    } else {
        gst_vdec_base_dump_input_buffer(self, buf);
        codec_ret = self->decoder->PushInputBuffer(buf);
    }
    gst_buffer_unref(buf);
    GST_VIDEO_DECODER_STREAM_LOCK(self);
    GstFlowReturn ret = GST_FLOW_OK;
    switch (codec_ret) {
        case GST_CODEC_OK:
            break;
        case GST_CODEC_FLUSH:
            ret = GST_FLOW_FLUSHING;
            GST_DEBUG_OBJECT(self, "Flushing");
            break;
        default:
            ret = GST_FLOW_ERROR;
            GST_ELEMENT_WARNING(self, STREAM, ENCODE, ("Hardware encoder error!"), ("pull"));
    }
    return ret;
}

static GstFlowReturn gst_vdec_base_handle_frame(GstVideoDecoder *decoder, GstVideoCodecFrame *frame)
{
    GST_DEBUG_OBJECT(decoder, "Handle frame");
    GstVdecBase *self = GST_VDEC_BASE(decoder);
    ON_SCOPE_EXIT(0) { gst_video_codec_frame_unref(frame); };
    g_return_val_if_fail(GST_IS_VDEC_BASE(self), GST_FLOW_ERROR);
    g_return_val_if_fail(self != nullptr && frame != nullptr && self->decoder != nullptr, GST_FLOW_ERROR);
    if (self->input.first_frame) {
        GST_WARNING_OBJECT(decoder, "KPI-TRACE-VDEC: first in frame");
        self->input.first_frame = FALSE;
    }

    if (gst_vdec_base_is_flushing(self)) {
        return GST_FLOW_FLUSHING;
    }
    gst_vdec_base_clean_all_frames(decoder);
    if (!self->prepared) {
        if (!gst_vdec_base_prepare(self)) {
            GST_WARNING_OBJECT(self, "hdi video dec enable failed");
            return GST_FLOW_ERROR;
        }

        /* When the resolution of HLS changes, the new one will block on the pad port and wait for connection.
            At this time, if the seek operation is issued, the pad port will be activated. At this time,
            the negotiation is unreliable and the frame data needs to be refreshed */
        if (gst_vdec_base_is_flushing(self)) {
            return GST_FLOW_FLUSHING;
        }
        self->prepared = TRUE;
    }
    GstPad *pad = GST_VIDEO_DECODER_SRC_PAD(self);
    if (!self->decoder_start) {
        gint ret = self->decoder->Start();
        g_return_val_if_fail(gst_codec_return_is_ok(self, ret, "start", TRUE), GST_FLOW_ERROR);
        self->decoder_start = TRUE;
        GST_WARNING_OBJECT(decoder, "KPI-TRACE-VDEC: start end");
    }
    if (gst_pad_get_task_state(pad) != GST_TASK_STARTED &&
        gst_pad_start_task(pad, (GstTaskFunction)gst_vdec_base_loop, decoder, nullptr) != TRUE) {
        return GST_FLOW_ERROR;
    }

    GstFlowReturn ret = gst_vdec_base_push_input_buffer(decoder, frame);
    return ret;
}

static void update_video_meta(const GstVdecBase *self, GstBuffer *buffer)
{
    GST_DEBUG_OBJECT(self, "Update buffer video meta");
    g_return_if_fail(self != nullptr);
    g_return_if_fail(buffer != nullptr);
    GstVideoMeta *video_meta = gst_buffer_get_video_meta(buffer);
    if (video_meta == nullptr) {
        gst_buffer_add_video_meta(buffer, GST_VIDEO_FRAME_FLAG_NONE, self->format, self->width, self->height);
    } else {
        video_meta->width = self->width;
        video_meta->height = self->height;
        video_meta->offset[0] = 0;
        video_meta->stride[0] = self->stride;
        video_meta->offset[1] = video_meta->stride[0] * self->stride_height;
    }
}

static GstVideoCodecFrame *gst_vdec_base_new_frame(GstVdecBase *self, GstBuffer *buffer)
{
    GST_DEBUG_OBJECT(self, "New frame");
    GstVideoCodecFrame *frame = g_slice_new0(GstVideoCodecFrame);
    if (frame == nullptr) {
        return nullptr;
    }
    frame->ref_count = 1;
    frame->system_frame_number = 0;
    frame->decode_frame_number = 0;
    frame->dts = GST_CLOCK_TIME_NONE;
    g_mutex_lock(&self->lock);
    if (self->player_mode == FALSE) {
        frame->pts = GST_BUFFER_PTS(buffer);
    } else {
        if (self->pts_list.empty()) {
            frame->pts = self->last_pts;
            GST_WARNING_OBJECT(self, "No pts available");
        } else {
            frame->pts = self->pts_list.front();
            GST_DEBUG_OBJECT(self, "Pts %" G_GUINT64_FORMAT, frame->pts);
            self->pts_list.pop_front();
        }
    }
    g_mutex_unlock(&self->lock);
    frame->duration = GST_CLOCK_TIME_NONE;
    frame->events = nullptr;

    return frame;
}
// copy for avshmem
static void copy_to_no_stride_buffer(GstVdecBase *self, GstVideoCodecFrame *frame)
{
    if (self->memtype == GST_MEMTYPE_SURFACE) {
        return;
    }
    GstBuffer *src_buffer = frame->output_buffer;
    frame->output_buffer = nullptr;
    ON_SCOPE_EXIT(0) { gst_buffer_unref(src_buffer); };
    // yuv buffer size
    guint size = (guint)(self->width * self->height * 3 / 2);
    guint stride = (guint)(self->real_stride == 0 ? self->stride : self->real_stride);
    guint stride_height = (guint)(self->real_stride_height == 0 ? self->stride_height : self->real_stride_height);
    guint offset = stride * stride_height;
    GstBuffer *dts_buffer = gst_buffer_new_allocate(nullptr, size, nullptr);
    g_return_if_fail(dts_buffer != nullptr);
    frame->output_buffer = dts_buffer;

    GstMapInfo dts_map = GST_MAP_INFO_INIT;
    if (gst_buffer_map(dts_buffer, &dts_map, GST_MAP_WRITE) != TRUE) {
        return;
    }
    ON_SCOPE_EXIT(1) { gst_buffer_unmap(dts_buffer, &dts_map); };
    GstMapInfo src_map = GST_MAP_INFO_INIT;
    if (gst_buffer_map(src_buffer, &src_map, GST_MAP_READ) != TRUE) {
        return;
    }
    ON_SCOPE_EXIT(2) { gst_buffer_unmap(src_buffer, &src_map); };
    // yuv buffer size
    g_return_if_fail(src_map.size >= offset * 3 / 2);
    errno_t ret = EOK;
    guint src_offset = 0;
    guint dts_offset = 0;
    for (int pos = 0; pos < self->height; pos++) {
        ret = memcpy_s(dts_map.data + dts_offset, dts_map.size - dts_offset, src_map.data + src_offset, self->width);
        g_return_if_fail(ret == EOK);
        dts_offset += (guint)self->width;
        src_offset += stride;
    }
    src_offset = offset;
    for (int pos = 0; pos < self->height / 2; pos++) {
        ret = memcpy_s(dts_map.data + dts_offset, dts_map.size - dts_offset, src_map.data + src_offset, self->width);
        g_return_if_fail(ret == EOK);
        dts_offset += self->width;
        src_offset += stride;
    }
    gint rst_stride[GST_VIDEO_MAX_PLANES] = { self->width, self->width, 0, 0 };
    gsize rst_offset[GST_VIDEO_MAX_PLANES] = { 0, self->width * self->height, 0, 0 };
    static const gint nplane = 2; // nv12 or nv21 planes count
    gst_buffer_add_video_meta_full(dts_buffer, GST_VIDEO_FRAME_FLAG_NONE, self->format,
        self->width, self->height, nplane, rst_offset, rst_stride);
}

static void gst_vdec_base_post_resolution_changed_message(GstVdecBase *self)
{
    GstMessage *msg_resolution_changed = nullptr;
    msg_resolution_changed = gst_message_new_resolution_changed(GST_OBJECT(self),
        self->width, self->height);
    if (msg_resolution_changed) {
        gst_element_post_message(GST_ELEMENT(self), msg_resolution_changed);
    }
    GST_DEBUG_OBJECT(self, "post resolution changed message");
}

static GstFlowReturn push_output_buffer(GstVdecBase *self, GstBuffer *buffer)
{
    MediaTrace::TraceBegin("VdecBase::FinishFrame", FAKE_POINTER(self));
    GST_DEBUG_OBJECT(self, "Push output buffer");
    g_return_val_if_fail(self != nullptr, GST_FLOW_ERROR);
    g_return_val_if_fail(buffer != nullptr, GST_FLOW_ERROR);
    update_video_meta(self, buffer);
    GstVideoCodecFrame *frame = gst_vdec_base_new_frame(self, buffer);
    g_return_val_if_fail(frame != nullptr, GST_FLOW_ERROR);
    gst_vdec_debug_output_time(self);

    if (self->resolution_changed == FALSE) {
        gst_vdec_base_post_resolution_changed_message(self);
        self->resolution_changed = TRUE;
        GST_WARNING_OBJECT(self, "KPI-TRACE-VDEC: first frame out");
    }

    frame->output_buffer = buffer;
    gst_vdec_base_dump_output_buffer(self, buffer);
    copy_to_no_stride_buffer(self, frame);
    GstFlowReturn flow_ret = gst_video_decoder_finish_frame(GST_VIDEO_DECODER(self), frame);
    MediaTrace::TraceEnd("VdecBase::FinishFrame", FAKE_POINTER(self));
    return flow_ret;
}

static gboolean gst_vdec_check_out_format_change(GstVdecBase *self)
{
    gboolean is_format_change = (self->width != self->rect.width) || (self->height != self->rect.height);

    if (is_format_change) {
        GST_INFO_OBJECT(self, "Format change width %d to %d, height %d to %d",
            self->width, self->output.width, self->height, self->output.height);
        self->width = self->rect.width;
        self->height = self->rect.height;
    }
    return is_format_change;
}

static gboolean gst_vdec_check_out_buffer_cnt(GstVdecBase *self)
{
    if (self->output.min_buffer_cnt > self->out_buffer_max_cnt) {
        GST_ERROR_OBJECT(self, "min buffer %u > max buffer %u", self->output.min_buffer_cnt, self->out_buffer_max_cnt);
        return FALSE;
    }
    if (self->output.buffer_cnt > self->out_buffer_max_cnt) {
        self->output.buffer_cnt = self->out_buffer_max_cnt;
    }
    gboolean is_buffer_cnt_change = self->out_buffer_cnt != self->output.buffer_cnt;
    GST_INFO_OBJECT(self, "Format change buffer %u to %u", self->out_buffer_cnt, self->output.buffer_cnt);
    self->out_buffer_cnt = self->output.buffer_cnt;
    return is_buffer_cnt_change;
}

static void gst_vdec_base_get_real_stride(GstVdecBase *self)
{
    self->real_stride = self->stride;
    self->real_stride_height = self->stride_height;
}

static GstFlowReturn gst_vdec_base_format_change(GstVdecBase *self)
{
    GST_STATE_LOCK(self);
    ON_SCOPE_EXIT(0) { GST_STATE_UNLOCK(self); };
    MediaTrace::TraceBegin("VdecBase::FormatChange", FAKE_POINTER(self));
    GST_WARNING_OBJECT(self, "KPI-TRACE-VDEC: format change start");
    g_return_val_if_fail(self != nullptr && self->decoder != nullptr, GST_FLOW_ERROR);
    gint ret = self->decoder->ActiveBufferMgr(GST_CODEC_OUTPUT, false);
    g_return_val_if_fail(gst_codec_return_is_ok(self, ret, "ActiveBufferMgr", TRUE), GST_FLOW_ERROR);
    ret = self->decoder->FreeOutputBuffers();
    g_return_val_if_fail(gst_codec_return_is_ok(self, ret, "freebuffer", TRUE), GST_FLOW_ERROR);
    ret = self->decoder->GetParameter(GST_VIDEO_OUTPUT_COMMON, GST_ELEMENT(self));
    g_return_val_if_fail(gst_codec_return_is_ok(self, ret, "GetParameter", TRUE), GST_FLOW_ERROR);
    gst_vdec_base_get_real_stride(self);
    gboolean format_change = gst_vdec_check_out_format_change(self);
    gboolean buffer_cnt_change = gst_vdec_check_out_buffer_cnt(self);
    if (format_change) {
        gst_vdec_base_post_resolution_changed_message(self);
        self->resolution_changed = TRUE;
    }
    if (format_change || (buffer_cnt_change && self->memtype != GST_MEMTYPE_SURFACE)) {
        g_return_val_if_fail(gst_vdec_base_set_outstate(self), GST_FLOW_ERROR);
        g_return_val_if_fail(gst_video_decoder_negotiate(GST_VIDEO_DECODER(self)), GST_FLOW_ERROR);
    } else if (buffer_cnt_change) {
        g_object_set(self->outpool, "dynamic-buffer-num", self->out_buffer_cnt, nullptr);
    }

    ret = self->decoder->ActiveBufferMgr(GST_CODEC_OUTPUT, true);
    g_return_val_if_fail(gst_codec_return_is_ok(self, ret, "ActiveBufferMgr", TRUE), GST_FLOW_ERROR);
    g_return_val_if_fail(gst_vdec_base_allocate_out_buffers(self), GST_FLOW_ERROR);
    ret = self->decoder->Start();
    g_return_val_if_fail(gst_codec_return_is_ok(self, ret, "Start", TRUE), GST_FLOW_ERROR);
    GST_WARNING_OBJECT(self, "KPI-TRACE-VDEC: format change end");
    MediaTrace::TraceEnd("VdecBase::FormatChange", FAKE_POINTER(self));
    return GST_FLOW_OK;
}

static GstFlowReturn gst_vdec_base_codec_eos(GstVdecBase *self)
{
    GST_DEBUG_OBJECT(self, "Eos");
    g_return_val_if_fail(self != nullptr, GST_FLOW_ERROR);
    g_return_val_if_fail(GST_VIDEO_DECODER_SRC_PAD(self) != nullptr, GST_FLOW_ERROR);
    g_mutex_lock(&self->drain_lock);
    if (self->draining) {
        GstQuery *query = gst_query_new_drain();

        if (!gst_pad_peer_query(GST_VIDEO_DECODER_SRC_PAD(self), query)) {
            GST_WARNING_OBJECT(self, "drain query failed");
        }
        gst_query_unref(query);

        GST_DEBUG_OBJECT(self, "Drained");
        self->draining = FALSE;
        g_cond_broadcast(&self->drain_cond);
    }
    g_mutex_unlock(&self->drain_lock);
    gst_vdec_base_pause_loop(self);
    return GST_FLOW_OK;
}

static void gst_vdec_base_pause_loop(GstVdecBase *self)
{
    GST_DEBUG_OBJECT(self, "Pause loop start");
    g_return_if_fail(self != nullptr);
    g_mutex_lock(&self->drain_lock);
    if (self->draining) {
        self->draining = FALSE;
        g_cond_broadcast(&self->drain_cond);
    }
    g_mutex_unlock(&self->drain_lock);
    GST_DEBUG_OBJECT(self, "pause loop end");
    gst_pad_pause_task(GST_VIDEO_DECODER_SRC_PAD(self));
}

static gboolean gst_vdec_base_push_out_buffers(GstVdecBase *self)
{
    MediaTrace trace("Surface::AcquireBuffer");
    GST_DEBUG_OBJECT(self, "Push out buffers");
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(self->decoder != nullptr, FALSE);
    GstBuffer *buffer = nullptr;
    GstBufferPool *pool = gst_video_decoder_get_buffer_pool(GST_VIDEO_DECODER(self));
    ON_SCOPE_EXIT(0) { gst_object_unref(pool); };
    GstFlowReturn flow = GST_FLOW_OK;
    gint codec_ret = GST_CODEC_OK;
    GstBufferPoolAcquireParams params;
    g_return_val_if_fail(memset_s(&params, sizeof(params), 0, sizeof(params)) == EOK, FALSE);
    if (self->coding_outbuf_cnt > BLOCKING_ACQUIRE_BUFFER_THRESHOLD) {
        params.flags = GST_BUFFER_POOL_ACQUIRE_FLAG_DONTWAIT;
    }
    while (flow == GST_FLOW_OK) {
        flow = gst_buffer_pool_acquire_buffer(pool, &buffer, &params);
        if (flow == GST_FLOW_OK) {
            g_return_val_if_fail(buffer != nullptr, FALSE);
            // the buffer ref give to decoder
            codec_ret = self->decoder->PushOutputBuffer(buffer);
            if (codec_ret == GST_CODEC_FLUSH) {
                GST_INFO_OBJECT(self, "Input Buffer is Flush");
                return FALSE;
            }
            g_return_val_if_fail(gst_codec_return_is_ok(self, codec_ret, "push buffer", TRUE), FALSE);
            self->coding_outbuf_cnt++;
            params.flags = GST_BUFFER_POOL_ACQUIRE_FLAG_DONTWAIT;
        }
    }
    if (flow != GST_FLOW_EOS) {
        GST_ELEMENT_ERROR(self, STREAM, DECODE, ("hardware decoder error!"), ("%s", "acquire buffer"));
        return FALSE;
    }
    return TRUE;
}

static void gst_vdec_base_loop(GstVdecBase *self)
{
    GST_DEBUG_OBJECT(self, "Loop in");
    g_return_if_fail(self != nullptr);
    g_return_if_fail(self->decoder != nullptr);

    GstBuffer *gst_buffer = nullptr;
    if (gst_vdec_base_push_out_buffers(self) != TRUE) {
        gst_vdec_base_pause_loop(self);
        return;
    }
    GST_DEBUG_OBJECT(self, "coding buffers %u", self->coding_outbuf_cnt);
    MediaTrace::TraceBegin("VdecBase::PullOutputBuffer", FAKE_POINTER(self));
    gint codec_ret = self->decoder->PullOutputBuffer(&gst_buffer);
    MediaTrace::TraceEnd("VdecBase::PullOutputBuffer", FAKE_POINTER(self));
    gint flow_ret = GST_FLOW_OK;
    GST_DEBUG_OBJECT(self, "Pull ret %d", codec_ret);
    switch (codec_ret) {
        case GST_CODEC_OK:
            self->coding_outbuf_cnt--;
            flow_ret = push_output_buffer(self, gst_buffer);
            break;
        case GST_CODEC_FORMAT_CHANGE:
            (void)gst_vdec_base_format_change(self);
            return;
        case GST_CODEC_EOS:
            self->coding_outbuf_cnt--;
            flow_ret = gst_vdec_base_codec_eos(self);
            break;
        case GST_CODEC_FLUSH:
            flow_ret = GST_FLOW_FLUSHING;
            break;
        case GST_CODEC_ERROR:
            GST_ELEMENT_WARNING(self, STREAM, DECODE, ("Hardware decoder error!"), ("pull"));
            flow_ret = GST_FLOW_ERROR;
            break;
        default:
            flow_ret = GST_FLOW_ERROR;
            GST_ERROR_OBJECT(self, "Unknown error");
    }
    switch (flow_ret) {
        case GST_FLOW_OK:
            return;
        case GST_FLOW_FLUSHING:
            GST_DEBUG_OBJECT(self, "Flushing");
            break;
        case GST_FLOW_EOS:
            GST_DEBUG_OBJECT(self, "Eos");
            gst_pad_push_event(GST_VIDEO_DECODER_SRC_PAD(self), gst_event_new_eos());
            break;
        default:
            gst_pad_push_event(GST_VIDEO_DECODER_SRC_PAD(self), gst_event_new_eos());
    }
    gst_vdec_base_pause_loop(self);
}

static gboolean gst_vdec_caps_fix_sink_caps(GstVdecBase *self)
{
    GstCaps *templ_caps = gst_pad_get_pad_template_caps(GST_VIDEO_DECODER_SRC_PAD(self));
    g_return_val_if_fail(templ_caps != nullptr, FALSE);
    ON_SCOPE_EXIT(0) { gst_caps_unref(templ_caps); };
    GstCaps *pool_caps = gst_caps_intersect(self->sink_caps, templ_caps);
    if (gst_caps_is_empty(pool_caps)) {
        gst_caps_unref(pool_caps);
        return FALSE;
    }
    pool_caps = gst_caps_fixate(pool_caps);
    g_return_val_if_fail(pool_caps != nullptr, FALSE);
    gst_caps_unref(self->sink_caps);
    self->sink_caps = pool_caps;

    GstStructure *structure = gst_caps_get_structure(pool_caps, 0);
    const gchar *format_str = gst_structure_get_string(structure, "format");
    g_return_val_if_fail(format_str != nullptr, FALSE);

    GstVideoFormat format = gst_video_format_from_string(format_str);
    g_return_val_if_fail(format != GST_VIDEO_FORMAT_UNKNOWN, FALSE);

    self->format = format;
    gint ret = self->decoder->SetParameter(GST_VIDEO_FORMAT, GST_ELEMENT(self));
    g_return_val_if_fail(gst_codec_return_is_ok(self, ret, "setparameter", TRUE), FALSE);
    return TRUE;
}

static gboolean gst_vdec_base_pre_init_surface(GstVdecBase *self)
{
    GST_DEBUG_OBJECT(self, "surface pre init");
    g_return_val_if_fail(self != nullptr, FALSE);
    g_return_val_if_fail(self->decoder != nullptr, FALSE);
    g_return_val_if_fail(self->sink_caps != nullptr, FALSE);
    g_return_val_if_fail(self->outpool != nullptr, FALSE);

    guint size = 0;
    GstStructure *structure = gst_caps_get_structure(self->sink_caps, 0);
    g_return_val_if_fail(structure != nullptr, FALSE);
    gst_structure_set(structure, "width", G_TYPE_INT, self->width, nullptr);
    gst_structure_set(structure, "height", G_TYPE_INT, self->height, nullptr);

    self->memtype = GST_MEMTYPE_SURFACE;
    gint ret = self->decoder->SetParameter(GST_VIDEO_SURFACE_INIT, GST_ELEMENT(self));
    g_return_val_if_fail(gst_codec_return_is_ok(self, ret, "GST_VIDEO_SURFACE_INIT", TRUE), FALSE);

    if (!gst_vdec_caps_fix_sink_caps(self)) {
        g_signal_emit(self, signals[SIGNAL_CAPS_FIX_ERROR], 0, nullptr);
        return FALSE;
    }
    gst_vdec_base_update_out_port_def(self, &size);
    gst_vdec_base_update_out_pool(self, &self->outpool, self->sink_caps, size);
    gst_buffer_pool_set_active(self->outpool, TRUE);
    self->pre_init_pool = TRUE;
    return TRUE;
}

static gboolean gst_vdec_base_set_format(GstVideoDecoder *decoder, GstVideoCodecState *state)
{
    GstVdecBase *self = GST_VDEC_BASE(decoder);
    g_return_val_if_fail(self != nullptr, FALSE);
    if (self->prepared || self->has_set_format) {
        return TRUE;
    }
    g_return_val_if_fail(self->decoder != nullptr, FALSE);
    g_return_val_if_fail(state != nullptr, FALSE);
    GstVideoInfo *info = &state->info;
    g_return_val_if_fail(info != nullptr, FALSE);
    gboolean is_format_change = FALSE;
    gint ret = self->decoder->GetParameter(GST_VIDEO_INPUT_COMMON, GST_ELEMENT(self));
    g_return_val_if_fail(ret == GST_CODEC_OK, FALSE);
    GST_INFO_OBJECT(self, "Input params is min buffer count %u, buffer count %u, buffer size is %u",
        self->input.min_buffer_cnt, self->input.buffer_cnt, self->input.buffer_size);

    GST_DEBUG_OBJECT(self, "Setting new caps");

    constexpr gfloat epsilon = 1e-6;
    is_format_change = is_format_change || self->width != info->width;
    is_format_change = is_format_change || self->height != GST_VIDEO_INFO_FIELD_HEIGHT(info);
    is_format_change = is_format_change || (self->frame_rate < epsilon && self->frame_rate > -epsilon &&
        info->fps_n != 0);

    if (is_format_change && info->width != 0 && GST_VIDEO_INFO_FIELD_HEIGHT(info) != 0) {
        self->width = info->width;
        self->height = GST_VIDEO_INFO_FIELD_HEIGHT(info);
        self->frame_rate  = static_cast<gfloat>(info->fps_n) / static_cast<gfloat>(info->fps_d);
        gst_vdec_base_post_resolution_changed_message(self);
        self->resolution_changed = TRUE;
        GST_DEBUG_OBJECT(self, "width: %d, height: %d, frame_rate: %.3f", self->width, self->height, self->frame_rate);
    }

    GST_DEBUG_OBJECT(self, "Setting input port definition");
    ret = self->decoder->SetParameter(GST_VIDEO_INPUT_COMMON, GST_ELEMENT(self));
    g_return_val_if_fail(ret == GST_CODEC_OK, FALSE);
    ret = self->decoder->GetParameter(GST_VIDEO_INPUT_COMMON, GST_ELEMENT(self));
    g_return_val_if_fail(ret == GST_CODEC_OK, FALSE);
    self->input_state = gst_video_codec_state_ref(state);
    if (self->performance_mode) {
        g_return_val_if_fail(gst_vdec_base_pre_init_surface(self) != FALSE, FALSE);
    }
    self->has_set_format = TRUE;

    return TRUE;
}

static GstFlowReturn gst_vdec_base_finish(GstVideoDecoder *decoder)
{
    GstVdecBase *self = GST_VDEC_BASE(decoder);
    g_return_val_if_fail(self != nullptr, GST_FLOW_ERROR);
    g_return_val_if_fail(self->decoder != nullptr, GST_FLOW_ERROR);
    GST_DEBUG_OBJECT(self, "Finish codec start");
    GstPad *pad = GST_VIDEO_DECODER_SRC_PAD(self);
    if (gst_pad_get_task_state(pad) != GST_TASK_STARTED) {
        GST_DEBUG_OBJECT(self, "vdec not start yet");
        return GST_FLOW_OK;
    }
    GstVdecBaseClass *kclass = GST_VDEC_BASE_GET_CLASS(self);
    if (kclass->handle_slice_buffer != nullptr && self->enable_slice_cat == true) {
        bool ready_push_slice_buffer = false;
        GstBuffer *cat_buffer = kclass->handle_slice_buffer(self, nullptr, ready_push_slice_buffer, true);
        if (cat_buffer != nullptr && ready_push_slice_buffer == true) {
            GST_VIDEO_DECODER_STREAM_UNLOCK(self);
            gint codec_ret = GST_CODEC_OK;
            if (!gst_vdec_check_ashmem_buffer(cat_buffer) && self->input_need_ashmem) {
                codec_ret = gst_vdec_base_push_input_buffer_with_copy(self, cat_buffer);
            } else {
                gst_vdec_base_dump_input_buffer(self, cat_buffer);
                codec_ret = self->decoder->PushInputBuffer(cat_buffer);
            }
            if (codec_ret != GST_CODEC_OK) {
                GST_ERROR_OBJECT(self, "Finish push buffer failed");
            }
            gst_buffer_unref(cat_buffer);
        }
    } else {
        GST_VIDEO_DECODER_STREAM_UNLOCK(self);
    }
    
    g_mutex_lock(&self->drain_lock);
    self->draining = TRUE;
    gint ret = self->decoder->PushInputBuffer(nullptr);
    if (ret != GST_CODEC_OK) {
        GST_ERROR_OBJECT(self, "Failed to push input buffer for draining: %d", ret);
        g_mutex_unlock(&self->drain_lock);
        GST_VIDEO_DECODER_STREAM_LOCK(self);
        return GST_FLOW_ERROR;
    }
    gint64 wait_time = std::min(DRAIN_TIME_OUT_MAX,
        std::max(DRAIN_TIME_OUT_MIN, DRAIN_TIME_OUT_BY_FRAMERATE(self->frame_rate)));
    gint64 wait_until = g_get_monotonic_time() + wait_time;
    GST_DEBUG_OBJECT(self, "Waiting until codec is drained, wait %" G_GINT64_FORMAT " us", wait_time);
    if (!g_cond_wait_until(&self->drain_cond, &self->drain_lock, wait_until)) {
        GST_ERROR_OBJECT(self, "Drain timed out");
    } else {
        GST_DEBUG_OBJECT(self, "finish hdi end");
    }
    g_mutex_unlock(&self->drain_lock);
    GST_VIDEO_DECODER_STREAM_LOCK(self);

    return GST_FLOW_OK;
}

static GstFlowReturn gst_vdec_base_drain(GstVideoDecoder *decoder)
{
    GST_DEBUG_OBJECT(decoder, "Draining codec start");
    g_return_val_if_fail(decoder != nullptr, GST_FLOW_ERROR);
    GstFlowReturn ret = gst_vdec_base_finish(decoder);
    gst_vdec_base_flush(decoder);
    GST_DEBUG_OBJECT(decoder, "Draining codec end");
    return ret;
}

static gboolean gst_vdec_base_event(GstVideoDecoder *decoder, GstEvent *event)
{
    g_return_val_if_fail(decoder != nullptr, FALSE);
    g_return_val_if_fail(event != nullptr, FALSE);
    g_return_val_if_fail(GST_VIDEO_DECODER_CLASS(parent_class) != nullptr, FALSE);
    GstVdecBase *self = GST_VDEC_BASE(decoder);
    GST_DEBUG_OBJECT(self, "Gst_vdec_base_sink_event, type=%s", GST_EVENT_TYPE_NAME(event));

    gboolean ret = TRUE;
    switch (GST_EVENT_TYPE(event)) {
        case GST_EVENT_FLUSH_START:
            {
                GST_WARNING_OBJECT(self, "KPI-TRACE-VDEC: flush start");
                if (!self->prepared) {
                    gst_vdec_base_set_flushing(self, TRUE);
                    break;
                }
                GST_VIDEO_DECODER_STREAM_LOCK(self);
                GstVdecBaseClass *kclass = GST_VDEC_BASE_GET_CLASS(self);
                if (kclass->flush_cache_slice_buffer != nullptr) {
                    (void)kclass->flush_cache_slice_buffer(self);
                }
                gst_vdec_base_set_flushing(self, TRUE);
                self->decoder_start = FALSE;
                if (self->decoder != nullptr) {
                    (void)self->decoder->Flush(GST_CODEC_ALL);
                }
                GST_VIDEO_DECODER_STREAM_UNLOCK(self);
            }
            break;
        case GST_EVENT_FLUSH_STOP:
            self->flushing_stoping = TRUE;
            ret = GST_VIDEO_DECODER_CLASS(parent_class)->sink_event(decoder, event);
            {
                g_mutex_lock(&self->lock);
                std::list<GstClockTime> empty;
                self->pts_list.swap(empty);
                self->last_pts = GST_CLOCK_TIME_NONE;
                g_mutex_unlock(&self->lock);
            }
            gst_vdec_base_set_flushing(self, FALSE);
            self->flushing_stoping = FALSE;
            GST_WARNING_OBJECT(self, "KPI-TRACE-VDEC: flush stop");
            return ret;
        default:
            break;
    }

    ret = GST_VIDEO_DECODER_CLASS(parent_class)->sink_event(decoder, event);
    return ret;
}

static GstBufferPool *gst_vdec_base_new_out_shmem_pool(GstVdecBase *self, GstCaps *outcaps, gint size)
{
    g_return_val_if_fail(outcaps != nullptr, nullptr);
    GstShMemPool *pool = gst_shmem_pool_new();
    g_return_val_if_fail(pool != nullptr, nullptr);
    ON_SCOPE_EXIT(0) { gst_object_unref(pool); };
    g_return_val_if_fail(self->output.allocator != nullptr, nullptr);

    self->output.av_shmem_pool = std::make_shared<OHOS::Media::AVSharedMemoryPool>("vdec_out");
    (void)gst_shmem_pool_set_avshmempool(pool, self->output.av_shmem_pool);
    (void)gst_shmem_allocator_set_pool(self->output.allocator, self->output.av_shmem_pool);
    GstStructure *config = gst_buffer_pool_get_config(GST_BUFFER_POOL_CAST(pool));
    g_return_val_if_fail(config != nullptr, nullptr);
    gst_buffer_pool_config_set_allocator(config, GST_ALLOCATOR_CAST(self->output.allocator),
            &self->output.allocParams);
    gst_buffer_pool_config_set_params(config, outcaps, size, self->out_buffer_cnt, self->out_buffer_cnt);
    g_return_val_if_fail(gst_buffer_pool_set_config(GST_BUFFER_POOL_CAST(pool), config), nullptr);
    CANCEL_SCOPE_EXIT_GUARD(0);
    return GST_BUFFER_POOL(pool);
}

static GstBufferPool *gst_vdec_base_new_in_shmem_pool(GstVdecBase *self, GstCaps *outcaps, gint size,
    guint min_buffer_cnt, guint max_buffer_cnt)
{
    g_return_val_if_fail(outcaps != nullptr, nullptr);
    GstShMemPool *pool = gst_shmem_pool_new();
    g_return_val_if_fail(pool != nullptr, nullptr);
    ON_SCOPE_EXIT(0) { gst_object_unref(pool); };
    g_return_val_if_fail(self->input.allocator != nullptr, nullptr);

    self->input.av_shmem_pool = std::make_shared<OHOS::Media::AVSharedMemoryPool>("vdec_in");
    (void)gst_shmem_pool_set_avshmempool(pool, self->input.av_shmem_pool);
    (void)gst_shmem_allocator_set_pool(self->input.allocator, self->input.av_shmem_pool);
    GstStructure *config = gst_buffer_pool_get_config(GST_BUFFER_POOL_CAST(pool));
    g_return_val_if_fail(config != nullptr, nullptr);
    gst_buffer_pool_config_set_allocator(config, GST_ALLOCATOR_CAST(self->input.allocator), &self->input.allocParams);
    gst_buffer_pool_config_set_params(config, outcaps, size, min_buffer_cnt, max_buffer_cnt);
    g_return_val_if_fail(gst_buffer_pool_set_config(GST_BUFFER_POOL_CAST(pool), config), nullptr);
    CANCEL_SCOPE_EXIT_GUARD(0);
    return GST_BUFFER_POOL(pool);
}

static void gst_vdec_base_update_out_pool(GstVdecBase *self, GstBufferPool **pool, GstCaps *outcaps,
    gint size)
{
    g_return_if_fail(*pool != nullptr);
    ON_SCOPE_EXIT(0) { gst_object_unref(*pool); *pool = nullptr; };
    GstStructure *config = gst_buffer_pool_get_config(*pool);
    g_return_if_fail(config != nullptr);
    gst_buffer_pool_config_set_params(config, outcaps, size, self->out_buffer_cnt, self->out_buffer_cnt);
    if (self->memtype == GST_MEMTYPE_SURFACE) {
        gst_structure_set(config, "usage", G_TYPE_UINT64, self->usage, nullptr);
    }
    g_return_if_fail(gst_buffer_pool_set_config(*pool, config));
    CANCEL_SCOPE_EXIT_GUARD(0);
}

static gboolean gst_vdec_base_check_mem_type(GstVdecBase *self, GstQuery *query)
{
    guint index = 0;
    const GstStructure *buffer_type_struct = nullptr;
    if (gst_query_find_allocation_meta(query, GST_BUFFER_TYPE_META_API_TYPE, &index)) {
        (void)gst_query_parse_nth_allocation_meta(query, index, &buffer_type_struct);
        (void)get_memtype(self, buffer_type_struct);
    } else {
        GST_INFO_OBJECT(self, "No meta api");
        return FALSE;
    }
    return TRUE;
}

static gboolean gst_vdec_base_decide_allocation_with_pre_init_pool(GstVdecBase *self, GstQuery *query)
{
    g_return_val_if_fail(self != nullptr && query != nullptr, FALSE);
    GST_DEBUG_OBJECT(self, "Performance mode");
    guint pool_num = gst_query_get_n_allocation_pools(query);
    gboolean update_pool = FALSE;
    if (pool_num > 0) {
        update_pool = TRUE;
    }
    GstBufferPool *pool = self->outpool;
    guint size = self->output.buffer_size;
    if (update_pool) {
        gst_query_set_nth_allocation_pool(query, 0, pool, size, self->output.buffer_cnt, self->output.buffer_cnt);
    } else {
        gst_query_add_allocation_pool(query, pool, size, self->output.buffer_cnt, self->output.buffer_cnt);
    }
    return TRUE;
}

static gboolean gst_vdec_base_decide_allocation(GstVideoDecoder *decoder, GstQuery *query)
{
    g_return_val_if_fail(decoder != nullptr && query != nullptr, FALSE);
    GstVdecBase *self = GST_VDEC_BASE(decoder);
    if (self->performance_mode && self->pre_init_pool) {
        self->pre_init_pool = FALSE;
        return gst_vdec_base_decide_allocation_with_pre_init_pool(self, query);
    }
    GstCaps *outcaps = nullptr;
    GstVideoInfo vinfo;
    GstBufferPool *pool = nullptr;
    guint size = 0;
    guint min_buf = 0;
    guint max_buf = 0;
    gst_query_parse_allocation(query, &outcaps, nullptr);
    gst_video_info_init(&vinfo);
    if (outcaps != nullptr) {
        gst_video_info_from_caps(&vinfo, outcaps);
    }
    gboolean update_pool = FALSE;
    guint pool_num = gst_query_get_n_allocation_pools(query);
    if (pool_num > 0) {
        gst_query_parse_nth_allocation_pool(query, 0, &pool, &size, &min_buf, &max_buf);
        if (gst_vdec_base_check_mem_type(self, query) != TRUE) {
            gst_object_unref(pool);
            pool = nullptr;
        }
        update_pool = TRUE;
    }
    size = vinfo.size;
    self->out_buffer_max_cnt = max_buf == 0 ? DEFAULT_MAX_QUEUE_SIZE : max_buf;
    g_return_val_if_fail(gst_vdec_base_update_out_port_def(self, &size), FALSE);
    if (pool == nullptr) {
        pool = gst_vdec_base_new_out_shmem_pool(self, outcaps, size);
    } else {
        gst_vdec_base_update_out_pool(self, &pool, outcaps, size);
    }
    g_return_val_if_fail(pool != nullptr, FALSE);
    if (update_pool) {
        gst_query_set_nth_allocation_pool(query, 0, pool, size, self->output.buffer_cnt, self->output.buffer_cnt);
    } else {
        gst_query_add_allocation_pool(query, pool, size, self->output.buffer_cnt, self->output.buffer_cnt);
    }
    GST_DEBUG_OBJECT(decoder, "Pool ref %u", (reinterpret_cast<GObject*>(pool)->ref_count));
    gst_object_unref(self->outpool);
    self->outpool = pool;
    return TRUE;
}

static gboolean gst_vdec_base_check_allocate_input(GstVdecBase *self)
{
    MediaTrace::TraceBegin("VdecBase::AllocateInputBuffer", FAKE_POINTER(self));
    g_return_val_if_fail(self != nullptr, FALSE);
    if (self->inpool == nullptr) {
        self->inpool = gst_vdec_base_new_in_shmem_pool(self, self->input_state->caps, self->input.buffer_size,
                self->input.buffer_cnt, self->input.buffer_cnt);
        gst_buffer_pool_set_active(self->inpool, TRUE);
        g_return_val_if_fail(self->inpool != nullptr, FALSE);
        g_return_val_if_fail(gst_vdec_base_allocate_in_buffers(self), FALSE);
    }
    MediaTrace::TraceEnd("VdecBase::AllocateInputBuffer", FAKE_POINTER(self));
    return TRUE;
}

static gboolean gst_vdec_base_propose_allocation(GstVideoDecoder *decoder, GstQuery *query)
{
    g_return_val_if_fail(decoder != nullptr, FALSE);
    g_return_val_if_fail(query != nullptr, FALSE);
    GstVdecBase *self = GST_VDEC_BASE(decoder);
    GstCaps *incaps = nullptr;
    GstBufferPool *pool = nullptr;
    guint size = 0;
    gboolean update_pool = FALSE;
    gst_query_parse_allocation(query, &incaps, nullptr);
    guint pool_num = gst_query_get_n_allocation_pools(query);
    if (pool_num > 0) {
        GST_DEBUG_OBJECT(decoder, "Get bufferpool num %u", pool_num);
        update_pool = TRUE;
    }
    size = self->input.buffer_size;
    pool = gst_vdec_base_new_in_shmem_pool(self, incaps, size, self->input.buffer_cnt, self->input.buffer_cnt);
    g_return_val_if_fail(pool != nullptr, FALSE);
    if (update_pool) {
        gst_query_set_nth_allocation_pool(query, 0, pool, size, self->input.buffer_cnt, self->input.buffer_cnt);
    } else {
        gst_query_add_allocation_pool(query, pool, size, self->input.buffer_cnt, self->input.buffer_cnt);
    }
    if (self->inpool) {
        gst_object_unref(self->inpool);
    }
    self->inpool = pool;
    GST_DEBUG_OBJECT(decoder, "pool ref_count %u", ((GObject *)pool)->ref_count);
    gst_buffer_pool_set_active(pool, TRUE);
    g_return_val_if_fail(gst_vdec_base_allocate_in_buffers(self), FALSE);
    gst_query_add_allocation_meta(query, GST_BUFFER_TYPE_META_API_TYPE, nullptr);
    return TRUE;
}