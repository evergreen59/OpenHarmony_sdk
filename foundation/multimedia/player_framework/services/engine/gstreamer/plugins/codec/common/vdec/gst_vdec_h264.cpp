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

#include "gst_vdec_h264.h"
#include "securec.h"

#define gst_vdec_h264_parent_class parent_class
G_DEFINE_TYPE(GstVdecH264, gst_vdec_h264, GST_TYPE_VDEC_BASE);

static gboolean get_slice_flag(GstVdecH264 *self, GstMapInfo *info, bool &ready_push);
static GstBuffer *handle_slice_buffer(GstVdecBase *self, GstBuffer *buffer, bool &ready_push, bool is_finish);
static gboolean cat_slice_buffer(GstVdecBase *self, GstMapInfo *src_info);
static void flush_cache_slice_buffer(GstVdecBase *self);
static GstStateChangeReturn gst_vdec_h264_change_state(GstElement *element, GstStateChange transition);
static void gst_vdec_h264_finalize(GObject *object);
static gboolean gst_buffer_extend(GstBuffer **buffer, gsize &size);

static void gst_vdec_h264_class_init(GstVdecH264Class *klass)
{
    GST_DEBUG_OBJECT(klass, "Init h264 class");
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);
    GstVdecBaseClass *base_class = GST_VDEC_BASE_CLASS(klass);
    base_class->handle_slice_buffer = handle_slice_buffer;
    base_class->flush_cache_slice_buffer = flush_cache_slice_buffer;
    element_class->change_state = gst_vdec_h264_change_state;
    gobject_class->finalize = gst_vdec_h264_finalize;

    gst_element_class_set_static_metadata(element_class,
        "Hardware Driver Interface H.264 Video Decoder",
        "Codec/Decoder/Video/Hardware",
        "Decode H.264 video streams",
        "OpenHarmony");
    const gchar *sink_caps_string = "video/x-h264, "
        "alignment=(string) nal, "
        "stream-format=(string){ byte-stream }";
    GstCaps *sink_caps = gst_caps_from_string(sink_caps_string);

    if (sink_caps != nullptr) {
        GstPadTemplate *sink_templ = gst_pad_template_new("sink", GST_PAD_SINK, GST_PAD_ALWAYS, sink_caps);
        gst_element_class_add_pad_template(element_class, sink_templ);
        gst_caps_unref(sink_caps);
    }
}

static void gst_vdec_h264_init(GstVdecH264 *self)
{
    g_mutex_init(&self->cat_lock);
    self->is_slice_buffer = false;
    self->has_data_after_sps = true;
    self->cache_offset = 0;
    self->cache_slice_buffer = nullptr;
    GstVdecBase *base = GST_VDEC_BASE(self);
    base->compress_format = OHOS::Media::GstCompressionFormat::GST_AVC;
}

static gboolean get_slice_flag(GstVdecH264 *self, GstMapInfo *info, bool &ready_push)
{
    guint8 offset = 2;
    for (gsize i = 0; i < info->size - offset; i++) {
        if (info->data[i] == 0x01) {
            gboolean is_data_frame = true;
            // 0x1F is the mask of last 5 bits, 0x07 is SPS flag, continuous SPS header may be I frame.
            if ((info->data[i + 1] & 0x1F) == 0x07 && self->has_data_after_sps == false) {
                GST_DEBUG("continuous SPS header, which is regarded as I frame");
                return true;
            }
            if ((info->data[i + 1] & 0x1F) == 0x06 || // 0x1F is the mask of last 5 bits, 0x06 is SEI flag
                (info->data[i + 1] & 0x1F) == 0x07 || // 0x1F is the mask of last 5 bits, 0x07 is SPS flag
                (info->data[i + 1] & 0x1F) == 0x08) { // 0x1F is the mask of last 5 bits, 0x08 is PPS flag
                is_data_frame = false;
                self->has_data_after_sps = false;
            } else {
                self->has_data_after_sps = true;
            }
            if (is_data_frame == false && self->is_slice_buffer == false) {
                ready_push = true;
                return false;
            } else if (is_data_frame == false && self->is_slice_buffer == true) {
                return true;
            } else if (is_data_frame == true && (info->data[i + offset] & 0x80) == 0x80) {
                return true;
            }
            break;
        }
    }
    return false;
}

