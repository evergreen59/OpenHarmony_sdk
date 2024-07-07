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

#include "config.h"
#include "gst_audio_server_sink.h"
#include <cinttypes>
#include <gst/gst.h>
#include "gst/audio/audio.h"
#include "media_errors.h"
#include "media_log.h"
#include "audio_sink_factory.h"

static GstStaticPadTemplate g_sinktemplate = GST_STATIC_PAD_TEMPLATE("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS("audio/x-raw, "
        "format = (string) S16LE, "
        "layout = (string) interleaved, "
        "rate = (int) [ 1, MAX ], "
        "channels = (int) [ 1, MAX ]"));

using namespace OHOS::Media;
namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "audio_server_sink"};
    constexpr float DEFAULT_VOLUME = 1.0f;
    constexpr uint32_t DEFAULT_BITS_PER_SAMPLE = 16;
    constexpr uint64_t DEFAULT_AUDIO_RENDER_DELAY = 145000; // unit us, empirical value
}

enum {
    PROP_0,
    PROP_BITS_PER_SAMPLE,
    PROP_CHANNELS,
    PROP_SAMPLE_RATE,
    PROP_APP_UID,
    PROP_APP_PID,
    PROP_VOLUME,
    PROP_MAX_VOLUME,
    PROP_MIN_VOLUME,
    PROP_AUDIO_RENDERER_DESC,
    PROP_AUDIO_RENDERER_FLAG,
    PROP_AUDIO_INTERRUPT_MODE,
    PROP_LAST_RENDER_PTS,
    PROP_ENABLE_OPT_RENDER_DELAY,
    PROP_LAST_RUNNING_TIME_DIFF,
};

#define gst_audio_server_sink_parent_class parent_class
G_DEFINE_TYPE(GstAudioServerSink, gst_audio_server_sink, GST_TYPE_BASE_SINK);

static void gst_audio_server_sink_finalize(GObject *object);
static void gst_audio_server_sink_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void gst_audio_server_sink_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static GstStateChangeReturn gst_audio_server_sink_change_state(GstElement *element, GstStateChange transition);
static GstCaps *gst_audio_server_sink_get_caps(GstBaseSink *basesink, GstCaps *caps);
static gboolean gst_audio_server_sink_set_caps(GstBaseSink *basesink, GstCaps *caps);
static gboolean gst_audio_server_sink_event(GstBaseSink *basesink, GstEvent *event);
static gboolean gst_audio_server_sink_start(GstBaseSink *basesink);
static gboolean gst_audio_server_sink_stop(GstBaseSink *basesink);
static GstFlowReturn gst_audio_server_sink_render(GstBaseSink *basesink, GstBuffer *buffer);
static void gst_audio_server_sink_clear_cache_buffer(GstAudioServerSink *sink);
static GstClockTime gst_audio_server_sink_update_reach_time(GstBaseSink *basesink, GstClockTime reach_time);

