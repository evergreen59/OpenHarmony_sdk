/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "gst_mux_bin.h"
#include "av_common.h"
#include "gstbasesink.h"
#include "gstbaseparse.h"

enum {
    PROP_0,
    PROP_FD,
    PROP_MUX,
    PROP_DEGREES,
    PROP_LATITUDE,
    PROP_LONGITUDE,
};

enum {
    SIGNAL_ADD_TRACK,
    LAST_SIGNAL
};

static guint gst_mux_bin_signals[LAST_SIGNAL] = { 0 };
using namespace OHOS;
#define gst_mux_bin_parent_class parent_class
G_DEFINE_TYPE(GstMuxBin, gst_mux_bin, GST_TYPE_PIPELINE);

static void gst_mux_bin_finalize(GObject *object);
static void gst_mux_bin_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *param_spec);
static GstStateChangeReturn gst_mux_bin_change_state(GstElement *element, GstStateChange transition);

static void gst_mux_bin_add_track(GstMuxBin *mux_bin, const char *src_name, const char *parse_name, int32_t track_type)
{
    g_return_if_fail(mux_bin != nullptr);
    g_return_if_fail(src_name != nullptr);
    g_return_if_fail(parse_name != nullptr);
    GstTrackInfo *info = g_new(GstTrackInfo, 1);
    g_return_if_fail(info != nullptr);
    info->srcName_ = g_strdup(static_cast<char *>(src_name));
    info->parseName_ = g_strdup(static_cast<char *>(parse_name));
    switch (static_cast<OHOS::Media::MediaType>(track_type)) {
        case OHOS::Media::MEDIA_TYPE_VID:
            mux_bin->video_src_list = g_slist_append(mux_bin->video_src_list, info);
            break;
        case OHOS::Media::MEDIA_TYPE_AUD:
            mux_bin->audio_src_list = g_slist_append(mux_bin->audio_src_list, info);
            break;
        default:
            break;
    }
}