static GstBuffer *handle_slice_buffer(GstVdecBase *self, GstBuffer *buffer, bool &ready_push, bool is_finish)
{
    GstVdecH264 *vdec_h264 = GST_VDEC_H264(self);
    GstBuffer *buf = nullptr;
    g_mutex_lock(&vdec_h264->cat_lock);
    if (is_finish) {
        buf = vdec_h264->cache_slice_buffer;
        vdec_h264->cache_slice_buffer = nullptr;
        gst_buffer_set_size(buf, vdec_h264->cache_offset);
        vdec_h264->is_slice_buffer = false;
        ready_push = true;
        g_mutex_unlock(&vdec_h264->cat_lock);
        return buf;
    }

    GstMapInfo info = GST_MAP_INFO_INIT;
    if (!gst_buffer_map(buffer, &info, GST_MAP_READ)) {
        GST_ERROR_OBJECT(self, "map buffer fail");
        gst_buffer_ref(buffer);
        g_mutex_unlock(&vdec_h264->cat_lock);
        return buffer;
    }
    gboolean slice_flag = get_slice_flag(vdec_h264, &info, ready_push);
    if (ready_push) {
        gst_buffer_unmap(buffer, &info);
        gst_buffer_ref(buffer);
        g_mutex_unlock(&vdec_h264->cat_lock);
        return buffer;
    }

    if (slice_flag == true && vdec_h264->is_slice_buffer == false) { // cache the first slice frame
        (void)cat_slice_buffer(self, &info);
        vdec_h264->is_slice_buffer = true;
        ready_push = false;
    } else if (slice_flag == false && vdec_h264->is_slice_buffer == true) { // cache the middle slice frame
        (void)cat_slice_buffer(self, &info);
        ready_push = false;
    } else if (slice_flag == true && vdec_h264->is_slice_buffer == true) { // get full frame, cache next first slice
        buf = vdec_h264->cache_slice_buffer;
        vdec_h264->cache_slice_buffer = nullptr;
        gst_buffer_set_size(buf, vdec_h264->cache_offset);
        ready_push = true;
        vdec_h264->cache_offset = 0;
        (void)cat_slice_buffer(self, &info);
    } else {
        buf = buffer;
        gst_buffer_ref(buffer);
        ready_push = true;
    }

    gst_buffer_unmap(buffer, &info);
    g_mutex_unlock(&vdec_h264->cat_lock);
    return buf;
}

static gboolean cat_slice_buffer(GstVdecBase *self, GstMapInfo *src_info)
{
    GstVdecH264 *vdec_h264 = GST_VDEC_H264(self);
    if (vdec_h264->cache_slice_buffer == nullptr) {
        vdec_h264->cache_slice_buffer = gst_buffer_new_allocate (nullptr, src_info->size, nullptr);
        if (vdec_h264->cache_slice_buffer == nullptr) {
            GST_ERROR_OBJECT(self, "allocate buffer is nullptr");
            return false;
        }
    } else {
        gsize extend_size = src_info->size + vdec_h264->cache_offset;
        if (!gst_buffer_extend(&(vdec_h264->cache_slice_buffer), extend_size)) {
            GST_ERROR_OBJECT(self, "gst_buffer_extend fail");
            return false;
        }
    }

    GstMapInfo cache_info = GST_MAP_INFO_INIT;
    if (!gst_buffer_map(vdec_h264->cache_slice_buffer, &cache_info, GST_MAP_WRITE)) {
        GST_ERROR_OBJECT(self, "map buffer fail");
        return false;
    }
    errno_t ret = memcpy_s(cache_info.data + vdec_h264->cache_offset,
        cache_info.size - vdec_h264->cache_offset, src_info->data, src_info->size);
    if (ret != EOK) {
        GST_ERROR_OBJECT(self, "memcpy_s fail");
        gst_buffer_unmap(vdec_h264->cache_slice_buffer, &cache_info);
        return false;
    }
    vdec_h264->cache_offset += src_info->size;
    gst_buffer_unmap(vdec_h264->cache_slice_buffer, &cache_info);
    return true;
}

