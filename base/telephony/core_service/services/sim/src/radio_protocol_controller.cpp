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
#include "radio_protocol_controller.h"

#include "radio_event.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
static const int64_t COMMUNICATION_TIMEOUT = 45 * 1000; // Set the timeout millisecond for radio protocol communication
static const int64_t SET_ACTIVE_OUT_TIME = 10 * 1000;
std::mutex RadioProtocolController::ctx_;
std::condition_variable RadioProtocolController::cv_;

RadioProtocolController::RadioProtocolController(std::shared_ptr<Telephony::ITelRilManager> telRilManager,
    const std::shared_ptr<AppExecFwk::EventRunner> &runner)
    : AppExecFwk::EventHandler(runner), telRilManager_(telRilManager)
{
    if (runner != nullptr) {
        runner->Run();
    }
}

void RadioProtocolController::Init()
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("RadioProtocolController::Init telRilManager_ is nullptr");
        return;
    }

    slotCount_ = SIM_SLOT_COUNT;
    InitMemberFunc();
    // make sure communication is in the initial state
    CleanUpCommunication();
    for (int32_t i = 0; i < slotCount_; i++) {
        RadioProtocol protocol;
        protocol.slotId = i;
        protocol.sessionId = 0;
        protocol.phase = RadioProtocolPhase::RADIO_PROTOCOL_PHASE_INITIAL;
        protocol.technology = 0xFFFFFFFF;
        protocol.modemId = 0;
        protocol.status = RadioProtocolStatus::RADIO_PROTOCOL_STATUS_NONE;
        radioProtocol_.emplace_back(protocol);
        telRilManager_->RegisterCoreNotify(i, shared_from_this(), RADIO_SIM_RADIO_PROTOCOL_NOTIFY, nullptr);
    }
}

int32_t RadioProtocolController::GetRadioProtocolTech(int32_t slotId)
{
    return static_cast<int32_t>(radioProtocol_[slotId].technology);
}

void RadioProtocolController::GetRadioProtocol(int32_t slotId)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("RadioProtocolController::GetRadioProtocol telRilManager_ is nullptr");
        return;
    }
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(RADIO_SIM_GET_RADIO_PROTOCOL);
    if (event == nullptr) {
        TELEPHONY_LOGE("RadioProtocolController::GetRadioProtocol event is nullptr");
        return;
    }
    event->SetOwner(shared_from_this());
    telRilManager_->GetRadioProtocol(slotId, event);
}

bool RadioProtocolController::SetRadioProtocol(int32_t slotId)
{
    if (isCommunicating_) {
        TELEPHONY_LOGE("RadioProtocolController::SetRadioProtocol protocol is communicating, can not set now");
        return false;
    }

    std::unique_lock<std::mutex> radioProtocolLock(radioProtocolMutex_);
    communicationResponseResult_ = false;
    CleanUpCommunication();
    ExecuteCheckCommunication();
    while (isCommunicating_) {
        TELEPHONY_LOGI("RadioProtocolController::SetRadioProtocol wait for the communication to finish");
        radioProtocolCv_.wait(radioProtocolLock);
    }
    return communicationResponseResult_;
}

void RadioProtocolController::InitMemberFunc()
{
    memberFuncMap_[MSG_SIM_TIME_OUT_ACTIVE] = &RadioProtocolController::ProcessActiveSimTimeOutDone;
    memberFuncMap_[MSG_SIM_SET_ACTIVE] = &RadioProtocolController::ProcessActiveSimToRilResponse;
    memberFuncMap_[RADIO_SIM_GET_RADIO_PROTOCOL] = &RadioProtocolController::ProcessGetRadioProtocol;
    memberFuncMap_[RADIO_SIM_CHECK_RADIO_PROTOCOL] = &RadioProtocolController::ProcessCheckRadioProtocol;
    memberFuncMap_[RADIO_SIM_UPDATE_RADIO_PROTOCOL] = &RadioProtocolController::ProcessUpdateRadioProtocol;
    memberFuncMap_[RADIO_SIM_RADIO_PROTOCOL_NOTIFY] = &RadioProtocolController::ProcessRadioProtocolNotify;
    memberFuncMap_[RADIO_SIM_SET_RADIO_PROTOCOL_COMPLETE] = &RadioProtocolController::ProcessSetRadioProtocolComplete;
    memberFuncMap_[RADIO_SIM_SET_RADIO_PROTOCOL_TIMEOUT] = &RadioProtocolController::ProcessSetRadioProtocolTimeout;
}

