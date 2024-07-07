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

#ifndef CODEC_COMMON_H
#define CODEC_COMMON_H

#include <string>
#include <gst/audio/audio.h>
#include <gst/gst.h>
#include "avcodec_info.h"
#include "avcodec_common.h"
#include "audio_info.h"
#include "avsharedmemory.h"
#include "format.h"
#include "surface.h"

namespace OHOS {
namespace Media {
enum InnerCodecMimeType : int32_t {
    CODEC_MIME_TYPE_DEFAULT = -1,
    /** H263 */
    CODEC_MIME_TYPE_VIDEO_H263,
    /** H264 */
    CODEC_MIME_TYPE_VIDEO_AVC,
    /** MPEG2 */
    CODEC_MIME_TYPE_VIDEO_MPEG2,
    /** HEVC */
    CODEC_MIME_TYPE_VIDEO_HEVC,
    /** MPEG4 */
    CODEC_MIME_TYPE_VIDEO_MPEG4,
    /** MP3 */
    CODEC_MIME_TYPE_AUDIO_MPEG,
    /** AAC */
    CODEC_MIME_TYPE_AUDIO_AAC,
    /** VORBIS */
    CODEC_MIME_TYPE_AUDIO_VORBIS,
    /** FLAC */
    CODEC_MIME_TYPE_AUDIO_FLAC,
    /** OPUS */
    CODEC_MIME_TYPE_AUDIO_OPUS,
};

enum VideoEncoderBitrateMode : int32_t {
    VIDEO_ENCODER_BITRATE_MODE_CBR = 0,
    VIDEO_ENCODER_BITRATE_MODE_VBR,
    VIDEO_ENCODER_BITRATE_MODE_CQ,
};

enum BufferFilterMode : int32_t {
    BUFFER_FILTER_MODE_INVALID = 0,
    BUFFER_FILTER_MODE_ADD_ADTS,
};

struct AdtsFixedHeader {
    uint32_t objectType = 0;
    uint32_t samplingIndex = 0;
    uint32_t channelConfig = 0;
};

struct BufferWrapper {
    enum Owner : int32_t {
        APP = 0,
        SERVER,
        DOWNSTREAM
    };
    explicit BufferWrapper(Owner owner)
        : owner_(owner)
    {
    }

    ~BufferWrapper()
    {
        if (gstBuffer_ != nullptr) {
            gst_buffer_unref(gstBuffer_);
        }
    }

    GstBuffer *gstBuffer_ = nullptr;
    AVSharedMemory *mem_ = nullptr;
    sptr<SurfaceBuffer> surfaceBuffer_ = nullptr;
    Owner owner_ = DOWNSTREAM;
};

struct ProcessorConfig {
    ProcessorConfig(GstCaps *caps, bool isEncoder)
        : caps_(caps),
          isEncoder_(isEncoder)
    {
    }
    ~ProcessorConfig()
    {
        if (caps_ != nullptr) {
            gst_caps_unref(caps_);
        }
    }
    GstCaps *caps_ = nullptr;
    bool needCodecData_ = false;
    bool needParser_ = false;
    bool needFilter_ = false;
    bool isEncoder_ = false;
    uint32_t bufferSize_ = 0;
    uint32_t videoRotation_ = 0;
    BufferFilterMode filterMode_ = BUFFER_FILTER_MODE_INVALID;
    AdtsFixedHeader adtsHead_;
};

std::string PixelFormatToGst(VideoPixelFormat pixel);
std::string MPEG4ProfileToGst(MPEG4Profile profile);
std::string AVCProfileToGst(AVCProfile profile);
std::string HEVCProfileToGst(HEVCProfile profile);
std::string RawAudioFormatToGst(AudioStandard::AudioSampleFormat format);
int32_t MapCodecMime(const std::string &mime, InnerCodecMimeType &name);
int32_t CapsToFormat(GstCaps *caps, Format &format);
uint32_t PixelBufferSize(VideoPixelFormat pixel, uint32_t width, uint32_t height, uint32_t alignment);
} // namespace Media
} // namespace OHOS
#endif // CODEC_COMMON_H
