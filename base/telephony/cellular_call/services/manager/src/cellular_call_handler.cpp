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

#include "cellular_call_handler.h"

#include "cellular_call_config.h"
#include "cellular_call_hisysevent.h"
#include "cellular_call_service.h"
#include "hitrace_meter.h"
#include "hril_call_parcel.h"
#include "ims_call_client.h"
#include "operator_config_types.h"
#include "radio_event.h"
#include "resource_utils.h"

namespace OHOS {
namespace Telephony {
const uint32_t GET_CS_CALL_DATA_ID = 10001;
const uint32_t GET_IMS_CALL_DATA_ID = 10002;
const uint32_t OPERATOR_CONFIG_CHANGED_ID = 10004;
const int64_t DELAY_TIME = 100;
const int64_t FAST_DELAY_TIME = 250;

CellularCallHandler::CellularCallHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner, const EventFwk::CommonEventSubscribeInfo &subscriberInfo)
    : AppExecFwk::EventHandler(runner), CommonEventSubscriber(subscriberInfo)
{
    InitBasicFuncMap();
    InitConfigFuncMap();
    InitSupplementFuncMap();
    InitActiveReportFuncMap();
}

void CellularCallHandler::InitBasicFuncMap()
{
    requestFuncMap_[RadioEvent::RADIO_DIAL] = &CellularCallHandler::DialResponse;
    requestFuncMap_[RadioEvent::RADIO_HANGUP_CONNECT] = &CellularCallHandler::CommonResultResponse;
    requestFuncMap_[RadioEvent::RADIO_REJECT_CALL] = &CellularCallHandler::CommonResultResponse;
    requestFuncMap_[RadioEvent::RADIO_ACCEPT_CALL] = &CellularCallHandler::CommonResultResponse;
    requestFuncMap_[RadioEvent::RADIO_HOLD_CALL] = &CellularCallHandler::CommonResultResponse;
    requestFuncMap_[RadioEvent::RADIO_ACTIVE_CALL] = &CellularCallHandler::CommonResultResponse;
    requestFuncMap_[RadioEvent::RADIO_SWAP_CALL] = &CellularCallHandler::CommonResultResponse;
    requestFuncMap_[RadioEvent::RADIO_JOIN_CALL] = &CellularCallHandler::CommonResultResponse;
    requestFuncMap_[RadioEvent::RADIO_SPLIT_CALL] = &CellularCallHandler::CommonResultResponse;
    requestFuncMap_[RadioEvent::RADIO_CALL_SUPPLEMENT] = &CellularCallHandler::CommonResultResponse;
    requestFuncMap_[RadioEvent::RADIO_SEND_DTMF] = &CellularCallHandler::SendDtmfResponse;
    requestFuncMap_[RadioEvent::RADIO_START_DTMF] = &CellularCallHandler::StartDtmfResponse;
    requestFuncMap_[RadioEvent::RADIO_STOP_DTMF] = &CellularCallHandler::StopDtmfResponse;
    requestFuncMap_[RadioEvent::RADIO_CURRENT_CALLS] = &CellularCallHandler::GetCsCallsDataResponse;
    requestFuncMap_[RadioEvent::RADIO_GET_CALL_FAIL_REASON] = &CellularCallHandler::GetCallFailReasonResponse;

    requestFuncMap_[GET_CS_CALL_DATA_ID] = &CellularCallHandler::GetCsCallsDataRequest;
    requestFuncMap_[GET_IMS_CALL_DATA_ID] = &CellularCallHandler::GetImsCallsDataRequest;
    requestFuncMap_[REGISTER_HANDLER_ID] = &CellularCallHandler::RegisterHandler;
    requestFuncMap_[MMIHandlerId::EVENT_MMI_Id] = &CellularCallHandler::GetMMIResponse;

    requestFuncMap_[RadioEvent::RADIO_IMS_GET_CALL_DATA] = &CellularCallHandler::GetImsCallsDataResponse;
}

void CellularCallHandler::InitConfigFuncMap()
{
    requestFuncMap_[RadioEvent::RADIO_SET_CMUT] = &CellularCallHandler::SetMuteResponse;
    requestFuncMap_[RadioEvent::RADIO_GET_CMUT] = &CellularCallHandler::GetMuteResponse;
    requestFuncMap_[RadioEvent::RADIO_SET_CALL_PREFERENCE_MODE] = &CellularCallHandler::SetDomainPreferenceModeResponse;
    requestFuncMap_[RadioEvent::RADIO_GET_CALL_PREFERENCE_MODE] = &CellularCallHandler::GetDomainPreferenceModeResponse;
    requestFuncMap_[RadioEvent::RADIO_SET_IMS_SWITCH_STATUS] = &CellularCallHandler::SetImsSwitchStatusResponse;
    requestFuncMap_[RadioEvent::RADIO_GET_IMS_SWITCH_STATUS] = &CellularCallHandler::GetImsSwitchStatusResponse;
    requestFuncMap_[RadioEvent::RADIO_SET_EMERGENCY_CALL_LIST] = &CellularCallHandler::SetEmergencyCallListResponse;
    requestFuncMap_[RadioEvent::RADIO_GET_EMERGENCY_CALL_LIST] = &CellularCallHandler::GetEmergencyCallListResponse;
    requestFuncMap_[OPERATOR_CONFIG_CHANGED_ID] = &CellularCallHandler::HandleOperatorConfigChanged;
}

void CellularCallHandler::InitSupplementFuncMap()
{
    requestFuncMap_[RadioEvent::RADIO_GET_CALL_WAIT] = &CellularCallHandler::GetCallWaitingResponse;
    requestFuncMap_[RadioEvent::RADIO_SET_CALL_WAIT] = &CellularCallHandler::SetCallWaitingResponse;
    requestFuncMap_[RadioEvent::RADIO_GET_CALL_FORWARD] = &CellularCallHandler::GetCallTransferResponse;
    requestFuncMap_[RadioEvent::RADIO_SET_CALL_FORWARD] = &CellularCallHandler::SetCallTransferInfoResponse;
    requestFuncMap_[RadioEvent::RADIO_GET_CALL_CLIP] = &CellularCallHandler::GetClipResponse;
    requestFuncMap_[RadioEvent::RADIO_GET_CALL_CLIR] = &CellularCallHandler::GetClirResponse;
    requestFuncMap_[RadioEvent::RADIO_SET_CALL_CLIR] = &CellularCallHandler::SetClirResponse;
    requestFuncMap_[RadioEvent::RADIO_GET_CALL_RESTRICTION] = &CellularCallHandler::GetCallRestrictionResponse;
    requestFuncMap_[RadioEvent::RADIO_SET_CALL_RESTRICTION] = &CellularCallHandler::SetCallRestrictionResponse;
    requestFuncMap_[RadioEvent::RADIO_SET_USSD] = &CellularCallHandler::SendUssdResponse;
    requestFuncMap_[MMIHandlerId::EVENT_SET_UNLOCK_PIN_PUK_ID] = &CellularCallHandler::SendUnlockPinPukResponse;
}

void CellularCallHandler::InitActiveReportFuncMap()
{
    requestFuncMap_[RadioEvent::RADIO_CALL_STATUS_INFO] = &CellularCallHandler::CsCallStatusInfoReport;
    requestFuncMap_[RadioEvent::RADIO_IMS_CALL_STATUS_INFO] = &CellularCallHandler::ImsCallStatusInfoReport;
    requestFuncMap_[RadioEvent::RADIO_AVAIL] = &CellularCallHandler::GetCsCallData;
    requestFuncMap_[RadioEvent::RADIO_NOT_AVAIL] = &CellularCallHandler::GetCsCallData;
    requestFuncMap_[RadioEvent::RADIO_CALL_USSD_NOTICE] = &CellularCallHandler::UssdNotifyResponse;
    requestFuncMap_[RadioEvent::RADIO_CALL_RINGBACK_VOICE] = &CellularCallHandler::CallRingBackVoiceResponse;
    requestFuncMap_[RadioEvent::RADIO_CALL_SRVCC_STATUS] = &CellularCallHandler::UpdateSrvccStateReport;
    requestFuncMap_[RadioEvent::RADIO_CALL_SS_NOTICE] = &CellularCallHandler::SsNotifyResponse;
    requestFuncMap_[RadioEvent::RADIO_CALL_EMERGENCY_NUMBER_REPORT] = &CellularCallHandler::ReportEccChanged;
    requestFuncMap_[RadioEvent::RADIO_SIM_STATE_CHANGE] = &CellularCallHandler::SimStateChangeReport;
    requestFuncMap_[RadioEvent::RADIO_SIM_RECORDS_LOADED] = &CellularCallHandler::SimRecordsLoadedReport;
    requestFuncMap_[RadioEvent::RADIO_CALL_RSRVCC_STATUS] = &CellularCallHandler::UpdateRsrvccStateReport;
#ifdef CALL_MANAGER_AUTO_START_OPTIMIZE
    requestFuncMap_[RadioEvent::RADIO_STATE_CHANGED] = &CellularCallHandler::RadioStateChangeProcess;
#endif
}

void CellularCallHandler::RegisterImsCallCallbackHandler()
{
    // Register IMS
    std::shared_ptr<ImsCallClient> imsCallClient = DelayedSingleton<ImsCallClient>::GetInstance();
    if (imsCallClient != nullptr) {
        imsCallClient->RegisterImsCallCallbackHandler(slotId_, shared_from_this());
    }
}

void CellularCallHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CellularCallHandler::ProcessEvent, event is nullptr");
        return;
    }

    uint32_t eventId = event->GetInnerEventId();
    TELEPHONY_LOGI("CellularCallHandler::ProcessEvent(), eventId = %{public}d, slotId = %{public}d", eventId, slotId_);

    auto itFunc = requestFuncMap_.find(event->GetInnerEventId());
    if (itFunc != requestFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(event);
        }
    }
    TELEPHONY_LOGI("CellularCallHandler::ProcessEvent, default case, need check.");
}