void RadioProtocolController::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("RadioProtocolController::ProcessEvent event is nullptr");
        return;
    }
    auto id = event->GetInnerEventId();
    TELEPHONY_LOGI("RadioProtocolController::ProcessEvent id = %{public}d", id);
    auto itFunc = memberFuncMap_.find(id);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc == nullptr) {
            TELEPHONY_LOGE("RadioProtocolController::ProcessEvent memberFunc is nullptr");
            return;
        }
        (this->*memberFunc)(event);
    }
}

void RadioProtocolController::ProcessGetRadioProtocol(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("RadioProtocolController::ProcessGetRadioProtocol event is nullptr");
        return;
    }
    std::shared_ptr<RadioProtocol> radioProtocol = event->GetSharedObject<RadioProtocol>();
    if (radioProtocol == nullptr) {
        TELEPHONY_LOGE("RadioProtocolController::ProcessGetRadioProtocol radioProtocol is nullptr");
        return;
    }
    UpdateRadioProtocol(radioProtocol);
}

void RadioProtocolController::ProcessCheckRadioProtocol(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (!ProcessResponseInfoOfEvent(event)) {
        TELEPHONY_LOGE("RadioProtocolController::ProcessCheckRadioProtocol failed due to invalid sessionId");
        return;
    }

    if (--communicatingSlotCount_ != 0) {
        TELEPHONY_LOGI("RadioProtocolController::ProcessCheckRadioProtocol wait for all sim to complete check");
        return;
    }

    if (communicationFailed_) {
        ExecuteCompleteCommunication();
    } else {
        ExecuteUpdateCommunication();
    }
}

void RadioProtocolController::ProcessUpdateRadioProtocol(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (!ProcessResponseInfoOfEvent(event)) {
        TELEPHONY_LOGE("RadioProtocolController::ProcessUpdateRadioProtocol failed due to invalid sessionId");
        return;
    }
}

void RadioProtocolController::ProcessRadioProtocolNotify(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("RadioProtocolController::ProcessRadioProtocolNotify event is nullptr");
        return;
    }
    std::shared_ptr<RadioProtocol> radioProtocol = event->GetSharedObject<RadioProtocol>();
    if (radioProtocol == nullptr || radioProtocol->sessionId != sessionId_) {
        TELEPHONY_LOGE("RadioProtocolController::ProcessRadioProtocolNotify failed due to invalid sessionId");
        return;
    }

    if (radioProtocol->status == RadioProtocolStatus::RADIO_PROTOCOL_STATUS_FAIL) {
        TELEPHONY_LOGE("RadioProtocolController::ProcessRadioProtocolNotify update failed");
        communicationFailed_ = true;
    } else {
        UpdateRadioProtocol(radioProtocol);
    }
    if (--communicatingSlotCount_ != 0) {
        TELEPHONY_LOGI("RadioProtocolController::ProcessRadioProtocolNotify wait for all sim to complete update");
        return;
    }

    ExecuteCompleteCommunication();
}

void RadioProtocolController::ProcessSetRadioProtocolComplete(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("RadioProtocolController::ProcessSetRadioProtocolComplete event is nullptr");
        return;
    }
    if (!ProcessResponseInfoOfEvent(event)) {
        TELEPHONY_LOGE("RadioProtocolController::ProcessSetRadioProtocolComplete failed due to invalid sessionId");
        return;
    }

    if (--communicatingSlotCount_ != 0) {
        TELEPHONY_LOGI("RadioProtocolController::ProcessSetRadioProtocolComplete wait for all sim to complete");
        return;
    }

    ProcessCommunicationResponse(!communicationFailed_);
    CleanUpCommunication();
}

void RadioProtocolController::ProcessSetRadioProtocolTimeout(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("RadioProtocolController::ProcessSetRadioProtocolTimeout event is nullptr");
        return;
    }
    auto sessionId = event->GetParam();
    if (sessionId != sessionId_) {
        TELEPHONY_LOGE("RadioProtocolController::ProcessSetRadioProtocolTimeout failed due to invalid sessionId");
        return;
    }

    sessionId_++;
    communicatingSlotCount_ = 0;
    communicationFailed_ = true;
    ExecuteCompleteCommunication();
}

void RadioProtocolController::ExecuteCheckCommunication()
{
    sessionId_++;
    isCommunicating_ = true;
    SendEvent(RADIO_SIM_SET_RADIO_PROTOCOL_TIMEOUT, sessionId_, COMMUNICATION_TIMEOUT);
    BuildRadioProtocolForCommunication(RadioProtocolPhase::RADIO_PROTOCOL_PHASE_CHECK,
        RadioProtocolStatus::RADIO_PROTOCOL_STATUS_NONE);
    ResetNextCommunicationSlotCount();
    SendRadioProtocolEvent(oldRadioProtocol_, RADIO_SIM_CHECK_RADIO_PROTOCOL);
}

