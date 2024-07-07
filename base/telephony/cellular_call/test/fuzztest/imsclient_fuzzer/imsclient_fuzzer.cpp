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

#include "imsclient_fuzzer.h"

#include <cstddef>
#include <cstdint>
#define private public
#include "addcellularcalltoken_fuzzer.h"
#include "cellular_call_service.h"
#include "ims_call_client.h"
#include "ims_call_proxy.h"
#include "securec.h"
#include "system_ability_definition.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
constexpr int32_t SERVICE_STATE_RUNNING = 1;

bool IsServiceInited()
{
    auto service = DelayedSingleton<CellularCallService>::GetInstance();
    if (service == nullptr) {
        return g_isInited;
    }
    if (service->GetServiceRunningState() != SERVICE_STATE_RUNNING) {
        service->OnStart();
    }
    if (!g_isInited && service->GetServiceRunningState() == SERVICE_STATE_RUNNING) {
        g_isInited = true;
    }
    return g_isInited;
}

void TestImsCallClientWithCallInfo(
    const uint8_t *data, size_t size, const std::shared_ptr<ImsCallClient> &imsCallClient)
{
    ImsCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(struct ImsCallInfo), 0x00, sizeof(struct ImsCallInfo)) != EOK) {
        return;
    }
    size_t length = std::min(static_cast<size_t>(sizeof(callInfo.phoneNum) - 1), size);
    std::string number(reinterpret_cast<const char *>(data), length);
    int32_t mode = static_cast<int32_t>(size % 2);
    int32_t slotId = static_cast<int32_t>(size % 2);
    int32_t videoState = static_cast<int32_t>(size % 3);
    int32_t index = static_cast<int32_t>(size % 3);
    const char *cDtmfCode = number.c_str();
    if (strcpy_s(callInfo.phoneNum, sizeof(callInfo.phoneNum), number.c_str()) != EOK) {
        return;
    }
    callInfo.slotId = slotId;
    callInfo.videoState = videoState;
    callInfo.index = index;
    std::vector<std::string> numberList;
    numberList.push_back(number);
    imsCallClient->Dial(callInfo, static_cast<CLIRMode>(mode));
    imsCallClient->HangUp(callInfo);
    imsCallClient->Answer(callInfo);
    imsCallClient->Reject(callInfo);
    imsCallClient->RejectWithReason(callInfo, ImsRejectReason::USER_DECLINE);
    imsCallClient->UpdateImsCallMode(callInfo, static_cast<ImsCallMode>(mode));
    imsCallClient->InviteToConference(slotId, numberList);
    imsCallClient->KickOutFromConference(slotId, numberList);
    imsCallClient->StartDtmf(slotId, *cDtmfCode, index);
    imsCallClient->SendDtmf(slotId, *cDtmfCode, index);
    imsCallClient->StopDtmf(slotId, index);
    imsCallClient->StartRtt(slotId, number);
    imsCallClient->StopRtt(slotId);
    imsCallClient->SetDomainPreferenceMode(slotId, mode);
    imsCallClient->GetDomainPreferenceMode(slotId);
    imsCallClient->SetImsSwitchStatus(slotId, mode);
    imsCallClient->GetImsSwitchStatus(slotId);
    imsCallClient->SetMute(slotId, mode);
    imsCallClient->GetMute(slotId);
}

void TestImsCallClientWithSlotAndType(
    const uint8_t *data, size_t size, const std::shared_ptr<ImsCallClient> &imsCallClient)
{
    int32_t slotId = static_cast<int32_t>(size % 2);
    int32_t callType = static_cast<int32_t>(size % 2);
    std::string info(reinterpret_cast<const char *>(data), size);
    int32_t x = static_cast<int32_t>(size % 3);
    int32_t y = static_cast<int32_t>(size % 3);
    int32_t z = static_cast<int32_t>(size % 4);
    int32_t width = static_cast<int32_t>(size % 4);
    int32_t height = static_cast<int32_t>(size % 5);
    int32_t callingUid = static_cast<int32_t>(size % 5);
    int32_t callingPid = static_cast<int32_t>(size % 6);
    int32_t mode = static_cast<int32_t>(size % 6);
    imsCallClient->HoldCall(slotId, callType);
    imsCallClient->UnHoldCall(slotId, callType);
    imsCallClient->SwitchCall(slotId, callType);
    imsCallClient->CombineConference(slotId);
    imsCallClient->GetImsCallsDataRequest(slotId, callType);
    imsCallClient->GetLastCallFailReason(slotId);
    imsCallClient->CtrlCamera(Str8ToStr16(info), callingUid, callingPid);
    imsCallClient->SetPreviewWindow(x, y, z, width, height);
    imsCallClient->SetDisplayWindow(x, y, z, width, height);
    imsCallClient->SetPauseImage(Str8ToStr16(info));
    imsCallClient->SetDeviceDirection(mode);
    imsCallClient->SetCameraZoom(mode);
    ImsCapabilityList imsCapabilityList;
    ImsCapability capbility;
    capbility.enable = mode;
    capbility.imsCapabilityType = static_cast<ImsCapabilityType>(callType);
    capbility.imsRadioTech = static_cast<ImsRegTech>(callType);
    imsCapabilityList.imsCapabilities.push_back(capbility);
    imsCallClient->UpdateImsCapabilities(slotId, imsCapabilityList);
}