void CellularCallHandler::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    EventFwk::Want want = data.GetWant();
    std::string action = want.GetAction();
    TELEPHONY_LOGI("Slot%{public}d: action=%{public}s code=%{public}d", slotId_, action.c_str(), data.GetCode());
    if (action == EventFwk::CommonEventSupport::COMMON_EVENT_OPERATOR_CONFIG_CHANGED) {
        int32_t slotId = want.GetIntParam(BROADCAST_ARG_SLOT_ID, DEFAULT_SIM_SLOT_ID);
        if (slotId_ != slotId) {
            return;
        }
        this->SendEvent(OPERATOR_CONFIG_CHANGED_ID, DELAY_TIME, Priority::HIGH);
    }
}

void CellularCallHandler::GetCsCallData(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("GetCsCallData return, event is nullptr");
        return;
    }
    TELEPHONY_LOGI("GetCsCallData event id: %{public}d", event->GetInnerEventId());
    this->SendEvent(GET_CS_CALL_DATA_ID, DELAY_TIME, Priority::HIGH);
}

void CellularCallHandler::GetImsCallData(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("GetImsCallData return, event is nullptr");
        return;
    }
    TELEPHONY_LOGI("GetImsCallData event id: %{public}d", event->GetInnerEventId());
    this->SendEvent(GET_IMS_CALL_DATA_ID, DELAY_TIME, Priority::HIGH);
}

void CellularCallHandler::CellularCallIncomingStartTrace(const int32_t state)
{
    if (state == static_cast<int32_t>(TelCallState::CALL_STATUS_INCOMING)) {
        StartAsyncTrace(HITRACE_TAG_OHOS, "CellularCallIncoming", getpid());
    }
}

void CellularCallHandler::CellularCallIncomingFinishTrace(const int32_t state)
{
    if (state == static_cast<int32_t>(TelCallState::CALL_STATUS_INCOMING)) {
        FinishAsyncTrace(HITRACE_TAG_OHOS, "CellularCallIncoming", getpid());
    }
}

