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

#include "core_manager_inner.h"

#include "network_search_types.h"
#include "parameter.h"
#include "radio_event.h"
#include "string_ex.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

using namespace OHOS::Telephony;
namespace OHOS {
namespace Telephony {
constexpr int NETWORK_IS_NULL = -1;
constexpr int32_t INVALID_VALUE = -1;

CoreManagerInner::CoreManagerInner() {}

CoreManagerInner &CoreManagerInner::GetInstance()
{
    static CoreManagerInner instance;
    return instance;
}

void CoreManagerInner::OnInit(std::shared_ptr<INetworkSearch> networkSearchManager,
    std::shared_ptr<ISimManager> simManager, std::shared_ptr<ITelRilManager> telRilManager)
{
    networkSearchManager_ = networkSearchManager;
    simManager_ = simManager;
    telRilManager_ = telRilManager;
    isInitAllObj_ = true;
}

bool CoreManagerInner::IsInitFinished(void)
{
    return isInitAllObj_;
}

void CoreManagerInner::SetTelRilMangerObj(std::shared_ptr<ITelRilManager> telRilManager)
{
    telRilManager_ = telRilManager;
}

bool CoreManagerInner::IsInitFinishedForTelRil(void)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telrilmanager is null");
    }
    return telRilManager_ != nullptr;
}

int32_t CoreManagerInner::GetDefaultSlotId(void)
{
    return DEFAULT_SIM_SLOT_ID;
}

int32_t CoreManagerInner::GetMaxSimCount(void)
{
    char simSlotCount[SYSPARA_SIZE] = { 0 };
    GetParameter(TEL_SIM_SLOT_COUNT, DEFAULT_SLOT_COUNT, simSlotCount, SYSPARA_SIZE);
    int32_t slotCount = std::atoi(simSlotCount);
    return slotCount;
}

int32_t CoreManagerInner::RegisterCoreNotify(
    int32_t slotId, const std::shared_ptr<AppExecFwk::EventHandler> &handler, int what, int32_t *obj)
{
    if (what >= RadioEvent::RADIO_PS_CONNECTION_ATTACHED && what <= RadioEvent::RADIO_EMERGENCY_STATE_CLOSE) {
        if (networkSearchManager_ == nullptr) {
            TELEPHONY_LOGE("networkSearchManager is null!");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        networkSearchManager_->RegisterCoreNotify(slotId, handler, what);
    } else if ((what >= RadioEvent::RADIO_SIM_STATE_CHANGE) && (what <= RadioEvent::RADIO_SIM_ACCOUNT_LOADED)) {
        if (simManager_ == nullptr) {
            TELEPHONY_LOGE("simManager_ is null");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        simManager_->RegisterCoreNotify(slotId, handler, what);
    } else {
        if (telRilManager_ == nullptr) {
            TELEPHONY_LOGE("telRilManager is null!");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return telRilManager_->RegisterCoreNotify(slotId, handler, what, obj);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CoreManagerInner::UnRegisterCoreNotify(
    int32_t slotId, const std::shared_ptr<AppExecFwk::EventHandler> &observerCallBack, int what)
{
    if (what >= RadioEvent::RADIO_PS_CONNECTION_ATTACHED && what <= RadioEvent::RADIO_EMERGENCY_STATE_CLOSE) {
        if (networkSearchManager_ == nullptr) {
            TELEPHONY_LOGE("networkSearchManager is null!");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        networkSearchManager_->UnRegisterCoreNotify(slotId, observerCallBack, what);
    } else if (what >= RadioEvent::RADIO_SIM_STATE_CHANGE && what <= RadioEvent::RADIO_SIM_RECORDS_LOADED) {
        if (simManager_ == nullptr) {
            TELEPHONY_LOGE("simManager_ is null");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        simManager_->UnRegisterCoreNotify(slotId, observerCallBack, what);
    } else {
        if (telRilManager_ == nullptr) {
            TELEPHONY_LOGE("telRilManager is null!");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return telRilManager_->UnRegisterCoreNotify(slotId, observerCallBack, what);
    }
    return TELEPHONY_SUCCESS;
}

void CoreManagerInner::RegisterCellularDataObject(const sptr<NetworkSearchCallBackBase> &callback)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return;
    }
    networkSearchManager_->RegisterCellularDataObject(callback);
}

void CoreManagerInner::UnRegisterCellularDataObject(const sptr<NetworkSearchCallBackBase> &callback)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return;
    }
    networkSearchManager_->UnRegisterCellularDataObject(callback);
}

void CoreManagerInner::RegisterCellularCallObject(const sptr<NetworkSearchCallBackBase> &callback)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return;
    }
    networkSearchManager_->RegisterCellularCallObject(callback);
}

void CoreManagerInner::UnRegisterCellularCallObject(const sptr<NetworkSearchCallBackBase> &callback)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return;
    }
    networkSearchManager_->UnRegisterCellularCallObject(callback);
}

