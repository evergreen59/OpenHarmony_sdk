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

#include "cellular_call_service.h"

#include "cellular_call_callback.h"
#include "cellular_call_dump_helper.h"
#include "cellular_call_hisysevent.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "emergency_utils.h"
#include "ims_call_client.h"
#include "module_service_utils.h"
#include "radio_event.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "telephony_permission.h"

namespace OHOS {
namespace Telephony {
const uint32_t CONNECT_MAX_TRY_COUNT = 20;
const uint32_t CONNECT_CORE_SERVICE_WAIT_TIME = 2000; // ms
bool g_registerResult =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<CellularCallService>::GetInstance().get());

CellularCallService::CellularCallService() : SystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID, true)
{
    state_ = ServiceRunningState::STATE_STOPPED;
}

CellularCallService::~CellularCallService()
{
    state_ = ServiceRunningState::STATE_STOPPED;
    if (statusChangeListener_ != nullptr) {
        statusChangeListener_.clear();
        statusChangeListener_ = nullptr;
    }
}

bool CellularCallService::Init()
{
    TELEPHONY_LOGI("CellularCallService::Init start");
    eventLoop_ = AppExecFwk::EventRunner::Create("CellularCallServiceLoop");
    if (eventLoop_ == nullptr) {
        TELEPHONY_LOGE("CellularCallService::Init return, failed to create EventRunner");
        return false;
    }
    CreateHandler();
    SendEventRegisterHandler();
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    callManagerListener_ = new (std::nothrow) SystemAbilityStatusChangeListener();
    if (samgrProxy == nullptr || callManagerListener_ == nullptr) {
        TELEPHONY_LOGE("samgrProxy or callManagerListener_ is nullptr");
    } else {
        int32_t ret = samgrProxy->SubscribeSystemAbility(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID, callManagerListener_);
        TELEPHONY_LOGI("SubscribeSystemAbility TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID result:%{public}d", ret);
    }
    // connect ims_service
    DelayedSingleton<ImsCallClient>::GetInstance()->Init();
    TELEPHONY_LOGI("CellularCallService::Init, init success");
    return true;
}

void CellularCallService::OnStart()
{
    TELEPHONY_LOGI("CellularCallService OnStart");
    bindTime_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        TELEPHONY_LOGE("CellularCallService::OnStart return, has already started.");
        return;
    }
    if (!Init()) {
        TELEPHONY_LOGE("CellularCallService::OnStart return, failed to init service.");
        return;
    }
    state_ = ServiceRunningState::STATE_RUNNING;
    if (eventLoop_ != nullptr) {
        eventLoop_->Run();
    }
    bool ret = Publish(DelayedSingleton<CellularCallService>::GetInstance().get());
    if (!ret) {
        TELEPHONY_LOGE("CellularCallService::OnStart Publish failed!");
    }
    endTime_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    TELEPHONY_LOGI("CellularCallService start success.");
}

void CellularCallService::OnStop()
{
    TELEPHONY_LOGI("CellularCallService stop service");
    if (eventLoop_ != nullptr) {
        eventLoop_.reset();
    }
    DelayedSingleton<ImsCallClient>::GetInstance()->UnInit();
    state_ = ServiceRunningState::STATE_STOPPED;
    HandlerResetUnRegister();
}

void CellularCallService::RegisterHandler()
{
    TELEPHONY_LOGI("connect core service Register Handler start");
    networkSearchCallBack_ = (std::make_unique<CellularCallCallback>()).release();
    for (uint32_t i = 0; i < CONNECT_MAX_TRY_COUNT; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(CONNECT_CORE_SERVICE_WAIT_TIME));
        if (CoreManagerInner::GetInstance().IsInitFinishedForTelRil()) {
            TELEPHONY_LOGI("connect core service Register Handler start");
            RegisterCoreServiceHandler();
            CoreManagerInner::GetInstance().RegisterCellularCallObject(networkSearchCallBack_);
            break;
        }
        TELEPHONY_LOGW("connect core service Register Handler null or not init");
    }
    TELEPHONY_LOGI("connect core service Register Handler end");
}

void CellularCallService::CreateHandler()
{
    ModuleServiceUtils obtain;
    std::vector<int32_t> slotVector = obtain.GetSlotInfo();
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_OPERATOR_CONFIG_CHANGED);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);

    for (const auto &it : slotVector) {
        auto handler = std::make_shared<CellularCallHandler>(eventLoop_, subscriberInfo);
        TELEPHONY_LOGI("setSlotId:%{public}d", it);
        handler->SetSlotId(it);
        handler->RegisterImsCallCallbackHandler();
        handlerMap_.insert(std::make_pair(it, handler));
        auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        statusChangeListener_ = new (std::nothrow) SystemAbilityStatusChangeListener(handler);
        if (samgrProxy == nullptr || statusChangeListener_ == nullptr) {
            TELEPHONY_LOGE("samgrProxy or statusChangeListener_ is nullptr");
        } else {
            int32_t ret = samgrProxy->SubscribeSystemAbility(COMMON_EVENT_SERVICE_ID, statusChangeListener_);
            TELEPHONY_LOGI("SubscribeSystemAbility COMMON_EVENT_SERVICE_ID result:%{public}d", ret);
        }
    }
}