void CellularCallHandler::ReportCsCallsData(const CallInfoList &callInfoList)
{
    auto serviceInstance = DelayedSingleton<CellularCallService>::GetInstance();
    if (serviceInstance == nullptr) {
        TELEPHONY_LOGE("ReportCsCallsData return, serviceInstance is nullptr");
        return;
    }
    auto csControl = serviceInstance->GetCsControl(slotId_);
    CallInfo callInfo;
    std::vector<CallInfo>::const_iterator it = callInfoList.calls.begin();
    for (; it != callInfoList.calls.end(); ++it) {
        callInfo.state = (*it).state;
    }
    CellularCallIncomingStartTrace(callInfo.state);
    if (callInfoList.callSize == 0) {
        if (isInCsRedial_) {
            TELEPHONY_LOGI("ReportCsCallsData return, ignore hangup during cs redial");
            isInCsRedial_ = false;
            return;
        }
        if (csControl == nullptr) {
            TELEPHONY_LOGE("ReportCsCallsData return, cs_control is nullptr");
            CellularCallIncomingFinishTrace(callInfo.state);
            return;
        }
        if (csControl->ReportCallsData(slotId_, callInfoList) != TELEPHONY_SUCCESS) {
            CellularCallIncomingFinishTrace(callInfo.state);
        }
        serviceInstance->SetCsControl(slotId_, nullptr);
        return;
    }
    if (isInCsRedial_) {
        TELEPHONY_LOGI("ReportCsCallsData return, ignore cs call state change during cs redial");
        return;
    }
    if (callInfoList.callSize == 1) {
        if (csControl == nullptr) {
            csControl = std::make_shared<CSControl>();
            serviceInstance->SetCsControl(slotId_, csControl);
        }
    }
    if (csControl == nullptr) {
        TELEPHONY_LOGE("ReportCsCallsData return, cs_control is nullptr");
        CellularCallIncomingFinishTrace(callInfo.state);
        return;
    }
    if (csControl->ReportCallsData(slotId_, callInfoList) != TELEPHONY_SUCCESS) {
        CellularCallIncomingFinishTrace(callInfo.state);
    }
}

void CellularCallHandler::ReportImsCallsData(const ImsCurrentCallList &imsCallInfoList)
{
    auto serviceInstance = DelayedSingleton<CellularCallService>::GetInstance();
    if (serviceInstance == nullptr) {
        TELEPHONY_LOGE("ReportImsCallsData return, serviceInstance is nullptr");
        return;
    }
    ImsCurrentCall imsCallInfo;
    std::vector<ImsCurrentCall>::const_iterator it = imsCallInfoList.calls.begin();
    for (; it != imsCallInfoList.calls.end(); ++it) {
        imsCallInfo.state = (*it).state;
    }
    CellularCallIncomingStartTrace(imsCallInfo.state);
    TELEPHONY_LOGI("ReportImsCallsData, imsCallInfoList.callSize:%{public}d", imsCallInfoList.callSize);
    auto imsControl = serviceInstance->GetImsControl(slotId_);
    if (imsCallInfoList.callSize == 0) {
        if (imsControl == nullptr) {
            TELEPHONY_LOGE("ReportImsCallsData return, ims_control is nullptr");
            return;
        }
        if (imsControl->ReportImsCallsData(slotId_, imsCallInfoList) != TELEPHONY_SUCCESS) {
            CellularCallIncomingFinishTrace(imsCallInfo.state);
        }
        serviceInstance->SetImsControl(slotId_, nullptr);
        return;
    }
    if (imsCallInfoList.callSize == 1) {
        if (imsControl == nullptr) {
            imsControl = std::make_shared<IMSControl>();
            TELEPHONY_LOGI("ReportImsCallsData, make control");
            serviceInstance->SetImsControl(slotId_, imsControl);
        }
    }
    if (imsControl == nullptr) {
        TELEPHONY_LOGE("ReportImsCallsData return, ims_control is nullptr");
        CellularCallIncomingFinishTrace(imsCallInfo.state);
        return;
    }
    if (imsControl->ReportImsCallsData(slotId_, imsCallInfoList) != TELEPHONY_SUCCESS) {
        CellularCallIncomingFinishTrace(imsCallInfo.state);
    }
}

void CellularCallHandler::GetCsCallsDataResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("GetCsCallsDataResponse return, event is nullptr");
        return;
    }
    // Returns list of current calls of ME. If command succeeds but no calls are available,
    // no information response is sent to TE. Refer subclause 9.2 for possible <err> values.
    auto callInfoList = event->GetSharedObject<CallInfoList>();
    if (callInfoList == nullptr) {
        TELEPHONY_LOGE("GetCsCallsDataResponse, Cannot get the callInfoList, need to get rilResponseInfo");
        auto rilResponseInfo = event->GetSharedObject<HRilRadioResponseInfo>();
        if (rilResponseInfo == nullptr) {
            TELEPHONY_LOGE("GetCsCallsDataResponse return, callInfoList and rilResponseInfo is nullptr");
            return;
        }
        if (rilResponseInfo->error == HRilErrType::NONE) {
            TELEPHONY_LOGE("GetCsCallsDataResponse return, failed to query the call list but no reason!");
            return;
        }
        TELEPHONY_LOGE("GetCsCallsDataResponse error, report to call_manager");
        CellularCallEventInfo eventInfo;
        eventInfo.eventType = CellularCallEventType::EVENT_REQUEST_RESULT_TYPE;
        eventInfo.eventId = RequestResultEventId::RESULT_GET_CURRENT_CALLS_FAILED;
        if (registerInstance_ == nullptr) {
            TELEPHONY_LOGE("GetCsCallsDataResponse return, GetInstance is nullptr");
            return;
        }
        registerInstance_->ReportEventResultInfo(eventInfo);
        return;
    }
    ReportCsCallsData(*callInfoList);
}

void CellularCallHandler::GetImsCallsDataResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("GetImsCallsDataResponse return, event is nullptr");
        return;
    }
    // Returns list of current calls of ME. If command succeeds but no calls are available,
    // no information response is sent to TE. Refer subclause 9.2 for possible <err> values.
    auto imsCallInfoList = event->GetSharedObject<ImsCurrentCallList>();
    if (imsCallInfoList == nullptr) {
        TELEPHONY_LOGE("GetImsCallsDataResponse, Cannot get the imsCallInfoList, need to get rilResponseInfo");
        auto rilResponseInfo = event->GetSharedObject<HRilRadioResponseInfo>();
        if (rilResponseInfo == nullptr) {
            TELEPHONY_LOGE("GetImsCallsDataResponse return, callInfoList and rilResponseInfo is nullptr");
            return;
        }
        if (rilResponseInfo->error == HRilErrType::NONE) {
            TELEPHONY_LOGE("GetImsCallsDataResponse return, failed to query the call list but no reason!");
            return;
        }
        TELEPHONY_LOGE("GetImsCallsDataResponse error, report to call_manager");
        if (registerInstance_ == nullptr) {
            TELEPHONY_LOGE("GetImsCallsDataResponse return, GetInstance is nullptr");
            return;
        }
        registerInstance_->ReportGetCallDataResult(static_cast<int32_t>(rilResponseInfo->error));
        return;
    }
    ReportImsCallsData(*imsCallInfoList);
}

