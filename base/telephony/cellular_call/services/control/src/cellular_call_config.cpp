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

#include "cellular_call_config.h"

#include "cellular_call_data_struct.h"
#include "cellular_call_register.h"
#include "cellular_call_service.h"
#include "core_manager_inner.h"
#include "module_service_utils.h"
#include "parameters.h"
#include "string_ex.h"
#include "telephony_types.h"

namespace OHOS {
namespace Telephony {
const int32_t SIM_PRESENT = 1;
const int32_t SIM_ABSENT = 0;
const int32_t IMS_SWITCH_VALUE_DISABLED = 0;
const int32_t IMS_SWITCH_VALUE_ENABLED = 1;
const int32_t IMS_SWITCH_STATUS_OFF = 0;
const int32_t IMS_SWITCH_STATUS_ON = 1;
const int32_t SAVE_IMS_SWITCH_FAILED = 0;
const int32_t SAVE_IMS_SWITCH_SUCCESS_CHANGED = 1;
const int32_t SAVE_IMS_SWITCH_SUCCESS_NOT_CHANGED = 2;
const int32_t INVALID_SIM_ID = 0;
const int32_t IMS_GBA_BIT = 0x02;
const int32_t SYSTEM_PARAMETER_LENGTH = 0x02;
const int MCC_LEN = 3;
const std::string LAST_ICCID_KEY = "persist.telephony.last_iccid";

std::map<int32_t, int32_t> CellularCallConfig::modeMap_;
std::map<int32_t, int32_t> CellularCallConfig::modeTempMap_;
std::map<int32_t, bool> CellularCallConfig::imsSwitchOnByDefault_;
std::map<int32_t, bool> CellularCallConfig::hideImsSwitch_;
std::map<int32_t, bool> CellularCallConfig::volteSupported_;
std::map<int32_t, std::vector<int32_t>> CellularCallConfig::nrModeSupportedList_;
std::map<int32_t, bool> CellularCallConfig::volteProvisioningSupported_;
std::map<int32_t, bool> CellularCallConfig::ssOverUtSupported_;
std::map<int32_t, bool> CellularCallConfig::imsGbaRequired_;
std::map<int32_t, bool> CellularCallConfig::utProvisioningSupported_;
std::map<int32_t, bool> CellularCallConfig::imsPreferForEmergency_;
std::map<int32_t, int32_t> CellularCallConfig::callWaitingServiceClass_;
std::map<int32_t, std::vector<std::string>> CellularCallConfig::imsCallDisconnectResoninfoMapping_;
std::map<int32_t, bool> CellularCallConfig::forceVolteSwitchOn_;
std::mutex mutex_;
std::map<int32_t, std::vector<EmergencyCall>> CellularCallConfig::eccListRadioMap_;
std::map<int32_t, std::vector<EmergencyCall>> CellularCallConfig::eccListConfigMap_;
std::vector<EmergencyCall> CellularCallConfig::eccList3gppHasSim_;
std::vector<EmergencyCall> CellularCallConfig::eccList3gppNoSim_;
std::map<int32_t, std::vector<EmergencyCall>> CellularCallConfig::allEccList_;
std::map<int32_t, int32_t> CellularCallConfig::simState_;
bool CellularCallConfig::isOperatorConfigInit_ = false;

void CellularCallConfig::InitDefaultOperatorConfig()
{
    for (int32_t i = DEFAULT_SIM_SLOT_ID; i < SIM_SLOT_COUNT; ++i) {
        CellularCallConfig::imsSwitchOnByDefault_.insert(std::pair<int, bool>(i, true));
        CellularCallConfig::hideImsSwitch_.insert(std::pair<int, bool>(i, false));
        CellularCallConfig::volteSupported_.insert(std::pair<int, bool>(i, false));
        CellularCallConfig::nrModeSupportedList_.insert(std::pair<int, std::vector<int32_t>>(
            i, std::vector<int32_t> { CARRIER_NR_AVAILABILITY_NSA, CARRIER_NR_AVAILABILITY_SA }));
        CellularCallConfig::volteProvisioningSupported_.insert(std::pair<int, bool>(i, false));
        CellularCallConfig::ssOverUtSupported_.insert(std::pair<int, bool>(i, false));
        CellularCallConfig::imsGbaRequired_.insert(std::pair<int, bool>(i, false));
        CellularCallConfig::utProvisioningSupported_.insert(std::pair<int, bool>(i, false));
        CellularCallConfig::imsPreferForEmergency_.insert(std::pair<int, bool>(i, true));
        CellularCallConfig::callWaitingServiceClass_.insert(
            std::pair<int, int32_t>(i, DEFAULT_CALL_WAITING_SERVICE_CLASS_CONFIG));
        CellularCallConfig::imsCallDisconnectResoninfoMapping_.insert(
            std::pair<int, std::vector<std::string>>(i, IMS_CALL_DISCONNECT_REASONINFO_MAPPING_CONFIG));
        CellularCallConfig::forceVolteSwitchOn_.insert(std::pair<int, bool>(i, false));
    }
}

CellularCallConfig::CellularCallConfig()
{
    if (!isOperatorConfigInit_) {
        InitDefaultOperatorConfig();
        isOperatorConfigInit_ = true;
    }
}

int32_t CellularCallConfig::SetDomainPreferenceMode(int32_t slotId, int32_t mode)
{
    if (mode < DomainPreferenceMode::CS_VOICE_ONLY || mode > DomainPreferenceMode::IMS_PS_VOICE_ONLY) {
        TELEPHONY_LOGE("SetDomainPreferenceMode return, mode out of range!");
        return CALL_ERR_PARAMETER_OUT_OF_RANGE;
    }
    modeTempMap_[slotId] = mode;
    return configRequest_.SetDomainPreferenceModeRequest(slotId, mode);
}

int32_t CellularCallConfig::GetDomainPreferenceMode(int32_t slotId)
{
    return configRequest_.GetDomainPreferenceModeRequest(slotId);
}

int32_t CellularCallConfig::SetImsSwitchStatus(int32_t slotId, bool active)
{
    TELEPHONY_LOGI(
        "CellularCallConfig::SetImsSwitchStatus entry, slotId: %{public}d, active: %{public}d", slotId, active);
    if (!volteSupported_[slotId]) {
        TELEPHONY_LOGE("Enable ims switch failed due to volte is not supported.");
        return CALL_ERR_VOLTE_NOT_SUPPORT;
    }
    if (active && !IsVolteProvisioned(slotId)) {
        TELEPHONY_LOGE("Enable ims switch failed due to volte provisioning disabled.");
        return CALL_ERR_VOLTE_PROVISIONING_DISABLED;
    }
    int32_t simId = CoreManagerInner::GetInstance().GetSimId(slotId);
    if (simId <= INVALID_SIM_ID) {
        TELEPHONY_LOGE("failed due to invalid sim id %{public}d", simId);
        return TELEPHONY_ERR_SLOTID_INVALID;
    }

    active = ChangeImsSwitchWithOperatorConfig(slotId, active);
    int32_t ret = SaveImsSwitch(slotId, BooleanToImsSwitchValue(active));
    if (ret == SAVE_IMS_SWITCH_FAILED) {
        return TELEPHONY_ERR_DATABASE_WRITE_FAIL;
    } else if (ret == SAVE_IMS_SWITCH_SUCCESS_NOT_CHANGED) {
        return TELEPHONY_SUCCESS;
    }

    SimState simState = SimState::SIM_STATE_UNKNOWN;
    CoreManagerInner::GetInstance().GetSimState(slotId, simState);
    TELEPHONY_LOGI("active: %{public}d simState : %{public}d", active, simState);
    if (simState == SimState::SIM_STATE_LOADED || simState == SimState::SIM_STATE_READY) {
        UpdateImsCapabilities(slotId, !active);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConfig::GetImsSwitchStatus(int32_t slotId, bool &enabled)
{
    TELEPHONY_LOGI("entry, slotId: %{public}d", slotId);
    auto itorHide = hideImsSwitch_.find(slotId);
    if (itorHide != hideImsSwitch_.end()) {
        if (itorHide->second) {
            auto itorSwitch = imsSwitchOnByDefault_.find(slotId);
            if (itorSwitch != imsSwitchOnByDefault_.end()) {
                enabled = imsSwitchOnByDefault_[slotId];
            }
        } else {
            int32_t imsSwitchStatus = GetSwitchStatus(slotId);
            enabled = imsSwitchStatus;
        }
    } else {
        TELEPHONY_LOGE("do not find hideImsSwitch");
        int32_t imsSwitchStatus = GetSwitchStatus(slotId);
        enabled = imsSwitchStatus;
    }
    return TELEPHONY_SUCCESS;
}

void CellularCallConfig::HandleSimStateChanged(int32_t slotId)
{
    TELEPHONY_LOGI("CellularCallConfig::HandleSimStateChanged entry, slotId: %{public}d", slotId);
    if (IsNeedUpdateEccListWhenSimStateChanged(slotId)) {
        MergeEccCallList(slotId);
        SetEmergencyCallList(slotId);
    }
}

void CellularCallConfig::HandleSimRecordsLoaded(int32_t slotId)
{
    SimState simState = SimState::SIM_STATE_UNKNOWN;
    CoreManagerInner::GetInstance().GetSimState(slotId, simState);
    TELEPHONY_LOGI("HandleSimRecordsLoaded slotId: %{public}d, sim state is :%{public}d", slotId, simState);
}

void CellularCallConfig::HandleOperatorConfigChanged(int32_t slotId)
{
    OperatorConfig operatorConfig;
    int32_t ret = CoreManagerInner::GetInstance().GetOperatorConfigs(slotId, operatorConfig);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("failed due to get operator config");
        return;
    }
    UpdateEccWhenOperatorConfigChange(slotId, operatorConfig);
    int32_t result = ParseAndCacheOperatorConfigs(slotId, operatorConfig);
    if (result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("failed due to parse operator config");
        return;
    }

    ResetImsSwitch(slotId);
    UpdateImsCapabilities(slotId, true);
}

int32_t CellularCallConfig::ParseAndCacheOperatorConfigs(int32_t slotId, OperatorConfig &poc)
{
    TELEPHONY_LOGI("CellularCallConfig::ParseAndCacheOperatorConfigs start. slotId %{public}d", slotId);
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE(" invalid slot id %{public}d", slotId);
        return TELEPHONY_ERROR;
    }

    ParseBoolOperatorConfigs(slotId, imsSwitchOnByDefault_, poc, KEY_IMS_SWITCH_ON_BY_DEFAULT_BOOL);
    ParseBoolOperatorConfigs(slotId, hideImsSwitch_, poc, KEY_HIDE_IMS_SWITCH_BOOL);
    ParseBoolOperatorConfigs(slotId, volteSupported_, poc, KEY_VOLTE_SUPPORTED_BOOL);
    ParseBoolOperatorConfigs(slotId, volteProvisioningSupported_, poc, KEY_VOLTE_PROVISIONING_SUPPORTED_BOOL);
    ParseBoolOperatorConfigs(slotId, ssOverUtSupported_, poc, KEY_SS_OVER_UT_SUPPORTED_BOOL);
    ParseBoolOperatorConfigs(slotId, imsGbaRequired_, poc, KEY_IMS_GBA_REQUIRED_BOOL);
    ParseBoolOperatorConfigs(slotId, utProvisioningSupported_, poc, KEY_UT_PROVISIONING_SUPPORTED_BOOL);
    ParseBoolOperatorConfigs(slotId, imsPreferForEmergency_, poc, KEY_IMS_PREFER_FOR_EMERGENCY_BOOL);
    ParseBoolOperatorConfigs(slotId, forceVolteSwitchOn_, poc, KEY_FORCE_VOLTE_SWITCH_ON_BOOL);

    if (poc.intArrayValue.count(KEY_NR_MODE_SUPPORTED_LIST_INT_ARRAY) > 0) {
        nrModeSupportedList_[slotId] = poc.intArrayValue[KEY_NR_MODE_SUPPORTED_LIST_INT_ARRAY];
    }
    if (poc.intValue.count(KEY_CALL_WAITING_SERVICE_CLASS_INT) > 0) {
        callWaitingServiceClass_[slotId] = poc.intValue[KEY_CALL_WAITING_SERVICE_CLASS_INT];
    }
    if (poc.stringArrayValue.count(KEY_IMS_CALL_DISCONNECT_REASONINFO_MAPPING_STRING_ARRAY) > 0) {
        imsCallDisconnectResoninfoMapping_[slotId] =
            poc.stringArrayValue[KEY_IMS_CALL_DISCONNECT_REASONINFO_MAPPING_STRING_ARRAY];
    }
    return TELEPHONY_SUCCESS;
}

void CellularCallConfig::ParseBoolOperatorConfigs(
    int32_t slotId, std::map<int32_t, bool> &config, OperatorConfig &poc, std::string configName)
{
    auto it = poc.boolValue.find(configName);
    if (it != poc.boolValue.end()) {
        config[slotId] = it->second;
    } else {
        TELEPHONY_LOGE("do't find operator config %{public}s", configName.c_str());
    }
}

void CellularCallConfig::ResetImsSwitch(int32_t slotId)
{
    bool hasSimCard = false;
    CoreManagerInner::GetInstance().HasSimCard(slotId, hasSimCard);
    if (!hasSimCard) {
        TELEPHONY_LOGE("return due to no sim card");
        return;
    }
    std::u16string iccId;
    CoreManagerInner::GetInstance().GetSimIccId(slotId, iccId);
    if (IsSimChanged(slotId, Str16ToStr8(iccId)) && forceVolteSwitchOn_[slotId]) {
        int32_t ret = CoreManagerInner::GetInstance().SaveImsSwitch(
            slotId, BooleanToImsSwitchValue(imsSwitchOnByDefault_[slotId]));
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("SaveImsSwitch failed");
        }
    }
}

void CellularCallConfig::UpdateImsCapabilities(int32_t slotId, bool needUpdateUtCapability)
{
    bool isGbaValid = IsGbaValid(slotId);
    ImsCapabilityList imsCapabilityList;

    UpdateImsVoiceCapabilities(slotId, isGbaValid, imsCapabilityList);
    if (needUpdateUtCapability) {
        UpdateImsUtCapabilities(slotId, isGbaValid, imsCapabilityList);
    }
    configRequest_.UpdateImsCapabilities(slotId, imsCapabilityList);
    configRequest_.SetImsSwitchStatusRequest(slotId, IsNeedTurnOnIms(imsCapabilityList));
}

bool CellularCallConfig::IsGbaValid(int32_t slotId)
{
    if (imsGbaRequired_[slotId]) {
        std::u16string simist = CoreManagerInner::GetInstance().GetSimIst(slotId);
        std::string simistStr = Str16ToStr8(simist);
        // If carrier requires that IMS is only available if GBA capable SIM is used,
        // then this function checks GBA bit in EF IST.
        // Format of EF IST is defined in 3GPP TS 31.103 (Section 4.2.7).
        if (!simistStr.empty() && simistStr.length() > 1) {
            bool result = (IMS_GBA_BIT & simistStr.at(1)) != 0;
            return result;
        }
    }
    return true;
}

void CellularCallConfig::UpdateImsVoiceCapabilities(
    int32_t slotId, bool isGbaValid, ImsCapabilityList &imsCapabilityList)
{
    ImsCapability volteCapability;
    volteCapability.imsCapabilityType = ImsCapabilityType::CAPABILITY_TYPE_VOICE;
    volteCapability.imsRadioTech = ImsRegTech::IMS_REG_TECH_LTE;
    volteCapability.enable =
        volteSupported_[slotId] && isGbaValid && GetSwitchStatus(slotId) && IsVolteProvisioned(slotId);
    imsCapabilityList.imsCapabilities.push_back(volteCapability);

    ImsCapability vonrCapability;
    vonrCapability.imsCapabilityType = ImsCapabilityType::CAPABILITY_TYPE_VOICE;
    vonrCapability.imsRadioTech = ImsRegTech::IMS_REG_TECH_NR;
    vonrCapability.enable = volteCapability.enable && IsVonrSupported(slotId, isGbaValid);
    imsCapabilityList.imsCapabilities.push_back(vonrCapability);
}

void CellularCallConfig::UpdateImsUtCapabilities(int32_t slotId, bool isGbaValid, ImsCapabilityList &imsCapabilityList)
{
    ImsCapability utCapability;
    utCapability.imsCapabilityType = ImsCapabilityType::CAPABILITY_TYPE_UT;
    utCapability.imsRadioTech = ImsRegTech::IMS_REG_TECH_LTE;
    utCapability.enable = ssOverUtSupported_[slotId] && isGbaValid && IsUtProvisioned(slotId);
    imsCapabilityList.imsCapabilities.push_back(utCapability);
}

bool CellularCallConfig::IsVolteProvisioned(int32_t slotId)
{
    if (volteProvisioningSupported_[slotId]) {
        int32_t volteFeatureValue;
        int32_t result = configRequest_.GetImsFeatureValueRequest(FeatureType::TYPE_VOICE_OVER_LTE, volteFeatureValue);
        if (result != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("get volte feature value failed");
            return false;
        }
        return volteFeatureValue == ImsFeatureIntResult::IMS_FEATURE_INT_VALUE_ENABLED;
    }
    return true;
}

bool CellularCallConfig::IsVonrSupported(int32_t slotId, bool isGbaValid)
{
    if (std::find(nrModeSupportedList_[slotId].begin(), nrModeSupportedList_[slotId].end(),
        CARRIER_NR_AVAILABILITY_SA) == nrModeSupportedList_[slotId].end()) {
        return false;
    }
    return isGbaValid;
}

bool CellularCallConfig::IsUtProvisioned(int32_t slotId)
{
    if (utProvisioningSupported_[slotId]) {
        int32_t utFeatureValue;
        int32_t result = configRequest_.GetImsFeatureValueRequest(FeatureType::TYPE_SS_OVER_UT, utFeatureValue);
        if (result != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("get ut feature value failed");
            return false;
        }
        return utFeatureValue == ImsFeatureIntResult::IMS_FEATURE_INT_VALUE_ENABLED;
    }
    return true;
}

EmergencyCall CellularCallConfig::BuildEmergencyCall(int32_t slotId, const EmergencyInfo &from)
{
    EmergencyCall to = {};
    to.eccNum = from.eccNum;
    to.eccType = EccType(from.category);
    to.simpresent = SimpresentType(from.simpresent);
    to.mcc = from.mcc;
    to.abnormalService = AbnormalServiceType(from.abnormalService);
    return to;
}

bool CellularCallConfig::IsNeedTurnOnIms(const ImsCapabilityList &imsCapabilityList)
{
    for (auto imsCapabilitie : imsCapabilityList.imsCapabilities) {
        if (imsCapabilitie.imsCapabilityType == ImsCapabilityType::CAPABILITY_TYPE_VOICE
            || imsCapabilitie.imsCapabilityType == ImsCapabilityType::CAPABILITY_TYPE_VIDEO) {
            if (imsCapabilitie.enable) {
                return true;
            }
        }
    }
    return false;
}

bool CellularCallConfig::IsSimChanged(int32_t slotId, std::string iccid)
{
    const int32_t sysparaSize = SYSTEM_PARAMETER_LENGTH;
    char lastIccid[sysparaSize] = { 0 };
    std::string key = LAST_ICCID_KEY + std::to_string(slotId);
    GetParameter(key.c_str(), "", lastIccid, sysparaSize);

    if (iccid.compare(lastIccid) != 0) {
        SetParameter(key.c_str(), iccid.c_str());
        return true;
    }
    return false;
}

bool CellularCallConfig::ChangeImsSwitchWithOperatorConfig(int32_t slotId, bool active)
{
    auto itorHide = hideImsSwitch_.find(slotId);
    if (itorHide != hideImsSwitch_.end()) {
        if (itorHide->second) {
            auto itorSwitch = imsSwitchOnByDefault_.find(slotId);
            if (itorSwitch != imsSwitchOnByDefault_.end()) {
                active = imsSwitchOnByDefault_[slotId];
                return active;
            }
        }
    }
    TELEPHONY_LOGE("do not find hideImsSwitch or imsSwitchOnByDefault config");
    return active;
}

int32_t CellularCallConfig::SaveImsSwitch(int32_t slotId, int32_t imsSwitchValue)
{
    int32_t lastImsSwitchValue;
    int32_t queryRet = CoreManagerInner::GetInstance().QueryImsSwitch(slotId, lastImsSwitchValue);
    if (queryRet != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("query ims switch failed");
        return SAVE_IMS_SWITCH_FAILED;
    }

    if (imsSwitchValue == lastImsSwitchValue) {
        TELEPHONY_LOGI("ims switch status do not change, imsSwitchValue: %{public}d", imsSwitchValue);
        return SAVE_IMS_SWITCH_SUCCESS_NOT_CHANGED;
    }
    int32_t saveRet = CoreManagerInner::GetInstance().SaveImsSwitch(slotId, imsSwitchValue);
    if (saveRet != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("save ims switch failed!");
        return SAVE_IMS_SWITCH_FAILED;
    }
    return SAVE_IMS_SWITCH_SUCCESS_CHANGED;
}

void CellularCallConfig::HandleSetLteImsSwitchResult(int32_t slotId, HRilErrType result)
{
    TELEPHONY_LOGI("CellularCallConfig::HandleSetLteImsSwitchResult entry, slotId: %{public}d", slotId);
    if (result != HRilErrType::NONE) {
        TELEPHONY_LOGE("HandleSetLteImsSwitchResult set ims switch to modem failed!");
        // need to reset the Ims Switch parameter and notify APP to update UI.
    }
}

void CellularCallConfig::GetDomainPreferenceModeResponse(int32_t slotId, int32_t mode)
{
    modeMap_[slotId] = mode;
}

void CellularCallConfig::GetImsSwitchStatusResponse(int32_t slotId, int32_t active) {}

int32_t CellularCallConfig::GetPreferenceMode(int32_t slotId) const
{
    return modeMap_[slotId];
}

int32_t CellularCallConfig::GetSwitchStatus(int32_t slotId) const
{
    TELEPHONY_LOGI("CellularCallConfig::GetSwitchStatus entry, slotId: %{public}d", slotId);
    int32_t imsSwitchStatus;
    int32_t ret = CoreManagerInner::GetInstance().QueryImsSwitch(slotId, imsSwitchStatus);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("get ims switch failed");
        return imsSwitchOnByDefault_[slotId] ? IMS_SWITCH_STATUS_ON : IMS_SWITCH_STATUS_OFF;
    }
    TELEPHONY_LOGI("imsSwitchStatus : %{public}d", imsSwitchStatus);
    return imsSwitchStatus;
}