int32_t CoreManagerInner::RegisterSimAccountCallback(
    const std::string &bundleName, const sptr<SimAccountCallback> &callback)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->RegisterSimAccountCallback(bundleName, callback);
}

int32_t CoreManagerInner::UnregisterSimAccountCallback(const std::string &bundleName)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->UnregisterSimAccountCallback(bundleName);
}

/******************** telRilManager start *******************/
int32_t CoreManagerInner::SetUssd(int32_t slotId, int32_t eventId, const std::string str,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetUssd(slotId, str, response);
}

int32_t CoreManagerInner::GetUssd(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetUssd(slotId, response);
}

int32_t CoreManagerInner::GetMute(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetMute(slotId, response);
}

int32_t CoreManagerInner::SetMute(
    int32_t slotId, int32_t eventId, int32_t mute, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetMute(slotId, mute, response);
}

int32_t CoreManagerInner::GetEmergencyCallList(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetEmergencyCallList(slotId, response);
}

int32_t CoreManagerInner::SetEmergencyCallList(int32_t slotId, int32_t eventId, std::vector<EmergencyCall> &eccVec,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    TELEPHONY_LOGI("SetEmergencyCallList start");
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetEmergencyCallList(slotId, eccVec, response);
}

int32_t CoreManagerInner::GetCallFailReason(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetCallFailReason(slotId, response);
}

int32_t CoreManagerInner::SetCallPreferenceMode(
    int32_t slotId, int32_t eventId, int32_t mode, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetCallPreferenceMode(slotId, mode, response);
}

int32_t CoreManagerInner::GetCallPreferenceMode(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetCallPreferenceMode(slotId, response);
}

int32_t CoreManagerInner::SetPreferredNetworkPara(int32_t slotId, int32_t eventId, int32_t preferredNetworkType,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetPreferredNetwork(slotId, preferredNetworkType, response);
}

int32_t CoreManagerInner::GetPreferredNetworkPara(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetPreferredNetwork(slotId, response);
}

int32_t CoreManagerInner::GetOperatorInfo(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetOperatorInfo(slotId, response);
}

int32_t CoreManagerInner::GetCellInfoList(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetCellInfoList(slotId, response);
}

int32_t CoreManagerInner::GetCurrentCellInfo(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetCurrentCellInfo(slotId, response);
}

int32_t CoreManagerInner::SendGsmSms(int32_t slotId, int32_t eventId, GsmSimMessageParam &gsmMessage,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId, gsmMessage.refId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SendGsmSms(slotId, gsmMessage.smscPdu, gsmMessage.pdu, response);
}

int32_t CoreManagerInner::SendCdmaSms(int32_t slotId, int32_t eventId, std::string pdu, int64_t refId,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId, refId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SendCdmaSms(slotId, pdu, response);
}

int32_t CoreManagerInner::AddSimMessage(int32_t slotId, int32_t eventId, const SimMessageParam &simMessage,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->AddSimMessage(slotId, simMessage, response);
}

int32_t CoreManagerInner::DelSimMessage(
    int32_t slotId, int32_t eventId, int32_t gsmIndex, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->DelSimMessage(slotId, gsmIndex, response);
}

int32_t CoreManagerInner::GetSmscAddr(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetSmscAddr(slotId, response);
}

int32_t CoreManagerInner::SetSmscAddr(int32_t slotId, int32_t eventId, int32_t tosca, std::string address,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetSmscAddr(slotId, tosca, address, response);
}

int32_t CoreManagerInner::SetCBConfig(int32_t slotId, int32_t eventId, const CBConfigParam &cbConfig,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetCBConfig(slotId, cbConfig, response);
}

int32_t CoreManagerInner::SetCdmaCBConfig(int32_t slotId, int32_t eventId, CdmaCBConfigInfoList &cdmaCBConfigInfoList,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetCdmaCBConfig(slotId, cdmaCBConfigInfoList, response);
}