static void gst_mux_bin_class_init(GstMuxBinClass *klass)
{
    g_return_if_fail(klass != nullptr);
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GstElementClass *gstelement_class = GST_ELEMENT_CLASS(klass);

    g_return_if_fail(gobject_class != nullptr);
    g_return_if_fail(gstelement_class != nullptr);

    gobject_class->finalize = gst_mux_bin_finalize;
    gobject_class->set_property = gst_mux_bin_set_property;

    g_object_class_install_property(gobject_class, PROP_FD,
        g_param_spec_int("fd", "FD", "fd of the output file",
            G_MININT32, G_MAXINT32, -1, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_MUX,
        g_param_spec_string("mux", "Mux", "type of the mux",
            nullptr, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_DEGREES,
        g_param_spec_int("rotation", "Rotation", "rotation angle of the output file",
            0, G_MAXINT32, 0, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_LATITUDE,
        g_param_spec_int("latitude", "Latitude", "latitude of the output file",
            G_MININT32, G_MAXINT32, 0, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobject_class, PROP_LONGITUDE,
        g_param_spec_int("longitude", "Longitude", "longitude of the output file",
            G_MININT32, G_MAXINT32, 0, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    gst_mux_bin_signals[SIGNAL_ADD_TRACK] =
        g_signal_new("add-track", G_TYPE_FROM_CLASS(klass),
            static_cast<GSignalFlags>(G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION),
            G_STRUCT_OFFSET(GstMuxBinClass, add_track), NULL, NULL, NULL,
            G_TYPE_NONE, 3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);  // 3 parameters

    gst_element_class_set_static_metadata(gstelement_class,
        "Mux Bin", "Generic/Bin/Mux",
        "Auto construct mux pipeline", "OpenHarmony");

    klass->add_track = gst_mux_bin_add_track;
    gstelement_class->change_state = gst_mux_bin_change_state;
}

static void gst_mux_bin_init(GstMuxBin *mux_bin)
{
    g_return_if_fail(mux_bin != nullptr);
    GST_INFO_OBJECT(mux_bin, "gst_mux_bin_init");
    mux_bin->video_src_list = nullptr;
    mux_bin->audio_src_list = nullptr;
    mux_bin->split_mux_sink = nullptr;
    mux_bin->out_fd = -1;
    mux_bin->mux = nullptr;
    mux_bin->rotation = 0;
    mux_bin->latitude = 0;
    mux_bin->longitude = 0;
}

static void gst_mux_bin_free_list(GSList *list)
{
    GSList *iter = list;
    while (iter != nullptr && iter->data != nullptr) {
        if ((reinterpret_cast<GstTrackInfo *>(iter->data))->srcName_ != nullptr) {
            g_free((reinterpret_cast<GstTrackInfo *>(iter->data))->srcName_);
            (reinterpret_cast<GstTrackInfo *>(iter->data))->srcName_ = nullptr;
        }
        if ((reinterpret_cast<GstTrackInfo *>(iter->data))->parseName_ != nullptr) {
            g_free((reinterpret_cast<GstTrackInfo *>(iter->data))->parseName_);
            (reinterpret_cast<GstTrackInfo *>(iter->data))->parseName_ = nullptr;
        }
        if ((reinterpret_cast<GstTrackInfo *>(iter->data))->src_ != nullptr) {
            gst_object_unref((reinterpret_cast<GstTrackInfo *>(iter->data))->src_);
            (reinterpret_cast<GstTrackInfo *>(iter->data))->src_ = nullptr;
        }
        if ((reinterpret_cast<GstTrackInfo *>(iter->data))->parse_ != nullptr) {
            gst_object_unref((reinterpret_cast<GstTrackInfo *>(iter->data))->parse_);
            (reinterpret_cast<GstTrackInfo *>(iter->data))->parse_ = nullptr;
        }
        g_free(iter->data);
        iter->data = nullptr;
        iter = iter->next;
    }
}

static void gst_mux_bin_finalize(GObject *object)
{
    g_return_if_fail(object != nullptr);
    GstMuxBin *mux_bin = GST_MUX_BIN(object);
    GST_INFO_OBJECT(mux_bin, "gst_mux_bin_finalize");

    if (mux_bin->mux != nullptr) {
        g_free(mux_bin->mux);
        mux_bin->mux = nullptr;
    }

    if (mux_bin->split_mux_sink != nullptr) {
        gst_object_unref(mux_bin->split_mux_sink);
        mux_bin->split_mux_sink = nullptr;
    }

    gst_mux_bin_free_list(mux_bin->video_src_list);
    g_slist_free(mux_bin->video_src_list);
    mux_bin->video_src_list = nullptr;

    gst_mux_bin_free_list(mux_bin->audio_src_list);
    g_slist_free(mux_bin->audio_src_list);
    mux_bin->audio_src_list = nullptr;

    G_OBJECT_CLASS(parent_class)->finalize(object);
}


static void gst_mux_bin_set_property(GObject *object, guint prop_id,
    const GValue *value, GParamSpec *param_spec)
{
    g_return_if_fail(object != nullptr);
    g_return_if_fail(value != nullptr);
    (void)param_spec;
    GstMuxBin *mux_bin = GST_MUX_BIN(object);
    switch (prop_id) {
        case PROP_FD:
            mux_bin->out_fd = g_value_get_int(value);
            break;
        case PROP_MUX:
            mux_bin->mux = g_strdup(g_value_get_string(value));
            break;
        case PROP_DEGREES:
            mux_bin->rotation = g_value_get_int(value);
            break;
        case PROP_LATITUDE:
            mux_bin->latitude = g_value_get_int(value);
            break;
        case PROP_LONGITUDE:
            mux_bin->longitude = g_value_get_int(value);
            break;
        default:
            break;
    }
}

static bool gst_mux_bin_create_splitmuxsink(GstMuxBin *mux_bin)
{
    g_return_val_if_fail(mux_bin != nullptr, false);
    g_return_val_if_fail(mux_bin->out_fd >= 0, false);
    g_return_val_if_fail(mux_bin->mux != nullptr, false);
    GST_INFO_OBJECT(mux_bin, "gst_mux_bin_create_splitmuxsink");

    mux_bin->split_mux_sink = GST_ELEMENT(gst_object_ref_sink(
        gst_element_factory_make("splitmuxsink", "splitmuxsink")));
    g_return_val_if_fail(mux_bin->split_mux_sink != nullptr, false);

    GstElement *fdsink = gst_element_factory_make("fdsink", "fdsink");
    g_return_val_if_fail(fdsink != nullptr, false);

    g_object_set(fdsink, "fd", mux_bin->out_fd, nullptr);
    gst_base_sink_set_async_enabled(GST_BASE_SINK(fdsink), FALSE);
    g_object_set(mux_bin->split_mux_sink, "sink", fdsink, nullptr);

    GstElement *qtmux = gst_element_factory_make(mux_bin->mux, mux_bin->mux);
    g_return_val_if_fail(qtmux != nullptr, false);
    g_object_set(qtmux, "orientation-hint", mux_bin->rotation, "set-latitude", mux_bin->latitude,
        "set-longitude", mux_bin->longitude, nullptr);
    g_object_set(mux_bin->split_mux_sink, "muxer", qtmux, nullptr);

    return true;
}

static GstElement *gst_mux_bin_create_parse(GstMuxBin *mux_bin, const char* parse_name)
{
    g_return_val_if_fail(mux_bin != nullptr, nullptr);
    GST_INFO_OBJECT(mux_bin, "gst_mux_bin_create_parse");

    GstElement *parse = nullptr;
    if (strncmp(parse_name, "h264parse", strlen("h264parse")) == 0) {
        parse = GST_ELEMENT(gst_object_ref_sink(gst_element_factory_make("h264parse", parse_name)));
        g_return_val_if_fail(parse != nullptr, nullptr);
    } else if (strncmp(parse_name, "mpeg4videoparse", strlen("mpeg4videoparse")) == 0) {
        parse = GST_ELEMENT(gst_object_ref_sink(gst_element_factory_make("mpeg4videoparse", parse_name)));
        g_return_val_if_fail(parse != nullptr, nullptr);
        g_object_set(parse, "config-interval", -1, "drop", false, nullptr);
    } else if (strncmp(parse_name, "aacparse", strlen("aacparse")) == 0) {
        parse = GST_ELEMENT(gst_object_ref_sink(gst_element_factory_make("aacparse", parse_name)));
        g_return_val_if_fail(parse != nullptr, nullptr);
    } else {
        GST_ERROR_OBJECT(mux_bin, "Invalid videoParse");
    }

    return parse;
}

static bool gst_mux_bin_create_src(GstMuxBin *mux_bin, OHOS::Media::MediaType track_type)
{
    g_return_val_if_fail(mux_bin != nullptr, false);
    GST_INFO_OBJECT(mux_bin, "gst_mux_bin_create_src");

    GSList *iter = nullptr;
    switch (track_type) {
        case OHOS::Media::MEDIA_TYPE_VID:
            iter = mux_bin->video_src_list;
            break;
        case OHOS::Media::MEDIA_TYPE_AUD:
            iter = mux_bin->audio_src_list;
            break;
        default:
            break;
    }
    while (iter != nullptr) {
        GstElement *app_src = GST_ELEMENT(gst_object_ref_sink(gst_element_factory_make(
            "appsrc", (reinterpret_cast<GstTrackInfo *>(iter->data))->srcName_)));
        g_return_val_if_fail(app_src != nullptr, false);
        g_object_set(app_src, "is-live", true, "format", GST_FORMAT_TIME, nullptr);
        (reinterpret_cast<GstTrackInfo *>(iter->data))->src_ = app_src;
        if (strstr((reinterpret_cast<GstTrackInfo *>(iter->data))->parseName_, "parse")) {
            GstElement *parse = gst_mux_bin_create_parse(mux_bin,
                (reinterpret_cast<GstTrackInfo *>(iter->data))->parseName_);
            g_return_val_if_fail(parse != nullptr, false);
            (reinterpret_cast<GstTrackInfo *>(iter->data))->parse_ = parse;
        } else {
            (reinterpret_cast<GstTrackInfo *>(iter->data))->parse_ = nullptr;
        }
        iter = iter->next;
    }

    return true;
}

static bool gst_mux_bin_create_element(GstMuxBin *mux_bin)
{
    g_return_val_if_fail(mux_bin != nullptr, false);
    GST_INFO_OBJECT(mux_bin, "gst_mux_bin_create_element");

    if (!gst_mux_bin_create_splitmuxsink(mux_bin)) {
        GST_ERROR_OBJECT(mux_bin, "Failed to call gst_mux_bin_create_splitmuxsink");
        return false;
    }

    if (!gst_mux_bin_create_src(mux_bin, OHOS::Media::MEDIA_TYPE_VID)) {
        GST_ERROR_OBJECT(mux_bin, "Failed to call create_video_src");
        return false;
    }

    if (!gst_mux_bin_create_src(mux_bin, OHOS::Media::MEDIA_TYPE_AUD)) {
        GST_ERROR_OBJECT(mux_bin, "Failed to call create_audio_src");
        return false;
    }

    return true;
}

static bool gst_mux_bin_add_element_to_bin(GstMuxBin *mux_bin)
{
    g_return_val_if_fail(mux_bin != nullptr, false);
    g_return_val_if_fail(mux_bin->split_mux_sink != nullptr, false);
    GST_INFO_OBJECT(mux_bin, "gst_mux_bin_add_element_to_bin");

    bool ret;
    GSList *iter = mux_bin->video_src_list;
    while (iter != nullptr) {
        ret = gst_bin_add(GST_BIN(mux_bin), (reinterpret_cast<GstTrackInfo *>(iter->data))->src_);
        g_return_val_if_fail(ret == TRUE, false);
        if ((reinterpret_cast<GstTrackInfo *>(iter->data))->parse_ != nullptr) {
            ret = gst_bin_add(GST_BIN(mux_bin), (reinterpret_cast<GstTrackInfo *>(iter->data))->parse_);
            g_return_val_if_fail(ret == TRUE, false);
        }
        iter = iter->next;
    }
    iter = mux_bin->audio_src_list;
    while (iter != nullptr) {
        ret = gst_bin_add(GST_BIN(mux_bin), (reinterpret_cast<GstTrackInfo *>(iter->data))->src_);
        g_return_val_if_fail(ret == TRUE, false);
        if ((reinterpret_cast<GstTrackInfo *>(iter->data))->parse_ != nullptr) {
            ret = gst_bin_add(GST_BIN(mux_bin), (reinterpret_cast<GstTrackInfo *>(iter->data))->parse_);
            g_return_val_if_fail(ret == TRUE, false);
        }
        iter = iter->next;
    }
    ret = gst_bin_add(GST_BIN(mux_bin), mux_bin->split_mux_sink);
    g_return_val_if_fail(ret == TRUE, false);

    return true;
}

static bool gst_mux_bin_connect_parse(
    GstMuxBin *mux_bin, GstElement *parse, GstPad *upstream_pad, GstPad *downstream_pad)
{
    g_return_val_if_fail(mux_bin != nullptr, false);
    g_return_val_if_fail(parse != nullptr, false);
    g_return_val_if_fail(upstream_pad != nullptr, false);
    g_return_val_if_fail(downstream_pad != nullptr, false);
    GST_INFO_OBJECT(mux_bin, "gst_mux_bin_connect_parse");

    GstPad *parse_sink_pad = gst_element_get_static_pad(parse, "sink");
    if (gst_pad_link(upstream_pad, parse_sink_pad) != GST_PAD_LINK_OK) {
        gst_object_unref(parse_sink_pad);
        GST_ERROR_OBJECT(mux_bin, "Failed to link src_src_pad and parse_sink_pad");
        return false;
    }
    gst_object_unref(parse_sink_pad);
    GstPad *parse_src_pad = gst_element_get_static_pad(parse, "src");
    if (gst_pad_link(parse_src_pad, downstream_pad) != GST_PAD_LINK_OK) {
        gst_object_unref(parse_src_pad);
        GST_ERROR_OBJECT(mux_bin, "Failed to link parse_src_pad and split_mux_sink_sink_pad");
        return false;
    }
    gst_object_unref(parse_src_pad);
    
    return true;
}

static bool gst_mux_bin_connect_element(GstMuxBin *mux_bin, OHOS::Media::MediaType type)
{
    g_return_val_if_fail(mux_bin != nullptr, false);
    g_return_val_if_fail(mux_bin->split_mux_sink != nullptr, false);
    GST_INFO_OBJECT(mux_bin, "gst_mux_bin_connect_element");

    GSList *iter = nullptr;
    if (type == OHOS::Media::MEDIA_TYPE_VID) {
        iter = mux_bin->video_src_list;
    } else if (type == OHOS::Media::MEDIA_TYPE_AUD) {
        iter = mux_bin->audio_src_list;
    } else {
        GST_ERROR_OBJECT(mux_bin, "Failed to check track type");
        return false;
    }

    while (iter != nullptr) {
        GstPad *src_src_pad = gst_element_get_static_pad((reinterpret_cast<GstTrackInfo *>(iter->data))->src_, "src");
        GstPad *split_mux_sink_sink_pad = nullptr;
        if (type == OHOS::Media::MEDIA_TYPE_VID) {
            split_mux_sink_sink_pad = gst_element_request_pad_simple(mux_bin->split_mux_sink, "video");
        } else if (type == OHOS::Media::MEDIA_TYPE_AUD) {
            split_mux_sink_sink_pad = gst_element_request_pad_simple(mux_bin->split_mux_sink, "audio_%u");
        }
        if ((reinterpret_cast<GstTrackInfo *>(iter->data))->parse_ != nullptr) {
            if (!gst_mux_bin_connect_parse(mux_bin, (reinterpret_cast<GstTrackInfo *>(iter->data))->parse_,
                src_src_pad, split_mux_sink_sink_pad)) {
                gst_object_unref(split_mux_sink_sink_pad);
                gst_object_unref(src_src_pad);
                GST_ERROR_OBJECT(mux_bin, "Failed to call connect_parse");
                return false;
            }
        } else {
            if (gst_pad_link(src_src_pad, split_mux_sink_sink_pad) != GST_PAD_LINK_OK) {
                gst_object_unref(split_mux_sink_sink_pad);
                gst_object_unref(src_src_pad);
                GST_ERROR_OBJECT(mux_bin, "Failed to link src_src_pad and split_mux_sink_sink_pad");
                return false;
            }
        }
        gst_object_unref(split_mux_sink_sink_pad);
        gst_object_unref(src_src_pad);
        iter = iter->next;
    }

    return true;
}

static GstStateChangeReturn gst_mux_bin_change_state(GstElement *element, GstStateChange transition)
{
    g_return_val_if_fail(element != nullptr, GST_STATE_CHANGE_FAILURE);
    GstMuxBin *mux_bin = GST_MUX_BIN(element);
    GST_INFO_OBJECT(mux_bin, "gst_mux_bin_change_state");

    switch (transition) {
        case GST_STATE_CHANGE_NULL_TO_READY: {
            if (!gst_mux_bin_create_element(mux_bin)) {
                GST_ERROR_OBJECT(mux_bin, "Failed to create element");
                return GST_STATE_CHANGE_FAILURE;
            }
            if (!gst_mux_bin_add_element_to_bin(mux_bin)) {
                GST_ERROR_OBJECT(mux_bin, "Failed to add element to bin");
                return GST_STATE_CHANGE_FAILURE;
            }
            break;
        }
        case GST_STATE_CHANGE_READY_TO_PAUSED: {
            if (!gst_mux_bin_connect_element(mux_bin, OHOS::Media::MEDIA_TYPE_VID)) {
                GST_ERROR_OBJECT(mux_bin, "Failed to connect element");
                return GST_STATE_CHANGE_FAILURE;
            }
            if (!gst_mux_bin_connect_element(mux_bin, OHOS::Media::MEDIA_TYPE_AUD)) {
                GST_ERROR_OBJECT(mux_bin, "Failed to connect element");
                return GST_STATE_CHANGE_FAILURE;
            }
            break;
        }
        default:
            break;
    }
    return GST_ELEMENT_CLASS(parent_class)->change_state(element, transition);
}

static gboolean plugin_init(GstPlugin *plugin)
{
    g_return_val_if_fail(plugin != nullptr, FALSE);
    return gst_element_register(plugin, "muxbin", GST_RANK_PRIMARY, GST_TYPE_MUX_BIN);
}

GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    _avmuxer_bin,
    "GStreamer Mux Bin",
    plugin_init,
    PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
