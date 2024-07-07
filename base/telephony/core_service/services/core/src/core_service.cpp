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

#include "core_service.h"

#include "core_manager_inner.h"
#include "core_service_dump_helper.h"
#include "ims_core_service_client.h"
#include "network_search_manager.h"
#include "network_search_types.h"
#include "parameter.h"
#include "sim_manager.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "telephony_common_utils.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "telephony_permission.h"

namespace OHOS {
namespace Telephony {
const bool G_REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<CoreService>::GetInstance().get());

CoreService::CoreService() : SystemAbility(TELEPHONY_CORE_SERVICE_SYS_ABILITY_ID, true) {}

CoreService::~CoreService() {}

void CoreService::OnStart()
{
    bindTime_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        TELEPHONY_LOGE("CoreService has already started.");
        return;
    }

    if (!registerToService_) {
        bool ret = Publish(DelayedSingleton<CoreService>::GetInstance().get());
        if (!ret) {
            TELEPHONY_LOGE("CoreService::Init Publish failed!");
            return;
        }
        registerToService_ = true;
    }

    if (!Init()) {
        TELEPHONY_LOGE("failed to init CoreService");
        return;
    }
    state_ = ServiceRunningState::STATE_RUNNING;
    endTime_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    TELEPHONY_LOGI("CoreService start success");
}

bool CoreService::Init()
{
    TELEPHONY_LOGI("CoreService::Init");
    telRilManager_ = std::make_shared<TelRilManager>();
    if (telRilManager_ != nullptr) {
        if (!telRilManager_->OnInit()) {
            TELEPHONY_LOGE("TelRilManager init is failed!");
            return false;
        }
    }
    CoreManagerInner::GetInstance().SetTelRilMangerObj(telRilManager_);
    int32_t slotCount = GetMaxSimCount();
    simManager_ = std::make_shared<SimManager>(telRilManager_);
    if (simManager_ != nullptr) {
        simManager_->OnInit(slotCount);
    } else {
        TELEPHONY_LOGE("SimManager init is failed!");
        return false;
    }
    // connect ims_service
    DelayedSingleton<ImsCoreServiceClient>::GetInstance()->Init();
    networkSearchManager_ = std::make_shared<NetworkSearchManager>(telRilManager_, simManager_);
    if (networkSearchManager_ != nullptr) {
        if (!networkSearchManager_->OnInit()) {
            TELEPHONY_LOGE("NetworkSearchManager init is failed!");
            return false;
        }
    }
    simManager_->SetNetworkSearchManager(networkSearchManager_);
    CoreManagerInner::GetInstance().OnInit(networkSearchManager_, simManager_, telRilManager_);
    TELEPHONY_LOGI("CoreService::Init success");
    return true;
}

void CoreService::OnStop()
{
    state_ = ServiceRunningState::STATE_NOT_START;
    registerToService_ = false;
    DelayedSingleton<ImsCoreServiceClient>::GetInstance()->UnInit();
    telRilManager_->DeInit();
    TELEPHONY_LOGI("CoreService Stop success");
}

int32_t CoreService::GetServiceRunningState()
{
    return static_cast<int32_t>(state_);
}

int32_t CoreService::GetPsRadioTech(int32_t slotId, int32_t &psRadioTech)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_NETWORK_INFO)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetPsRadioTech(slotId, psRadioTech);
}

int32_t CoreService::GetCsRadioTech(int32_t slotId, int32_t &csRadioTech)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_NETWORK_INFO)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetCsRadioTech(slotId, csRadioTech);
}

int32_t CoreService::SetNetworkSelectionMode(int32_t slotId, int32_t selectMode,
    const sptr<NetworkInformation> &networkInformation, bool resumeSelection,
    const sptr<INetworkSearchCallback> &callback)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    TELEPHONY_LOGI("CoreService::SetNetworkSelectionMode selectMode:%{public}d", selectMode);
    return networkSearchManager_->SetNetworkSelectionMode(
        slotId, selectMode, networkInformation, resumeSelection, callback);
}

int32_t CoreService::GetSignalInfoList(int32_t slotId, std::vector<sptr<SignalInformation>> &signals)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetSignalInfoList(slotId, signals);
}

std::u16string CoreService::GetOperatorNumeric(int32_t slotId)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return std::u16string();
    }
    return networkSearchManager_->GetOperatorNumeric(slotId);
}

