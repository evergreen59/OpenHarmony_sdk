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
#include "gst_audio_capture_src.h"
#include <gst/gst.h>
#include <gst/audio/audio.h>
#include "media_errors.h"
#include "audio_capture_factory.h"

static GstStaticPadTemplate gst_audio_capture_src_template =
GST_STATIC_PAD_TEMPLATE("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS("audio/x-raw, "
        "format = (string) S16LE, "
        "rate = (int) [ 1, MAX ], "
        "layout = (string) interleaved, "
        "channels = (int) [ 1, MAX ]"));

enum {
    PROP_0,
    PROP_SOURCE_TYPE,
    PROP_SAMPLE_RATE,
    PROP_CHANNELS,
    PROP_BITRATE,
    PROP_TOKEN_ID,
    PROP_APP_UID,
    PROP_APP_PID,
    PROP_BYPASS_AUDIO_SERVICE,
    PROP_SUPPORTED_AUDIO_PARAMS
};

using namespace OHOS::Media;

#define gst_audio_capture_src_parent_class parent_class
G_DEFINE_TYPE(GstAudioCaptureSrc, gst_audio_capture_src, GST_TYPE_PUSH_SRC);

static void gst_audio_capture_src_finalize(GObject *object);
static void gst_audio_capture_src_set_property(GObject *object, guint prop_id,
    const GValue *value, GParamSpec *pspec);
static void gst_audio_capture_src_get_property(GObject *object, guint prop_id,
    GValue *value, GParamSpec *pspec);
static GstFlowReturn gst_audio_capture_src_create(GstPushSrc *psrc, GstBuffer **outbuf);
static GstStateChangeReturn gst_audio_capture_src_change_state(GstElement *element, GstStateChange transition);
static gboolean gst_audio_capture_src_negotiate(GstBaseSrc *basesrc);
static void gst_audio_capture_src_getbuffer_timeout(GstPushSrc *psrc);
static void gst_audio_capture_src_mgr_init(GstAudioCaptureSrc *src);
static void gst_audio_capture_src_mgr_enable_watchdog(GstAudioCaptureSrc *src);
static void gst_audio_capture_src_mgr_disable_watchdog(GstAudioCaptureSrc *src);

void AudioManager::Alarm()
{
    gst_audio_capture_src_getbuffer_timeout(&owner_);
}

#define GST_TYPE_AUDIO_CAPTURE_SRC_SOURCE_TYPE (gst_audio_capture_src_source_type_get_type())
static GType gst_audio_capture_src_source_type_get_type(void)
{
    static GType audio_capture_src_source_type = 0;
    static const GEnumValue source_types[] = {
        {AUDIO_SOURCE_TYPE_DEFAULT, "MIC", "MIC"},
        {AUDIO_SOURCE_TYPE_MIC, "MIC", "MIC"},
        {0, nullptr, nullptr}
    };
    if (!audio_capture_src_source_type) {
        audio_capture_src_source_type = g_enum_register_static("AudioSourceType", source_types);
    }
    return audio_capture_src_source_type;
}