int32_t CoreManagerInner::GetCBConfig(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetCBConfig(slotId, response);
}

int32_t CoreManagerInner::GetCdmaCBConfig(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetCdmaCBConfig(slotId, response);
}

int32_t CoreManagerInner::SendSmsMoreMode(int32_t slotId, int32_t eventId, GsmSimMessageParam &gsmMessage,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId, gsmMessage.refId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SendSmsMoreMode(slotId, gsmMessage.smscPdu, gsmMessage.pdu, response);
}

int32_t CoreManagerInner::SendSmsAck(int32_t slotId, int32_t eventId, bool success, int32_t cause,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SendSmsAck(slotId, success, cause, response);
}

int32_t CoreManagerInner::AddCdmaSimMessage(int32_t slotId, int32_t eventId, int32_t status, std::string pdu,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->AddCdmaSimMessage(slotId, status, pdu, response);
}

int32_t CoreManagerInner::DelCdmaSimMessage(
    int32_t slotId, int32_t eventId, int32_t cdmaIndex, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->DelCdmaSimMessage(slotId, cdmaIndex, response);
}

int32_t CoreManagerInner::UpdateCdmaSimMessage(int32_t slotId, int32_t eventId, const CdmaSimMessageParam &cdmaSimMsg,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->UpdateCdmaSimMessage(slotId, cdmaSimMsg, response);
}

int32_t CoreManagerInner::GetNetworkSearchInformation(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetNetworkSearchInformation(slotId, response);
}

int32_t CoreManagerInner::GetNetworkSelectionMode(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetNetworkSelectionMode(slotId, response);
}

int32_t CoreManagerInner::SetNetworkSelectionMode(int32_t slotId, int32_t eventId, int32_t automaticFlag,
    std::string oper, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetNetworkSelectionMode(slotId, automaticFlag, oper, response);
}

int32_t CoreManagerInner::SetRadioState(
    int32_t slotId, int32_t eventId, int fun, int rst, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetRadioState(slotId, fun, rst, response);
}

int32_t CoreManagerInner::GetRadioState(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetRadioState(slotId, response);
}

int32_t CoreManagerInner::ShutDown(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->ShutDown(slotId, response);
}

int32_t CoreManagerInner::Dial(int32_t slotId, int32_t eventId, std::string address, int clirMode,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->Dial(slotId, address, clirMode, response);
}

int32_t CoreManagerInner::Reject(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->Reject(slotId, response);
}

int32_t CoreManagerInner::Hangup(
    int32_t slotId, int32_t eventId, int32_t gsmIndex, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->Hangup(slotId, gsmIndex, response);
}

int32_t CoreManagerInner::Answer(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->Answer(slotId, response);
}

int32_t CoreManagerInner::GetCallList(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetCallList(slotId, response);
}

int32_t CoreManagerInner::HoldCall(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->HoldCall(slotId, response);
}

int32_t CoreManagerInner::UnHoldCall(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->UnHoldCall(slotId, response);
}

int32_t CoreManagerInner::SwitchCall(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SwitchCall(slotId, response);
}

int32_t CoreManagerInner::CombineConference(
    int32_t slotId, int32_t eventId, int32_t callType, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->CombineConference(slotId, callType, response);
}

int32_t CoreManagerInner::SeparateConference(int32_t slotId, int32_t eventId, int32_t callIndex, int32_t callType,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SeparateConference(slotId, callIndex, callType, response);
}

int32_t CoreManagerInner::CallSupplement(
    int32_t slotId, int32_t eventId, int32_t type, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->CallSupplement(slotId, type, response);
}

int32_t CoreManagerInner::GetClip(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetClip(slotId, response);
}

int32_t CoreManagerInner::SetClip(
    int32_t slotId, int32_t eventId, int32_t action, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetClip(slotId, action, response);
}

int32_t CoreManagerInner::GetClir(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetClir(slotId, response);
}

int32_t CoreManagerInner::SetClir(
    int32_t slotId, int32_t eventId, int32_t action, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetClir(slotId, action, response);
}

int32_t CoreManagerInner::SetCallWaiting(
    int32_t slotId, int32_t eventId, int32_t activate, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetCallWaiting(slotId, activate, response);
}