void CellularCallHandler::DialResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("DialResponse return, event is nullptr");
        return;
    }
    auto result = event->GetSharedObject<HRilRadioResponseInfo>();
    if (result == nullptr) {
        TELEPHONY_LOGE("DialResponse return, result is nullptr");
        return;
    }
    struct CallBehaviorParameterInfo info = { 0 };
    DelayedSingleton<CellularCallHiSysEvent>::GetInstance()->GetCallParameterInfo(info);
    if (result->error != HRilErrType::NONE) {
        TELEPHONY_LOGI("DialResponse, dial return error, report to call_manager");
        CellularCallEventInfo eventInfo;
        eventInfo.eventType = CellularCallEventType::EVENT_REQUEST_RESULT_TYPE;

        /*
         * 3GPP TS 27.007 V3.9.0 (2001-06)
         * If ME has succeeded in establishing a logical link between application protocols and external interface,
         * it will send CONNECT message to the TE. Otherwise, the NO CARRIER response will be returned.
         */
        if (result->error == HRilErrType::HRIL_ERR_CMD_NO_CARRIER) {
            eventInfo.eventId = RequestResultEventId::RESULT_DIAL_NO_CARRIER;
        } else {
            eventInfo.eventId = RequestResultEventId::RESULT_DIAL_SEND_FAILED;
        }
        if (registerInstance_ == nullptr) {
            TELEPHONY_LOGE("DialResponse return, GetInstance is nullptr");
            return;
        }
        registerInstance_->ReportEventResultInfo(eventInfo);
        CellularCallHiSysEvent::WriteDialCallBehaviorEvent(info, CallResponseResult::COMMAND_FAILURE);
    } else {
        CellularCallHiSysEvent::WriteDialCallBehaviorEvent(info, CallResponseResult::COMMAND_SUCCESS);
    }
}

void CellularCallHandler::CommonResultEventHandling(
    const AppExecFwk::InnerEvent::Pointer &event, CellularCallEventInfo &eventInfo)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CommonResultResponse return, event is nullptr");
        return;
    }
    eventInfo.eventType = CellularCallEventType::EVENT_REQUEST_RESULT_TYPE;
    switch (event->GetInnerEventId()) {
        case RadioEvent::RADIO_HANGUP_CONNECT:
            eventInfo.eventId = RequestResultEventId::RESULT_END_SEND_FAILED;
            break;
        case RadioEvent::RADIO_REJECT_CALL:
            eventInfo.eventId = RequestResultEventId::RESULT_REJECT_SEND_FAILED;
            break;
        case RadioEvent::RADIO_ACCEPT_CALL:
            eventInfo.eventId = RequestResultEventId::RESULT_ACCEPT_SEND_FAILED;
            break;
        case RadioEvent::RADIO_HOLD_CALL:
            eventInfo.eventId = RequestResultEventId::RESULT_HOLD_SEND_FAILED;
            break;
        case RadioEvent::RADIO_ACTIVE_CALL:
            eventInfo.eventId = RequestResultEventId::RESULT_ACTIVE_SEND_FAILED;
            break;
        case RadioEvent::RADIO_SWAP_CALL:
            eventInfo.eventId = RequestResultEventId::RESULT_SWAP_SEND_FAILED;
            break;
        case RadioEvent::RADIO_JOIN_CALL:
            eventInfo.eventId = RequestResultEventId::RESULT_JOIN_SEND_FAILED;
            break;
        case RadioEvent::RADIO_SPLIT_CALL:
            eventInfo.eventId = RequestResultEventId::RESULT_SPLIT_SEND_FAILED;
            break;
        case RadioEvent::RADIO_CALL_SUPPLEMENT:
            eventInfo.eventId = RequestResultEventId::RESULT_SUPPLEMENT_SEND_FAILED;
            break;
        default:
            break;
    }
}

void CellularCallHandler::CommonResultResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CommonResultResponse return, event is nullptr");
        return;
    }
    auto result = event->GetSharedObject<HRilRadioResponseInfo>();
    if (result == nullptr) {
        TELEPHONY_LOGE("CommonResultResponse return, result is nullptr");
        return;
    }
    struct CallBehaviorParameterInfo info = { 0 };
    DelayedSingleton<CellularCallHiSysEvent>::GetInstance()->GetCallParameterInfo(info);
    if (result->error != HRilErrType::NONE) {
        CellularCallEventInfo eventInfo;
        eventInfo.eventId = RequestResultEventId::INVALID_REQUEST_RESULT_EVENT_ID;
        CommonResultEventHandling(event, eventInfo);
        if (eventInfo.eventId == RequestResultEventId::RESULT_END_SEND_FAILED ||
            eventInfo.eventId == RequestResultEventId::RESULT_REJECT_SEND_FAILED) {
            CellularCallHiSysEvent::WriteHangUpCallBehaviorEvent(info, CallResponseResult::COMMAND_FAILURE);
        } else if (eventInfo.eventId == RequestResultEventId::RESULT_ACCEPT_SEND_FAILED) {
            CellularCallHiSysEvent::WriteAnswerCallBehaviorEvent(info, CallResponseResult::COMMAND_FAILURE);
        } else {
            TELEPHONY_LOGI("eventId is not within the scope of processing");
        }
        if (registerInstance_ == nullptr) {
            TELEPHONY_LOGE("CommonResultResponse return, registerInstance_ is nullptr");
            return;
        }
        registerInstance_->ReportEventResultInfo(eventInfo);
        return;
    }
    uint32_t id = event->GetInnerEventId();
    if (id == RadioEvent::RADIO_HANGUP_CONNECT || id == RadioEvent::RADIO_REJECT_CALL) {
        CellularCallHiSysEvent::WriteHangUpCallBehaviorEvent(info, CallResponseResult::COMMAND_SUCCESS);
    } else if (id == RadioEvent::RADIO_ACCEPT_CALL) {
        CellularCallHiSysEvent::WriteAnswerCallBehaviorEvent(info, CallResponseResult::COMMAND_SUCCESS);
    } else {
        TELEPHONY_LOGI("id is not within the scope of processing");
    }
}

