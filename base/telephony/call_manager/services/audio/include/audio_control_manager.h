/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_AUDIO_MANAGER_H
#define TELEPHONY_AUDIO_MANAGER_H

#include <mutex>
#include <set>

#include "singleton.h"

#include "call_manager_inner_type.h"

#include "audio_proxy.h"
#include "call_state_listener_base.h"
#include "tone.h"
#include "ring.h"
#include "audio_device_manager.h"
#include "audio_scene_processor.h"

namespace OHOS {
namespace Telephony {
class AudioControlManager : public CallStateListenerBase, public std::enable_shared_from_this<AudioControlManager> {
    DECLARE_DELAYED_SINGLETON(AudioControlManager)
public:
    void Init();
    int32_t SetAudioDevice(AudioDevice device);
    bool PlayRingtone(); // plays the default ringtone
    bool PlayRingtone(const std::string &phoneNum); // plays the default ringtone
    bool PlayRingtone(const std::string &phoneNum, const std::string &ringtonePath); // plays the specific ringtone
    bool StopRingtone();
    int32_t PlayRingback();
    int32_t StopRingback();
    int32_t PlayWaitingTone();
    int32_t StopWaitingTone();
    int32_t PlayCallTone(ToneDescriptor type);
    int32_t StopCallTone();
    int32_t MuteRinger();
    int32_t SetMute(bool on);
    void SetVolumeAudible();
    bool IsTonePlaying() const;
    bool IsAudioActivated() const;
    bool IsCurrentRinging() const;
    bool IsActiveCallExists() const;
    bool ShouldSwitchActive() const;
    bool ShouldSwitchDialing() const;
    bool ShouldSwitchAlerting() const;
    bool ShouldSwitchIncoming() const;
    AudioDevice GetInitAudioDevice() const;
    std::set<sptr<CallBase>> GetCallList();
    sptr<CallBase> GetCurrentActiveCall() const;
    AudioInterruptState GetAudioInterruptState();
    bool UpdateCurrentCallState();
    void SetRingState(RingState state);
    void SetLocalRingbackNeeded(bool isNeeded);
    void SetAudioInterruptState(AudioInterruptState state);
    void NewCallCreated(sptr<CallBase> &callObjectPtr) override;
    void CallDestroyed(const DisconnectedDetails &details) override;
    void IncomingCallActivated(sptr<CallBase> &callObjectPtr) override;
    void IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content) override;
    void CallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState) override;

private:
    RingState ringState_ = RingState::STOPPED;
    void HandleNextState(sptr<CallBase> &callObjectPtr, TelCallState nextState);
    void HandlePriorState(sptr<CallBase> &callObjectPtr, TelCallState priorState);
    void HandleCallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState);
    void HandleNewActiveCall(sptr<CallBase> &callObjectPtr);
    bool IsNumberAllowed(const std::string &phoneNum);
    void PlayCallEndedTone(TelCallState priorState, TelCallState nextState, CallEndedType type);
    sptr<CallBase> GetCallBase(const std::string &phoneNum) const;
    AudioInterruptState audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_DEACTIVATED;
    bool ShouldPlayRingtone() const;
    bool IsEmergencyCallExists() const;
    bool isTonePlaying_;
    bool isLocalRingbackNeeded_;
    std::set<sptr<CallBase>> totalCalls_;
    std::unique_ptr<Ring> ring_;
    std::unique_ptr<Tone> tone_;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_AUDIO_MANAGER_H
