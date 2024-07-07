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

#include "gst_vdec_h265.h"

G_DEFINE_TYPE(GstVdecH265, gst_vdec_h265, GST_TYPE_VDEC_BASE);

static void gst_vdec_h265_class_init(GstVdecH265Class *klass)
{
    GST_DEBUG_OBJECT(klass, "Init h265 class");
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);

    gst_element_class_set_static_metadata(element_class,
        "Hardware Driver Interface H.265 Video Decoder",
        "Codec/Decoder/Video/Hardware",
        "Decode H.265 video streams",
        "OpenHarmony");
    const gchar *sink_caps_string = "video/x-h265, "
        "alignment=(string) nal, "
        "stream-format=(string){ byte-stream }";
    GstCaps *sink_caps = gst_caps_from_string(sink_caps_string);

    if (sink_caps != nullptr) {
        GstPadTemplate *sink_templ = gst_pad_template_new("sink", GST_PAD_SINK, GST_PAD_ALWAYS, sink_caps);
        gst_element_class_add_pad_template(element_class, sink_templ);
        gst_caps_unref(sink_caps);
    }
}

static void gst_vdec_h265_init(GstVdecH265 *self)
{
    GstVdecBase *base = GST_VDEC_BASE(self);
    base->compress_format = OHOS::Media::GstCompressionFormat::GST_HEVC;
}