int32_t CoreService::GetOperatorName(int32_t slotId, std::u16string &operatorName)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetOperatorName(slotId, operatorName);
}

int32_t CoreService::GetNetworkState(int32_t slotId, sptr<NetworkState> &networkState)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_NETWORK_INFO)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetNetworkStatus(slotId, networkState);
}

int32_t CoreService::SetRadioState(int32_t slotId, bool isOn, const sptr<INetworkSearchCallback> &callback)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    TELEPHONY_LOGI("CoreService::SetRadioState --> slotId:%{public}d, isOn:%{public}d", slotId, isOn);
    return networkSearchManager_->SetRadioState(slotId, isOn, 0, callback);
}

int32_t CoreService::GetRadioState(int32_t slotId, const sptr<INetworkSearchCallback> &callback)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_NETWORK_INFO)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetRadioState(slotId, callback);
}

int32_t CoreService::GetIsoCountryCodeForNetwork(int32_t slotId, std::u16string &countryCode)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetIsoCountryCodeForNetwork(slotId, countryCode);
}

int32_t CoreService::GetImei(int32_t slotId, std::u16string &imei)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetImei(slotId, imei);
}

int32_t CoreService::GetMeid(int32_t slotId, std::u16string &meid)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetMeid(slotId, meid);
}

int32_t CoreService::GetUniqueDeviceId(int32_t slotId, std::u16string &deviceId)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetUniqueDeviceId(slotId, deviceId);
}

bool CoreService::IsNrSupported(int32_t slotId)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return false;
    }
    return networkSearchManager_->IsNrSupported(slotId);
}

int32_t CoreService::GetNrOptionMode(int32_t slotId, NrMode &mode)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetNrOptionMode(slotId, mode);
}

int32_t CoreService::HasSimCard(int32_t slotId, bool &hasSimCard)
{
    TELEPHONY_LOGD("CoreService::HasSimCard(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->HasSimCard(slotId, hasSimCard);
}

int32_t CoreService::GetSimState(int32_t slotId, SimState &simState)
{
    TELEPHONY_LOGD("CoreService::GetSimState(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    return simManager_->GetSimState(slotId, simState);
}

int32_t CoreService::GetCardType(int32_t slotId, CardType &cardType)
{
    TELEPHONY_LOGD("CoreService::GetCardType(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    return simManager_->GetCardType(slotId, cardType);
}

int32_t CoreService::GetISOCountryCodeForSim(int32_t slotId, std::u16string &countryCode)
{
    TELEPHONY_LOGD("CoreService::GetISOCountryCodeForSim(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    return simManager_->GetISOCountryCodeForSim(slotId, countryCode);
}

int32_t CoreService::GetSimSpn(int32_t slotId, std::u16string &spn)
{
    TELEPHONY_LOGD("CoreService::GetSimSpn(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetSimSpn(slotId, spn);
}

int32_t CoreService::GetSimIccId(int32_t slotId, std::u16string &iccId)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::GetSimIccId(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetSimIccId(slotId, iccId);
}

int32_t CoreService::GetSimOperatorNumeric(int32_t slotId, std::u16string &operatorNumeric)
{
    TELEPHONY_LOGD("CoreService::GetSimOperatorNumeric(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetSimOperatorNumeric(slotId, operatorNumeric);
}

int32_t CoreService::GetIMSI(int32_t slotId, std::u16string &imsi)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::GetIMSI(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetIMSI(slotId, imsi);
}

bool CoreService::IsSimActive(int32_t slotId)
{
    TELEPHONY_LOGD("CoreService::IsSimActive(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return false;
    }
    return simManager_->IsSimActive(slotId);
}

int32_t CoreService::GetSlotId(int32_t simId)
{
    TELEPHONY_LOGD("CoreService::GetSlotId(), simId = %{public}d", simId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("CoreService::GetSlotId(), simManager_ is nullptr!");
        return TELEPHONY_ERROR;
    }
    return simManager_->GetSlotId(simId);
}

int32_t CoreService::GetSimId(int32_t slotId)
{
    TELEPHONY_LOGD("CoreService::GetSimId(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("CoreService::GetSimId(), simManager_ is nullptr!");
        return TELEPHONY_ERROR;
    }
    return simManager_->GetSimId(slotId);
}

int32_t CoreService::GetNetworkSearchInformation(int32_t slotId, const sptr<INetworkSearchCallback> &callback)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetNetworkSearchInformation(slotId, callback);
}

int32_t CoreService::GetNetworkSelectionMode(int32_t slotId, const sptr<INetworkSearchCallback> &callback)
{
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetNetworkSelectionMode(slotId, callback);
}

std::u16string CoreService::GetLocaleFromDefaultSim()
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CoreService::GetLocaleFromDefaultSim, Permission denied!");
        return std::u16string();
    }
    TELEPHONY_LOGD("CoreService::GetLocaleFromDefaultSim()");
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return std::u16string();
    }
    int32_t slotId = INVALID_VALUE;
    simManager_->GetPrimarySlotId(slotId);
    if (slotId < DEFAULT_SIM_SLOT_ID) {
        slotId = DEFAULT_SIM_SLOT_ID;
    }
    return simManager_->GetLocaleFromDefaultSim(slotId);
}

int32_t CoreService::GetSimGid1(int32_t slotId, std::u16string &gid1)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CoreService::GetSimGid1, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::GetSimGid1(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetSimGid1(slotId, gid1);
}

std::u16string CoreService::GetSimGid2(int32_t slotId)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CoreService::GetSimGid2, Permission denied!");
        return std::u16string();
    }
    TELEPHONY_LOGD("CoreService::GetSimGid2(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return std::u16string();
    }
    return simManager_->GetSimGid2(slotId);
}

