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

#ifndef GST_COMMON_UTILS_H
#define GST_COMMON_UTILS_H

#include <gst/gst.h>

// only C style code is accepted in this file
enum VideoStreamType {
    VIDEO_STREAM_TYPE_UNKNOWN = 0,
    VIDEO_STREAM_TYPE_ES_AVC,
    VIDEO_STREAM_TYPE_YUV_420,
    VIDEO_STREAM_TYPE_RGBA,
};

enum CodecBinType {
    CODEC_BIN_TYPE_UNKNOWN = -1,
    CODEC_BIN_TYPE_VIDEO_ENCODER = 0,
    CODEC_BIN_TYPE_VIDEO_DECODER,
    CODEC_BIN_TYPE_AUDIO_ENCODER,
    CODEC_BIN_TYPE_AUDIO_DECODER,
};

struct AudioBuffer {
    uint64_t timestamp;
    uint32_t dataSeq;
    uint32_t dataLen;
    uint64_t duration;
    GstBuffer *gstBuffer;
};

enum AudioStreamType {
    AUDIO_STREAM_TYPE_UNKNOWN = 0,
    AUDIO_STREAM_TYPE_PCM_S32_LE,
};

enum AudioSourceType {
    AUDIO_SOURCE_TYPE_UNKNOWN = -1,
    AUDIO_SOURCE_TYPE_DEFAULT = 0,
    AUDIO_SOURCE_TYPE_MIC = 1,
};
#endif  // GST_COMMON_UTILS_H
