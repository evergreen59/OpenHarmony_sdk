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

#include "processor_aenc_impl.h"
#include "media_errors.h"
#include "media_log.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "ProcessorAencImpl"};
    constexpr uint32_t DEFAULT_BUFFER_SIZE = 50000;
    constexpr int32_t MAX_CHANNELS = 6;
    static const GstAudioChannelPosition CHANNEL_POSITION[6][6] = {
        {
            GST_AUDIO_CHANNEL_POSITION_MONO
        },
        {
            GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT,
            GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT
        },
        {
            GST_AUDIO_CHANNEL_POSITION_FRONT_CENTER,
            GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT,
            GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT
        },
        {
            GST_AUDIO_CHANNEL_POSITION_FRONT_CENTER,
            GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT,
            GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT,
            GST_AUDIO_CHANNEL_POSITION_REAR_CENTER
        },
        {
            GST_AUDIO_CHANNEL_POSITION_FRONT_CENTER,
            GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT,
            GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT,
            GST_AUDIO_CHANNEL_POSITION_REAR_LEFT,
            GST_AUDIO_CHANNEL_POSITION_REAR_RIGHT
        },
        {
            GST_AUDIO_CHANNEL_POSITION_FRONT_CENTER,
            GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT,
            GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT,
            GST_AUDIO_CHANNEL_POSITION_REAR_LEFT,
            GST_AUDIO_CHANNEL_POSITION_REAR_RIGHT,
            GST_AUDIO_CHANNEL_POSITION_LFE1
        },
    };

    enum MPEG4SamplingFrequencies : int32_t {
        MPEG4_SAMPLING_FREQUENCY_0 = 0,
        MPEG4_SAMPLING_FREQUENCY_1,
        MPEG4_SAMPLING_FREQUENCY_2,
        MPEG4_SAMPLING_FREQUENCY_3,
        MPEG4_SAMPLING_FREQUENCY_4,
        MPEG4_SAMPLING_FREQUENCY_5,
        MPEG4_SAMPLING_FREQUENCY_6,
        MPEG4_SAMPLING_FREQUENCY_7,
        MPEG4_SAMPLING_FREQUENCY_8,
        MPEG4_SAMPLING_FREQUENCY_9,
        MPEG4_SAMPLING_FREQUENCY_10,
        MPEG4_SAMPLING_FREQUENCY_11,
        MPEG4_SAMPLING_FREQUENCY_12,
        MPEG4_SAMPLING_FREQUENCY_13,
    };

    constexpr size_t SAMPLING_ROW_COUNT = 14;
    static const int32_t MPEG4_SAMPLING_FREQUENCIES[SAMPLING_ROW_COUNT][2] = {
        { 96000, MPEG4_SAMPLING_FREQUENCY_0 },
        { 88200, MPEG4_SAMPLING_FREQUENCY_1 },
        { 64000, MPEG4_SAMPLING_FREQUENCY_2 },
        { 48000, MPEG4_SAMPLING_FREQUENCY_3 },
        { 44100, MPEG4_SAMPLING_FREQUENCY_4 },
        { 32000, MPEG4_SAMPLING_FREQUENCY_5 },
        { 24000, MPEG4_SAMPLING_FREQUENCY_6 },
        { 22050, MPEG4_SAMPLING_FREQUENCY_7 },
        { 16000, MPEG4_SAMPLING_FREQUENCY_8 },
        { 12000, MPEG4_SAMPLING_FREQUENCY_9 },
        { 11025, MPEG4_SAMPLING_FREQUENCY_10 },
        { 8000, MPEG4_SAMPLING_FREQUENCY_11 },
        { 7350, MPEG4_SAMPLING_FREQUENCY_12 },
        { -1, MPEG4_SAMPLING_FREQUENCY_13 },
    };
}