static gboolean gst_buffer_extend(GstBuffer **buffer, gsize &size)
{
    g_return_val_if_fail(buffer != nullptr && *buffer != nullptr, FALSE);
    GstBuffer *ext_buffer = gst_buffer_new_allocate (nullptr, size, nullptr);
    g_return_val_if_fail(ext_buffer != nullptr, FALSE);

    GstMapInfo info = GST_MAP_INFO_INIT;
    GstMapInfo ext_info = GST_MAP_INFO_INIT;
    if (!gst_buffer_map(*buffer, &info, GST_MAP_READ)) {
        GST_ERROR("map buffer fail");
        return false;
    }

    if (!gst_buffer_map(ext_buffer, &ext_info, GST_MAP_WRITE)) {
        GST_ERROR("map buffer fail");
        gst_buffer_unmap(*buffer, &info);
        return false;
    }

    errno_t ret = memcpy_s(ext_info.data, ext_info.size, info.data, info.size);
    gst_buffer_unmap(*buffer, &info);
    gst_buffer_unmap(ext_buffer, &ext_info);
    if (ret != EOK) {
        GST_ERROR("memcpy_s fail");
        return false;
    }
    gst_buffer_unref(*buffer);
    *buffer = ext_buffer;
    return true;
}

static void flush_cache_slice_buffer(GstVdecBase *self)
{
    GstVdecH264 *vdec_h264 = GST_VDEC_H264(self);
    if (vdec_h264 != nullptr) {
        g_mutex_lock(&vdec_h264->cat_lock);
        vdec_h264->cache_offset = 0;
        vdec_h264->is_slice_buffer = false;
        vdec_h264->has_data_after_sps = true;
        if (vdec_h264->cache_slice_buffer != nullptr) {
            gst_buffer_unref(vdec_h264->cache_slice_buffer);
            vdec_h264->cache_slice_buffer = nullptr;
        }
        g_mutex_unlock(&vdec_h264->cat_lock);
    }
}

static GstStateChangeReturn gst_vdec_h264_change_state(GstElement *element, GstStateChange transition)
{
    g_return_val_if_fail(element != nullptr, GST_STATE_CHANGE_FAILURE);
    GstVdecH264 *vdec_h264 = GST_VDEC_H264(element);
    GstStateChangeReturn ret = GST_ELEMENT_CLASS(parent_class)->change_state(element, transition);
    switch (transition) {
        case GST_STATE_CHANGE_PAUSED_TO_READY:
            g_mutex_lock(&vdec_h264->cat_lock);
            if (vdec_h264->cache_slice_buffer != nullptr) {
                gst_buffer_unref(vdec_h264->cache_slice_buffer);
                vdec_h264->cache_slice_buffer = nullptr;
                vdec_h264->cache_offset = 0;
                vdec_h264->is_slice_buffer = false;
                vdec_h264->has_data_after_sps = true;
            }
            g_mutex_unlock(&vdec_h264->cat_lock);
            break;
        default:
            break;
    }
    return ret;
}

static void gst_vdec_h264_finalize(GObject *object)
{
    GST_DEBUG_OBJECT(object, "Vdec H264 Finalize");
    g_return_if_fail(object != nullptr);

    GstVdecH264 *vdec_h264 = GST_VDEC_H264(object);
    if (vdec_h264->cache_slice_buffer != nullptr) {
        gst_buffer_unref(vdec_h264->cache_slice_buffer);
        vdec_h264->cache_slice_buffer = nullptr;
    }
    g_mutex_clear(&vdec_h264->cat_lock);
    G_OBJECT_CLASS(parent_class)->finalize(object);
}