int32_t CellularCallConfig::SetImsConfig(ImsConfigItem item, const std::string &value)
{
    return configRequest_.SetImsConfigRequest(item, value);
}

int32_t CellularCallConfig::SetImsConfig(ImsConfigItem item, int32_t value)
{
    return configRequest_.SetImsConfigRequest(item, value);
}

int32_t CellularCallConfig::GetImsConfig(ImsConfigItem item)
{
    return configRequest_.GetImsConfigRequest(item);
}

int32_t CellularCallConfig::SetImsFeatureValue(FeatureType type, int32_t value)
{
    return configRequest_.SetImsFeatureValueRequest(type, value);
}

int32_t CellularCallConfig::GetImsFeatureValue(FeatureType type)
{
    int32_t imsFeatureValue;
    int32_t ret = configRequest_.GetImsFeatureValueRequest(type, imsFeatureValue);
    GetImsFeatureValueResponse response;
    response.result = ret;
    response.value = imsFeatureValue;
    DelayedSingleton<CellularCallRegister>::GetInstance()->ReportGetImsFeatureResult(response);
    return ret;
}

int32_t CellularCallConfig::CtrlCamera(const std::u16string &cameraId, int32_t callingUid, int32_t callingPid)
{
    return configRequest_.CtrlCameraRequest(cameraId, callingUid, callingPid);
}

