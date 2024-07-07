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

#include "audiodevice_fuzzer.h"

#include <cstddef>
#include <cstdint>
#define private public
#include "addcalltoken_fuzzer.h"
#include "audio_control_manager.h"
#include "call_manager_service.h"
#include "cs_call.h"
#include "system_ability_definition.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
constexpr int32_t AUDIO_INTERRUPT_STATE_NUM = 4;
constexpr int32_t BOOL_NUM = 2;
constexpr int32_t CALL_TYPE_NUM = 4;
constexpr int32_t RING_STATE_NUM = 2;
constexpr int32_t DIAL_TYPE = 3;
constexpr int32_t VIDIO_TYPE_NUM = 2;
constexpr int32_t TEL_CALL_STATE_NUM = 9;
constexpr int32_t AUDIO_DEVICE_NUM = 6;
constexpr int32_t AUDIO_EVENT = 30;
constexpr int32_t AUDIO_SCENE_NUM = 4;
constexpr int32_t AUDIO_VOLUME_TYPE_NUM = 13;

bool IsServiceInited()
{
    if (!g_isInited) {
        DelayedSingleton<CallManagerService>::GetInstance()->OnStart();
        if (DelayedSingleton<CallManagerService>::GetInstance()->GetServiceRunningState() ==
            static_cast<int32_t>(CallManagerService::ServiceRunningState::STATE_RUNNING)) {
            g_isInited = true;
        }
    }
    return g_isInited;
}

void AudioControlManagerFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    std::shared_ptr<AudioControlManager> audioControlManager = DelayedSingleton<AudioControlManager>::GetInstance();
    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(size % DIAL_TYPE);
    paraInfo.callType = static_cast<CallType>(size % CALL_TYPE_NUM);
    paraInfo.videoState = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    paraInfo.callState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    TelCallState priorState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    TelCallState nextState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    std::string message(reinterpret_cast<const char *>(data), size);
    AudioInterruptState state = static_cast<AudioInterruptState>(size % AUDIO_INTERRUPT_STATE_NUM);
    DisconnectedDetails details;
    bool isMute = static_cast<bool>(size % BOOL_NUM);
    RingState ringState = static_cast<RingState>(size % RING_STATE_NUM);
    AudioDevice device = static_cast<AudioDevice>(size % AUDIO_DEVICE_NUM);

    audioControlManager->SetAudioInterruptState(state);
    audioControlManager->SetAudioDevice(device);
    audioControlManager->CallDestroyed(details);
    audioControlManager->NewCallCreated(callObjectPtr);
    audioControlManager->IncomingCallActivated(callObjectPtr);
    audioControlManager->IncomingCallHungUp(callObjectPtr, true, message);
    audioControlManager->CallStateUpdated(callObjectPtr, priorState, nextState);
    audioControlManager->HandleCallStateUpdated(callObjectPtr, priorState, nextState);
    audioControlManager->HandleNextState(callObjectPtr, nextState);
    audioControlManager->HandleNextState(callObjectPtr, priorState);
    audioControlManager->HandleNewActiveCall(callObjectPtr);
    audioControlManager->StopRingtone();
    audioControlManager->GetInitAudioDevice();
    audioControlManager->SetMute(isMute);
    audioControlManager->MuteRinger();
    audioControlManager->GetCallList();
    audioControlManager->GetCurrentActiveCall();
    audioControlManager->IsEmergencyCallExists();
    audioControlManager->GetAudioInterruptState();
    audioControlManager->SetVolumeAudible();
    audioControlManager->SetRingState(ringState);
    audioControlManager->SetLocalRingbackNeeded(isMute);
    audioControlManager->IsAudioActivated();
    audioControlManager->StopCallTone();
    audioControlManager->IsCurrentRinging();
    audioControlManager->StopRingback();
    audioControlManager->StopWaitingTone();
}

void AudioDeviceManagerFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    std::shared_ptr<AudioDeviceManager> audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    AudioDevice device = static_cast<AudioDevice>(size % AUDIO_DEVICE_NUM);
    AudioEvent event = static_cast<AudioEvent>(size % AUDIO_EVENT);
    std::string bluetoothAddress(reinterpret_cast<const char *>(data), size);
    bool available = static_cast<bool>(size % BOOL_NUM);

    audioDeviceManager->Init();
    audioDeviceManager->InitAudioDevice();
    audioDeviceManager->ProcessEvent(event);
    audioDeviceManager->SwitchDevice(event);
    audioDeviceManager->ConnectBtScoWithAddress(bluetoothAddress);
    audioDeviceManager->SwitchDevice(device);
    audioDeviceManager->EnableBtSco();
    audioDeviceManager->EnableWiredHeadset();
    audioDeviceManager->EnableSpeaker();
    audioDeviceManager->EnableEarpiece();
    audioDeviceManager->DisableAll();
    audioDeviceManager->IsBtScoDevEnable();
    audioDeviceManager->IsSpeakerDevEnable();
    audioDeviceManager->IsEarpieceDevEnable();
    audioDeviceManager->IsWiredHeadsetDevEnable();
    audioDeviceManager->SetBtScoDevEnable();
    audioDeviceManager->SetSpeakerDevEnable();
    audioDeviceManager->SetEarpieceDevEnable();
    audioDeviceManager->SetWiredHeadsetDevEnable();
    audioDeviceManager->GetCurrentAudioDevice();
    audioDeviceManager->SetCurrentAudioDevice(device);
    audioDeviceManager->IsEarpieceAvailable();
    audioDeviceManager->IsSpeakerAvailable();
    audioDeviceManager->IsBtScoConnected();
    audioDeviceManager->IsWiredHeadsetConnected();
    audioDeviceManager->SetSpeakerAvailable(available);
    audioDeviceManager->SetWiredHeadsetAvailable(available);
    audioDeviceManager->SetBtScoAvailable(available);
}

void AudioProxyFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    std::shared_ptr<AudioProxy> audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    std::shared_ptr<AudioControlManager> audioControlManager = DelayedSingleton<AudioControlManager>::GetInstance();
    AudioStandard::AudioScene audioScene = static_cast<AudioStandard::AudioScene>(size % AUDIO_SCENE_NUM);
    AudioStandard::AudioVolumeType audioVolumeType =
        static_cast<AudioStandard::AudioVolumeType>(size % AUDIO_VOLUME_TYPE_NUM);
    std::string phoneNum(reinterpret_cast<const char *>(data), size);
    int32_t volume = static_cast<int32_t>(size);
    bool isMute = static_cast<bool>(size % BOOL_NUM);

    audioControlManager->GetCallBase(phoneNum);
    audioProxy->SetAudioScene(audioScene);
    audioProxy->SetVolumeAudible();
    audioProxy->IsMicrophoneMute();
    audioProxy->SetMicrophoneMute(isMute);
    audioProxy->SetEarpieceDevActive();
    audioProxy->SetSpeakerDevActive();
    audioProxy->SetBluetoothDevActive();
    audioProxy->SetWiredHeadsetDevActive();
    audioProxy->GetRingerMode();
    audioProxy->GetVolume(audioVolumeType);
    audioProxy->SetVolume(audioVolumeType, volume);
    audioProxy->SetMaxVolume(audioVolumeType);
    audioProxy->IsStreamActive(audioVolumeType);
    audioProxy->IsStreamMute(audioVolumeType);
    audioProxy->GetMaxVolume(audioVolumeType);
    audioProxy->GetMinVolume(audioVolumeType);
    audioProxy->SetAudioDeviceChangeCallback();
    audioProxy->IsVibrateMode();
    audioProxy->StartVibrate();
    audioProxy->CancelVibrate();
    audioProxy->GetSystemRingtoneUri();
    audioProxy->GetDefaultRingPath();
    audioProxy->GetDefaultTonePath();
    audioProxy->GetDefaultDtmfPath();
}

void AudioSceneProcessorFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    std::shared_ptr<AudioSceneProcessor> audioSceneProcessor = DelayedSingleton<AudioSceneProcessor>::GetInstance();
    std::shared_ptr<AudioControlManager> audioControlManager = DelayedSingleton<AudioControlManager>::GetInstance();
    std::string phoneNum(reinterpret_cast<const char *>(data), size);

    audioControlManager->IsNumberAllowed(phoneNum);
    audioSceneProcessor->Init();
    audioSceneProcessor->SwitchCS();
    audioSceneProcessor->SwitchIMS();
    audioSceneProcessor->SwitchOTT();
    audioSceneProcessor->SwitchHolding();
    audioSceneProcessor->SwitchInactive();
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    AudioControlManagerFunc(data, size);
    AudioDeviceManagerFunc(data, size);
    AudioProxyFunc(data, size);
    AudioSceneProcessorFunc(data, size);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::AddCallTokenFuzzer token;
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