int32_t CoreManagerInner::SetCallTransferInfo(int32_t slotId, int32_t eventId, const CallTransferParam &callTransfer,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetCallTransferInfo(slotId, callTransfer, response);
}

int32_t CoreManagerInner::GetCallTransferInfo(int32_t slotId, int32_t eventId, const int32_t reason,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetCallTransferInfo(slotId, reason, response);
}

int32_t CoreManagerInner::GetCallWaiting(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetCallWaiting(slotId, response);
}

int32_t CoreManagerInner::GetCallRestriction(
    int32_t slotId, int32_t eventId, std::string fac, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetCallRestriction(slotId, fac, response);
}

int32_t CoreManagerInner::SetCallRestriction(int32_t slotId, int32_t eventId,
    const CallRestrictionParam &callRestriction, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetCallRestriction(slotId, callRestriction, response);
}

int32_t CoreManagerInner::SendDTMF(int32_t slotId, int32_t eventId, const DtmfParam &dtmfParam,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SendDtmf(slotId, dtmfParam, response);
}

int32_t CoreManagerInner::SendDTMF(int32_t slotId, int32_t eventId, char cDTMFCode, int32_t index,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SendDtmf(slotId, cDTMFCode, index, response);
}

int32_t CoreManagerInner::StartDTMF(int32_t slotId, int32_t eventId, char cDTMFCode, int32_t index,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->StartDtmf(slotId, cDTMFCode, index, response);
}

int32_t CoreManagerInner::StopDTMF(
    int32_t slotId, int32_t eventId, int32_t index, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->StopDtmf(slotId, index, response);
}

int32_t CoreManagerInner::SetDataPermitted(
    int32_t slotId, int32_t eventId, int32_t dataPermitted, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetDataPermitted(slotId, dataPermitted, response);
}

int32_t CoreManagerInner::SetInitApnInfo(int32_t slotId, int32_t eventId, const DataProfile &dataProfile,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetInitApnInfo(slotId, dataProfile, response);
}

int32_t CoreManagerInner::ActivatePdpContext(int32_t slotId, int32_t eventId, const ActivateDataParam &activateData,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId, activateData.param);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->ActivatePdpContext(slotId, activateData, response);
}

int32_t CoreManagerInner::DeactivatePdpContext(int32_t slotId, int32_t eventId,
    const DeactivateDataParam &deactivateData, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId, deactivateData.param);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->DeactivatePdpContext(slotId, deactivateData.cid, deactivateData.reason, response);
}

int32_t CoreManagerInner::GetPdpContextList(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetPdpContextList(slotId, response);
}

int32_t CoreManagerInner::SetLinkBandwidthReportingRule(int32_t slotId, int32_t eventId,
    LinkBandwidthRule linkBandwidth, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->SetLinkBandwidthReportingRule(slotId, linkBandwidth, response);
}

int32_t CoreManagerInner::GetLinkBandwidthInfo(
    int32_t slotId, int32_t eventId, const int32_t cid, const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetLinkBandwidthInfo(slotId, cid, response);
}

int32_t CoreManagerInner::GetSignalStrength(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetSignalStrength(slotId, response);
}

int32_t CoreManagerInner::GetCsRegStatus(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetCsRegStatus(slotId, response);
}

int32_t CoreManagerInner::GetPsRegStatus(
    int32_t slotId, int32_t eventId, const std::shared_ptr<AppExecFwk::EventHandler> &handler) const
{
    if (telRilManager_ == nullptr) {
        TELEPHONY_LOGE("telRilManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    response->SetOwner(handler);
    return telRilManager_->GetPsRegStatus(slotId, response);
}
/******************** telRilManager end *******************/
/******************** networkSearchManager start *******************/
int32_t CoreManagerInner::GetPsRadioTech(int32_t slotId, int32_t &psRadioTech)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetPsRadioTech(slotId, psRadioTech);
}

int32_t CoreManagerInner::GetCsRadioTech(int32_t slotId, int32_t &csRadioTech)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetCsRadioTech(slotId, csRadioTech);
}

int32_t CoreManagerInner::GetPsRegState(int32_t slotId)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return NETWORK_IS_NULL;
    }
    return networkSearchManager_->GetPsRegState(slotId);
}

int32_t CoreManagerInner::GetCsRegState(int32_t slotId)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return NETWORK_IS_NULL;
    }
    return networkSearchManager_->GetCsRegState(slotId);
}