int32_t CellularCallConfig::SetPreviewWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height)
{
    return configRequest_.SetPreviewWindowRequest(x, y, z, width, height);
}

int32_t CellularCallConfig::SetDisplayWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height)
{
    return configRequest_.SetDisplayWindowRequest(x, y, z, width, height);
}

int32_t CellularCallConfig::SetCameraZoom(float zoomRatio)
{
    return configRequest_.SetCameraZoomRequest(zoomRatio);
}

int32_t CellularCallConfig::SetPauseImage(const std::u16string &path)
{
    return configRequest_.SetPauseImageRequest(path);
}

int32_t CellularCallConfig::SetDeviceDirection(int32_t rotation)
{
    return configRequest_.SetDeviceDirectionRequest(rotation);
}

void CellularCallConfig::SetTempMode(int32_t slotId)
{
    modeMap_[slotId] = modeTempMap_[slotId];
}

void CellularCallConfig::InitModeActive()
{
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    modeMap_[slotId] = DomainPreferenceMode::IMS_PS_VOICE_PREFERRED;
    eccListRadioMap_.clear();
    eccList3gppHasSim_.clear();
    eccList3gppNoSim_.clear();
    eccListConfigMap_.clear();
    allEccList_.clear();
    eccList3gppHasSim_.push_back(BuildDefaultEmergencyCall("112"));
    eccList3gppHasSim_.push_back(BuildDefaultEmergencyCall("991"));
    eccList3gppNoSim_.push_back(BuildDefaultEmergencyCall("112"));
    eccList3gppNoSim_.push_back(BuildDefaultEmergencyCall("991"));
    eccList3gppNoSim_.push_back(BuildDefaultEmergencyCall("000"));
    eccList3gppNoSim_.push_back(BuildDefaultEmergencyCall("08"));
    eccList3gppNoSim_.push_back(BuildDefaultEmergencyCall("110"));
    eccList3gppNoSim_.push_back(BuildDefaultEmergencyCall("118"));
    eccList3gppNoSim_.push_back(BuildDefaultEmergencyCall("119"));
    eccList3gppNoSim_.push_back(BuildDefaultEmergencyCall("999"));
    TELEPHONY_LOGI("InitModeActive finish");
}