void CellularCallHandler::SendDtmfResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("SendDtmfResponse return, event is nullptr");
        return;
    }
    auto result = event->GetSharedObject<HRilRadioResponseInfo>();
    if (result == nullptr) {
        TELEPHONY_LOGE("SendDtmfResponse return, result is nullptr");
        return;
    }
    CellularCallEventInfo eventInfo;
    eventInfo.eventType = CellularCallEventType::EVENT_REQUEST_RESULT_TYPE;
    if (result->error != HRilErrType::NONE) {
        eventInfo.eventId = RequestResultEventId::RESULT_SEND_DTMF_FAILED;
    } else {
        eventInfo.eventId = RequestResultEventId::RESULT_SEND_DTMF_SUCCESS;
    }
    if (registerInstance_ == nullptr) {
        TELEPHONY_LOGE("SendDtmfResponse return, GetInstance is nullptr");
        return;
    }
    TELEPHONY_LOGI("SendDtmfResponse: report to call manager");
    registerInstance_->ReportEventResultInfo(eventInfo);
}

void CellularCallHandler::StartDtmfResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("StartDtmfResponse return, event is nullptr");
        return;
    }
    auto result = event->GetSharedObject<HRilRadioResponseInfo>();
    if (result == nullptr) {
        TELEPHONY_LOGE("StartDtmfResponse return, result is nullptr");
        return;
    }

    if (registerInstance_ == nullptr) {
        TELEPHONY_LOGE("StartDtmfResponse return, GetInstance is nullptr");
        return;
    }
    TELEPHONY_LOGI("StartDtmfResponse: report to call manager");
    registerInstance_->ReportStartDtmfResult(static_cast<int32_t>(result->error));
}

void CellularCallHandler::SimStateChangeReport(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("SimStateChangeReport return, event is nullptr");
        return;
    }
    CellularCallConfig config;
    config.HandleSimStateChanged(slotId_);
}

void CellularCallHandler::SimRecordsLoadedReport(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("SimRecordsLoadedReport return, event is nullptr");
        return;
    }
    TELEPHONY_LOGI("SimRecordsLoadedReport: report to cellular call config");
    CellularCallConfig config;
    config.HandleSimRecordsLoaded(slotId_);
}

void CellularCallHandler::StopDtmfResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("StopDtmfResponse return, event is nullptr");
        return;
    }
    auto result = event->GetSharedObject<HRilRadioResponseInfo>();
    if (result == nullptr) {
        TELEPHONY_LOGE("StopDtmfResponse return, result is nullptr");
        return;
    }

    if (registerInstance_ == nullptr) {
        TELEPHONY_LOGE("StopDtmfResponse return, GetInstance is nullptr");
        return;
    }
    TELEPHONY_LOGI("StopDtmfResponse: report to call manager");
    registerInstance_->ReportStopDtmfResult(static_cast<int32_t>(result->error));
}

void CellularCallHandler::SetSlotId(int32_t id)
{
    slotId_ = id;
}

int32_t CellularCallHandler::GetSlotId()
{
    return slotId_;
}

int64_t CellularCallHandler::CurrentTimeMillis()
{
    int64_t timems =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    return timems;
}

bool CellularCallHandler::IsCanRequestCallsData()
{
    int64_t timems = CurrentTimeMillis();
    if ((timems - lastTime_) < FAST_DELAY_TIME) {
        return false;
    }
    lastTime_ = timems;
    return true;
}

void CellularCallHandler::GetCsCallsDataRequest(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (!IsCanRequestCallsData()) {
        TELEPHONY_LOGE("GetCsCallsDataRequest return, IsCanRequestCallsData false");
    }
    lastCallsDataFlag_ = CurrentTimeMillis();
    CellularCallConnectionCS connectionCs;
    connectionCs.GetCsCallsDataRequest(slotId_, lastCallsDataFlag_);
}

void CellularCallHandler::GetImsCallsDataRequest(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (!IsCanRequestCallsData()) {
        TELEPHONY_LOGE("GetImsCallsDataRequest return, IsCanRequestCallsData false");
    }
    lastCallsDataFlag_ = CurrentTimeMillis();
    CellularCallConnectionIMS connectionIms;
    connectionIms.GetImsCallsDataRequest(slotId_, lastCallsDataFlag_);
}

void CellularCallHandler::RegisterHandler(const AppExecFwk::InnerEvent::Pointer &event)
{
    CellularCallConnectionCS connectionCs;
    connectionCs.RegisterHandler();
}

void CellularCallHandler::SetDomainPreferenceModeResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("SetDomainPreferenceModeResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("SetDomainPreferenceModeResponse return, event is nullptr");
        return;
    }
    auto info = event->GetSharedObject<HRilRadioResponseInfo>();
    if (info == nullptr) {
        TELEPHONY_LOGE("SetDomainPreferenceModeResponse return, info is nullptr");
        return;
    }
    CellularCallEventInfo eventInfo;
    eventInfo.eventType = CellularCallEventType::EVENT_REQUEST_RESULT_TYPE;
    if (info->error != HRilErrType::NONE) {
        eventInfo.eventId = RequestResultEventId::RESULT_SET_CALL_PREFERENCE_MODE_FAILED;
    } else {
        eventInfo.eventId = RequestResultEventId::RESULT_SET_CALL_PREFERENCE_MODE_SUCCESS;

        CellularCallConfig config;
        config.SetTempMode(slotId_);
    }
    if (registerInstance_ == nullptr) {
        TELEPHONY_LOGE("SetDomainPreferenceModeResponse return, GetInstance is nullptr");
        return;
    }
    registerInstance_->ReportEventResultInfo(eventInfo);
}