void CellularCallService::HandlerResetUnRegister()
{
    TELEPHONY_LOGI("HandlerResetUnRegister");
    for (const auto &it : handlerMap_) {
        int32_t slot = it.first;
        auto handler = it.second;
        if (handler != nullptr) {
            handler.reset();
        }
        CoreManagerInner::GetInstance().UnRegisterCoreNotify(slot, handler, RadioEvent::RADIO_AVAIL);
        CoreManagerInner::GetInstance().UnRegisterCoreNotify(slot, handler, RadioEvent::RADIO_NOT_AVAIL);
        CoreManagerInner::GetInstance().UnRegisterCoreNotify(slot, handler, RadioEvent::RADIO_SIM_RECORDS_LOADED);
        CoreManagerInner::GetInstance().UnRegisterCoreNotify(slot, handler, RadioEvent::RADIO_CALL_STATUS_INFO);
        CoreManagerInner::GetInstance().UnRegisterCoreNotify(slot, handler, RadioEvent::RADIO_CALL_USSD_NOTICE);
        CoreManagerInner::GetInstance().UnRegisterCoreNotify(slot, handler, RadioEvent::RADIO_CALL_SS_NOTICE);
        CoreManagerInner::GetInstance().UnRegisterCoreNotify(slot, handler, RadioEvent::RADIO_CALL_RINGBACK_VOICE);
        CoreManagerInner::GetInstance().UnRegisterCoreNotify(
            slot, handler, RadioEvent::RADIO_CALL_EMERGENCY_NUMBER_REPORT);
        CoreManagerInner::GetInstance().UnRegisterCoreNotify(slot, handler, RadioEvent::RADIO_CALL_SRVCC_STATUS);
        CoreManagerInner::GetInstance().UnRegisterCoreNotify(slot, handler, RadioEvent::RADIO_CALL_RSRVCC_STATUS);
#ifdef CALL_MANAGER_AUTO_START_OPTIMIZE
        CoreManagerInner::GetInstance().UnRegisterCoreNotify(slot, handler, RadioEvent::RADIO_STATE_CHANGED);
#endif
        if (GetCsControl(slot) != nullptr) {
            GetCsControl(slot)->ReleaseAllConnection();
        }
        if (GetImsControl(slot) != nullptr) {
            GetImsControl(slot)->ReleaseAllConnection();
        }
    }
}

void CellularCallService::RegisterCoreServiceHandler()
{
    TELEPHONY_LOGI("RegisterCoreServiceHandle");
    for (const auto &it : handlerMap_) {
        int32_t slot = it.first;
        auto handler = it.second;
        if (handler != nullptr) {
            CoreManagerInner::GetInstance().RegisterCoreNotify(slot, handler, RadioEvent::RADIO_AVAIL, nullptr);
            CoreManagerInner::GetInstance().RegisterCoreNotify(slot, handler, RadioEvent::RADIO_NOT_AVAIL, nullptr);
            CoreManagerInner::GetInstance().RegisterCoreNotify(
                slot, handler, RadioEvent::RADIO_SIM_STATE_CHANGE, nullptr);
            CoreManagerInner::GetInstance().RegisterCoreNotify(
                slot, handler, RadioEvent::RADIO_SIM_RECORDS_LOADED, nullptr);
            CoreManagerInner::GetInstance().RegisterCoreNotify(
                slot, handler, RadioEvent::RADIO_CALL_STATUS_INFO, nullptr);
            CoreManagerInner::GetInstance().RegisterCoreNotify(
                slot, handler, RadioEvent::RADIO_CALL_USSD_NOTICE, nullptr);
            CoreManagerInner::GetInstance().RegisterCoreNotify(
                slot, handler, RadioEvent::RADIO_CALL_SS_NOTICE, nullptr);
            CoreManagerInner::GetInstance().RegisterCoreNotify(
                slot, handler, RadioEvent::RADIO_CALL_EMERGENCY_NUMBER_REPORT, nullptr);
            CoreManagerInner::GetInstance().RegisterCoreNotify(
                slot, handler, RadioEvent::RADIO_CALL_RINGBACK_VOICE, nullptr);
            CoreManagerInner::GetInstance().RegisterCoreNotify(
                slot, handler, RadioEvent::RADIO_CALL_SRVCC_STATUS, nullptr);
            CoreManagerInner::GetInstance().RegisterCoreNotify(
                slot, handler, RadioEvent::RADIO_CALL_RSRVCC_STATUS, nullptr);
#ifdef CALL_MANAGER_AUTO_START_OPTIMIZE
            CoreManagerInner::GetInstance().RegisterCoreNotify(slot, handler, RadioEvent::RADIO_STATE_CHANGED, nullptr);
#endif
        }
        CellularCallConfig config;
        config.InitModeActive();
        if (config.GetDomainPreferenceMode(slot) != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGW("RegisterCoreServiceHandler, GetDomainPreferenceMode request fail");
        }
        if (config.GetEmergencyCallList(it.first) != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGW("RegisterCoreServiceHandler, GetEmergencyCallList request fail");
        }
    }
}

void CellularCallService::SendEventRegisterHandler()
{
    int64_t delayTime = 1000;
    int32_t slot = DEFAULT_SIM_SLOT_ID;
    auto handler = handlerMap_[slot];
    if (handler == nullptr) {
        TELEPHONY_LOGE("SendEventRegisterHandler return, handler is nullptr");
        return;
    }
    handler->SendEvent(handler->REGISTER_HANDLER_ID, delayTime, CellularCallHandler::Priority::HIGH);
}

int32_t CellularCallService::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    if (fd < 0) {
        TELEPHONY_LOGE("dump fd invalid");
        return TELEPHONY_ERR_FAIL;
    }
    std::vector<std::string> argsInStr;
    for (const auto &arg : args) {
        argsInStr.emplace_back(Str16ToStr8(arg));
    }
    std::string result;
    CellularCallDumpHelper dumpHelper;
    if (dumpHelper.Dump(argsInStr, result)) {
        int32_t ret = dprintf(fd, "%s", result.c_str());
        if (ret < 0) {
            TELEPHONY_LOGE("dprintf to dump fd failed");
            return TELEPHONY_ERR_FAIL;
        }
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGW("dumpHelper failed");
    return TELEPHONY_ERR_FAIL;
}

