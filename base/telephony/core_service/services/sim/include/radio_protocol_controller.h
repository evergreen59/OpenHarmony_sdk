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

#ifndef OHOS_RADIO_PROTOCOL_CONTROLLER_H
#define OHOS_RADIO_PROTOCOL_CONTROLLER_H

#include <mutex>
#include <condition_variable>

#include "event_handler.h"
#include "event_runner.h"
#include "i_tel_ril_manager.h"
#include "sim_constant.h"

namespace OHOS {
namespace Telephony {
class RadioProtocolController : public AppExecFwk::EventHandler {
public:
    RadioProtocolController(std::shared_ptr<Telephony::ITelRilManager> telRilManager,
        const std::shared_ptr<AppExecFwk::EventRunner> &runner);
    virtual ~RadioProtocolController() = default;

    void Init();
    int32_t GetRadioProtocolTech(int32_t slotId);
    void GetRadioProtocol(int32_t slotId);
    bool SetRadioProtocol(int32_t slotId);
    void UnRegisterEvents();
    void RadioProtocolControllerWait();
    bool RadioProtocolControllerPoll();
    bool SetActiveSimToRil(int32_t slotId, int32_t type, int32_t enable);
    int32_t GetActiveSimToRilResult();

public:
    static std::mutex ctx_;
    static std::condition_variable cv_;

private:
    void InitMemberFunc();
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event);
    void ProcessGetRadioProtocol(const AppExecFwk::InnerEvent::Pointer &event);
    void ProcessCheckRadioProtocol(const AppExecFwk::InnerEvent::Pointer &event);
    void ProcessUpdateRadioProtocol(const AppExecFwk::InnerEvent::Pointer &event);
    void ProcessRadioProtocolNotify(const AppExecFwk::InnerEvent::Pointer &event);
    void ProcessSetRadioProtocolComplete(const AppExecFwk::InnerEvent::Pointer &event);
    void ProcessSetRadioProtocolTimeout(const AppExecFwk::InnerEvent::Pointer &event);
    void ExecuteCheckCommunication();
    void ExecuteUpdateCommunication();
    void ExecuteCompleteCommunication();
    void ResetNextCommunicationSlotCount();
    void BuildRadioProtocolForCommunication(RadioProtocolPhase phase, RadioProtocolStatus status);
    void SendRadioProtocolEvent(std::vector<RadioProtocol> radioProtocol, uint32_t eventId);
    void UpdateRadioProtocol(std::shared_ptr<RadioProtocol> radioProtocol);
    void CleanUpCommunication();
    void ProcessCommunicationResponse(bool result);
    bool ProcessResponseInfoOfEvent(const AppExecFwk::InnerEvent::Pointer &event);
    void ProcessActiveSimToRilResponse(const AppExecFwk::InnerEvent::Pointer &event);
    void ProcessActiveSimTimeOutDone(const AppExecFwk::InnerEvent::Pointer &event);
    void RadioProtocolControllerContinue();

private:
    using ProcessFunc = void (RadioProtocolController::*)(const AppExecFwk::InnerEvent::Pointer &event);
    std::shared_ptr<Telephony::ITelRilManager> telRilManager_ = nullptr;
    std::map<int32_t, ProcessFunc> memberFuncMap_;
    std::vector<RadioProtocol> radioProtocol_;
    std::vector<RadioProtocol> oldRadioProtocol_;
    std::vector<RadioProtocol> newRadioProtocol_;
    int32_t slotCount_ = 0;
    int32_t sessionId_ = 0;
    int32_t communicatingSlotCount_ = 0;
    bool communicationFailed_ = false;
    bool isCommunicating_ = false;
    bool communicationResponseResult_ = false;
    std::mutex radioProtocolMutex_;
    std::condition_variable radioProtocolCv_;
    bool responseReady_ = false;
    int32_t activeResponse_ = 0;
};
} // namespace Telephony
} // namespace OHOS
#endif // OHOS_RADIO_PROTOCOL_CONTROLLER_H