int32_t CoreManagerInner::GetPsRoamingState(int32_t slotId)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return NETWORK_IS_NULL;
    }
    return networkSearchManager_->GetPsRoamingState(slotId);
}

bool CoreManagerInner::SetNetworkSelectionMode(int32_t slotId, int32_t selectMode,
    const sptr<NetworkInformation> &networkInformation, bool resumeSelection,
    const sptr<INetworkSearchCallback> &callback)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return false;
    }
    return networkSearchManager_->SetNetworkSelectionMode(
        slotId, selectMode, networkInformation, resumeSelection, callback);
}

int32_t CoreManagerInner::GetSignalInfoList(int32_t slotId, std::vector<sptr<SignalInformation>> &signals)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetSignalInfoList(slotId, signals);
}

std::u16string CoreManagerInner::GetOperatorNumeric(int32_t slotId)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return std::u16string();
    }
    return networkSearchManager_->GetOperatorNumeric(slotId);
}

int32_t CoreManagerInner::GetOperatorName(int32_t slotId, std::u16string &operatorName)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetOperatorName(slotId, operatorName);
}

int32_t CoreManagerInner::GetNetworkStatus(int32_t slotId, sptr<NetworkState> &networkState)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetNetworkStatus(slotId, networkState);
}

int32_t CoreManagerInner::SetRadioState(
    int32_t slotId, bool isOn, int32_t rst, const sptr<INetworkSearchCallback> &callback)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->SetRadioState(slotId, isOn, rst, callback);
}

int32_t CoreManagerInner::GetRadioState(int32_t slotId)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return NETWORK_IS_NULL;
    }
    return networkSearchManager_->GetRadioState(slotId);
}

int32_t CoreManagerInner::GetRadioState(int32_t slotId, const sptr<INetworkSearchCallback> &callback)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetRadioState(slotId, callback);
}

int32_t CoreManagerInner::GetIsoCountryCodeForNetwork(int32_t slotId, std::u16string &countryCode)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetIsoCountryCodeForNetwork(slotId, countryCode);
}

int32_t CoreManagerInner::GetImei(int32_t slotId, std::u16string &imei)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetImei(slotId, imei);
}

int32_t CoreManagerInner::GetMeid(int32_t slotId, std::u16string &meid)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetMeid(slotId, meid);
}

int32_t CoreManagerInner::GetUniqueDeviceId(int32_t slotId, std::u16string &deviceId)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetUniqueDeviceId(slotId, deviceId);
}

PhoneType CoreManagerInner::GetPhoneType(int32_t slotId)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return PhoneType::PHONE_TYPE_IS_NONE;
    }
    return networkSearchManager_->GetPhoneType(slotId);
}

sptr<CellLocation> CoreManagerInner::GetCellLocation(int32_t slotId)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return nullptr;
    }
    return networkSearchManager_->GetCellLocation(slotId);
}

int32_t CoreManagerInner::GetNetworkSearchInformation(int32_t slotId, const sptr<INetworkSearchCallback> &callback)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetNetworkSearchInformation(slotId, callback);
}

int32_t CoreManagerInner::GetNetworkSelectionMode(int32_t slotId, const sptr<INetworkSearchCallback> &callback)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetNetworkSelectionMode(slotId, callback);
}

int32_t CoreManagerInner::GetCellInfoList(int32_t slotId, std::vector<sptr<CellInformation>> &cellInfo)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetCellInfoList(slotId, cellInfo);
}

int32_t CoreManagerInner::SendUpdateCellLocationRequest(int32_t slotId)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->SendUpdateCellLocationRequest(slotId);
}