void RadioProtocolController::ExecuteUpdateCommunication()
{
    BuildRadioProtocolForCommunication(RadioProtocolPhase::RADIO_PROTOCOL_PHASE_UPDATE,
        RadioProtocolStatus::RADIO_PROTOCOL_STATUS_NONE);
    ResetNextCommunicationSlotCount();
    SendRadioProtocolEvent(newRadioProtocol_, RADIO_SIM_UPDATE_RADIO_PROTOCOL);
}

void RadioProtocolController::ExecuteCompleteCommunication()
{
    TELEPHONY_LOGI("RadioProtocolController::ExecuteCompleteCommunication failed:%{public}d", communicationFailed_);
    BuildRadioProtocolForCommunication(RadioProtocolPhase::RADIO_PROTOCOL_PHASE_COMPLETE,
        communicationFailed_ ? RadioProtocolStatus::RADIO_PROTOCOL_STATUS_FAIL :
        RadioProtocolStatus::RADIO_PROTOCOL_STATUS_SUCCESS);
    ResetNextCommunicationSlotCount();
    SendRadioProtocolEvent(communicationFailed_ ? oldRadioProtocol_ : newRadioProtocol_,
        RADIO_SIM_SET_RADIO_PROTOCOL_COMPLETE);
}

void RadioProtocolController::ResetNextCommunicationSlotCount()
{
    communicatingSlotCount_ = slotCount_;
}

void RadioProtocolController::BuildRadioProtocolForCommunication(RadioProtocolPhase phase, RadioProtocolStatus status)
{
    switch (phase) {
        case RadioProtocolPhase::RADIO_PROTOCOL_PHASE_CHECK: {
            for (int32_t i = 0; i < slotCount_; i++) {
                RadioProtocol radioProtocol;
                radioProtocol.slotId = i;
                radioProtocol.sessionId = sessionId_;
                radioProtocol.phase = phase;
                radioProtocol.technology = radioProtocol_[i].technology;
                radioProtocol.modemId = radioProtocol_[i].modemId;
                radioProtocol.status = status;
                oldRadioProtocol_.emplace_back(radioProtocol);
                newRadioProtocol_.emplace_back(radioProtocol);
            }
            // Switch the tech and modemId of main and non-main card
            for (int32_t i = 0; i < slotCount_; i++) {
                newRadioProtocol_[i].technology = radioProtocol_[slotCount_ - 1 - i].technology;
                newRadioProtocol_[i].modemId = radioProtocol_[slotCount_ - 1 - i].modemId;
            }
            break;
        }
        case RadioProtocolPhase::RADIO_PROTOCOL_PHASE_UPDATE:
        case RadioProtocolPhase::RADIO_PROTOCOL_PHASE_NOTIFY:
        case RadioProtocolPhase::RADIO_PROTOCOL_PHASE_COMPLETE: {
            for (int32_t i = 0; i < slotCount_; i++) {
                oldRadioProtocol_[i].sessionId = sessionId_;
                oldRadioProtocol_[i].phase = phase;
                oldRadioProtocol_[i].status = status;
                newRadioProtocol_[i].sessionId = sessionId_;
                newRadioProtocol_[i].phase = phase;
                newRadioProtocol_[i].status = status;
            }
            break;
        }
        default:
            TELEPHONY_LOGE("RadioProtocolController::BuildRadioProtocolForCommunication invalid phase");
            break;
    }
}

void RadioProtocolController::SendRadioProtocolEvent(std::vector<RadioProtocol> radioProtocol, uint32_t eventId)
{
    if (telRilManager_ == nullptr || radioProtocol.empty()) {
        TELEPHONY_LOGE("RadioProtocolController::SendRadioProtocol telRilManager_ or radioProtocol is nullptr");
        ProcessCommunicationResponse(false);
        CleanUpCommunication();
        return;
    }

    for (int32_t i = 0; i < slotCount_; i++) {
        AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(eventId);
        if (event == nullptr) {
            TELEPHONY_LOGE("RadioProtocolController::SendRadioProtocol event is nullptr");
            ProcessCommunicationResponse(false);
            CleanUpCommunication();
            return;
        }
        event->SetOwner(shared_from_this());
        telRilManager_->SetRadioProtocol(i, radioProtocol[i], event);
    }
}