void TestImsCallClientWithSettingFunction(
    const uint8_t *data, size_t size, const std::shared_ptr<ImsCallClient> &imsCallClient)
{
    CallTransferInfo transferInfo;
    if (memset_s(&transferInfo, sizeof(struct CallTransferInfo), 0x00, sizeof(struct CallTransferInfo)) != EOK) {
        return;
    }
    size_t length = std::min(static_cast<size_t>(sizeof(transferInfo.transferNum) - 1), size);
    std::string number(reinterpret_cast<const char *>(data), length);
    int32_t mode = static_cast<int32_t>(size % 2);
    int32_t slotId = static_cast<int32_t>(size % 2);
    int32_t item = static_cast<int32_t>(size % 3);
    int32_t value = static_cast<int32_t>(size % 4);
    int32_t type = static_cast<int32_t>(size % 4);
    imsCallClient->SetImsConfig(static_cast<ImsConfigItem>(item), number);
    imsCallClient->SetImsConfig(static_cast<ImsConfigItem>(item), value);
    imsCallClient->GetImsConfig(static_cast<ImsConfigItem>(item));
    imsCallClient->SetImsFeatureValue(static_cast<FeatureType>(type), value);
    imsCallClient->GetImsFeatureValue(static_cast<FeatureType>(type), value);
    imsCallClient->SetClip(slotId, mode);
    imsCallClient->GetClip(slotId);
    imsCallClient->SetClir(slotId, mode);
    imsCallClient->GetClir(slotId);
    imsCallClient->SetCallWaiting(slotId, mode, type);
    imsCallClient->GetCallWaiting(slotId);
    imsCallClient->SetColr(slotId, mode);
    imsCallClient->GetColr(slotId);
    imsCallClient->SetColp(slotId, mode);
    imsCallClient->GetColp(slotId);
    if (strcpy_s(transferInfo.transferNum, sizeof(transferInfo.transferNum), number.c_str()) != EOK) {
        return;
    }
    transferInfo.settingType = static_cast<CallTransferSettingType>(type);
    transferInfo.type = static_cast<CallTransferType>(type);
    imsCallClient->SetCallTransfer(slotId, type, mode, number, type);
    imsCallClient->GetCallTransfer(slotId, type);
    imsCallClient->SetCallRestriction(slotId, number, mode, number);
    imsCallClient->GetCallRestriction(slotId, number);
}

void TestImsCallProxyWithCallInfo(const uint8_t *data, size_t size, const sptr<ImsCallInterface> &proxy)
{
    ImsCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(struct ImsCallInfo), 0x00, sizeof(struct ImsCallInfo)) != EOK) {
        return;
    }
    size_t length = std::min(static_cast<size_t>(sizeof(callInfo.phoneNum) - 1), size);
    std::string number(reinterpret_cast<const char *>(data), length);
    int32_t mode = static_cast<int32_t>(size % 2);
    int32_t slotId = static_cast<int32_t>(size % 2);
    int32_t videoState = static_cast<int32_t>(size % 3);
    int32_t index = static_cast<int32_t>(size % 3);
    const char *cDtmfCode = number.c_str();
    if (strcpy_s(callInfo.phoneNum, sizeof(callInfo.phoneNum), number.c_str()) != EOK) {
        return;
    }
    callInfo.slotId = slotId;
    callInfo.videoState = videoState;
    callInfo.index = index;
    std::vector<std::string> numberList;
    numberList.push_back(number);
    proxy->Dial(callInfo, static_cast<CLIRMode>(mode));
    proxy->HangUp(callInfo);
    proxy->Answer(callInfo);
    proxy->RejectWithReason(callInfo, ImsRejectReason::USER_DECLINE);
    proxy->UpdateImsCallMode(callInfo, static_cast<ImsCallMode>(mode));
    proxy->InviteToConference(slotId, numberList);
    proxy->KickOutFromConference(slotId, numberList);
    proxy->StartDtmf(slotId, *cDtmfCode, index);
    proxy->SendDtmf(slotId, *cDtmfCode, index);
    proxy->StopDtmf(slotId, index);
    proxy->StartRtt(slotId, number);
    proxy->StopRtt(slotId);
    proxy->SetDomainPreferenceMode(slotId, mode);
    proxy->GetDomainPreferenceMode(slotId);
    proxy->SetImsSwitchStatus(slotId, mode);
    proxy->GetImsSwitchStatus(slotId);
    proxy->SetMute(slotId, mode);
    proxy->GetMute(slotId);
}