static void gst_audio_server_sink_class_init(GstAudioServerSinkClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GstElementClass *gstelement_class = GST_ELEMENT_CLASS(klass);
    GstBaseSinkClass *gstbasesink_class = GST_BASE_SINK_CLASS(klass);
    g_return_if_fail((gobject_class != nullptr) && (gstelement_class != nullptr) && (gstbasesink_class != nullptr));

    gobject_class->finalize = gst_audio_server_sink_finalize;
    gobject_class->set_property = gst_audio_server_sink_set_property;
    gobject_class->get_property = gst_audio_server_sink_get_property;

    g_signal_new("interrupt-event", G_TYPE_FROM_CLASS(klass),
        static_cast<GSignalFlags>(G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION), 0, NULL,
        NULL, NULL, G_TYPE_NONE, 3, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT); // 3 parameters

    g_signal_new("audio-state-event", G_TYPE_FROM_CLASS(klass),
        static_cast<GSignalFlags>(G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION), 0, NULL,
        NULL, NULL, G_TYPE_NONE, 1, G_TYPE_UINT); // 1 parameters

    g_object_class_install_property(gobject_class, PROP_BITS_PER_SAMPLE,
        g_param_spec_uint("bps", "Bits Per Sample",
            "Audio Format", 0, G_MAXINT32, 0,
            (GParamFlags)(G_PARAM_READABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_CHANNELS,
        g_param_spec_uint("channels", "Channels",
            "Channels", 0, G_MAXINT32, 0,
            (GParamFlags)(G_PARAM_READABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_SAMPLE_RATE,
        g_param_spec_uint("sample-rate", "Sample Rate",
            "Sample Rate", 0, G_MAXINT32, 0,
            (GParamFlags)(G_PARAM_READABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_APP_UID,
        g_param_spec_int("app-uid", "Appuid",
            "APP UID", 0, G_MAXINT32, 0,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_APP_PID,
        g_param_spec_int("app-pid", "Apppid",
            "APP PID", 0, G_MAXINT32, 0,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_VOLUME,
        g_param_spec_float("volume", "Volume",
            "Volume", 0, G_MAXFLOAT, 0,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_MAX_VOLUME,
        g_param_spec_float("max-volume", "Maximum Volume",
            "Maximum Volume", 0, G_MAXFLOAT, G_MAXFLOAT,
            (GParamFlags)(G_PARAM_READABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_MIN_VOLUME,
        g_param_spec_float("min-volume", "Minimum Volume",
            "Minimum Volume", 0, G_MAXFLOAT, 0,
            (GParamFlags)(G_PARAM_READABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_AUDIO_RENDERER_DESC,
        g_param_spec_int("audio-renderer-desc", "Audio Renderer Desc",
            "Audio Renderer Desc", 0, G_MAXINT32, 0,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_AUDIO_RENDERER_FLAG,
        g_param_spec_int("audio-renderer-flag", "Audio Renderer Flag",
            "Audio Renderer Flag", 0, G_MAXINT32, 0,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_AUDIO_INTERRUPT_MODE,
        g_param_spec_int("audio-interrupt-mode", "Audio Interrupt Mode",
            "Audio Interrupt Mode", 0, G_MAXINT32, 0,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_LAST_RENDER_PTS,
        g_param_spec_uint64("last-render-pts", "last-render-pts", "last render pts", 0, G_MAXUINT64,
            0, (GParamFlags)(G_PARAM_READABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_ENABLE_OPT_RENDER_DELAY,
        g_param_spec_boolean("enable-opt-render-delay", "enable opt render delay",
            "If TRUE, use DEFAULT_AUDIO_RENDER_DELAY instead of the latency provided by AudioStandard",
            FALSE, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_LAST_RUNNING_TIME_DIFF,
        g_param_spec_int64("last-running-time-diff", "last running time diff", "last running time diff",
            0, G_MAXINT64, 0, (GParamFlags)(G_PARAM_READABLE | G_PARAM_STATIC_STRINGS)));

    gst_element_class_set_static_metadata(gstelement_class,
        "Audio server sink", "Sink/Audio",
        "Push pcm data to Audio server", "OpenHarmony");

    gst_element_class_add_static_pad_template(gstelement_class, &g_sinktemplate);

    gstelement_class->change_state = gst_audio_server_sink_change_state;

    gstbasesink_class->get_caps = gst_audio_server_sink_get_caps;
    gstbasesink_class->set_caps = gst_audio_server_sink_set_caps;
    gstbasesink_class->event = gst_audio_server_sink_event;
    gstbasesink_class->start = gst_audio_server_sink_start;
    gstbasesink_class->stop = gst_audio_server_sink_stop;
    gstbasesink_class->render = gst_audio_server_sink_render;
    gstbasesink_class->update_reach_time = gst_audio_server_sink_update_reach_time;
}

static void gst_audio_server_sink_init(GstAudioServerSink *sink)
{
    g_return_if_fail(sink != nullptr);
    sink->audio_sink = nullptr;
    sink->bits_per_sample = DEFAULT_BITS_PER_SAMPLE;
    sink->channels = 0;
    sink->sample_rate = 0;
    sink->appuid = 0;
    sink->apppid = 0;
    sink->volume = DEFAULT_VOLUME;
    sink->max_volume = G_MAXFLOAT;
    sink->min_volume = 0;
    sink->min_buffer_size = 0;
    sink->min_frame_count = 0;
    sink->pause_cache_buffer = nullptr;
    sink->frame_after_segment = FALSE;
    sink->renderer_desc = 0;
    sink->renderer_flag = 0;
    g_mutex_init(&sink->render_lock);
    sink->last_render_pts = 0;
    sink->enable_opt_render_delay = FALSE;
    sink->last_running_time_diff = 0;
}

static void gst_audio_server_sink_finalize(GObject *object)
{
    g_return_if_fail(object != nullptr);
    GstAudioServerSink *sink = GST_AUDIO_SERVER_SINK(object);
    g_return_if_fail(sink != nullptr);
    GST_INFO_OBJECT(sink, "gst_audio_server_sink_finalize in");

    g_mutex_clear(&sink->render_lock);
    if (sink->audio_sink != nullptr) {
        (void)sink->audio_sink->Release();
        sink->audio_sink = nullptr;
    }
    gst_audio_server_sink_clear_cache_buffer(sink);

    G_OBJECT_CLASS(parent_class)->finalize(object);
}

static gboolean gst_audio_server_sink_set_volume(GstAudioServerSink *sink, gfloat volume)
{
    gboolean ret = FALSE;
    g_return_val_if_fail(sink != nullptr, FALSE);
    g_return_val_if_fail(sink->audio_sink != nullptr, FALSE);
    g_return_val_if_fail(volume <= sink->max_volume, FALSE);
    g_return_val_if_fail(volume >= sink->min_volume, FALSE);

    if (sink->audio_sink->SetVolume(volume) == MSERR_OK) {
        sink->volume = volume;
        ret = TRUE;
    }

    GST_INFO_OBJECT(sink, "set volume(%f) finish, ret=%d", volume, ret);
    return ret;
}

static void gst_audio_server_sink_interrupt_callback(GstBaseSink *basesink,
    guint eventType, guint forceType, guint hintType)
{
    GstAudioServerSink *sink = GST_AUDIO_SERVER_SINK(basesink);
    g_signal_emit_by_name(sink, "interrupt-event", eventType, forceType, hintType);
}

static void gst_audio_server_sink_state_callback(GstBaseSink *basesink, guint state)
{
    GstAudioServerSink *sink = GST_AUDIO_SERVER_SINK(basesink);
    g_signal_emit_by_name(sink, "audio-state-event", state);
}

static void gst_audio_server_sink_error_callback(GstBaseSink *basesink, const std::string &errMsg)
{
    MEDIA_LOGE("audio render error: %{public}s", errMsg.c_str());
    GstAudioServerSink *sink = GST_AUDIO_SERVER_SINK(basesink);
    GST_ELEMENT_ERROR(sink, STREAM, FAILED, (NULL), ("audio render error: %s", errMsg.c_str()));
}

static void gst_audio_server_sink_set_property(GObject *object, guint prop_id,
    const GValue *value, GParamSpec *pspec)
{
    g_return_if_fail(object != nullptr);
    g_return_if_fail(value != nullptr);
    (void)pspec;
    GstAudioServerSink *sink = GST_AUDIO_SERVER_SINK(object);
    g_return_if_fail(sink != nullptr);
    switch (prop_id) {
        case PROP_VOLUME:
            if (gst_audio_server_sink_set_volume(sink, g_value_get_float(value))) {
                GST_INFO_OBJECT(sink, "set volume success!");
                g_object_notify(G_OBJECT(sink), "volume");
            }
            break;
        case PROP_AUDIO_RENDERER_DESC:
            sink->renderer_desc = g_value_get_int(value);
            break;
        case PROP_APP_UID:
            sink->appuid = g_value_get_int(value);
            GST_INFO_OBJECT(sink, "set app uid success!");
            g_object_notify(G_OBJECT(sink), "app-uid");
            break;
        case PROP_APP_PID:
            sink->apppid = g_value_get_int(value);
            GST_INFO_OBJECT(sink, "set app uid success!");
            g_object_notify(G_OBJECT(sink), "app-pid");
            break;
        case PROP_AUDIO_RENDERER_FLAG:
            sink->renderer_flag = g_value_get_int(value);
            break;
        case PROP_AUDIO_INTERRUPT_MODE:
            g_return_if_fail(sink->audio_sink != nullptr);
            sink->audio_sink->SetAudioInterruptMode(g_value_get_int(value));
            break;
        case PROP_ENABLE_OPT_RENDER_DELAY:
            sink->enable_opt_render_delay = g_value_get_boolean(value);
            break;
        default:
            break;
    }
}

static void gst_audio_server_sink_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    g_return_if_fail(object != nullptr);
    g_return_if_fail(value != nullptr);
    (void)pspec;
    GstAudioServerSink *sink = GST_AUDIO_SERVER_SINK(object);
    g_return_if_fail(sink != nullptr);
    switch (prop_id) {
        case PROP_BITS_PER_SAMPLE:
            g_value_set_uint(value, sink->bits_per_sample);
            break;
        case PROP_CHANNELS:
            g_value_set_uint(value, sink->channels);
            break;
        case PROP_SAMPLE_RATE:
            g_value_set_uint(value, sink->sample_rate);
            break;
        case PROP_VOLUME:
            if (sink->audio_sink != nullptr) {
                (void)sink->audio_sink->GetVolume(sink->volume);
            }
            g_value_set_float(value, sink->volume);
            break;
        case PROP_MAX_VOLUME:
            g_value_set_float(value, sink->max_volume);
            break;
        case PROP_MIN_VOLUME:
            g_value_set_float(value, sink->min_volume);
            break;
        case PROP_LAST_RENDER_PTS:
            g_mutex_lock(&sink->render_lock);
            g_value_set_uint64(value, static_cast<guint64>(sink->last_render_pts));
            g_mutex_unlock(&sink->render_lock);
            break;
        case PROP_LAST_RUNNING_TIME_DIFF:
            g_mutex_lock(&sink->render_lock);
            g_value_set_int64(value, static_cast<gint64>(sink->last_running_time_diff));
            g_mutex_unlock(&sink->render_lock);
            break;
        default:
            break;
    }
}

static GstCaps *gst_audio_server_sink_get_caps(GstBaseSink *basesink, GstCaps *caps)
{
    (void)caps;
    GstAudioServerSink *sink = GST_AUDIO_SERVER_SINK(basesink);
    g_return_val_if_fail(sink != nullptr, FALSE);
    g_return_val_if_fail(sink->audio_sink != nullptr, FALSE);
    return sink->audio_sink->GetCaps();
}

static GstClockTime gst_audio_server_sink_update_reach_time(GstBaseSink *basesink, GstClockTime reach_time)
{
    g_return_val_if_fail(basesink != nullptr, reach_time);
    GstAudioServerSink *sink = GST_AUDIO_SERVER_SINK(basesink);
    GstClockTime base_time = gst_element_get_base_time(GST_ELEMENT(basesink)); // get base time
    GstClockTime cur_clock_time = gst_clock_get_time(GST_ELEMENT_CLOCK(basesink)); // get current clock time
    if (!GST_CLOCK_TIME_IS_VALID(base_time) || !GST_CLOCK_TIME_IS_VALID(cur_clock_time)) {
        return reach_time;
    }
    if (cur_clock_time < base_time) {
        return reach_time;
    }
    GstClockTime cur_running_time = cur_clock_time - base_time; // get running time
    g_mutex_lock(&sink->render_lock);
    sink->last_running_time_diff =
        static_cast<GstClockTimeDiff>(cur_running_time) - static_cast<GstClockTimeDiff>(reach_time);
    g_mutex_unlock(&sink->render_lock);

    return GST_BASE_SINK_CLASS(parent_class)->update_reach_time(basesink, reach_time);
}

static gboolean gst_audio_server_sink_set_caps(GstBaseSink *basesink, GstCaps *caps)
{
    g_return_val_if_fail(basesink != nullptr, FALSE);
    g_return_val_if_fail(caps != nullptr, FALSE);
    GstAudioServerSink *sink = GST_AUDIO_SERVER_SINK(basesink);
    g_return_val_if_fail(sink != nullptr, FALSE);
    g_return_val_if_fail(sink->audio_sink != nullptr, FALSE);

    GST_INFO_OBJECT(basesink, "caps=%" GST_PTR_FORMAT, caps);
    GstStructure *structure = gst_caps_get_structure(caps, 0);
    g_return_val_if_fail(structure != nullptr, FALSE);
    gint channels = 0;
    gint rate = 0;
    if (!gst_structure_get_int(structure, "rate", &rate) || !gst_structure_get_int(structure, "channels", &channels)) {
        GST_ERROR_OBJECT(basesink, "Incomplete caps");
        return FALSE;
    }
    g_return_val_if_fail(channels > 0 && rate > 0, FALSE);
    sink->sample_rate = static_cast<uint32_t>(rate);
    sink->channels = static_cast<uint32_t>(channels);
    g_return_val_if_fail(sink->audio_sink->SetParameters(sink->bits_per_sample, sink->channels,
        sink->sample_rate) == MSERR_OK, FALSE);
    g_return_val_if_fail(sink->audio_sink->SetVolume(sink->volume) == MSERR_OK, FALSE);
    g_return_val_if_fail(sink->audio_sink->GetParameters(sink->bits_per_sample,
        sink->channels, sink->sample_rate) == MSERR_OK, FALSE);
    g_return_val_if_fail(sink->audio_sink->GetMinimumBufferSize(sink->min_buffer_size) == MSERR_OK, FALSE);
    g_return_val_if_fail(sink->audio_sink->GetMinimumFrameCount(sink->min_frame_count) == MSERR_OK, FALSE);

    if (GST_STATE(sink) == GST_STATE_PLAYING) {
        g_return_val_if_fail(sink->audio_sink->Start() == MSERR_OK, FALSE);
    }

    return TRUE;
}

static gboolean gst_audio_server_sink_event(GstBaseSink *basesink, GstEvent *event)
{
    g_return_val_if_fail(basesink != nullptr, FALSE);
    g_return_val_if_fail(event != nullptr, FALSE);
    GstAudioServerSink *sink = GST_AUDIO_SERVER_SINK(basesink);
    g_return_val_if_fail(sink != nullptr, FALSE);
    GstEventType type = GST_EVENT_TYPE(event);
    switch (type) {
        case GST_EVENT_EOS:
            if (sink->audio_sink == nullptr) {
                break;
            }
            if (sink->audio_sink->Drain() != MSERR_OK) {
                GST_ERROR_OBJECT(basesink, "fail to call Drain when handling EOS event");
            }
            break;
        case GST_EVENT_SEGMENT:
            g_mutex_lock(&sink->render_lock);
            sink->frame_after_segment = TRUE;
            g_mutex_unlock(&sink->render_lock);
            break;
        case GST_EVENT_FLUSH_START:
            basesink->stream_group_done = FALSE;
            gst_audio_server_sink_clear_cache_buffer(sink);
            if (sink->audio_sink == nullptr) {
                break;
            }
            if (sink->audio_sink->Flush() != MSERR_OK) {
                GST_ERROR_OBJECT(basesink, "fail to call Flush when handling SEEK event");
            }
            GST_DEBUG_OBJECT(basesink, "received FLUSH_START");
            break;
        case GST_EVENT_FLUSH_STOP:
            GST_DEBUG_OBJECT(basesink, "received FLUSH_STOP");
            break;
        case GST_EVENT_STREAM_GROUP_DONE:
            basesink->stream_group_done = TRUE;
            GST_DEBUG_OBJECT(basesink, "received STREAM_GROUP_DONE, set stream_group_done TRUE");
            if (basesink->need_preroll) {
                /* may async start to change state, preroll STREAM_GROUP_DONE to async done */
                gst_base_sink_do_preroll (basesink, GST_MINI_OBJECT_CAST(event));
            }
            break;
        case GST_EVENT_STREAM_START:
            basesink->stream_group_done = FALSE;
            GST_DEBUG_OBJECT(basesink, "received STREAM_START, set stream_group_done FALSE");
            break;
        default:
            break;
    }
    return GST_BASE_SINK_CLASS(parent_class)->event(basesink, event);
}

static gboolean gst_audio_server_sink_start(GstBaseSink *basesink)
{
    g_return_val_if_fail(basesink != nullptr, FALSE);
    GstAudioServerSink *sink = GST_AUDIO_SERVER_SINK(basesink);
    MEDIA_LOGI("uid: %{public}d, pid: %{public}d", sink->appuid, sink->apppid);
    g_return_val_if_fail(sink != nullptr, FALSE);
    sink->audio_sink = OHOS::Media::AudioSinkFactory::CreateAudioSink(basesink);
    g_return_val_if_fail(sink->audio_sink != nullptr, FALSE);
    g_return_val_if_fail(sink->audio_sink->SetRendererInfo(sink->renderer_desc,
        sink->renderer_flag) == MSERR_OK, FALSE);
    g_return_val_if_fail(sink->audio_sink->Prepare(sink->appuid, sink->apppid) == MSERR_OK, FALSE);
    sink->audio_sink->SetAudioSinkCb(gst_audio_server_sink_interrupt_callback,
                                     gst_audio_server_sink_state_callback,
                                     gst_audio_server_sink_error_callback);
    g_return_val_if_fail(sink->audio_sink->GetMaxVolume(sink->max_volume) == MSERR_OK, FALSE);
    g_return_val_if_fail(sink->audio_sink->GetMinVolume(sink->min_volume) == MSERR_OK, FALSE);

    return TRUE;
}

static void gst_audio_server_sink_clear_cache_buffer(GstAudioServerSink *sink)
{
    std::unique_lock<std::mutex> lock(sink->mutex_);
    if (sink->pause_cache_buffer != nullptr) {
        gst_buffer_unref(sink->pause_cache_buffer);
        sink->pause_cache_buffer = nullptr;
    }
}

static gboolean gst_audio_server_sink_stop(GstBaseSink *basesink)
{
    g_return_val_if_fail(basesink != nullptr, FALSE);
    GstAudioServerSink *sink = GST_AUDIO_SERVER_SINK(basesink);
    g_return_val_if_fail(sink != nullptr, FALSE);
    g_return_val_if_fail(sink->audio_sink != nullptr, FALSE);
    g_return_val_if_fail(sink->audio_sink->Stop() == MSERR_OK, FALSE);
    g_return_val_if_fail(sink->audio_sink->Release() == MSERR_OK, FALSE);
    sink->audio_sink = nullptr;

    return TRUE;
}

static GstStateChangeReturn gst_audio_server_sink_change_state(GstElement *element, GstStateChange transition)
{
    g_return_val_if_fail(element != nullptr, GST_STATE_CHANGE_FAILURE);
    GstAudioServerSink *sink = GST_AUDIO_SERVER_SINK(element);
    g_return_val_if_fail(sink != nullptr, GST_STATE_CHANGE_FAILURE);
    GstBaseSink *basesink = GST_BASE_SINK(element);
    g_return_val_if_fail(basesink != nullptr, GST_STATE_CHANGE_FAILURE);

    switch (transition) {
        case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
            MEDIA_LOGD("GST_STATE_CHANGE_PAUSED_TO_PLAYING");
            g_return_val_if_fail(sink->audio_sink != nullptr, GST_STATE_CHANGE_FAILURE);
            if (sink->audio_sink->Start() != MSERR_OK) {
                GST_ERROR_OBJECT(sink, "audio sink Start failed!");
                GST_ELEMENT_ERROR(sink, STREAM, FAILED, ("audio sink Start failed!"), (NULL));
                return GST_STATE_CHANGE_FAILURE;
            }

            if (sink->pause_cache_buffer != nullptr) {
                GST_INFO_OBJECT(basesink, "pause to play");
                if (gst_audio_server_sink_render(basesink, sink->pause_cache_buffer) != GST_FLOW_OK) {
                    GST_ERROR_OBJECT(sink, "audio sink gst_audio_server_sink_render failed!");
                    GST_ELEMENT_ERROR(sink, STREAM, FAILED,
                        ("audio sink gst_audio_server_sink_render failed!"), (NULL));
                    return GST_STATE_CHANGE_FAILURE;
                }
                gst_buffer_unref(sink->pause_cache_buffer);
                sink->pause_cache_buffer = nullptr;
            }
            break;
        default:
            break;
    }
    GstStateChangeReturn ret = GST_ELEMENT_CLASS(parent_class)->change_state(element, transition);

    switch (transition) {
        case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
            MEDIA_LOGD("GST_STATE_CHANGE_PLAYING_TO_PAUSED");
            {
                std::unique_lock<std::mutex> lock(sink->mutex_);
                g_return_val_if_fail(sink->audio_sink != nullptr, GST_STATE_CHANGE_FAILURE);
                /**
                 * When executing release task, call stop first.
                 * if report pause fail message, it will abort stop task. Then,
                 * release task will free resources. Due to stop failed, the plugin
                 * not stopped will use destoryed mutex. It will lead to service crash.
                 */
                (void)sink->audio_sink->Pause();
            }
            break;
        case GST_STATE_CHANGE_PAUSED_TO_READY:
            MEDIA_LOGD("GST_STATE_CHANGE_PAUSED_TO_READY");
            gst_audio_server_sink_clear_cache_buffer(sink);
            g_mutex_lock(&sink->render_lock);
            sink->last_render_pts = 0;
            sink->last_running_time_diff = 0;
            g_mutex_unlock(&sink->render_lock);
            break;
        default:
            break;
    }

    return ret;
}

static void gst_audio_server_sink_get_latency(GstAudioServerSink *sink, const GstBuffer *buffer)
{
    g_mutex_lock(&sink->render_lock);
    if (sink->frame_after_segment) {
        sink->frame_after_segment = FALSE;
        uint64_t latency = 0;
        GST_INFO_OBJECT(sink, "the first audio frame after segment has been sent to audio server");
        if (sink->audio_sink->GetLatency(latency) != MSERR_OK) {
            GST_INFO_OBJECT(sink, "fail to get latency");
        } else {
            GST_INFO_OBJECT(sink, "frame render latency is (%" PRIu64 ")", latency);
        }
        if (sink->enable_opt_render_delay) {
            /* the latency provided by GetLatency() is not accurate.
             * so we set DEFAULT_AUDIO_RENDER_DELAY to basesink.
             */
            gst_base_sink_set_render_delay(GST_BASE_SINK(sink), DEFAULT_AUDIO_RENDER_DELAY * GST_USECOND);
        }
    }
    if (GST_CLOCK_TIME_IS_VALID(GST_BUFFER_PTS(buffer))) {
        sink->last_render_pts = GST_BUFFER_PTS(buffer);
    }
    g_mutex_unlock(&sink->render_lock);
}

static GstFlowReturn gst_audio_server_sink_render(GstBaseSink *basesink, GstBuffer *buffer)
{
    g_return_val_if_fail(basesink != nullptr, GST_FLOW_ERROR);
    g_return_val_if_fail(buffer != nullptr, GST_FLOW_ERROR);
    if (gst_buffer_get_size(buffer) == 0) {
        GST_INFO_OBJECT(basesink, "gst_buffer_get_size = 0");
        return GST_FLOW_OK;
    }
    GstAudioServerSink *sink = GST_AUDIO_SERVER_SINK(basesink);
    g_return_val_if_fail(sink != nullptr, GST_FLOW_ERROR);
    g_return_val_if_fail(sink->audio_sink != nullptr, GST_FLOW_ERROR);

    {
        std::unique_lock<std::mutex> lock(sink->mutex_);
        if (!sink->audio_sink->Writeable()) {
            if (sink->pause_cache_buffer == nullptr) {
                sink->pause_cache_buffer = gst_buffer_ref(buffer);
                g_return_val_if_fail(sink->pause_cache_buffer != nullptr, GST_FLOW_ERROR);
                GST_INFO_OBJECT(basesink, "cache buffer for pause state");
                return GST_FLOW_OK;
            } else {
                GST_ERROR_OBJECT(basesink, "cache buffer is not null");
            }
        }

        GstMapInfo map;
        if (gst_buffer_map(buffer, &map, GST_MAP_READ) != TRUE) {
            GST_ERROR_OBJECT(basesink, "unknown error happened during gst_buffer_map");
            return GST_FLOW_ERROR;
        }
        if (sink->audio_sink->Write(map.data, map.size) != MSERR_OK) {
            GST_ERROR_OBJECT(basesink, "unknown error happened during Write");
            gst_buffer_unmap(buffer, &map);
            return GST_FLOW_ERROR;
        }
        gst_buffer_unmap(buffer, &map);
    }

    gst_audio_server_sink_get_latency(sink, buffer);
    return GST_FLOW_OK;
}

static gboolean plugin_init(GstPlugin *plugin)
{
    g_return_val_if_fail(plugin != nullptr, FALSE);
    gboolean ret = gst_element_register(plugin, "audioserversink", GST_RANK_PRIMARY, GST_TYPE_AUDIO_SERVER_SINK);
    return ret;
}

GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    _audio_server_sink,
    "GStreamer Audio Server Sink",
    plugin_init,
    PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