int32_t CoreManagerInner::GetPreferredNetwork(int32_t slotId, const sptr<INetworkSearchCallback> &callback)
{
    if (networkSearchManager_ != nullptr) {
        return networkSearchManager_->GetPreferredNetwork(slotId, callback);
    }
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CoreManagerInner::SetPreferredNetwork(
    int32_t slotId, int32_t networkMode, const sptr<INetworkSearchCallback> &callback)
{
    if (networkSearchManager_ != nullptr) {
        return networkSearchManager_->SetPreferredNetwork(slotId, networkMode, callback);
    }
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

bool CoreManagerInner::IsNrSupported(int32_t slotId)
{
    if (networkSearchManager_ != nullptr) {
        return networkSearchManager_->IsNrSupported(slotId);
    }
    return false;
}

void CoreManagerInner::DcPhysicalLinkActiveUpdate(int32_t slotId, bool isActive)
{
    if (networkSearchManager_ != nullptr) {
        networkSearchManager_->DcPhysicalLinkActiveUpdate(slotId, isActive);
    }
}

int32_t CoreManagerInner::GetNrOptionMode(int32_t slotId, NrMode &mode)
{
    if (networkSearchManager_ != nullptr) {
        return networkSearchManager_->GetNrOptionMode(slotId, mode);
    }
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

FrequencyType CoreManagerInner::GetFrequencyType(int32_t slotId) const
{
    if (networkSearchManager_ != nullptr) {
        return networkSearchManager_->GetFrequencyType(slotId);
    }
    return FrequencyType::FREQ_TYPE_UNKNOWN;
}

NrState CoreManagerInner::GetNrState(int32_t slotId) const
{
    if (networkSearchManager_ != nullptr) {
        return networkSearchManager_->GetNrState(slotId);
    }
    return NrState::NR_STATE_NOT_SUPPORT;
}

int32_t CoreManagerInner::GetImsRegStatus(int32_t slotId, ImsServiceType imsSrvType, ImsRegInfo &info) const
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetImsRegStatus(slotId, imsSrvType, info);
}
/******************** networkSearchManager end ************************/
/******************** simManager_ start *******************/

int32_t CoreManagerInner::ObtainSpnCondition(int32_t slotId, bool roaming, std::string operatorNum)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return 0;
    }
    return simManager_->ObtainSpnCondition(slotId, roaming, operatorNum);
}

int32_t CoreManagerInner::GetSimSpn(int32_t slotId, std::u16string &spn)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetSimSpn(slotId, spn);
}

int32_t CoreManagerInner::SetVoiceMailInfo(
    int32_t slotId, const std::u16string &mailName, const std::u16string &mailNumber)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SetVoiceMailInfo(slotId, mailName, mailNumber);
}

int32_t CoreManagerInner::QueryIccDiallingNumbers(
    int slotId, int type, std::vector<std::shared_ptr<DiallingNumbersInfo>> &result)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("iccDiallingNumbersManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->QueryIccDiallingNumbers(slotId, type, result);
}

int32_t CoreManagerInner::AddIccDiallingNumbers(
    int slotId, int type, const std::shared_ptr<DiallingNumbersInfo> &diallingNumber)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("iccDiallingNumbersManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->AddIccDiallingNumbers(slotId, type, diallingNumber);
}

int32_t CoreManagerInner::DelIccDiallingNumbers(
    int slotId, int type, const std::shared_ptr<DiallingNumbersInfo> &diallingNumber)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("iccDiallingNumbersManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->DelIccDiallingNumbers(slotId, type, diallingNumber);
}

int32_t CoreManagerInner::UpdateIccDiallingNumbers(
    int slotId, int type, const std::shared_ptr<DiallingNumbersInfo> &diallingNumber)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("iccDiallingNumbersManager is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->UpdateIccDiallingNumbers(slotId, type, diallingNumber);
}

int32_t CoreManagerInner::AddSmsToIcc(int slotId, int status, std::string &pdu, std::string &smsc)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->AddSmsToIcc(slotId, status, pdu, smsc);
}

int32_t CoreManagerInner::UpdateSmsIcc(int slotId, int index, int status, std::string &pduData, std::string &smsc)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->UpdateSmsIcc(slotId, index, status, pduData, smsc);
}

std::vector<std::string> CoreManagerInner::ObtainAllSmsOfIcc(int slotId)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        std::vector<std::string> result;
        return result;
    }
    return simManager_->ObtainAllSmsOfIcc(slotId);
}

int32_t CoreManagerInner::DelSmsIcc(int slotId, int index)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->DelSmsIcc(slotId, index);
}

bool CoreManagerInner::IsSimActive(int32_t slotId)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return false;
    }
    return simManager_->IsSimActive(slotId);
}

int32_t CoreManagerInner::SetActiveSim(int32_t slotId, int32_t enable)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SetActiveSim(slotId, enable);
}

int32_t CoreManagerInner::GetSimAccountInfo(int32_t slotId, IccAccountInfo &info)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetSimAccountInfo(slotId, info);
}

int32_t CoreManagerInner::SetDefaultVoiceSlotId(int32_t slotId)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SetDefaultVoiceSlotId(slotId);
}