int32_t CellularCallService::GetServiceRunningState()
{
    return static_cast<int32_t>(state_);
}

std::string CellularCallService::GetBindTime()
{
    std::ostringstream oss;
    oss << bindTime_;
    return oss.str();
}

std::string CellularCallService::GetEndTime()
{
    std::ostringstream oss;
    oss << endTime_;
    return oss.str();
}

std::string CellularCallService::GetSpendTime()
{
    spendTime_ = endTime_ - bindTime_;
    std::ostringstream oss;
    oss << spendTime_;
    return oss.str();
}

int32_t CellularCallService::Dial(const CellularCallInfo &callInfo)
{
    if (!TelephonyPermission::CheckPermission(Permission::PLACE_CALL)) {
        TELEPHONY_LOGE("Check permission failed, no PLACE_CALL permisson.");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }

    if (!IsValidSlotId(callInfo.slotId)) {
        TELEPHONY_LOGE("CellularCallService::Dial return, invalid slot id");
        CellularCallHiSysEvent::WriteDialCallFaultEvent(callInfo.accountId, static_cast<int32_t>(callInfo.callType),
            callInfo.videoState, CALL_ERR_INVALID_SLOT_ID, "invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    if (srvccState_ == SrvccState::STARTED) {
        CellularCallHiSysEvent::WriteDialCallFaultEvent(callInfo.accountId, static_cast<int32_t>(callInfo.callType),
            callInfo.videoState, static_cast<int32_t>(CallErrorCode::CALL_ERROR_UNEXPECTED_SRVCC_STATE),
            "srvccState_ is STARTED");
        return TELEPHONY_ERR_FAIL;
    }
    bool useImsForEmergency = UseImsForEmergency(callInfo);
    if (IsNeedIms(callInfo.slotId) || useImsForEmergency) {
        auto imsControl = GetImsControl(callInfo.slotId);
        if (imsControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::Dial ims dial");
            imsControl = std::make_shared<IMSControl>();
            if (imsControl == nullptr) {
                TELEPHONY_LOGE("CellularCallService::Dial return, imsControl create fail");
                return TELEPHONY_ERR_LOCAL_PTR_NULL;
            }
            SetImsControl(callInfo.slotId, imsControl);
        }
        return imsControl->Dial(callInfo);
    }

    auto csControl = GetCsControl(callInfo.slotId);
    if (csControl == nullptr) {
        csControl = std::make_shared<CSControl>();
        if (csControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::Dial return, csControl create fail");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        SetCsControl(callInfo.slotId, csControl);
    }
    return csControl->Dial(callInfo);
}

int32_t CellularCallService::HangUp(const CellularCallInfo &callInfo, CallSupplementType type)
{
    DelayedSingleton<CellularCallHiSysEvent>::GetInstance()->SetCallParameterInfo(
        callInfo.slotId, static_cast<int32_t>(callInfo.callType), callInfo.videoState);
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("Check permission failed, no ANSWER_CALL permisson.");
        CellularCallHiSysEvent::WriteAnswerCallFaultEvent(callInfo.slotId, callInfo.callId, callInfo.videoState,
            TELEPHONY_ERR_PERMISSION_ERR, Permission::ANSWER_CALL);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (!IsValidSlotId(callInfo.slotId)) {
        TELEPHONY_LOGE("CellularCallService::HangUp return, invalid slot id");
        CellularCallHiSysEvent::WriteHangUpFaultEvent(
            callInfo.slotId, callInfo.callId, CALL_ERR_INVALID_SLOT_ID, "HangUp invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    if (srvccState_ == SrvccState::STARTED) {
        CellularCallHiSysEvent::WriteHangUpFaultEvent(callInfo.slotId, callInfo.callId,
            static_cast<int32_t>(CallErrorCode::CALL_ERROR_UNEXPECTED_SRVCC_STATE), "HangUp srvccState_ is STARTED");
        return TELEPHONY_ERR_FAIL;
    }
    if (CallType::TYPE_CS == callInfo.callType) {
        auto csControl = GetCsControl(callInfo.slotId);
        if (csControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::HangUp return, csControl is nullptr");
            CellularCallHiSysEvent::WriteHangUpFaultEvent(
                callInfo.slotId, callInfo.callId, TELEPHONY_ERR_LOCAL_PTR_NULL, "HangUp csControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return csControl->HangUp(callInfo, type);
    } else if (CallType::TYPE_IMS == callInfo.callType) {
        auto imsControl = GetImsControl(callInfo.slotId);
        if (imsControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::HangUp return, imsControl is nullptr");
            CellularCallHiSysEvent::WriteHangUpFaultEvent(
                callInfo.slotId, callInfo.callId, TELEPHONY_ERR_LOCAL_PTR_NULL, "HangUp imsControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return imsControl->HangUp(callInfo, type);
    }
    TELEPHONY_LOGE("CellularCallService::HangUp return, call type error.");
    CellularCallHiSysEvent::WriteHangUpFaultEvent(
        callInfo.slotId, callInfo.callId, TELEPHONY_ERR_ARGUMENT_INVALID, "HangUp call type error");
    return TELEPHONY_ERR_ARGUMENT_INVALID;
}

int32_t CellularCallService::Reject(const CellularCallInfo &callInfo)
{
    DelayedSingleton<CellularCallHiSysEvent>::GetInstance()->SetCallParameterInfo(
        callInfo.slotId, static_cast<int32_t>(callInfo.callType), callInfo.videoState);
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("Check permission failed, no ANSWER_CALL permisson.");
        CellularCallHiSysEvent::WriteAnswerCallFaultEvent(callInfo.slotId, callInfo.callId, callInfo.videoState,
            TELEPHONY_ERR_PERMISSION_ERR, Permission::ANSWER_CALL);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (!IsValidSlotId(callInfo.slotId)) {
        TELEPHONY_LOGE("CellularCallService::Reject return, invalid slot id");
        CellularCallHiSysEvent::WriteHangUpFaultEvent(
            callInfo.slotId, callInfo.callId, TELEPHONY_ERR_ARGUMENT_INVALID, "Reject call type error");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    if (srvccState_ == SrvccState::STARTED) {
        CellularCallHiSysEvent::WriteHangUpFaultEvent(callInfo.slotId, callInfo.callId,
            static_cast<int32_t>(CallErrorCode::CALL_ERROR_UNEXPECTED_SRVCC_STATE), "Reject srvccState_ is STARTED");
        return TELEPHONY_ERR_FAIL;
    }
    if (CallType::TYPE_CS == callInfo.callType) {
        auto csControl = GetCsControl(callInfo.slotId);
        if (csControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::Reject return, csControl is nullptr");
            CellularCallHiSysEvent::WriteHangUpFaultEvent(
                callInfo.slotId, callInfo.callId, TELEPHONY_ERR_LOCAL_PTR_NULL, "Reject csControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return csControl->Reject(callInfo);
    } else if (CallType::TYPE_IMS == callInfo.callType) {
        auto imsControl = GetImsControl(callInfo.slotId);
        if (imsControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::Reject return, imsControl is nullptr");
            CellularCallHiSysEvent::WriteHangUpFaultEvent(
                callInfo.slotId, callInfo.callId, TELEPHONY_ERR_LOCAL_PTR_NULL, "Reject imsControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return imsControl->Reject(callInfo);
    }
    TELEPHONY_LOGE("CellularCallService::Reject return, call type error.");
    CellularCallHiSysEvent::WriteHangUpFaultEvent(
        callInfo.slotId, callInfo.callId, TELEPHONY_ERR_ARGUMENT_INVALID, "Reject call type error");
    return TELEPHONY_ERR_ARGUMENT_INVALID;
}

int32_t CellularCallService::Answer(const CellularCallInfo &callInfo)
{
    DelayedSingleton<CellularCallHiSysEvent>::GetInstance()->SetCallParameterInfo(
        callInfo.slotId, static_cast<int32_t>(callInfo.callType), callInfo.videoState);
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("Check permission failed, no ANSWER_CALL permisson.");
        CellularCallHiSysEvent::WriteAnswerCallFaultEvent(callInfo.slotId, callInfo.callId, callInfo.videoState,
            TELEPHONY_ERR_PERMISSION_ERR, Permission::ANSWER_CALL);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }

    if (!IsValidSlotId(callInfo.slotId)) {
        TELEPHONY_LOGE("CellularCallService::Answer return, invalid slot id");
        CellularCallHiSysEvent::WriteAnswerCallFaultEvent(
            callInfo.slotId, callInfo.callId, callInfo.videoState, CALL_ERR_INVALID_SLOT_ID, "invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    if (srvccState_ == SrvccState::STARTED) {
        CellularCallHiSysEvent::WriteAnswerCallFaultEvent(callInfo.slotId, callInfo.callId, callInfo.videoState,
            static_cast<int32_t>(CallErrorCode::CALL_ERROR_UNEXPECTED_SRVCC_STATE), "srvccState_ is STARTED");
        return TELEPHONY_ERR_FAIL;
    }
    if (CallType::TYPE_CS == callInfo.callType) {
        auto csControl = GetCsControl(callInfo.slotId);
        if (csControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::Answer return, csControl is nullptr");
            CellularCallHiSysEvent::WriteAnswerCallFaultEvent(callInfo.slotId, callInfo.callId, callInfo.videoState,
                TELEPHONY_ERR_LOCAL_PTR_NULL, "csControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return csControl->Answer(callInfo);
    } else if (CallType::TYPE_IMS == callInfo.callType) {
        auto imsControl = GetImsControl(callInfo.slotId);
        if (imsControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::Answer return, imsControl is nullptr");
            CellularCallHiSysEvent::WriteAnswerCallFaultEvent(callInfo.slotId, callInfo.callId, callInfo.videoState,
                TELEPHONY_ERR_LOCAL_PTR_NULL, "imsControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return imsControl->Answer(callInfo);
    }
    TELEPHONY_LOGE("CellularCallService::Answer return, call type error.");
    CellularCallHiSysEvent::WriteAnswerCallFaultEvent(
        callInfo.slotId, callInfo.callId, callInfo.videoState, TELEPHONY_ERR_LOCAL_PTR_NULL, "call type error");
    return TELEPHONY_ERR_ARGUMENT_INVALID;
}

int32_t CellularCallService::RegisterCallManagerCallBack(const sptr<ICallStatusCallback> &callback)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CellularCallService::RegisterCallManagerCallBack return, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (DelayedSingleton<CellularCallRegister>::GetInstance() == nullptr) {
        TELEPHONY_LOGE("CellularCallService::RegisterCallManagerCallBack return, instance is nullptr.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return DelayedSingleton<CellularCallRegister>::GetInstance()->RegisterCallManagerCallBack(callback);
}

int32_t CellularCallService::UnRegisterCallManagerCallBack()
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CellularCallService::UnRegisterCallManagerCallBack return, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (DelayedSingleton<CellularCallRegister>::GetInstance() == nullptr) {
        TELEPHONY_LOGE("CellularCallService::UnRegisterCallManagerCallBack return, instance is nullptr.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return DelayedSingleton<CellularCallRegister>::GetInstance()->UnRegisterCallManagerCallBack();
}

int32_t CellularCallService::HoldCall(const CellularCallInfo &callInfo)
{
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("Check permission failed, no ANSWER_CALL permisson.");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (!IsValidSlotId(callInfo.slotId)) {
        TELEPHONY_LOGE("CellularCallService::HoldCall return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    if (srvccState_ == SrvccState::STARTED) {
        return TELEPHONY_ERR_FAIL;
    }
    if (CallType::TYPE_IMS == callInfo.callType) {
        auto imsControl = GetImsControl(callInfo.slotId);
        if (imsControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::HoldCall return, imsControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return imsControl->HoldCall(callInfo.slotId);
    } else if (CallType::TYPE_CS == callInfo.callType) {
        auto csControl = GetCsControl(callInfo.slotId);
        if (csControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::HoldCall return, csControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return csControl->HoldCall(callInfo.slotId);
    }
    TELEPHONY_LOGE("CellularCallService::HoldCall return, call type error.");
    return TELEPHONY_ERR_ARGUMENT_INVALID;
}

int32_t CellularCallService::UnHoldCall(const CellularCallInfo &callInfo)
{
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("Check permission failed, no ANSWER_CALL permisson.");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (!IsValidSlotId(callInfo.slotId)) {
        TELEPHONY_LOGE("CellularCallService::UnHoldCall return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    if (srvccState_ == SrvccState::STARTED) {
        return TELEPHONY_ERR_FAIL;
    }
    if (CallType::TYPE_IMS == callInfo.callType) {
        auto imsControl = GetImsControl(callInfo.slotId);
        if (imsControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::UnHoldCall return, imsControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return imsControl->UnHoldCall(callInfo.slotId);
    } else if (CallType::TYPE_CS == callInfo.callType) {
        auto csControl = GetCsControl(callInfo.slotId);
        if (csControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::UnHoldCall return, csControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return csControl->UnHoldCall(callInfo.slotId);
    }
    TELEPHONY_LOGE("CellularCallService::UnHoldCall return, call type error.");
    return TELEPHONY_ERR_ARGUMENT_INVALID;
}

int32_t CellularCallService::SwitchCall(const CellularCallInfo &callInfo)
{
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("Check permission failed, no ANSWER_CALL permisson.");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (!IsValidSlotId(callInfo.slotId)) {
        TELEPHONY_LOGE("CellularCallService::SwitchCall return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    if (srvccState_ == SrvccState::STARTED) {
        return TELEPHONY_ERR_FAIL;
    }
    if (CallType::TYPE_IMS == callInfo.callType) {
        auto imsControl = GetImsControl(callInfo.slotId);
        if (imsControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::SwitchCall return, imsControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return imsControl->SwitchCall(callInfo.slotId);
    } else if (CallType::TYPE_CS == callInfo.callType) {
        auto csControl = GetCsControl(callInfo.slotId);
        if (csControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::SwitchCall return, csControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return csControl->SwitchCall(callInfo.slotId);
    }
    TELEPHONY_LOGE("CellularCallService::SwitchCall return, call type error.");
    return TELEPHONY_ERR_ARGUMENT_INVALID;
}

int32_t CellularCallService::CombineConference(const CellularCallInfo &callInfo)
{
    if (!IsValidSlotId(callInfo.slotId)) {
        TELEPHONY_LOGE("CellularCallService::CombineConference return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    if (srvccState_ == SrvccState::STARTED) {
        return TELEPHONY_ERR_FAIL;
    }
    if (CallType::TYPE_IMS == callInfo.callType) {
        auto imsControl = GetImsControl(callInfo.slotId);
        if (imsControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::CombineConference return, imsControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return imsControl->CombineConference(callInfo.slotId);
    } else if (CallType::TYPE_CS == callInfo.callType) {
        auto csControl = GetCsControl(callInfo.slotId);
        if (csControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::CombineConference return, csControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return csControl->CombineConference(callInfo.slotId);
    }
    TELEPHONY_LOGE("CellularCallService::CombineConference return, call type error.");
    return TELEPHONY_ERR_ARGUMENT_INVALID;
}

int32_t CellularCallService::SeparateConference(const CellularCallInfo &callInfo)
{
    if (!IsValidSlotId(callInfo.slotId)) {
        TELEPHONY_LOGE("CellularCallService::SeparateConference return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    if (CallType::TYPE_IMS == callInfo.callType) {
        auto imsControl = GetImsControl(callInfo.slotId);
        if (imsControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::SeparateConference return, imsControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        std::vector<std::string> numberList;
        numberList.emplace_back(callInfo.phoneNum);
        return imsControl->KickOutFromConference(callInfo.slotId, numberList);
    } else if (CallType::TYPE_CS == callInfo.callType) {
        auto csControl = GetCsControl(callInfo.slotId);
        if (csControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::SeparateConference return, csControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return csControl->SeparateConference(callInfo.slotId, callInfo.phoneNum, callInfo.index);
    }
    TELEPHONY_LOGE("CellularCallService::SeparateConference return, call type error.");
    return TELEPHONY_ERR_ARGUMENT_INVALID;
}

int32_t CellularCallService::InviteToConference(int32_t slotId, const std::vector<std::string> &numberList)
{
    auto control = GetImsControl(slotId);
    if (control == nullptr) {
        TELEPHONY_LOGE("CellularCallService::InviteToConference return, control is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return control->InviteToConference(slotId, numberList);
}

int32_t CellularCallService::KickOutFromConference(int32_t slotId, const std::vector<std::string> &numberList)
{
    auto control = GetImsControl(slotId);
    if (control == nullptr) {
        TELEPHONY_LOGE("CellularCallService::KickOutFromConference return, control is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return control->KickOutFromConference(slotId, numberList);
}

int32_t CellularCallService::HangUpAllConnection()
{
    ModuleServiceUtils obtain;
    std::vector<int32_t> slotVector = obtain.GetSlotInfo();
    for (const auto &it : slotVector) {
        if (GetCsControl(it)) {
            GetCsControl(it)->HangUpAllConnection(it);
        }
        if (GetImsControl(it)) {
            GetImsControl(it)->HangUpAllConnection(it);
        }
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallService::HangUpAllConnection(int32_t slotId)
{
    if (GetCsControl(slotId)) {
        GetCsControl(slotId)->HangUpAllConnection(slotId);
    }
    if (GetImsControl(slotId)) {
        GetImsControl(slotId)->HangUpAllConnection(slotId);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallService::UpdateImsCallMode(const CellularCallInfo &callInfo, ImsCallMode mode)
{
    auto control = GetImsControl(callInfo.slotId);
    if (control == nullptr) {
        TELEPHONY_LOGE("CellularCallService::UpdateImsCallMode return, control is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return control->UpdateImsCallMode(callInfo, mode);
}

int32_t CellularCallService::StartDtmf(char cDtmfCode, const CellularCallInfo &callInfo)
{
    if (!IsValidSlotId(callInfo.slotId)) {
        TELEPHONY_LOGE("CellularCallService::StartDtmf return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    if (srvccState_ == SrvccState::STARTED) {
        return TELEPHONY_ERR_FAIL;
    }
    if (CallType::TYPE_IMS == callInfo.callType) {
        auto imsControl = GetImsControl(callInfo.slotId);
        if (imsControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::StartDtmf return, imsControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return imsControl->StartDtmf(imsControl->GetConnectionMap(), cDtmfCode, callInfo);
    } else if (CallType::TYPE_CS == callInfo.callType) {
        auto csControl = GetCsControl(callInfo.slotId);
        if (csControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::StartDtmf return, csControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return csControl->StartDtmf(csControl->GetConnectionMap(), cDtmfCode, callInfo);
    }
    TELEPHONY_LOGE("CellularCallService::StartDtmf return, call type error.");
    return TELEPHONY_ERR_ARGUMENT_INVALID;
}

int32_t CellularCallService::StopDtmf(const CellularCallInfo &callInfo)
{
    if (!IsValidSlotId(callInfo.slotId)) {
        TELEPHONY_LOGE("CellularCallService::StopDtmf return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    if (srvccState_ == SrvccState::STARTED) {
        return TELEPHONY_ERR_FAIL;
    }
    if (CallType::TYPE_IMS == callInfo.callType) {
        auto imsControl = GetImsControl(callInfo.slotId);
        if (imsControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::StopDtmf return, imsControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return imsControl->StopDtmf(imsControl->GetConnectionMap(), callInfo);
    } else if (CallType::TYPE_CS == callInfo.callType) {
        auto csControl = GetCsControl(callInfo.slotId);
        if (csControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::StopDtmf return, csControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return csControl->StopDtmf(csControl->GetConnectionMap(), callInfo);
    }
    TELEPHONY_LOGE("CellularCallService::StopDtmf return, call type error.");
    return TELEPHONY_ERR_ARGUMENT_INVALID;
}

int32_t CellularCallService::SendDtmf(char cDtmfCode, const CellularCallInfo &callInfo)
{
    if (!IsValidSlotId(callInfo.slotId)) {
        TELEPHONY_LOGE("CellularCallService::SendDtmf return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    if (srvccState_ == SrvccState::STARTED) {
        return TELEPHONY_ERR_FAIL;
    }
    if (CallType::TYPE_IMS == callInfo.callType) {
        auto imsControl = GetImsControl(callInfo.slotId);
        if (imsControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::SendDtmf return, imsControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return imsControl->SendDtmf(imsControl->GetConnectionMap(), cDtmfCode, callInfo);
    } else if (CallType::TYPE_CS == callInfo.callType) {
        auto csControl = GetCsControl(callInfo.slotId);
        if (csControl == nullptr) {
            TELEPHONY_LOGE("CellularCallService::SendDtmf return, csControl is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return csControl->SendDtmf(csControl->GetConnectionMap(), cDtmfCode, callInfo);
    }
    TELEPHONY_LOGE("CellularCallService::SendDtmf return, call type error.");
    return TELEPHONY_ERR_ARGUMENT_INVALID;
}

int32_t CellularCallService::StartRtt(int32_t slotId, const std::string &msg)
{
    auto control = GetImsControl(slotId);
    if (control == nullptr) {
        TELEPHONY_LOGE("CellularCallService::StartRtt return, control is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return control->StartRtt(slotId, msg);
}

int32_t CellularCallService::StopRtt(int32_t slotId)
{
    auto control = GetImsControl(slotId);
    if (control == nullptr) {
        TELEPHONY_LOGE("CellularCallService::StopRtt return, control is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return control->StopRtt(slotId);
}

int32_t CellularCallService::SetCallTransferInfo(int32_t slotId, const CallTransferInfo &cTInfo)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CellularCallService::SetCallTransferInfo return, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::SetCallTransferInfo return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallSupplement cellularCallSupplement;
    return cellularCallSupplement.SetCallTransferInfo(slotId, cTInfo);
}

int32_t CellularCallService::GetCallTransferInfo(int32_t slotId, CallTransferType type)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CellularCallService::GetCallTransferInfo return, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGI("CellularCallService::GetCallTransferInfo");
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::GetCallTransferInfo return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallSupplement cellularCallSupplement;
    return cellularCallSupplement.GetCallTransferInfo(slotId, type);
}

std::shared_ptr<CSControl> CellularCallService::GetCsControl(int32_t slotId)
{
    return csControlMap_[slotId];
}

std::shared_ptr<IMSControl> CellularCallService::GetImsControl(int32_t slotId)
{
    return imsControlMap_[slotId];
}

void CellularCallService::SetCsControl(int32_t slotId, const std::shared_ptr<CSControl> &csControl)
{
    csControlMap_[slotId] = csControl;
}

void CellularCallService::SetImsControl(int32_t slotId, const std::shared_ptr<IMSControl> &imsControl)
{
    imsControlMap_[slotId] = imsControl;
}

int32_t CellularCallService::SetCallWaiting(int32_t slotId, bool activate)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CellularCallService::SetCallWaiting return, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::SetCallWaiting return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallSupplement cellularCallSupplement;
    return cellularCallSupplement.SetCallWaiting(slotId, activate);
}

int32_t CellularCallService::GetCallWaiting(int32_t slotId)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CellularCallService::GetCallWaiting return, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGI("CellularCallService::GetCallWaiting");
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::GetCallWaiting return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallSupplement cellularCallSupplement;
    return cellularCallSupplement.GetCallWaiting(slotId);
}

int32_t CellularCallService::SetCallRestriction(int32_t slotId, const CallRestrictionInfo &crInfo)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CellularCallService::SetCallRestriction return, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGI("CellularCallService::SetCallRestriction");
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::SetCallRestriction return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallSupplement cellularCallSupplement;
    return cellularCallSupplement.SetCallRestriction(slotId, crInfo);
}

int32_t CellularCallService::GetCallRestriction(int32_t slotId, CallRestrictionType facType)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CellularCallService::GetCallRestriction return, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGI("CellularCallService::GetCallRestriction");
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::GetCallRestriction return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallSupplement cellularCallSupplement;
    return cellularCallSupplement.GetCallRestriction(slotId, facType);
}

int32_t CellularCallService::IsEmergencyPhoneNumber(int32_t slotId, const std::string &phoneNum, bool &enabled)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::IsEmergencyPhoneNumber return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    EmergencyUtils emergencyUtils;
    return emergencyUtils.IsEmergencyCall(slotId, phoneNum, enabled);
}

int32_t CellularCallService::SetEmergencyCallList(int32_t slotId, std::vector<EmergencyCall> &eccVec)
{
    TELEPHONY_LOGE("CellularCallService::SetEmergencyCallList start");
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::SetMute return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallConfig config;
    return config.SetEmergencyCallList(slotId, eccVec);
}

int32_t CellularCallService::SetDomainPreferenceMode(int32_t slotId, int32_t mode)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CellularCallService::SetDomainPreferenceMode return, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::SetDomainPreferenceMode return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallConfig config;
    return config.SetDomainPreferenceMode(slotId, mode);
}

int32_t CellularCallService::GetDomainPreferenceMode(int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::GetDomainPreferenceMode return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallConfig config;
    return config.GetDomainPreferenceMode(slotId);
}

int32_t CellularCallService::SetImsSwitchStatus(int32_t slotId, bool active)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CellularCallService::SetImsSwitchStatus return, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::SetImsSwitchStatus return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallConfig config;
    return config.SetImsSwitchStatus(slotId, active);
}

int32_t CellularCallService::GetImsSwitchStatus(int32_t slotId, bool &enabled)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::GetImsSwitchStatus return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallConfig config;
    return config.GetImsSwitchStatus(slotId, enabled);
}

int32_t CellularCallService::SetImsConfig(int32_t slotId, ImsConfigItem item, const std::string &value)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::SetImsConfig return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallConfig config;
    return config.SetImsConfig(item, value);
}

int32_t CellularCallService::SetImsConfig(int32_t slotId, ImsConfigItem item, int32_t value)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::SetImsConfig return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallConfig config;
    return config.SetImsConfig(item, value);
}

int32_t CellularCallService::GetImsConfig(int32_t slotId, ImsConfigItem item)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::GetImsConfig return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallConfig config;
    return config.GetImsConfig(item);
}

int32_t CellularCallService::SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::SetImsFeatureValue return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallConfig config;
    return config.SetImsFeatureValue(type, value);
}

int32_t CellularCallService::GetImsFeatureValue(int32_t slotId, FeatureType type)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::GetImsFeatureValue return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallConfig config;
    return config.GetImsFeatureValue(type);
}

bool CellularCallService::IsValidSlotId(int32_t slotId) const
{
    const int32_t slotSingle = 1;
    const int32_t slotDouble = 2;
    if (SIM_SLOT_COUNT == slotSingle) {
        return slotId == DEFAULT_SIM_SLOT_ID;
    } else if (SIM_SLOT_COUNT == slotDouble) {
        return slotId == SIM_SLOT_0 || slotId == SIM_SLOT_1;
    }
    return false;
}

bool CellularCallService::IsNeedIms(int32_t slotId) const
{
    ModuleServiceUtils moduleUtils;
    CellularCallConfig config;
    bool imsRegState = moduleUtils.GetImsRegistrationState(slotId);
    bool imsServiceConnected = moduleUtils.NeedCallImsService();
    int32_t preferenceMode = config.GetPreferenceMode(slotId);
    bool imsSwitchStatus = config.GetSwitchStatus(slotId);
    TELEPHONY_LOGI("IsNeedIms state:%{public}d, mode:%{public}d, status:%{public}d, connected:%{public}d", imsRegState,
        preferenceMode, imsSwitchStatus, imsServiceConnected);
    if (imsRegState && preferenceMode != DomainPreferenceMode::CS_VOICE_ONLY && imsSwitchStatus &&
        imsServiceConnected) {
        return true;
    }
    return false;
}

std::shared_ptr<CellularCallHandler> CellularCallService::GetHandler(int32_t slotId)
{
    return handlerMap_[slotId];
}

int32_t CellularCallService::CtrlCamera(const std::u16string &cameraId, int32_t callingUid, int32_t callingPid)
{
    CellularCallConfig config;
    return config.CtrlCamera(cameraId, callingUid, callingPid);
}

int32_t CellularCallService::SetPreviewWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height)
{
    CellularCallConfig config;
    return config.SetPreviewWindow(x, y, z, width, height);
}

int32_t CellularCallService::SetDisplayWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height)
{
    CellularCallConfig config;
    return config.SetDisplayWindow(x, y, z, width, height);
}

int32_t CellularCallService::SetCameraZoom(float zoomRatio)
{
    CellularCallConfig config;
    return config.SetCameraZoom(zoomRatio);
}

int32_t CellularCallService::SetPauseImage(const std::u16string &path)
{
    CellularCallConfig config;
    return config.SetPauseImage(path);
}

int32_t CellularCallService::SetDeviceDirection(int32_t rotation)
{
    CellularCallConfig config;
    return config.SetDeviceDirection(rotation);
}

int32_t CellularCallService::SetMute(int32_t slotId, int32_t mute)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::SetMute return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallConfig config;
    return config.SetMute(slotId, mute);
}

int32_t CellularCallService::GetMute(int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CellularCallService::GetMute return, invalid slot id");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    CellularCallConfig config;
    return config.GetMute(slotId);
}

void CellularCallService::SetSrvccState(int32_t srvccState)
{
    srvccState_ = srvccState;
}

int32_t CellularCallService::GetSrvccState()
{
    return srvccState_;
}

bool CellularCallService::UseImsForEmergency(const CellularCallInfo &callInfo)
{
    ModuleServiceUtils moduleUtils;
    CellularCallConfig config;
    bool enabled = false;
    if (IsEmergencyPhoneNumber(callInfo.slotId, callInfo.phoneNum, enabled) != TELEPHONY_SUCCESS) {
        return false;
    }
    if (enabled && moduleUtils.NeedCallImsService() && config.GetImsPreferForEmergencyConfig(callInfo.slotId)) {
        return true;
    }
    return false;
}

void CellularCallService::HandleCallManagerException()
{
    ModuleServiceUtils obtain;
    std::vector<int32_t> slotVector = obtain.GetSlotInfo();
    for (const auto &it : slotVector) {
        auto csControl = GetCsControl(it);
        if (csControl != nullptr) {
            csControl->SetHangupReportIgnoredFlag(true);
            csControl->HangUpAllConnection(it);
        }
        auto imsControl = GetImsControl(it);
        if (imsControl != nullptr) {
            imsControl->SetHangupReportIgnoredFlag(true);
            imsControl->HangUpAllConnection(it);
        }
    }
}

CellularCallService::SystemAbilityStatusChangeListener::SystemAbilityStatusChangeListener(
    std::shared_ptr<CellularCallHandler> &cellularCallHandler)
    : cellularCallHandler_(cellularCallHandler)
{}

void CellularCallService::SystemAbilityStatusChangeListener::OnAddSystemAbility(
    int32_t systemAbilityId, const std::string &deviceId)
{
    if (systemAbilityId != COMMON_EVENT_SERVICE_ID) {
        TELEPHONY_LOGE("systemAbilityId is not COMMON_EVENT_SERVICE_ID");
        return;
    }
    if (cellularCallHandler_ == nullptr) {
        TELEPHONY_LOGE("COMMON_EVENT_SERVICE_ID cellularCallHandler_ is nullptr");
        return;
    }
    bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(cellularCallHandler_);
    TELEPHONY_LOGI("subscribeResult = %{public}d", subscribeResult);
}

void CellularCallService::SystemAbilityStatusChangeListener::OnRemoveSystemAbility(
    int32_t systemAbilityId, const std::string &deviceId)
{
    switch (systemAbilityId) {
        case TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID: {
            auto cellularCallRegister = DelayedSingleton<CellularCallRegister>::GetInstance();
            if (cellularCallRegister != nullptr) {
                cellularCallRegister->UnRegisterCallManagerCallBack();
            }
            auto cellularCallService = DelayedSingleton<CellularCallService>::GetInstance();
            if (cellularCallService == nullptr) {
                TELEPHONY_LOGE("cellularCallService is nullptr");
                return;
            }
            cellularCallService->HandleCallManagerException();
            count_++;
            CellularCallHiSysEvent::WriteFoundationRestartFaultEvent(count_);
            break;
        }
        case COMMON_EVENT_SERVICE_ID: {
            if (cellularCallHandler_ == nullptr) {
                TELEPHONY_LOGE("cellularCallHandler_ is nullptr");
                return;
            }
            bool unSubscribeResult = EventFwk::CommonEventManager::UnSubscribeCommonEvent(cellularCallHandler_);
            TELEPHONY_LOGI("unSubscribeResult = %{public}d", unSubscribeResult);
            break;
        }
        default:
            TELEPHONY_LOGE("systemAbilityId is invalid");
            break;
    }
}

#ifdef CALL_MANAGER_AUTO_START_OPTIMIZE
void CellularCallService::StartCallManagerService()
{
    sptr<ISystemAbilityManager> managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        TELEPHONY_LOGE("GetSystemAbilityManager failed!");
        return;
    }

    sptr<IRemoteObject> iRemoteObjectPtr = managerPtr->GetSystemAbility(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    if (iRemoteObjectPtr == nullptr) {
        TELEPHONY_LOGE("GetSystemAbility failed!");
    }
}
#endif
} // namespace Telephony
} // namespace OHOS