std::u16string CoreService::GetSimEons(int32_t slotId, const std::string &plmn, int32_t lac, bool longNameRequired)
{
    TELEPHONY_LOGD("CoreService::GetSimEons(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("CoreService::GetSimEons, Permission denied!");
        return std::u16string();
    }
    return simManager_->GetSimEons(slotId, plmn, lac, longNameRequired);
}

int32_t CoreService::GetSimAccountInfo(int32_t slotId, IccAccountInfo &info)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::GetSimAccountInfo(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetSimAccountInfo(slotId, info);
}

int32_t CoreService::SetDefaultVoiceSlotId(int32_t slotId)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::SetDefaultVoiceSlotId(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SetDefaultVoiceSlotId(slotId);
}

int32_t CoreService::GetDefaultVoiceSlotId()
{
    TELEPHONY_LOGD("CoreService::GetDefaultVoiceSlotId()");
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERROR;
    }
    return simManager_->GetDefaultVoiceSlotId();
}

int32_t CoreService::SetPrimarySlotId(int32_t slotId)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::SetPrimarySlotId(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SetPrimarySlotId(slotId);
}

int32_t CoreService::GetPrimarySlotId(int32_t &slotId)
{
    TELEPHONY_LOGD("CoreService::GetPrimarySlotId()");
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetPrimarySlotId(slotId);
}

int32_t CoreService::SetShowNumber(int32_t slotId, const std::u16string &number)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::SetShowNumber(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SetShowNumber(slotId, number);
}

int32_t CoreService::GetShowNumber(int32_t slotId, std::u16string &showNumber)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::GetShowNumber(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetShowNumber(slotId, showNumber);
}

int32_t CoreService::SetShowName(int32_t slotId, const std::u16string &name)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::SetShowName(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SetShowName(slotId, name);
}

int32_t CoreService::GetShowName(int32_t slotId, std::u16string &showName)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::GetShowName(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetShowName(slotId, showName);
}

int32_t CoreService::GetActiveSimAccountInfoList(std::vector<IccAccountInfo> &iccAccountInfoList)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::GetActiveSimAccountInfoList");
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetActiveSimAccountInfoList(iccAccountInfoList);
}

int32_t CoreService::GetOperatorConfigs(int32_t slotId, OperatorConfig &poc)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::GetOperatorConfigs");
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetOperatorConfigs(slotId, poc);
}

int32_t CoreService::UnlockPin(const int32_t slotId, const std::u16string &pin, LockStatusResponse &response)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::UnlockPin(), pinLen = %{public}lu, slotId = %{public}d",
        static_cast<unsigned long>(pin.length()), slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->UnlockPin(slotId, Str16ToStr8(pin), response);
}

