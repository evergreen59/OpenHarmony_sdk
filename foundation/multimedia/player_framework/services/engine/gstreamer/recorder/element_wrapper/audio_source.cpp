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

#include "audio_source.h"
#include <gst/gst.h>
#include "media_errors.h"
#include "media_log.h"
#include "recorder_private_param.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "AudioSource"};
}

namespace OHOS {
namespace Media {
int32_t AudioSource::Init()
{
    gstElem_ = gst_element_factory_make("audiocapturesrc", name_.c_str());
    if (gstElem_ == nullptr) {
        MEDIA_LOGE("Create audiosource gst element failed! sourceId: %{public}d", desc_.handle_);
        return MSERR_INVALID_OPERATION;
    }
    g_object_set(gstElem_, "source-type", desc_.type_, nullptr);

    return MSERR_OK;
}

int32_t AudioSource::Configure(const RecorderParam &recParam)
{
    int ret = MSERR_OK;
    switch (recParam.type) {
        case RecorderPublicParamType::AUD_SAMPLERATE:
            ret = ConfigAudioSampleRate(recParam);
            break;
        case RecorderPublicParamType::AUD_CHANNEL:
            ret = ConfigAudioChannels(recParam);
            break;
        case RecorderPublicParamType::AUD_BITRATE:
            ret = ConfigAudioBitRate(recParam);
            break;
        case RecorderPrivateParamType::APP_INFO:
            ret = ConfigAppInfo(recParam);
            break;
        default:
            break;
    }

    return ret;
}

int32_t AudioSource::ConfigAudioSampleRate(const RecorderParam &recParam)
{
    const AudSampleRate &param = static_cast<const AudSampleRate &>(recParam);
    if (param.sampleRate <= 0) {
        MEDIA_LOGE("The required audio sample rate %{public}d invalid.", param.sampleRate);
        return MSERR_INVALID_VAL;
    }
    MEDIA_LOGI("Set audio sample rate: %{public}d", param.sampleRate);
    g_object_set(gstElem_, "sample-rate", static_cast<uint32_t>(param.sampleRate), nullptr);

    MarkParameter(static_cast<int32_t>(RecorderPublicParamType::AUD_SAMPLERATE));
    sampleRate_ = param.sampleRate;
    return MSERR_OK;
}

int32_t AudioSource::ConfigAudioChannels(const RecorderParam &recParam)
{
    const AudChannel &param = static_cast<const AudChannel &>(recParam);
    if (param.channel <= 0) {
        MEDIA_LOGE("The required audio channels %{public}d is invalid", param.channel);
        return MSERR_INVALID_VAL;
    }
    MEDIA_LOGI("Set audio channels: %{public}d", param.channel);
    g_object_set(gstElem_, "channels", static_cast<uint32_t>(param.channel), nullptr);

    MarkParameter(static_cast<int32_t>(RecorderPublicParamType::AUD_CHANNEL));
    channels_ = param.channel;
    return MSERR_OK;
}

int32_t AudioSource::ConfigAudioBitRate(const RecorderParam &recParam)
{
    const AudBitRate &param = static_cast<const AudBitRate &>(recParam);
    if (param.bitRate <= 0) {
        MEDIA_LOGE("The required audio bitrate %{public}d is invalid", param.bitRate);
        return MSERR_INVALID_VAL;
    }
    MEDIA_LOGI("Set audio bitrate: %{public}d", param.bitRate);
    g_object_set(gstElem_, "bitrate", static_cast<uint32_t>(param.bitRate), nullptr);

    MarkParameter(static_cast<int32_t>(RecorderPublicParamType::AUD_BITRATE));
    bitRate_ = param.bitRate;
    return MSERR_OK;
}

int32_t AudioSource::ConfigAppInfo(const RecorderParam &recParam)
{
    const AppInfo &param = static_cast<const AppInfo &>(recParam);
    g_object_set(gstElem_, "token-id", param.appTokenId_, nullptr);
    g_object_set(gstElem_, "app-uid", param.appUid_, nullptr);
    g_object_set(gstElem_, "app-pid", param.appPid_, nullptr);

    MEDIA_LOGI("Set app info done");
    MarkParameter(static_cast<int32_t>(RecorderPrivateParamType::APP_INFO));
    return MSERR_OK;
}

int32_t AudioSource::CheckConfigReady()
{
    std::set<int32_t> expectedParam = {
        RecorderPublicParamType::AUD_SAMPLERATE,
        RecorderPublicParamType::AUD_CHANNEL,
        RecorderPrivateParamType::APP_INFO,
    };

    if (!CheckAllParamsConfigured(expectedParam)) {
        MEDIA_LOGE("audiosource required parameter not configured completely, failed !");
        return MSERR_INVALID_OPERATION;
    }

    bool isSupportedParams = false;
    g_object_get(gstElem_, "supported-audio-params", &isSupportedParams, nullptr);
    CHECK_AND_RETURN_RET_LOG(isSupportedParams, MSERR_UNSUPPORT_AUD_PARAMS, "unsupport audio params");

    return MSERR_OK;
}

int32_t AudioSource::Prepare()
{
    MEDIA_LOGD("audio source prepare enter");
    g_object_set(gstElem_, "bypass-audio-service", FALSE, nullptr);
    return MSERR_OK;
}

int32_t AudioSource::Stop()
{
    MEDIA_LOGD("audio source stop enter");
    g_object_set(gstElem_, "bypass-audio-service", TRUE, nullptr);
    return MSERR_OK;
}

void AudioSource::Dump()
{
    MEDIA_LOGI("Audio [sourceId = 0x%{public}x]: sample rate = %{public}d, "
               "channels = %{public}d, bitRate = %{public}d",
               desc_.handle_, sampleRate_, channels_, bitRate_);
}

REGISTER_RECORDER_ELEMENT(AudioSource);
} // namespace Media
} // namespace OHOS