int32_t CoreManagerInner::SetDefaultSmsSlotId(int32_t slotId)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SetDefaultSmsSlotId(slotId);
}

int32_t CoreManagerInner::SetDefaultCellularDataSlotId(int32_t slotId)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SetDefaultCellularDataSlotId(slotId);
}

int32_t CoreManagerInner::SetPrimarySlotId(int32_t slotId)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SetPrimarySlotId(slotId);
}

int32_t CoreManagerInner::SetShowNumber(int32_t slotId, const std::u16string &number)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SetShowNumber(slotId, number);
}

int32_t CoreManagerInner::SetShowName(int32_t slotId, const std::u16string &name)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SetShowName(slotId, name);
}

int32_t CoreManagerInner::GetDefaultVoiceSlotId()
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERROR;
    }
    return simManager_->GetDefaultVoiceSlotId();
}

int32_t CoreManagerInner::GetDefaultSmsSlotId()
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERROR;
    }
    return simManager_->GetDefaultSmsSlotId();
}

int32_t CoreManagerInner::GetDefaultCellularDataSlotId()
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERROR;
    }
    return simManager_->GetDefaultCellularDataSlotId();
}

int32_t CoreManagerInner::GetPrimarySlotId(int32_t &slotId)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetPrimarySlotId(slotId);
}

int32_t CoreManagerInner::GetShowNumber(int32_t slotId, std::u16string &showNumber)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetShowNumber(slotId, showNumber);
}

int32_t CoreManagerInner::GetShowName(int32_t slotId, std::u16string &showName)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetShowName(slotId, showName);
}

int32_t CoreManagerInner::GetActiveSimAccountInfoList(std::vector<IccAccountInfo> &iccAccountInfoList)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetActiveSimAccountInfoList(iccAccountInfoList);
}

int32_t CoreManagerInner::GetOperatorConfigs(int32_t slotId, OperatorConfig &poc)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetOperatorConfigs(slotId, poc);
}

int32_t CoreManagerInner::GetSimOperatorNumeric(int32_t slotId, std::u16string &operatorNumeric)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetSimOperatorNumeric(slotId, operatorNumeric);
}

int32_t CoreManagerInner::GetISOCountryCodeForSim(int32_t slotId, std::u16string &countryCode)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetISOCountryCodeForSim(slotId, countryCode);
}

int32_t CoreManagerInner::GetSimIccId(int32_t slotId, std::u16string &iccId)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetSimIccId(slotId, iccId);
}

int32_t CoreManagerInner::GetIMSI(int32_t slotId, std::u16string &imsi)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetIMSI(slotId, imsi);
}

std::u16string CoreManagerInner::GetLocaleFromDefaultSim(int32_t slotId)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return u"";
    }
    return simManager_->GetLocaleFromDefaultSim(slotId);
}

std::int32_t CoreManagerInner::GetSlotId(int32_t simId)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERROR;
    }
    return simManager_->GetSlotId(simId);
}

std::int32_t CoreManagerInner::GetSimId(int32_t slotId)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERROR;
    }
    return simManager_->GetSimId(slotId);
}

int32_t CoreManagerInner::GetSimGid1(int32_t slotId, std::u16string &gid1)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetSimGid1(slotId, gid1);
}

std::u16string CoreManagerInner::GetSimGid2(int32_t slotId)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return u"";
    }
    return simManager_->GetSimGid2(slotId);
}

int32_t CoreManagerInner::GetOpName(int32_t slotId, std::u16string &opname)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetOpName(slotId, opname);
}

int32_t CoreManagerInner::GetOpKeyExt(int32_t slotId, std::u16string &opkeyExt)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetOpKeyExt(slotId, opkeyExt);
}

int32_t CoreManagerInner::GetOpKey(std::u16string &opkey)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t slotId = INVALID_VALUE;
    simManager_->GetPrimarySlotId(slotId);
    return GetOpKey(slotId, opkey);
}

int32_t CoreManagerInner::GetOpKey(int32_t slotId, std::u16string &opkey)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetOpKey(slotId, opkey);
}

int32_t CoreManagerInner::GetSimTelephoneNumber(int32_t slotId, std::u16string &telephoneNumber)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetSimTelephoneNumber(slotId, telephoneNumber);
}