int32_t CoreService::UnlockPuk(
    const int slotId, const std::u16string &newPin, const std::u16string &puk, LockStatusResponse &response)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGI("CoreService::UnlockPuk(), newPinLen = %{public}lu, pukLen = %{public}lu, slotId = %{public}d",
        static_cast<unsigned long>(newPin.length()), static_cast<unsigned long>(puk.length()), slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->UnlockPuk(slotId, Str16ToStr8(newPin), Str16ToStr8(puk), response);
}

int32_t CoreService::AlterPin(
    const int slotId, const std::u16string &newPin, const std::u16string &oldPin, LockStatusResponse &response)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGI("CoreService::AlterPin(), newPinLen = %{public}lu, oldPinLen = %{public}lu, slotId = %{public}d",
        static_cast<unsigned long>(newPin.length()), static_cast<unsigned long>(oldPin.length()), slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->AlterPin(slotId, Str16ToStr8(newPin), Str16ToStr8(oldPin), response);
}

int32_t CoreService::UnlockPin2(const int32_t slotId, const std::u16string &pin2, LockStatusResponse &response)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGI("CoreService::UnlockPin2(), pin2Len = %{public}lu, slotId = %{public}d",
        static_cast<unsigned long>(pin2.length()), slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->UnlockPin2(slotId, Str16ToStr8(pin2), response);
}

int32_t CoreService::UnlockPuk2(
    const int slotId, const std::u16string &newPin2, const std::u16string &puk2, LockStatusResponse &response)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGI("CoreService::UnlockPuk2(), newPin2Len = %{public}lu, puk2Len = %{public}lu, slotId = %{public}d",
        static_cast<unsigned long>(newPin2.length()), static_cast<unsigned long>(puk2.length()), slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->UnlockPuk2(slotId, Str16ToStr8(newPin2), Str16ToStr8(puk2), response);
}

int32_t CoreService::AlterPin2(
    const int slotId, const std::u16string &newPin2, const std::u16string &oldPin2, LockStatusResponse &response)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGI("CoreService::AlterPin2(), newPin2Len = %{public}lu, oldPin2Len = %{public}lu, slotId = %{public}d",
        static_cast<unsigned long>(newPin2.length()), static_cast<unsigned long>(oldPin2.length()), slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->AlterPin2(slotId, Str16ToStr8(newPin2), Str16ToStr8(oldPin2), response);
}

int32_t CoreService::SetLockState(int32_t slotId, const LockInfo &options, LockStatusResponse &response)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    std::u16string strPin = options.password;
    TELEPHONY_LOGI(
        "CoreService::SetLockState(),lockType = %{public}d, pinLen = %{public}lu, lockState = %{public}d, slotId "
        "= "
        "%{public}d",
        options.lockType, static_cast<unsigned long>(strPin.length()), options.lockState, slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SetLockState(slotId, options, response);
}

int32_t CoreService::GetLockState(int32_t slotId, LockType lockType, LockState &lockState)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CoreService::GetLockState, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGI("CoreService::GetLockState(), lockType = %{public}d, slotId = %{public}d", lockType, slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetLockState(slotId, lockType, lockState);
}

int32_t CoreService::RefreshSimState(int32_t slotId)
{
    TELEPHONY_LOGD("CoreService::RefreshSimState(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERROR;
    }
    return simManager_->RefreshSimState(slotId);
}

int32_t CoreService::SetActiveSim(int32_t slotId, int32_t enable)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::SetActiveSim(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SetActiveSim(slotId, enable);
}

int32_t CoreService::GetPreferredNetwork(int32_t slotId, const sptr<INetworkSearchCallback> &callback)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetPreferredNetwork(slotId, callback);
}

int32_t CoreService::SetPreferredNetwork(
    int32_t slotId, int32_t networkMode, const sptr<INetworkSearchCallback> &callback)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->SetPreferredNetwork(slotId, networkMode, callback);
}

int32_t CoreService::GetSimTelephoneNumber(int32_t slotId, std::u16string &telephoneNumber)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::GetSimTelephoneNumber(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetSimTelephoneNumber(slotId, telephoneNumber);
}

std::u16string CoreService::GetSimTeleNumberIdentifier(const int32_t slotId)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CoreService::GetSimTeleNumberIdentifier, Permission denied!");
        return std::u16string();
    }
    TELEPHONY_LOGD("CoreService::GetSimTeleNumberIdentifier(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return std::u16string();
    }
    return simManager_->GetSimTeleNumberIdentifier(slotId);
}