EmergencyCall CellularCallConfig::BuildDefaultEmergencyCall(const std::string &number)
{
    EmergencyCall emergencyCall;
    emergencyCall.eccNum = number;
    emergencyCall.eccType = EccType::TYPE_CATEGORY;
    emergencyCall.simpresent = SimpresentType::TYPE_HAS_CARD;
    emergencyCall.mcc = "";
    emergencyCall.abnormalService = AbnormalServiceType::TYPE_ALL;
    return emergencyCall;
}

void CellularCallConfig::UpdateEccWhenOperatorConfigChange(int32_t slotId, OperatorConfig &opc)
{
    eccListConfigMap_[slotId].clear();
    std::vector<EmergencyCall> configVector;
    std::string mcc = GetMcc(slotId);
    if (mcc.empty()) {
        TELEPHONY_LOGE("MergeEccCallList  countryCode is null");
        return;
    }
    std::vector<std::string> callList = opc.stringArrayValue[KEY_EMERGENCY_CALL_STRING_ARRAY];
    for (auto call : callList) {
        auto emergencyCall = BuildDefaultEmergencyCall(call);
        emergencyCall.mcc = mcc;
        configVector.push_back(emergencyCall);
    }
    eccListConfigMap_[slotId] = configVector;
    TELEPHONY_LOGI("UpdateEccWhenOperatorConfigChange slotId %{public}d config call size %{public}zu  success", slotId,
        callList.size());
    MergeEccCallList(slotId);
    SetEmergencyCallList(slotId);
}