void TestImsCallProxyWithSlotAndType(const uint8_t *data, size_t size, const sptr<ImsCallInterface> &proxy)
{
    int32_t slotId = static_cast<int32_t>(size % 2);
    int32_t callType = static_cast<int32_t>(size % 2);
    std::string info(reinterpret_cast<const char *>(data), size);
    int32_t x = static_cast<int32_t>(size % 3);
    int32_t y = static_cast<int32_t>(size % 3);
    int32_t z = static_cast<int32_t>(size % 4);
    int32_t width = static_cast<int32_t>(size % 4);
    int32_t height = static_cast<int32_t>(size % 5);
    int32_t callingUid = static_cast<int32_t>(size % 5);
    int32_t callingPid = static_cast<int32_t>(size % 6);
    int32_t mode = static_cast<int32_t>(size % 6);
    proxy->HoldCall(slotId, callType);
    proxy->UnHoldCall(slotId, callType);
    proxy->SwitchCall(slotId, callType);
    proxy->CombineConference(slotId);
    proxy->GetImsCallsDataRequest(slotId, callType);
    proxy->GetLastCallFailReason(slotId);
    proxy->CtrlCamera(Str8ToStr16(info), callingUid, callingPid);
    proxy->SetPreviewWindow(x, y, z, width, height);
    proxy->SetDisplayWindow(x, y, z, width, height);
    proxy->SetPauseImage(Str8ToStr16(info));
    proxy->SetDeviceDirection(mode);
    proxy->SetCameraZoom(mode);
    ImsCapabilityList imsCapabilityList;
    ImsCapability capbility;
    capbility.enable = mode;
    capbility.imsCapabilityType = static_cast<ImsCapabilityType>(callType);
    capbility.imsRadioTech = static_cast<ImsRegTech>(callType);
    imsCapabilityList.imsCapabilities.push_back(capbility);
    proxy->UpdateImsCapabilities(slotId, imsCapabilityList);
}

void TestImsCallProxyWithSettingFunction(const uint8_t *data, size_t size, const sptr<ImsCallInterface> &proxy)
{
    CallTransferInfo transferInfo;
    if (memset_s(&transferInfo, sizeof(struct CallTransferInfo), 0x00, sizeof(struct CallTransferInfo)) != EOK) {
        return;
    }
    size_t length = std::min(static_cast<size_t>(sizeof(transferInfo.transferNum) - 1), size);
    std::string number(reinterpret_cast<const char *>(data), length);
    int32_t mode = static_cast<int32_t>(size % 2);
    int32_t slotId = static_cast<int32_t>(size % 2);
    int32_t item = static_cast<int32_t>(size % 3);
    int32_t value = static_cast<int32_t>(size % 4);
    int32_t type = static_cast<int32_t>(size % 4);
    proxy->SetImsConfig(static_cast<ImsConfigItem>(item), number);
    proxy->SetImsConfig(static_cast<ImsConfigItem>(item), value);
    proxy->GetImsConfig(static_cast<ImsConfigItem>(item));
    proxy->SetImsFeatureValue(static_cast<FeatureType>(type), value);
    proxy->GetImsFeatureValue(static_cast<FeatureType>(type), value);
    proxy->SetClip(slotId, mode);
    proxy->GetClip(slotId);
    proxy->SetClir(slotId, mode);
    proxy->GetClir(slotId);
    proxy->SetCallWaiting(slotId, mode, type);
    proxy->GetCallWaiting(slotId);
    proxy->SetColr(slotId, mode);
    proxy->GetColr(slotId);
    proxy->SetColp(slotId, mode);
    proxy->GetColp(slotId);
    if (strcpy_s(transferInfo.transferNum, sizeof(transferInfo.transferNum), number.c_str()) != EOK) {
        return;
    }
    transferInfo.settingType = static_cast<CallTransferSettingType>(type);
    transferInfo.type = static_cast<CallTransferType>(type);
    proxy->SetCallTransfer(slotId, type, mode, number, type);
    proxy->GetCallTransfer(slotId, type);
    proxy->SetCallRestriction(slotId, number, mode, number);
    proxy->GetCallRestriction(slotId, number);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    auto imsCallClient = DelayedSingleton<ImsCallClient>::GetInstance();
    if (imsCallClient == nullptr) {
        return;
    }
    if (!IsServiceInited()) {
        return;
    }
    TestImsCallClientWithCallInfo(data, size, imsCallClient);
    TestImsCallClientWithSlotAndType(data, size, imsCallClient);
    TestImsCallClientWithSettingFunction(data, size, imsCallClient);

    auto managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        return;
    }
    auto remoteObjectPtr = managerPtr->CheckSystemAbility(TELEPHONY_IMS_SYS_ABILITY_ID);
    if (remoteObjectPtr == nullptr) {
        return;
    }
    sptr<ImsCallInterface> proxy = iface_cast<ImsCallInterface>(remoteObjectPtr);
    if (proxy == nullptr) {
        return;
    }
    TestImsCallProxyWithCallInfo(data, size, proxy);
    TestImsCallProxyWithSlotAndType(data, size, proxy);
    TestImsCallProxyWithSettingFunction(data, size, proxy);
    proxy.clear();
    proxy = nullptr;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::AddCellularCallTokenFuzzer token;
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