int32_t CoreService::GetVoiceMailIdentifier(int32_t slotId, std::u16string &voiceMailIdentifier)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::GetVoiceMailIdentifier(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetVoiceMailIdentifier(slotId, voiceMailIdentifier);
}

int32_t CoreService::GetVoiceMailNumber(int32_t slotId, std::u16string &voiceMailNumber)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::GetVoiceMailNumber(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetVoiceMailNumber(slotId, voiceMailNumber);
}

int32_t CoreService::QueryIccDiallingNumbers(
    int slotId, int type, std::vector<std::shared_ptr<DiallingNumbersInfo>> &reslut)
{
    if (!TelephonyPermission::CheckPermission(Permission::READ_CONTACTS)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::QueryIccDiallingNumbers");
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->QueryIccDiallingNumbers(slotId, type, reslut);
}

int32_t CoreService::AddIccDiallingNumbers(
    int slotId, int type, const std::shared_ptr<DiallingNumbersInfo> &diallingNumber)
{
    if (!TelephonyPermission::CheckPermission(Permission::WRITE_CONTACTS)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::AddIccDiallingNumbers");
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->AddIccDiallingNumbers(slotId, type, diallingNumber);
}

int32_t CoreService::DelIccDiallingNumbers(
    int slotId, int type, const std::shared_ptr<DiallingNumbersInfo> &diallingNumber)
{
    if (!TelephonyPermission::CheckPermission(Permission::WRITE_CONTACTS)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::DelIccDiallingNumbers");
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->DelIccDiallingNumbers(slotId, type, diallingNumber);
}

int32_t CoreService::UpdateIccDiallingNumbers(
    int slotId, int type, const std::shared_ptr<DiallingNumbersInfo> &diallingNumber)
{
    if (!TelephonyPermission::CheckPermission(Permission::WRITE_CONTACTS)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::UpdateIccDiallingNumbers");
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->UpdateIccDiallingNumbers(slotId, type, diallingNumber);
}

int32_t CoreService::SetVoiceMailInfo(
    const int32_t slotId, const std::u16string &mailName, const std::u16string &mailNumber)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::SetVoiceMailInfo(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SetVoiceMailInfo(slotId, mailName, mailNumber);
}

int32_t CoreService::GetMaxSimCount()
{
    char simSlotCount[SYSPARA_SIZE] = { 0 };
    GetParameter(TEL_SIM_SLOT_COUNT, DEFAULT_SLOT_COUNT, simSlotCount, SYSPARA_SIZE);
    int32_t slotCount = std::atoi(simSlotCount);
    return slotCount;
}

int32_t CoreService::GetOpKey(int32_t slotId, std::u16string &opkey)
{
    TELEPHONY_LOGD("CoreService::GetOpKey(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetOpKey(slotId, opkey);
}

int32_t CoreService::GetOpKeyExt(int32_t slotId, std::u16string &opkeyExt)
{
    TELEPHONY_LOGD("CoreService::GetOpKeyExt(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetOpKeyExt(slotId, opkeyExt);
}

int32_t CoreService::GetOpName(int32_t slotId, std::u16string &opname)
{
    TELEPHONY_LOGD("CoreService::GetOpName(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->GetOpName(slotId, opname);
}

int32_t CoreService::SendEnvelopeCmd(int32_t slotId, const std::string &cmd)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("CoreService::SendEnvelopeCmd simManager_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CoreService::SendEnvelopeCmd, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::SendEnvelopeCmd(), slotId = %{public}d", slotId);
    return simManager_->SendEnvelopeCmd(slotId, cmd);
}

int32_t CoreService::SendTerminalResponseCmd(int32_t slotId, const std::string &cmd)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("CoreService::SendEnvelopeCmd simManager_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CoreService::SendTerminalResponseCmd, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::SendTerminalResponseCmd(), slotId = %{public}d", slotId);
    return simManager_->SendTerminalResponseCmd(slotId, cmd);
}

int32_t CoreService::SendCallSetupRequestResult(int32_t slotId, bool accept)
{
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("CoreService::SendEnvelopeCmd simManager_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("CoreService::SendCallSetupRequestResult, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::SendCallSetupRequestResult(), slotId = %{public}d", slotId);
    return simManager_->SendCallSetupRequestResult(slotId, accept);
}

int32_t CoreService::UnlockSimLock(int32_t slotId, const PersoLockInfo &lockInfo, LockStatusResponse &response)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGI(
        "CoreService::UnlockSimLock(), lockType = %{public}d, slotId = %{public}d", lockInfo.lockType, slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->UnlockSimLock(slotId, lockInfo, response);
}

int32_t CoreService::GetImsRegStatus(int32_t slotId, ImsServiceType imsSrvType, ImsRegInfo &info)
{
    TELEPHONY_LOGI("CoreService::GetImsRegStatus --> slotId:%{public}d, imsSrvType:%{public}d", slotId, imsSrvType);
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Failed because no permission:GET_TELEPHONY_STATE");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("failed! network search manager is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetImsRegStatus(slotId, imsSrvType, info);
}

int32_t CoreService::GetCellInfoList(int32_t slotId, std::vector<sptr<CellInformation>> &cellInfo)
{
    if (!TelephonyPermission::CheckPermission(Permission::CELL_LOCATION)) {
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->GetCellInfoList(slotId, cellInfo);
}

int32_t CoreService::SendUpdateCellLocationRequest(int32_t slotId)
{
    if (!TelephonyPermission::CheckPermission(Permission::CELL_LOCATION)) {
        TELEPHONY_LOGE("CoreService::SendUpdateCellLocationRequest, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("networkSearchManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->SendUpdateCellLocationRequest(slotId);
}

int32_t CoreService::HasOperatorPrivileges(const int32_t slotId, bool &hasOperatorPrivileges)
{
    TELEPHONY_LOGD("CoreService::HasOperatorPrivileges(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->HasOperatorPrivileges(slotId, hasOperatorPrivileges);
}

int32_t CoreService::SimAuthentication(
    int32_t slotId, const std::string &aid, const std::string &authData, SimAuthenticationResponse &response)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Failed because no permission:GET_TELEPHONY_STATE");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGD("CoreService::SimAuthentication(), slotId = %{public}d", slotId);
    if (simManager_ == nullptr) {
        TELEPHONY_LOGE("simManager_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return simManager_->SimAuthentication(slotId, aid, authData, response);
}

int32_t CoreService::RegisterImsRegInfoCallback(
    int32_t slotId, ImsServiceType imsSrvType, const sptr<ImsRegInfoCallback> &callback)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Failed because no permission:GET_TELEPHONY_STATE");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("failed! network search manager is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->RegisterImsRegInfoCallback(slotId, imsSrvType, GetBundleName(), callback);
}

int32_t CoreService::UnregisterImsRegInfoCallback(int32_t slotId, ImsServiceType imsSrvType)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Failed because no permission:GET_TELEPHONY_STATE");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (networkSearchManager_ == nullptr) {
        TELEPHONY_LOGE("failed! network search manager is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return networkSearchManager_->UnregisterImsRegInfoCallback(slotId, imsSrvType, GetBundleName());
}

int32_t CoreService::Dump(std::int32_t fd, const std::vector<std::u16string> &args)
{
    if (fd < 0) {
        TELEPHONY_LOGE("dump fd invalid");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    std::vector<std::string> argsInStr;
    for (const auto &arg : args) {
        TELEPHONY_LOGI("Dump args: %s", Str16ToStr8(arg).c_str());
        argsInStr.emplace_back(Str16ToStr8(arg));
    }
    std::string result;
    CoreServiceDumpHelper dumpHelper;
    if (dumpHelper.Dump(argsInStr, result)) {
        TELEPHONY_LOGI("%s", result.c_str());
        std::int32_t ret = dprintf(fd, "%s", result.c_str());
        if (ret < 0) {
            TELEPHONY_LOGE("dprintf to dump fd failed");
            return TELEPHONY_ERROR;
        }
        return 0;
    }
    TELEPHONY_LOGW("dumpHelper failed");
    return TELEPHONY_ERROR;
}

int64_t CoreService::GetBindTime()
{
    return bindTime_;
}

int64_t CoreService::GetEndTime()
{
    return endTime_;
}

int64_t CoreService::GetSpendTime()
{
    return endTime_ - bindTime_;
}
} // namespace Telephony
} // namespace OHOS