void CellularCallConfig::MergeEccCallList(int32_t slotId_)
{
    std::lock_guard<std::mutex> lock(mutex_);
    allEccList_[slotId_].clear();
    for (auto ecc : eccListRadioMap_[slotId_]) {
        allEccList_[slotId_].push_back(ecc);
    }
    TELEPHONY_LOGI("MergeEccCallList merge radio slotId  %{public}d size  %{public}d", slotId_,
        static_cast<int32_t>(eccListRadioMap_[slotId_].size()));
    for (auto ecc : eccListConfigMap_[slotId_]) {
        allEccList_[slotId_].push_back(ecc);
    }
    TELEPHONY_LOGI("MergeEccCallList merge config slotId  %{public}d size  %{public}zu", slotId_,
        eccListConfigMap_[slotId_].size());
    SimState simState = SimState::SIM_STATE_UNKNOWN;
    CoreManagerInner::GetInstance().GetSimState(slotId_, simState);
    if (simState != SimState::SIM_STATE_NOT_PRESENT) {
        std::string mcc = GetMcc(slotId_);
        if (mcc.empty()) {
            TELEPHONY_LOGE("MergeEccCallList  countryCode is null");
            return;
        }
        for (auto ecc : eccList3gppHasSim_) {
            ecc.mcc = mcc;
            allEccList_[slotId_].push_back(ecc);
        }
    } else {
        for (auto ecc : eccList3gppNoSim_) {
            allEccList_[slotId_].push_back(ecc);
        }
    }
    UniqueEccCallList(slotId_);
}