void CellularCallHandler::GetDomainPreferenceModeResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGE("GetDomainPreferenceModeResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("GetDomainPreferenceModeResponse return, event is nullptr");
        return;
    }
    auto mode = event->GetSharedObject<int32_t>();
    if (mode == nullptr) {
        TELEPHONY_LOGI("GetDomainPreferenceModeResponse return, mode is nullptr");
        return;
    }
    CellularCallConfig config;
    config.GetDomainPreferenceModeResponse(slotId_, *mode);
}

void CellularCallHandler::SetImsSwitchStatusResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("SetImsSwitchStatusResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("SetImsSwitchStatusResponse return, event is nullptr");
        return;
    }
    auto info = event->GetSharedObject<HRilRadioResponseInfo>();
    if (info == nullptr) {
        TELEPHONY_LOGE("SetImsSwitchStatusResponse return, info is nullptr");
        return;
    }
    if (registerInstance_ == nullptr) {
        TELEPHONY_LOGE("SetImsSwitchStatusResponse return, GetInstance is nullptr");
        return;
    }
    CellularCallConfig config;
    config.HandleSetLteImsSwitchResult(slotId_, info->error);
}

void CellularCallHandler::GetImsSwitchStatusResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("GetImsSwitchStatusResponse entry");
}

void CellularCallHandler::CsCallStatusInfoReport(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CsCallStatusInfoReport return, event is nullptr");
        return;
    }
    if (srvccState_ == SrvccState::STARTED) {
        TELEPHONY_LOGI("CsCallStatusInfoReport ignore, srvcc started");
        return;
    }
    GetCsCallData(event);
}

void CellularCallHandler::ImsCallStatusInfoReport(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("ImsCallStatusInfoReport return, event is nullptr");
        return;
    }
    if (srvccState_ == SrvccState::STARTED) {
        TELEPHONY_LOGI("ImsCallStatusInfoReport ignore, srvcc started");
        return;
    }
    GetImsCallData(event);
}

void CellularCallHandler::UssdNotifyResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("CellularCallHandler::UssdNotifyResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("UssdNotifyResponse, event is nullptr");
        return;
    }
    auto result = event->GetSharedObject<UssdNoticeInfo>();
    if (result == nullptr) {
        TELEPHONY_LOGE("UssdNotifyResponse, result is nullptr");
        return;
    }
    CellularCallSupplement supplement;
    supplement.EventUssdNotify(*result);
}

void CellularCallHandler::SetMuteResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("CellularCallHandler::SetMuteResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("SetMuteResponse return, event is nullptr");
        return;
    }
    auto info = event->GetSharedObject<HRilRadioResponseInfo>();
    if (info == nullptr) {
        TELEPHONY_LOGE("SetMuteResponse return, info is nullptr");
        return;
    }
    MuteControlResponse response;
    if (registerInstance_ == nullptr) {
        TELEPHONY_LOGE("SetMuteResponse return, registerInstance_ is nullptr");
        return;
    }
    response.result = static_cast<int32_t>(info->error);
    TELEPHONY_LOGI("SetMuteResponse: report to call manager");
    registerInstance_->ReportSetMuteResult(response);
}

void CellularCallHandler::GetMuteResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("CellularCallHandler::GetMuteResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("GetMuteResponse return, event is nullptr");
        return;
    }
    MuteControlResponse response;
    auto mute = event->GetSharedObject<int32_t>();
    if (mute == nullptr) {
        TELEPHONY_LOGI("GetMuteResponse, mute is nullptr");
        auto info = event->GetSharedObject<HRilRadioResponseInfo>();
        if (info == nullptr) {
            TELEPHONY_LOGE("GetMuteResponse return, info is nullptr");
            return;
        }
        response.result = static_cast<int32_t>(info->error);
    } else {
        response.result = static_cast<int32_t>(HRilErrType::NONE);
        response.value = *mute;
    }
    if (registerInstance_ == nullptr) {
        TELEPHONY_LOGE("GetMuteResponse return, registerInstance_ is nullptr");
        return;
    }
    TELEPHONY_LOGI("GetMuteResponse: report to call manager");
    registerInstance_->ReportGetMuteResult(response);
}

void CellularCallHandler::GetEmergencyCallListResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("CellularCallHandler::GetEmergencyCallListResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("GetEmergencyCallListResponse return, event is nullptr");
        return;
    }
    auto eccList = event->GetSharedObject<EmergencyInfoList>();
    if (eccList == nullptr) {
        TELEPHONY_LOGE("UpdateEmergencyCallFromRadio return, eccList is nullptr");
        return;
    }
    CellularCallConfig config;
    config.UpdateEmergencyCallFromRadio(slotId_, *eccList);
}

void CellularCallHandler::SetEmergencyCallListResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("CellularCallHandler::SetEmergencyCallListResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("SetEmergencyCallListResponse return, event is nullptr");
        return;
    }
    auto info = event->GetSharedObject<HRilRadioResponseInfo>();
    if (info == nullptr) {
        TELEPHONY_LOGE("SetEmergencyCallListResponse return, info is nullptr");
        return;
    }
    SetEccListResponse response;
    if (registerInstance_ == nullptr) {
        TELEPHONY_LOGE("SetEmergencyCallListResponse return, registerInstance_ is nullptr");
        return;
    }
    response.result = static_cast<int32_t>(info->error);
    TELEPHONY_LOGI("SetEmergencyCallListResponse report to call manager");
    registerInstance_->ReportSetEmergencyCallListResponse(response);
}

void CellularCallHandler::CallRingBackVoiceResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("CellularCallHandler::CallRingBackVoiceResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("CallRingBackVoiceResponse return, event is nullptr");
        return;
    }
    auto ringBackVoice = event->GetSharedObject<RingbackVoice>();
    if (ringBackVoice == nullptr) {
        TELEPHONY_LOGE("CallRingBackVoiceResponse return, ringBackVoice is nullptr");
        return;
    }
    if (registerInstance_ == nullptr) {
        TELEPHONY_LOGE("CallRingBackVoiceResponse return, registerInstance_ is nullptr");
        return;
    }
    TELEPHONY_LOGI("CallRingBackVoiceResponse: report to call manager");
    registerInstance_->ReportCallRingBackResult(ringBackVoice->status);
}

