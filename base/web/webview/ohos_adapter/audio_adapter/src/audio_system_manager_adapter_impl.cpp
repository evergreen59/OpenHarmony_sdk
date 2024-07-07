/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "audio_system_manager_adapter_impl.h"

#include <unordered_map>

#include "audio_errors.h"
#include "audio_renderer_adapter_impl.h"
#include "nweb_log.h"

namespace OHOS::NWeb {
const std::unordered_map<AudioAdapterStreamType, AudioStreamType> STREAM_TYPE_MAP = {
    {AudioAdapterStreamType::STREAM_DEFAULT, AudioStreamType::STREAM_DEFAULT},
    {AudioAdapterStreamType::STREAM_VOICE_CALL, AudioStreamType::STREAM_VOICE_CALL},
    {AudioAdapterStreamType::STREAM_MUSIC, AudioStreamType::STREAM_MUSIC},
    {AudioAdapterStreamType::STREAM_RING, AudioStreamType::STREAM_RING},
    {AudioAdapterStreamType::STREAM_MEDIA, AudioStreamType::STREAM_MEDIA},
    {AudioAdapterStreamType::STREAM_VOICE_ASSISTANT, AudioStreamType::STREAM_VOICE_ASSISTANT},
    {AudioAdapterStreamType::STREAM_SYSTEM, AudioStreamType::STREAM_SYSTEM},
    {AudioAdapterStreamType::STREAM_ALARM, AudioStreamType::STREAM_ALARM},
    {AudioAdapterStreamType::STREAM_NOTIFICATION, AudioStreamType::STREAM_NOTIFICATION},
    {AudioAdapterStreamType::STREAM_BLUETOOTH_SCO, AudioStreamType::STREAM_BLUETOOTH_SCO},
    {AudioAdapterStreamType::STREAM_ENFORCED_AUDIBLE, AudioStreamType::STREAM_ENFORCED_AUDIBLE},
    {AudioAdapterStreamType::STREAM_DTMF, AudioStreamType::STREAM_DTMF},
    {AudioAdapterStreamType::STREAM_TTS, AudioStreamType::STREAM_TTS},
    {AudioAdapterStreamType::STREAM_ACCESSIBILITY, AudioStreamType::STREAM_ACCESSIBILITY},
    {AudioAdapterStreamType::STREAM_RECORDING, AudioStreamType::STREAM_RECORDING},
    {AudioAdapterStreamType::STREAM_ALL, AudioStreamType::STREAM_ALL},
};

AudioManagerCallbackAdapterImpl::AudioManagerCallbackAdapterImpl(
    std::shared_ptr<AudioManagerCallbackAdapter> cb) : cb_(cb) {};

void AudioManagerCallbackAdapterImpl::OnInterrupt(const InterruptAction &interruptAction)
{
    if (!cb_) {
        return;
    }
    switch (interruptAction.interruptHint) {
        case InterruptHint::INTERRUPT_HINT_PAUSE:
        case InterruptHint::INTERRUPT_HINT_STOP:
            cb_->OnSuspend();
            break;
        case InterruptHint::INTERRUPT_HINT_RESUME:
            cb_->OnResume();
            break;
        default:
            WVLOG_E("audio manager interrupt hint not foud, code: %{public}d", interruptAction.interruptHint);
            break;
    }
}

AudioSystemManagerAdapterImpl& AudioSystemManagerAdapterImpl::GetInstance()
{
    static AudioSystemManagerAdapterImpl instance;
    return instance;
}

bool AudioSystemManagerAdapterImpl::HasAudioOutputDevices() const
{
    DeviceType outputDeviceType = AudioSystemManager::GetInstance()->GetActiveOutputDevice();
    if (outputDeviceType == DeviceType::DEVICE_TYPE_NONE || outputDeviceType == DeviceType::DEVICE_TYPE_INVALID) {
        return false;
    }
    return true;
}

bool AudioSystemManagerAdapterImpl::HasAudioInputDevices() const
{
    DeviceType inputDeviceType = AudioSystemManager::GetInstance()->GetActiveInputDevice();
    if (inputDeviceType == DeviceType::DEVICE_TYPE_NONE || inputDeviceType == DeviceType::DEVICE_TYPE_INVALID) {
        return false;
    }
    return true;
}

int32_t AudioSystemManagerAdapterImpl::RequestAudioFocus(const AudioAdapterInterrupt &audioInterrupt)
{
    AudioInterrupt interruptParams;
    interruptParams.streamUsage = AudioRendererAdapterImpl::GetAudioStreamUsage(audioInterrupt.streamUsage);
    interruptParams.contentType = AudioRendererAdapterImpl::GetAudioContentType(audioInterrupt.contentType);
    interruptParams.streamType = GetStreamType(audioInterrupt.streamType);

    int32_t ret = AudioSystemManager::GetInstance()->RequestAudioFocus(interruptParams);
    if (ret != AudioStandard::SUCCESS) {
        WVLOG_E("audio request audio focus failed, code: %{public}d", ret);
        return AUDIO_ERROR;
    }
    return AUDIO_OK;
}

int32_t AudioSystemManagerAdapterImpl::AbandonAudioFocus(const AudioAdapterInterrupt &audioInterrupt)
{
    AudioInterrupt interruptParams;
    interruptParams.streamUsage = AudioRendererAdapterImpl::GetAudioStreamUsage(audioInterrupt.streamUsage);
    interruptParams.contentType = AudioRendererAdapterImpl::GetAudioContentType(audioInterrupt.contentType);
    interruptParams.streamType = GetStreamType(audioInterrupt.streamType);

    int32_t ret = AudioSystemManager::GetInstance()->AbandonAudioFocus(interruptParams);
    if (ret != AudioStandard::SUCCESS) {
        WVLOG_E("audio abandon audio focus failed, code: %{public}d", ret);
        return AUDIO_ERROR;
    }
    return AUDIO_OK;
}

int32_t AudioSystemManagerAdapterImpl::SetAudioManagerInterruptCallback(
    const std::shared_ptr<AudioManagerCallbackAdapter> &callback)
{
    if (callback == nullptr) {
        WVLOG_E("set audio manager interrupt callback is nullptr");
        return AUDIO_NULL_ERROR;
    }
    callback_ = std::make_shared<AudioManagerCallbackAdapterImpl>(callback);

    int32_t ret = AudioSystemManager::GetInstance()->SetAudioManagerInterruptCallback(callback_);
    if (ret != AudioStandard::SUCCESS) {
        WVLOG_E("audio manager set interrupt callback failed, code: %{public}d", ret);
        return AUDIO_ERROR;
    }
    return AUDIO_OK;
}

int32_t AudioSystemManagerAdapterImpl::UnsetAudioManagerInterruptCallback()
{
    int32_t ret = AudioSystemManager::GetInstance()->UnsetAudioManagerInterruptCallback();
    if (ret != AudioStandard::SUCCESS) {
        WVLOG_E("audio manager unset interrupt callback failed, code: %{public}d", ret);
        return AUDIO_ERROR;
    }
    callback_ = nullptr;
    return AUDIO_OK;
}

AudioStreamType AudioSystemManagerAdapterImpl::GetStreamType(AudioAdapterStreamType streamType)
{
    auto item = STREAM_TYPE_MAP.find(streamType);
    if (item == STREAM_TYPE_MAP.end()) {
        WVLOG_E("audio stream type not found");
        return AudioStreamType::STREAM_DEFAULT;
    }
    return item->second;
}
}  // namespace OHOS::NWeb