std::u16string CoreManagerInner::GetSimTeleNumberIdentifier(const int32_t slotId)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return u"";
    }
    return simManager_->GetSimTeleNumberIdentifier(slotId);
}

int32_t CoreManagerInner::GetVoiceMailIdentifier(int32_t slotId, std::u16string &voiceMailIdentifier)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetVoiceMailIdentifier(slotId, voiceMailIdentifier);
}

int32_t CoreManagerInner::GetVoiceMailNumber(int32_t slotId, std::u16string &voiceMailNumber)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetVoiceMailNumber(slotId, voiceMailNumber);
}

int32_t CoreManagerInner::HasSimCard(int32_t slotId, bool &hasSimCard)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->HasSimCard(slotId, hasSimCard);
}

int32_t CoreManagerInner::GetSimState(int32_t slotId, SimState &simState)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetSimState(slotId, simState);
}

int32_t CoreManagerInner::GetCardType(int32_t slotId, CardType &cardType)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetCardType(slotId, cardType);
}

int32_t CoreManagerInner::UnlockPin(int32_t slotId, const std::string &pin, LockStatusResponse &response)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->UnlockPin(slotId, pin, response);
}

int32_t CoreManagerInner::UnlockPuk(
    int32_t slotId, const std::string &newPin, const std::string &puk, LockStatusResponse &response)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->UnlockPuk(slotId, newPin, puk, response);
}

int32_t CoreManagerInner::AlterPin(
    int32_t slotId, const std::string &newPin, const std::string &oldPin, LockStatusResponse &response)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->AlterPin(slotId, newPin, oldPin, response);
}

int32_t CoreManagerInner::SetLockState(int32_t slotId, const LockInfo &options, LockStatusResponse &response)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SetLockState(slotId, options, response);
}

int32_t CoreManagerInner::GetLockState(int32_t slotId, LockType lockType, LockState &lockState)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetLockState(slotId, lockType, lockState);
}

int32_t CoreManagerInner::RefreshSimState(int32_t slotId)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return false;
    }
    return simManager_->RefreshSimState(slotId);
}

int32_t CoreManagerInner::UnlockPin2(int32_t slotId, const std::string &pin2, LockStatusResponse &response)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->UnlockPin2(slotId, pin2, response);
}

int32_t CoreManagerInner::UnlockPuk2(
    int32_t slotId, const std::string &newPin2, const std::string &puk2, LockStatusResponse &response)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->UnlockPuk2(slotId, newPin2, puk2, response);
}

int32_t CoreManagerInner::AlterPin2(
    int32_t slotId, const std::string &newPin2, const std::string &oldPin2, LockStatusResponse &response)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->AlterPin2(slotId, newPin2, oldPin2, response);
}

int32_t CoreManagerInner::SendEnvelopeCmd(int32_t slotId, const std::string &cmd)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SendEnvelopeCmd(slotId, cmd);
}

int32_t CoreManagerInner::SendTerminalResponseCmd(int32_t slotId, const std::string &cmd)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SendTerminalResponseCmd(slotId, cmd);
}

int32_t CoreManagerInner::SendCallSetupRequestResult(int32_t slotId, bool accept)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SendCallSetupRequestResult(slotId, accept);
}

int32_t CoreManagerInner::UnlockSimLock(int32_t slotId, const PersoLockInfo &lockInfo, LockStatusResponse &response)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->UnlockSimLock(slotId, lockInfo, response);
}

int32_t CoreManagerInner::HasOperatorPrivileges(const int32_t slotId, bool &hasOperatorPrivileges)
{
    TELEPHONY_LOGI("CoreManagerInner::HasOperatorPrivileges slotId:%{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ can not be null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->HasOperatorPrivileges(slotId, hasOperatorPrivileges);
}

std::u16string CoreManagerInner::GetSimIst(int32_t slotId)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return u"";
    }
    return simManager_->GetSimIst(slotId);
}

int32_t CoreManagerInner::SaveImsSwitch(int32_t slotId, int32_t imsSwitchValue)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERROR;
    }
    return simManager_->SaveImsSwitch(slotId, imsSwitchValue);
}

int32_t CoreManagerInner::QueryImsSwitch(int32_t slotId, int32_t &imsSwitchValue)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null!");
        return TELEPHONY_ERROR;
    }
    return simManager_->QueryImsSwitch(slotId, imsSwitchValue);
}
/******************** simManager_ end ************************/
} // namespace Telephony
} // namespace OHOS
