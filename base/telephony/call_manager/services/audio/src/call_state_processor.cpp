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

#include "call_state_processor.h"

#include "audio_scene_processor.h"

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
CallStateProcessor::CallStateProcessor() {}

CallStateProcessor::~CallStateProcessor()
{
    activeCalls_.clear();
    alertingCalls_.clear();
    incomingCalls_.clear();
    holdingCalls_.clear();
    dialingCalls_.clear();
}

void CallStateProcessor::AddCall(const std::string &phoneNum, TelCallState state)
{
    switch (state) {
        case TelCallState::CALL_STATUS_DIALING:
            if (dialingCalls_.count(phoneNum) == EMPTY_VALUE) {
                TELEPHONY_LOGI("add call , state : dialing");
                dialingCalls_.insert(phoneNum);
            }
            break;
        case TelCallState::CALL_STATUS_ALERTING:
            if (alertingCalls_.count(phoneNum) == EMPTY_VALUE) {
                TELEPHONY_LOGI("add call , state : alerting");
                alertingCalls_.insert(phoneNum);
            }
            break;
        case TelCallState::CALL_STATUS_INCOMING:
            if (incomingCalls_.count(phoneNum) == EMPTY_VALUE) {
                TELEPHONY_LOGI("add call , state : incoming");
                incomingCalls_.insert(phoneNum);
            }
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            if (activeCalls_.count(phoneNum) == EMPTY_VALUE) {
                TELEPHONY_LOGI("add call , state : active");
                activeCalls_.insert(phoneNum);
            }
            break;
        case TelCallState::CALL_STATUS_HOLDING:
            if (holdingCalls_.count(phoneNum) == EMPTY_VALUE) {
                TELEPHONY_LOGI("add call , state : holding");
                holdingCalls_.insert(phoneNum);
            }
            break;
        default:
            break;
    }
}

void CallStateProcessor::DeleteCall(const std::string &phoneNum, TelCallState state)
{
    switch (state) {
        case TelCallState::CALL_STATUS_DIALING:
            if (dialingCalls_.count(phoneNum) > EMPTY_VALUE) {
                TELEPHONY_LOGI("erase call , state : dialing");
                dialingCalls_.erase(phoneNum);
            }
            break;
        case TelCallState::CALL_STATUS_ALERTING:
            if (alertingCalls_.count(phoneNum) > EMPTY_VALUE) {
                TELEPHONY_LOGI("erase call , state : alerting");
                alertingCalls_.erase(phoneNum);
            }
            break;
        case TelCallState::CALL_STATUS_INCOMING:
            if (incomingCalls_.count(phoneNum) > EMPTY_VALUE) {
                TELEPHONY_LOGI("erase call , state : incoming");
                incomingCalls_.erase(phoneNum);
            }
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            if (activeCalls_.count(phoneNum) > EMPTY_VALUE) {
                TELEPHONY_LOGI("erase call , state : active");
                activeCalls_.erase(phoneNum);
            }
            break;
        case TelCallState::CALL_STATUS_HOLDING:
            if (holdingCalls_.count(phoneNum) > EMPTY_VALUE) {
                TELEPHONY_LOGI("erase call , state : holding");
                holdingCalls_.erase(phoneNum);
            }
            break;
        default:
            break;
    }
}

int32_t CallStateProcessor::GetCallNumber(TelCallState state)
{
    int32_t number = EMPTY_VALUE;
    switch (state) {
        case TelCallState::CALL_STATUS_DIALING:
            number = static_cast<int32_t>(dialingCalls_.size());
            break;
        case TelCallState::CALL_STATUS_ALERTING:
            number = static_cast<int32_t>(alertingCalls_.size());
            break;
        case TelCallState::CALL_STATUS_INCOMING:
            number = static_cast<int32_t>(incomingCalls_.size());
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            number = static_cast<int32_t>(activeCalls_.size());
            break;
        case TelCallState::CALL_STATUS_HOLDING:
            number = static_cast<int32_t>(holdingCalls_.size());
            break;
        default:
            break;
    }
    return number;
}

bool CallStateProcessor::ShouldSwitchState(TelCallState callState)
{
    bool shouldSwitch = false;
    switch (callState) {
        case TelCallState::CALL_STATUS_DIALING:
            shouldSwitch = (dialingCalls_.size() == EXIST_ONLY_ONE_CALL && activeCalls_.empty() &&
                incomingCalls_.empty() && alertingCalls_.empty());
            break;
        case TelCallState::CALL_STATUS_ALERTING:
            shouldSwitch = (alertingCalls_.size() == EXIST_ONLY_ONE_CALL && activeCalls_.empty() &&
                incomingCalls_.empty() && dialingCalls_.empty());
            break;
        case TelCallState::CALL_STATUS_INCOMING:
            shouldSwitch = (incomingCalls_.size() == EXIST_ONLY_ONE_CALL && activeCalls_.empty() &&
                dialingCalls_.empty() && alertingCalls_.empty());
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            shouldSwitch = (activeCalls_.size() == EXIST_ONLY_ONE_CALL);
            break;
        default:
            break;
    }
    return shouldSwitch;
}

bool CallStateProcessor::UpdateCurrentCallState()
{
    if (activeCalls_.size() > EMPTY_VALUE) {
        // no need to update call state while active calls exists
        return false;
    }
    AudioEvent event = AudioEvent::UNKNOWN_EVENT;
    if (holdingCalls_.size() > EMPTY_VALUE) {
        event = AudioEvent::SWITCH_HOLDING_STATE;
    } else if (incomingCalls_.size() > EMPTY_VALUE) {
        event = AudioEvent::SWITCH_INCOMING_STATE;
    } else if (dialingCalls_.size() > EMPTY_VALUE) {
        event = AudioEvent::SWITCH_DIALING_STATE;
    } else if (alertingCalls_.size() > EMPTY_VALUE) {
        event = AudioEvent::SWITCH_ALERTING_STATE;
    } else {
        event = AudioEvent::SWITCH_AUDIO_INACTIVE_STATE;
    }
    return DelayedSingleton<AudioSceneProcessor>::GetInstance()->ProcessEvent(event);
}

std::string CallStateProcessor::GetCurrentActiveCall() const
{
    if (activeCalls_.size() > EMPTY_VALUE) {
        return (*activeCalls_.begin());
    }
    return "";
}
} // namespace Telephony
} // namespace OHOS