void CellularCallHandler::GetCallFailReasonResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("CellularCallHandler::GetCallFailReasonResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("GetCallFailReasonResponse return, event is nullptr");
        return;
    }
    if (registerInstance_ == nullptr) {
        TELEPHONY_LOGE("GetCallFailReasonResponse return, registerInstance_ is nullptr");
        return;
    }
    auto reason = event->GetSharedObject<int32_t>();
    DisconnectedDetails details;
    if (reason == nullptr) {
        auto info = event->GetSharedObject<DisconnectedDetails>();
        if (info == nullptr) {
            TELEPHONY_LOGE("GetCallFailReasonResponse return, info is nullptr");
            return;
        }
        details.reason = static_cast<DisconnectedReason>(info->reason);
        details.message = (info->message.c_str() == nullptr) ? "" : info->message;
    } else {
        details.reason = static_cast<DisconnectedReason>(*reason);
        details.message = "";
    }

    if (details.message.empty()) {
        std::string callFailedMessageName = "";
        bool ret =
            ResourceUtils::Get().GetCallFailedMessageName(static_cast<int32_t>(details.reason), callFailedMessageName);
        if (!ret) {
            TELEPHONY_LOGE("get call failed message failed!");
            return;
        }
        ResourceUtils::Get().GetStringValueByName(callFailedMessageName, details.message);
    }
    CellularCallHiSysEvent::WriteCallEndBehaviorEvent(slotId_, static_cast<int32_t>(details.reason));
    registerInstance_->ReportCallFailReason(details);
}

void CellularCallHandler::UpdateSrvccStateReport(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("CellularCallHandler::UpdateSrvccStateReport entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("UpdateSrvccStateReport return, event is nullptr");
        return;
    }
    auto srvccStatus = event->GetSharedObject<SrvccStatus>();
    if (srvccStatus == nullptr) {
        TELEPHONY_LOGE("UpdateSrvccStateReport return, srvccStatus is nullptr");
        return;
    }
    TELEPHONY_LOGI("srvccStatus %{public}d", srvccStatus->status);
    srvccState_ = srvccStatus->status;
    auto serviceInstance_ = DelayedSingleton<CellularCallService>::GetInstance();
    if (serviceInstance_ != nullptr) {
        TELEPHONY_LOGE("UpdateSrvccState");
        serviceInstance_->SetSrvccState(srvccState_);
    }
    if (srvccState_ != SrvccState::COMPLETED) {
        TELEPHONY_LOGE("UpdateSrvccStateReport return, srvccState_ != SrvccState::COMPLETED");
        return;
    }
    SrvccStateCompleted();
}

void CellularCallHandler::ReportEccChanged(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("CellularCallHandler::ReportEccChanged entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("ReportEccChanged return, event is nullptr");
        return;
    }
    auto emergencyInfoList = event->GetSharedObject<EmergencyInfoList>();
    if (emergencyInfoList == nullptr) {
        TELEPHONY_LOGE("ReportEccChanged return, emergencyInfoList is nullptr");
        return;
    }
    CellularCallConfig config;
    auto calls = emergencyInfoList->calls;
    if (calls.size() > 0 && static_cast<uint32_t>(calls.back().total) != calls.size()) {
        TELEPHONY_LOGE("ReportEccChanged data error");
        auto endCall = calls.back();
        if (endCall.index < endCall.total) {
            return;
        }
        TELEPHONY_LOGI("ReportEccChanged try query");
        config.GetEmergencyCallList(slotId_);
        return;
    }
    config.UpdateEmergencyCallFromRadio(slotId_, *emergencyInfoList);
}

void CellularCallHandler::SrvccStateCompleted()
{
    TELEPHONY_LOGI("CellularCallHandler::SrvccStateCompleted entry");
    if (srvccState_ != SrvccState::COMPLETED) {
        TELEPHONY_LOGE("SrvccStateCompleted return, srvccState_ != SrvccState::COMPLETED");
        return;
    }
    auto serviceInstance_ = DelayedSingleton<CellularCallService>::GetInstance();
    if (serviceInstance_ == nullptr) {
        TELEPHONY_LOGE("SrvccStateCompleted return, GetInstance is nullptr");
        return;
    }
    auto csControl = serviceInstance_->GetCsControl(slotId_);
    if (csControl != nullptr) {
        TELEPHONY_LOGI("SrvccStateCompleted CsControl ReleaseAllConnection");
        csControl->ReleaseAllConnection();
        serviceInstance_->SetCsControl(slotId_, nullptr);
    } else {
        TELEPHONY_LOGI("SrvccStateCompleted CsControl is nullptr");
        csControl = std::make_shared<CSControl>();
        serviceInstance_->SetCsControl(slotId_, csControl);
    }
    auto imsControl = serviceInstance_->GetImsControl(slotId_);
    if (imsControl != nullptr) {
        TELEPHONY_LOGI("SrvccStateCompleted ImsControl ReleaseAllConnection");
        imsControl->ReleaseAllConnection();
        serviceInstance_->SetImsControl(slotId_, nullptr);
    } else {
        TELEPHONY_LOGI("SrvccStateCompleted imsControl is nullptr");
    }
    srvccState_ = SrvccState::SRVCC_NONE;
}

void CellularCallHandler::GetMMIResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("GetMMIResponse, event is nullptr");
        return;
    }
    std::unique_ptr<MMICodeUtils> mmiCodeUtils = event->GetUniqueObject<MMICodeUtils>();
    if (mmiCodeUtils == nullptr) {
        TELEPHONY_LOGE("CellularCallHandler::GetMMIResponse, mmiCodeUtils is nullptr");
        return;
    }
    mmiCodeUtils->ExecuteMmiCode(slotId_);
}

void CellularCallHandler::GetCallWaitingResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("CellularCallHandler::GetCallWaitingResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("GetCallWaitingResponse, event is nullptr");
        return;
    }
    auto result = event->GetSharedObject<CallWaitResult>();
    if (result == nullptr) {
        TELEPHONY_LOGE("CellularCallHandler::GetCallWaitingResponse, result is nullptr");
        return;
    }
    CellularCallSupplement supplement;
    supplement.EventGetCallWaiting(*result);
}

void CellularCallHandler::SetCallWaitingResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("SetCallWaitingResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("SetCallWaitingResponse, event is nullptr");
        return;
    }
    auto result = event->GetSharedObject<HRilRadioResponseInfo>();
    if (result == nullptr) {
        TELEPHONY_LOGE("SetCallWaitingResponse, result is nullptr");
        return;
    }
    CellularCallSupplement supplement;
    supplement.EventSetCallWaiting(*result);
}