static void gst_audio_capture_src_class_init(GstAudioCaptureSrcClass *klass)
{
    GObjectClass *gobject_class = reinterpret_cast<GObjectClass *>(klass);
    GstElementClass *gstelement_class = reinterpret_cast<GstElementClass *>(klass);
    GstBaseSrcClass *gstbasesrc_class = reinterpret_cast<GstBaseSrcClass *>(klass);
    GstPushSrcClass *gstpushsrc_class = reinterpret_cast<GstPushSrcClass *>(klass);
    g_return_if_fail((gobject_class != nullptr) && (gstelement_class != nullptr) &&
        (gstbasesrc_class != nullptr) && gstpushsrc_class != nullptr);

    gobject_class->finalize = gst_audio_capture_src_finalize;
    gobject_class->set_property = gst_audio_capture_src_set_property;
    gobject_class->get_property = gst_audio_capture_src_get_property;

    g_object_class_install_property(gobject_class, PROP_SOURCE_TYPE,
        g_param_spec_enum("source-type", "Source type",
            "Source type", GST_TYPE_AUDIO_CAPTURE_SRC_SOURCE_TYPE, AUDIO_SOURCE_TYPE_MIC,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_SAMPLE_RATE,
        g_param_spec_uint("sample-rate", "Sample-Rate", "Audio sampling rate", 0, G_MAXINT32, 0,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_CHANNELS,
        g_param_spec_uint("channels", "Channels", "Number of audio channels", 0, G_MAXINT32, 0,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_BITRATE,
        g_param_spec_uint("bitrate", "Bitrate", "Audio bitrate", 0, G_MAXINT32, 0,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_TOKEN_ID,
        g_param_spec_uint("token-id", "TokenID", "Token ID", 0, G_MAXUINT32, 0,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_APP_UID,
        g_param_spec_int("app-uid", "Appuid", "APP UID", 0, G_MAXINT32, 0,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_APP_PID,
        g_param_spec_int("app-pid", "Apppid", "APP PID", 0, G_MAXINT32, 0,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_BYPASS_AUDIO_SERVICE,
        g_param_spec_boolean("bypass-audio-service", "Bypass Audio Service",
            "do not enable audio service", FALSE, (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));
    
    g_object_class_install_property(gobject_class, PROP_SUPPORTED_AUDIO_PARAMS,
        g_param_spec_boolean("supported-audio-params", "issupport audio params",
            "issupport audio params", FALSE, (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    gst_element_class_set_static_metadata(gstelement_class,
        "Audio capture source", "Source/Audio",
        "Retrieve audio frame from audio buffer queue", "OpenHarmony");

    gst_element_class_add_static_pad_template(gstelement_class, &gst_audio_capture_src_template);

    gstelement_class->change_state = gst_audio_capture_src_change_state;
    gstbasesrc_class->negotiate = gst_audio_capture_src_negotiate;
    gstpushsrc_class->create = gst_audio_capture_src_create;
}

static void gst_audio_capture_src_init(GstAudioCaptureSrc *src)
{
    g_return_if_fail(src != nullptr);
    gst_base_src_set_format(GST_BASE_SRC(src), GST_FORMAT_TIME);
    gst_base_src_set_live(GST_BASE_SRC(src), TRUE);
    src->stream_type = AUDIO_STREAM_TYPE_UNKNOWN;
    src->source_type = AUDIO_SOURCE_TYPE_MIC;
    src->audio_capture = nullptr;
    src->audio_mgr = nullptr;
    src->src_caps = nullptr;
    src->bitrate = 0;
    src->channels = 0;
    src->sample_rate = 0;
    src->is_start = FALSE;
    src->need_caps_info = TRUE;
    src->token_id = 0;
    src->appuid = 0;
    src->apppid = 0;
    src->bypass_audio = FALSE;
    src->input_detection = TRUE;
    gst_base_src_set_blocksize(GST_BASE_SRC(src), 0);
}

static void gst_audio_capture_src_finalize(GObject *object)
{
    GST_DEBUG_OBJECT(object, "finalize");
    GstAudioCaptureSrc *src = GST_AUDIO_CAPTURE_SRC(object);
    g_return_if_fail(src != nullptr);
    if (src->src_caps != nullptr) {
        gst_caps_unref(src->src_caps);
        src->src_caps = nullptr;
    }

    if (src->audio_capture) {
        src->audio_capture = nullptr;
    }

    G_OBJECT_CLASS(parent_class)->finalize(object);
}

static void gst_audio_capture_src_set_property(GObject *object, guint prop_id,
    const GValue *value, GParamSpec *pspec)
{
    (void)pspec;
    GstAudioCaptureSrc *src = GST_AUDIO_CAPTURE_SRC(object);
    g_return_if_fail(src != nullptr);
    switch (prop_id) {
        case PROP_SOURCE_TYPE:
            src->source_type = (AudioSourceType)g_value_get_enum(value);
            break;
        case PROP_SAMPLE_RATE:
            src->sample_rate = g_value_get_uint(value);
            break;
        case PROP_CHANNELS:
            src->channels = g_value_get_uint(value);
            break;
        case PROP_BITRATE:
            src->bitrate = g_value_get_uint(value);
            break;
        case PROP_TOKEN_ID:
            src->token_id = g_value_get_uint(value);
            break;
        case PROP_APP_UID:
            src->appuid = g_value_get_int(value);
            break;
        case PROP_APP_PID:
            src->apppid = g_value_get_int(value);
            break;
        case PROP_BYPASS_AUDIO_SERVICE:
            src->bypass_audio = g_value_get_boolean(value);
            if (src->bypass_audio) {
                // Mutually exclusive protection is provided at the frame layer
                if (src->audio_capture) {
                    src->audio_capture->WakeUpAudioThreads();
                }
            }
            break;
        default:
            break;
    }
}

static void gst_audio_capture_src_get_property(GObject *object, guint prop_id,
    GValue *value, GParamSpec *pspec)
{
    (void)pspec;
    GstAudioCaptureSrc *src = GST_AUDIO_CAPTURE_SRC(object);
    g_return_if_fail(src != nullptr);
    switch (prop_id) {
        case PROP_SOURCE_TYPE:
            g_value_set_enum(value, src->source_type);
            break;
        case PROP_SAMPLE_RATE:
            g_value_set_uint(value, src->sample_rate);
            break;
        case PROP_CHANNELS:
            g_value_set_uint(value, src->channels);
            break;
        case PROP_BITRATE:
            g_value_set_uint(value, src->bitrate);
            break;
        case PROP_SUPPORTED_AUDIO_PARAMS:
            if (src->audio_capture == nullptr) {
                src->audio_capture = OHOS::Media::AudioCaptureFactory::CreateAudioCapture(src->stream_type);
                g_return_if_fail(src->audio_capture != nullptr);
            }
            g_value_set_boolean(value, src->audio_capture->IsSupportedCaptureParameter(
                src->bitrate, src->channels, src->sample_rate));
            break;
        default:
            break;
    }
}

static gboolean process_caps_info(GstAudioCaptureSrc *src)
{
    guint bitrate = 0;
    guint sample_rate = 0;
    guint channels = 0;
    g_return_val_if_fail(src != nullptr, FALSE);
    g_return_val_if_fail(src->audio_capture->GetCaptureParameter(bitrate, channels, sample_rate) == MSERR_OK, FALSE);

    gboolean is_valid_params = TRUE;
    guint64 channel_mask = 0;
    switch (channels) {
        case 1: {
            GstAudioChannelPosition positions[1] = {GST_AUDIO_CHANNEL_POSITION_MONO};
            if (!gst_audio_channel_positions_to_mask(positions, channels, FALSE, &channel_mask)) {
                GST_ERROR_OBJECT(src, "invalid channel positions");
                is_valid_params = FALSE;
            }
            break;
        }
        case 2: { // 2 channels
            GstAudioChannelPosition positions[2] = {GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT,
                                                    GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT};
            if (!gst_audio_channel_positions_to_mask(positions, channels, FALSE, &channel_mask)) {
                GST_ERROR_OBJECT(src, "invalid channel positions");
                is_valid_params = FALSE;
            }
            break;
        }
        default: {
            is_valid_params = FALSE;
            GST_ERROR_OBJECT(src, "invalid channels %u", channels);
            break;
        }
    }
    g_return_val_if_fail(is_valid_params == TRUE, FALSE);
    if (src->src_caps != nullptr) {
        gst_caps_unref(src->src_caps);
    }
    src->src_caps = gst_caps_new_simple("audio/x-raw",
                                        "rate", G_TYPE_INT, sample_rate,
                                        "channels", G_TYPE_INT, channels,
                                        "format", G_TYPE_STRING, "S16LE",
                                        "channel-mask", GST_TYPE_BITMASK, channel_mask,
                                        "layout", G_TYPE_STRING, "interleaved", nullptr);
    GstBaseSrc *basesrc = GST_BASE_SRC_CAST(src);
    basesrc->segment.start = 0;
    return TRUE;
}

static GstStateChangeReturn gst_state_change_forward_direction(GstAudioCaptureSrc *src, GstStateChange transition)
{
    g_return_val_if_fail(src != nullptr, GST_STATE_CHANGE_FAILURE);
    switch (transition) {
        case GST_STATE_CHANGE_NULL_TO_READY: {
            if (src->audio_capture == nullptr) {
                src->audio_capture = OHOS::Media::AudioCaptureFactory::CreateAudioCapture(src->stream_type);
                CHECK_AND_BREAK_REP_ERR(src->audio_capture != nullptr, src, "failed to CreateAudioCapture");
            }
            break;
        }
        case GST_STATE_CHANGE_READY_TO_PAUSED: {
            CHECK_AND_BREAK_REP_ERR(src->audio_capture != nullptr, src, "audio_capture is nullptr");
            AudioCapture::AppInfo appInfo = {};
            appInfo.appUid = src->appuid;
            appInfo.appPid = src->apppid;
            appInfo.appTokenId = src->token_id;
            CHECK_AND_BREAK_REP_ERR(src->audio_capture->SetCaptureParameter(src->bitrate, src->channels,
                src->sample_rate, appInfo) == MSERR_OK, src, "SetCaptureParameter failed");
            break;
        }
        case GST_STATE_CHANGE_PAUSED_TO_PLAYING: {
            CHECK_AND_BREAK_REP_ERR(src->audio_capture != nullptr, src, "audio_capture is nullptr");
            if (src->need_caps_info) {
                CHECK_AND_BREAK_REP_ERR(process_caps_info(src) == TRUE, src, "process caps info failed");
                src->need_caps_info = FALSE;
            }
            if (src->is_start == FALSE) {
                CHECK_AND_BREAK_REP_ERR(src->audio_capture->StartAudioCapture() == MSERR_OK,
                    src, "StartAudioCapture failed");
                gst_audio_capture_src_mgr_init(src);
                src->is_start = TRUE;
            } else {
                if (!src->bypass_audio) {
                    CHECK_AND_BREAK_REP_ERR(src->audio_capture->ResumeAudioCapture() == MSERR_OK,
                        src, "ResumeAudioCapture failed");
                    gst_audio_capture_src_mgr_enable_watchdog(src);
                } else {
                    src->audio_mgr = nullptr;
                    CHECK_AND_BREAK_REP_ERR(src->audio_capture->WakeUpAudioThreads() == MSERR_OK,
                        src, "WakeUpAudioThreads failed");
                }
            }
            break;
        }
        default:
            break;
    }
    return GST_STATE_CHANGE_SUCCESS;
}

static GstStateChangeReturn gst_audio_capture_src_change_state(GstElement *element, GstStateChange transition)
{
    g_return_val_if_fail(element != nullptr, GST_STATE_CHANGE_FAILURE);
    GstAudioCaptureSrc *src = GST_AUDIO_CAPTURE_SRC(element);

    GstStateChangeReturn ret = gst_state_change_forward_direction(src, transition);
    g_return_val_if_fail(ret == GST_STATE_CHANGE_SUCCESS, GST_STATE_CHANGE_FAILURE);

    ret = GST_ELEMENT_CLASS(parent_class)->change_state(element, transition);

    switch (transition) {
        case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
            gst_audio_capture_src_mgr_disable_watchdog(src);
            CHECK_AND_BREAK_REP_ERR(src->audio_capture != nullptr, src, "audio_capture is nullptr");
            if (!src->bypass_audio) {
                CHECK_AND_BREAK_REP_ERR(src->audio_capture->PauseAudioCapture() == MSERR_OK,
                    src, "PauseAudioCapture failed");
            }
            break;
        case GST_STATE_CHANGE_PAUSED_TO_READY:
            src->is_start = FALSE;
            CHECK_AND_BREAK_REP_ERR(src->audio_capture != nullptr, src, "audio_capture is nullptr");
            src->audio_mgr = nullptr;
            CHECK_AND_BREAK_REP_ERR(src->audio_capture->StopAudioCapture() == MSERR_OK, src,
                "StopAudioCapture failed");
            break;
        case GST_STATE_CHANGE_READY_TO_NULL:
            src->audio_capture = nullptr;
            break;
        default:
            break;
    }
    return ret;
}

static GstFlowReturn gst_audio_capture_src_create(GstPushSrc *psrc, GstBuffer **outbuf)
{
    g_return_val_if_fail((psrc != nullptr) && (outbuf != nullptr), GST_FLOW_ERROR);
    GstAudioCaptureSrc *src = GST_AUDIO_CAPTURE_SRC(psrc);
    g_return_val_if_fail(src != nullptr, GST_FLOW_ERROR);
    if (src->is_start == FALSE) {
        return GST_FLOW_EOS;
    }
    g_return_val_if_fail(src->audio_capture != nullptr, GST_FLOW_ERROR);

    if (src->input_detection && src->audio_mgr != nullptr) {
        src->audio_mgr->ResumeWatchDog();
    }
    std::shared_ptr<AudioBuffer> audio_buffer = src->audio_capture->GetBuffer();
    if (src->input_detection && src->audio_mgr != nullptr) {
        src->audio_mgr->PauseWatchDog();
    }
    if (audio_buffer == nullptr) {
        if ((!src->bypass_audio) && src->is_start) {
            GST_ELEMENT_ERROR (src, STREAM, FAILED, ("Input stream error, return null."),
                ("Input stream error, return null."));
        }
        return GST_FLOW_ERROR;
    }
    gst_base_src_set_blocksize(GST_BASE_SRC_CAST(src), audio_buffer->dataLen);

    *outbuf = audio_buffer->gstBuffer;
    GST_BUFFER_DURATION(*outbuf) = audio_buffer->duration;
    GST_BUFFER_TIMESTAMP(*outbuf) = audio_buffer->timestamp;
    return GST_FLOW_OK;
}

static gboolean gst_audio_capture_src_negotiate(GstBaseSrc *basesrc)
{
    g_return_val_if_fail(basesrc != nullptr, false);
    GstAudioCaptureSrc *src = GST_AUDIO_CAPTURE_SRC(basesrc);
    g_return_val_if_fail(src != nullptr, FALSE);
    (void)gst_base_src_wait_playing(basesrc);
    g_return_val_if_fail(src->src_caps != nullptr, FALSE);
    return gst_base_src_set_caps(basesrc, src->src_caps);
}

static void gst_audio_capture_src_mgr_init(GstAudioCaptureSrc *src)
{
    g_return_if_fail(src != nullptr);
    if (src->input_detection && src->audio_mgr == nullptr) {
        const guint32 timeoutMs = 3000; // Error will be reported if there is no data input in 3000ms by default.
        GstPushSrc *psrc = GST_PUSH_SRC(src);
        src->audio_mgr = std::make_shared<AudioManager>(*psrc, timeoutMs);
        g_return_if_fail(src->audio_mgr != nullptr);
    }
}

static void gst_audio_capture_src_mgr_enable_watchdog(GstAudioCaptureSrc *src)
{
    g_return_if_fail(src != nullptr);
    if (src->input_detection && src->audio_mgr != nullptr) {
        src->audio_mgr->EnableWatchDog();
        src->audio_mgr->PauseWatchDog();
    }
}

static void gst_audio_capture_src_mgr_disable_watchdog(GstAudioCaptureSrc *src)
{
    g_return_if_fail(src != nullptr);
    if (src->audio_mgr != nullptr) {
        src->audio_mgr->DisableWatchDog();
    }
}

static void gst_audio_capture_src_getbuffer_timeout(GstPushSrc *psrc)
{
    g_return_if_fail(psrc != nullptr);
    GstAudioCaptureSrc *src = GST_AUDIO_CAPTURE_SRC(psrc);
    g_return_if_fail(src != nullptr);

    GST_ELEMENT_ERROR (src, RESOURCE, READ,
        ("Audio input stream timeout, please confirm whether the input is normal."),
        ("Audio input stream timeout, please confirm whether the input is normal."));
}

static gboolean plugin_init(GstPlugin *plugin)
{
    g_return_val_if_fail(plugin != nullptr, false);
    return gst_element_register(plugin, "audiocapturesrc", GST_RANK_PRIMARY, GST_TYPE_AUDIO_CAPTURE_SRC);
}

GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    _audio_capture_src,
    "GStreamer Audio Capture Source",
    plugin_init,
    PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