void CellularCallConfig::UniqueEccCallList(int32_t slotId_)
{
    std::vector<EmergencyCall> uniques;
    for (auto call : allEccList_[slotId_]) {
        if (std::find(uniques.begin(), uniques.end(), call) == uniques.end()) {
            uniques.push_back(call);
        }
    }
    TELEPHONY_LOGI("UniqueEccCallList end slotId  %{public}d from size %{public}zu to size %{public}zu", slotId_,
        allEccList_[slotId_].size(), uniques.size());
}

std::string CellularCallConfig::GetMcc(int32_t slotId_)
{
    std::u16string operatorNumeric;
    CoreManagerInner::GetInstance().GetSimOperatorNumeric(slotId_, operatorNumeric);
    std::string imsi = Str16ToStr8(operatorNumeric);
    int len = static_cast<int>(imsi.length());
    std::string mcc = imsi;
    if (len >= MCC_LEN) {
        mcc = imsi.substr(0, MCC_LEN);
    }
    TELEPHONY_LOGI("getMcc slotd %{public}d mcc %{public}s end", slotId_, mcc.c_str());
    return mcc;
}

int32_t CellularCallConfig::SetEmergencyCallList(int32_t slotId)
{
    TELEPHONY_LOGI("SetEmergencyCallList start");
    return SetEmergencyCallList(slotId, allEccList_[slotId]);
}