void RadioProtocolController::UpdateRadioProtocol(std::shared_ptr<RadioProtocol> radioProtocol)
{
    int32_t slotId = radioProtocol->slotId;
    if ((slotId >= DEFAULT_SIM_SLOT_ID) && (slotId < SIM_SLOT_COUNT)) {
        radioProtocol_[slotId].sessionId = radioProtocol->sessionId;
        radioProtocol_[slotId].phase = radioProtocol->phase;
        radioProtocol_[slotId].technology = radioProtocol->technology;
        radioProtocol_[slotId].modemId = radioProtocol->modemId;
        radioProtocol_[slotId].status = radioProtocol->status;
    }
}

bool RadioProtocolController::ProcessResponseInfoOfEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("RadioProtocolController::ProcessResponseInfoOfEvent event is nullptr");
        return false;
    }
    std::shared_ptr<HRilRadioResponseInfo> responseInfo = event->GetSharedObject<HRilRadioResponseInfo>();
    if (responseInfo != nullptr && responseInfo->error != HRilErrType::NONE) {
        TELEPHONY_LOGE("RadioProtocolController::ProcessResponseInfoOfEvent error:%{public}d", responseInfo->error);
        communicationFailed_ = true;
        return true;
    }

    std::shared_ptr<RadioProtocol> radioProtocol = event->GetSharedObject<RadioProtocol>();
    return radioProtocol != nullptr && radioProtocol->sessionId == sessionId_;
}

void RadioProtocolController::CleanUpCommunication()
{
    RemoveEvent(RADIO_SIM_SET_RADIO_PROTOCOL_TIMEOUT);
    communicationFailed_ = false;
    std::vector<RadioProtocol>().swap(oldRadioProtocol_);
    std::vector<RadioProtocol>().swap(newRadioProtocol_);
}

void RadioProtocolController::ProcessCommunicationResponse(bool result)
{
    communicationResponseResult_ = result;
    isCommunicating_ = false;
    radioProtocolCv_.notify_all();
}

void RadioProtocolController::UnRegisterEvents()
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("RadioProtocolController::UnRegisterEvents telRilManager_ is nullptr");
        return;
    }
    for (int32_t i = 0; i < slotCount_; i++) {
        telRilManager_->UnRegisterCoreNotify(i, shared_from_this(), RADIO_SIM_RADIO_PROTOCOL_NOTIFY);
    }
}

void RadioProtocolController::RadioProtocolControllerWait()
{
    responseReady_ = false;
}

void RadioProtocolController::RadioProtocolControllerContinue()
{
    responseReady_ = true;
}

bool RadioProtocolController::RadioProtocolControllerPoll()
{
    return responseReady_;
}

void RadioProtocolController::ProcessActiveSimTimeOutDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("RadioProtocolController::ProcessActiveSimTimeOutDone");
    activeResponse_ = 1;
    RadioProtocolControllerContinue();
    cv_.notify_all();
}

bool RadioProtocolController::SetActiveSimToRil(int32_t slotId, int32_t type, int32_t enable)
{
    TELEPHONY_LOGI("RadioProtocolController::SetActiveSim(), enable=%{public}d", enable);
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("RadioProtocolController::SetActiveSim nullptr");
        return false;
    }
    auto event = AppExecFwk::InnerEvent::Get(MSG_SIM_SET_ACTIVE);
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return false;
    }
    event->SetOwner(shared_from_this());
    SendEvent(MSG_SIM_TIME_OUT_ACTIVE, SET_ACTIVE_OUT_TIME, Priority::LOW);
    telRilManager_->SetActiveSim(slotId, type, enable, event);
    return true;
}

void RadioProtocolController::ProcessActiveSimToRilResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("RadioProtocolController::ProcessActiveSimToRilResponse event is nullptr");
        return;
    }
    TELEPHONY_LOGI("RadioProtocolController::GetSetActiveSimResult");
    int32_t result = 0;
    std::shared_ptr<HRilErrType> param = event->GetSharedObject<HRilErrType>();
    std::shared_ptr<HRilRadioResponseInfo> response = event->GetSharedObject<HRilRadioResponseInfo>();
    if ((param == nullptr) && (response == nullptr)) {
        TELEPHONY_LOGE("RadioProtocolController::GetSetActiveSimResult() fail");
        RadioProtocolControllerContinue();
        cv_.notify_all();
        return;
    }
    if (param != nullptr) {
        result = static_cast<int32_t>(*param);
    } else {
        result = static_cast<int32_t>(response->error);
    }
    TELEPHONY_LOGI("RadioProtocolController::GetSetActiveSimResult(), activeResponse = %{public}d", result);
    activeResponse_ = result;
    RadioProtocolControllerContinue();
    cv_.notify_all();
}

int32_t RadioProtocolController::GetActiveSimToRilResult()
{
    RemoveEvent(MSG_SIM_TIME_OUT_ACTIVE);
    return activeResponse_;
}
} // namespace Telephony
} // namespace OHOS