namespace OHOS {
namespace Media {
ProcessorAencImpl::ProcessorAencImpl()
{
}

ProcessorAencImpl::~ProcessorAencImpl()
{
}

int32_t ProcessorAencImpl::ProcessMandatory(const Format &format)
{
    CHECK_AND_RETURN_RET(format.GetIntValue("channel_count", channels_) == true, MSERR_INVALID_VAL);
    CHECK_AND_RETURN_RET(format.GetIntValue("sample_rate", sampleRate_) == true, MSERR_INVALID_VAL);
    CHECK_AND_RETURN_RET(format.GetIntValue("audio_sample_format", audioSampleFormat_) == true, MSERR_INVALID_VAL);
    MEDIA_LOGD("channels:%{public}d, sampleRate:%{public}d, pcm:%{public}d",
        channels_, sampleRate_, audioSampleFormat_);

    gstRawFormat_ = RawAudioFormatToGst(static_cast<AudioStandard::AudioSampleFormat>(audioSampleFormat_));

    return MSERR_OK;
}

int32_t ProcessorAencImpl::ProcessOptional(const Format &format)
{
    if (format.GetValueType(std::string_view("profile")) == FORMAT_TYPE_INT32) {
        (void)format.GetIntValue("profile", profile_);
    }

    return MSERR_OK;
}

std::shared_ptr<ProcessorConfig> ProcessorAencImpl::GetInputPortConfig()
{
    CHECK_AND_RETURN_RET(channels_ > 0 && sampleRate_ > 0 && channels_ <= MAX_CHANNELS, nullptr);

    guint64 channelMask = 0;
    if (!gst_audio_channel_positions_to_mask(CHANNEL_POSITION[channels_ - 1], channels_, FALSE, &channelMask)) {
        MEDIA_LOGE("Invalid channel positions");
        return nullptr;
    }

    GstCaps *caps = gst_caps_new_simple("audio/x-raw",
        "rate", G_TYPE_INT, sampleRate_,
        "channels", G_TYPE_INT, channels_,
        "format", G_TYPE_STRING, gstRawFormat_.c_str(),
        "channel-mask", GST_TYPE_BITMASK, channelMask,
        "layout", G_TYPE_STRING, "interleaved", nullptr);
    CHECK_AND_RETURN_RET(caps != nullptr, nullptr);

    auto config = std::make_shared<ProcessorConfig>(caps, true);
    config->bufferSize_ = DEFAULT_BUFFER_SIZE;

    return config;
}

std::shared_ptr<ProcessorConfig> ProcessorAencImpl::GetOutputPortConfig()
{
    CHECK_AND_RETURN_RET(channels_ > 0 && sampleRate_ > 0 && channels_ <= MAX_CHANNELS, nullptr);

    GstCaps *caps = nullptr;
    switch (codecName_) {
        case CODEC_MIME_TYPE_AUDIO_AAC:
            caps = gst_caps_new_simple("audio/mpeg",
                "rate", G_TYPE_INT, sampleRate_,
                "channels", G_TYPE_INT, channels_,
                "mpegversion", G_TYPE_INT, 4,
                "stream-format", G_TYPE_STRING, "raw",
                "base-profile", G_TYPE_STRING, "lc", nullptr);
            break;
        case CODEC_MIME_TYPE_AUDIO_OPUS:
            caps = gst_caps_new_simple("audio/x-opus",
                "rate", G_TYPE_INT, sampleRate_, "channels", G_TYPE_INT, channels_, nullptr);
            break;
        default:
            break;
    }
    CHECK_AND_RETURN_RET_LOG(caps != nullptr, nullptr, "Unsupported format");

    auto config = std::make_shared<ProcessorConfig>(caps, true);
    config->bufferSize_ = DEFAULT_BUFFER_SIZE;
    if (codecName_ == CODEC_MIME_TYPE_AUDIO_AAC) {
        config->needFilter_ = true;
        config->filterMode_ = BUFFER_FILTER_MODE_ADD_ADTS;
        config->adtsHead_.channelConfig = static_cast<uint32_t>(channels_);
        config->adtsHead_.objectType = 2; // AAC_LC
        for (size_t i = 0; i < SAMPLING_ROW_COUNT; i++) {
            if (sampleRate_ >= MPEG4_SAMPLING_FREQUENCIES[i][0]) {
                config->adtsHead_.samplingIndex = MPEG4_SAMPLING_FREQUENCIES[i][1];
                break;
            }
        }
    }

    return config;
}
} // namespace Media
} // namespace OHOS