int32_t CellularCallConfig::SetMute(int32_t slotId, int32_t mute)
{
    return configRequest_.SetMuteRequest(slotId, mute);
}

int32_t CellularCallConfig::GetMute(int32_t slotId)
{
    return configRequest_.GetMuteRequest(slotId);
}

int32_t CellularCallConfig::GetEmergencyCallList(int32_t slotId)
{
    return configRequest_.GetEmergencyCallListRequest(slotId);
}

int32_t CellularCallConfig::SetEmergencyCallList(int32_t slotId, const std::vector<EmergencyCall> &eccVec)
{
    TELEPHONY_LOGI("SetEmergencyCallList start %{public}d", slotId);
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<EmergencyCall> uniques;
    for (auto call : eccVec) {
        if (std::find(eccListRadioMap_[slotId].begin(), eccListRadioMap_[slotId].end(), call) ==
            eccListRadioMap_[slotId].end()) {
            uniques.push_back(call);
        }
    }
    TELEPHONY_LOGI("select end slotId  %{public}d from size %{public}zu to size %{public}zu", slotId, eccVec.size(),
        uniques.size());
    if (uniques.size() <= 0) {
        TELEPHONY_LOGI("SetEmergencyCallList eccList has exit");
        return TELEPHONY_ERR_SUCCESS;
    }
    TELEPHONY_LOGI("SetEmergencyCallList  refresh size %{public}zu", uniques.size());
    for (auto ecc : uniques) {
        allEccList_[slotId].push_back(ecc);
    }
    return configRequest_.SetEmergencyCallListRequest(slotId, uniques);
}