void CellularCallHandler::GetClirResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("GetClirResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("GetClirResponse, event is nullptr");
        return;
    }
    auto getClirResult = event->GetSharedObject<GetClirResult>();
    if (getClirResult == nullptr) {
        TELEPHONY_LOGE("GetClirResponse, getClirResult is nullptr");
        return;
    }
    CellularCallSupplement supplement;
    supplement.EventGetClir(*getClirResult);
}

void CellularCallHandler::SetClirResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("SetClirResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("SetClirResponse, event is nullptr");
        return;
    }
    auto result = event->GetSharedObject<HRilRadioResponseInfo>();
    if (result == nullptr) {
        TELEPHONY_LOGE("SetClirResponse, result is nullptr");
        return;
    }
    CellularCallSupplement supplement;
    supplement.EventSetClir(*result);
}

void CellularCallHandler::GetClipResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("GetClipResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("GetClipResponse, event is nullptr");
        return;
    }
    auto getClipResult = event->GetSharedObject<GetClipResult>();
    if (getClipResult == nullptr) {
        TELEPHONY_LOGE("GetClipResponse, getClipResult is nullptr");
        return;
    }
    CellularCallSupplement supplement;
    supplement.EventGetClip(*getClipResult);
}

void CellularCallHandler::GetCallTransferResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("GetCallTransferResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("GetCallTransferResponse, event is nullptr");
        return;
    }
    auto cFQueryList = event->GetSharedObject<CallForwardQueryInfoList>();
    if (cFQueryList == nullptr) {
        TELEPHONY_LOGE("GetCallTransferResponse, cFQueryList is nullptr");
        return;
    }
    CellularCallSupplement supplement;
    supplement.EventGetCallTransferInfo(*cFQueryList);
}

void CellularCallHandler::SetCallTransferInfoResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("SetCallTransferInfoResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("SetCallTransferInfoResponse, event is nullptr");
        return;
    }
    auto result = event->GetSharedObject<HRilRadioResponseInfo>();
    if (result == nullptr) {
        TELEPHONY_LOGE("SetCallTransferInfoResponse, result is nullptr");
        return;
    }
    CellularCallSupplement supplement;
    supplement.EventSetCallTransferInfo(*result);
}

void CellularCallHandler::GetCallRestrictionResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("GetCallRestrictionResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("GetCallRestrictionResponse, event is nullptr");
        return;
    }
    auto result = event->GetSharedObject<CallRestrictionResult>();
    if (result == nullptr) {
        TELEPHONY_LOGE("GetCallRestrictionResponse, result is nullptr");
        return;
    }
    CellularCallSupplement supplement;
    supplement.EventGetCallRestriction(*result);
}

void CellularCallHandler::SetCallRestrictionResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("SetCallRestrictionResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("SetCallRestrictionResponse, event is nullptr");
        return;
    }
    auto result = event->GetSharedObject<HRilRadioResponseInfo>();
    if (result == nullptr) {
        TELEPHONY_LOGE("SetCallRestrictionResponse, result is nullptr");
        return;
    }
    CellularCallSupplement supplement;
    supplement.EventSetCallRestriction(*result);
}

void CellularCallHandler::SendUssdResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("SendUssdResponse, event is nullptr");
        return;
    }
    auto result = event->GetSharedObject<HRilRadioResponseInfo>();
    if (result == nullptr) {
        TELEPHONY_LOGE("SendUssdResponse, result is nullptr");
        return;
    }
    CellularCallSupplement supplement;
    supplement.EventSendUssd(*result);
}

void CellularCallHandler::SsNotifyResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("CellularCallHandler::SsNotifyResponse entry");
    if (event == nullptr) {
        TELEPHONY_LOGE("SsNotifyResponse, event is nullptr");
        return;
    }
    auto result = event->GetSharedObject<SsNoticeInfo>();
    if (result == nullptr) {
        TELEPHONY_LOGE("SsNotifyResponse, result is nullptr");
        return;
    }
    CellularCallSupplement supplement;
    supplement.EventSsNotify(*result);
}

void CellularCallHandler::SendUnlockPinPukResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("SendUnlockPinPukResponse, event is nullptr");
        return;
    }
    auto result = event->GetSharedObject<PinPukResponse>();
    if (result == nullptr) {
        TELEPHONY_LOGE("SendUnlockPinPukResponse, result is nullptr");
        return;
    }
    CellularCallSupplement supplement;
    supplement.EventSetPinPuk(*result);
}

void CellularCallHandler::HandleOperatorConfigChanged(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr");
        return;
    }

    CellularCallConfig config;
    config.HandleOperatorConfigChanged(slotId_);
}

void CellularCallHandler::UpdateRsrvccStateReport(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr");
        return;
    }

    isInCsRedial_ = true;
    auto serviceInstance = DelayedSingleton<CellularCallService>::GetInstance();
    if (serviceInstance == nullptr) {
        TELEPHONY_LOGE("ReportCsCallsData return, GetInstance is nullptr");
        return;
    }
    serviceInstance->SetCsControl(slotId_, nullptr);
}

#ifdef CALL_MANAGER_AUTO_START_OPTIMIZE
void CellularCallHandler::RadioStateChangeProcess(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr");
        return;
    }

    std::shared_ptr<HRilInt32Parcel> object = event->GetSharedObject<HRilInt32Parcel>();
    if (object == nullptr) {
        TELEPHONY_LOGE("Slot%{public}d: HandleRadioStateChanged object is nullptr!", slotId_);
        return;
    }
    TELEPHONY_LOGI("Slot%{public}d: Radio changed with state: %{public}d", slotId_, object->data);
    if (object->data == CORE_SERVICE_POWER_ON) {
        auto serviceInstance = DelayedSingleton<CellularCallService>::GetInstance();
        if (serviceInstance == nullptr) {
            TELEPHONY_LOGE("serviceInstance is nullptr");
            return;
        }
        serviceInstance->StartCallManagerService();
    }
}
#endif
} // namespace Telephony
} // namespace OHOS