bool CellularCallConfig::IsNeedUpdateEccListWhenSimStateChanged(int32_t slotId)
{
    SimState simState = SimState::SIM_STATE_UNKNOWN;
    CoreManagerInner::GetInstance().GetSimState(slotId, simState);
    int32_t simStateForEcc;
    switch (simState) {
        case SimState::SIM_STATE_READY:
        case SimState::SIM_STATE_LOADED: {
            simStateForEcc = SIM_PRESENT;
            break;
        }
        default: {
            simStateForEcc = SIM_ABSENT;
            break;
        }
    }
    bool result = (simState_[slotId] != simStateForEcc);
    simState_[slotId] = simStateForEcc;
    return result;
}

void CellularCallConfig::UpdateEmergencyCallFromRadio(int32_t slotId, const EmergencyInfoList &eccList)
{
    TELEPHONY_LOGI("UpdateEmergencyCallFromRadio %{publid}d size %{public}d", slotId, eccList.callSize);
    eccListRadioMap_[slotId] = std::vector<EmergencyCall>();
    for (auto ecc : eccList.calls) {
        TELEPHONY_LOGI("UpdateEmergencyCallFromRadio , data: eccNum %{public}s mcc %{public}s", ecc.eccNum.c_str(),
            ecc.mcc.c_str());
        eccListRadioMap_[slotId].push_back(BuildEmergencyCall(slotId, ecc));
    }
    MergeEccCallList(slotId);
}

std::vector<EmergencyCall> CellularCallConfig::GetEccCallList(int32_t slotId)
{
    TELEPHONY_LOGI("GetEccCallList  start %{public}d", slotId);
    std::lock_guard<std::mutex> lock(mutex_);
    TELEPHONY_LOGI("GetEccCallList size %{public}zu", allEccList_[slotId].size());
    for (auto ecc : allEccList_[slotId]) {
        TELEPHONY_LOGI("GetEccCallList, data: eccNum %{public}s mcc %{public}s", ecc.eccNum.c_str(), ecc.mcc.c_str());
    }
    return allEccList_[slotId];
}

int32_t CellularCallConfig::BooleanToImsSwitchValue(bool value)
{
    return value ? IMS_SWITCH_VALUE_ENABLED : IMS_SWITCH_VALUE_DISABLED;
}

bool CellularCallConfig::GetImsSwitchOnByDefaultConfig(int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slot id");
        return true;
    }
    return imsSwitchOnByDefault_[slotId];
}

bool CellularCallConfig::GethideImsSwitchConfig(int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slot id");
        return false;
    }
    return hideImsSwitch_[slotId];
}

bool CellularCallConfig::GetvolteSupportedConfig(int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slot id");
        return false;
    }
    return volteSupported_[slotId];
}

std::vector<int32_t> CellularCallConfig::GetNrModeSupportedListConfig(int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slot id");
        return std::vector<int32_t> { CARRIER_NR_AVAILABILITY_NSA, CARRIER_NR_AVAILABILITY_SA };
    }
    return nrModeSupportedList_[slotId];
}

bool CellularCallConfig::GetVolteProvisioningSupportedConfig(int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slot id");
        return false;
    }
    return volteProvisioningSupported_[slotId];
}

bool CellularCallConfig::GetSsOverUtSupportedConfig(int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slot id");
        return false;
    }
    return ssOverUtSupported_[slotId];
}

bool CellularCallConfig::GetImsGbaRequiredConfig(int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slot id");
        return false;
    }
    return imsGbaRequired_[slotId];
}

bool CellularCallConfig::GetUtProvisioningSupportedConfig(int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slot id");
        return false;
    }
    return utProvisioningSupported_[slotId];
}

bool CellularCallConfig::GetImsPreferForEmergencyConfig(int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slot id");
        return true;
    }
    return imsPreferForEmergency_[slotId];
}

std::int32_t CellularCallConfig::GetCallWaitingServiceClassConfig(int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slot id");
        return 1;
    }
    return callWaitingServiceClass_[slotId];
}

std::vector<std::string> CellularCallConfig::GetImsCallDisconnectResoninfoMappingConfig(int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slot id");
        return std::vector<std::string> {};
    }
    return imsCallDisconnectResoninfoMapping_[slotId];
}

bool CellularCallConfig::GetForceVolteSwitchOnConfig(int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slot id");
        return false;
    }
    return forceVolteSwitchOn_[slotId];
}

bool CellularCallConfig::IsValidSlotId(int32_t slotId)
{
    int32_t count = SIM_SLOT_COUNT;
    if ((slotId >= DEFAULT_SIM_SLOT_ID) && (slotId < count)) {
        return true;
    }

    TELEPHONY_LOGE("SlotId is InValid = %{public}d", slotId);
    return false;
}
} // namespace Telephony
} // namespace OHOS
