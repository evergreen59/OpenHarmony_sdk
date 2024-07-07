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

#include "core_service_proxy.h"

#include "network_search_types.h"
#include "parameter.h"
#include "sim_state_type.h"
#include "string_ex.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "telephony_types.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t MAX_SIZE = 1000;
bool CoreServiceProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(CoreServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write interface token failed");
        return false;
    }
    return true;
}

int32_t CoreServiceProxy::GetPsRadioTech(int32_t slotId, int32_t &psRadioTech)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetPsRadioTech WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetPsRadioTech Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_PS_RADIO_TECH), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetPsRadioTech failed, error code is %{public}d ", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        psRadioTech = reply.ReadInt32();
    }
    return result;
}

int32_t CoreServiceProxy::GetCsRadioTech(int32_t slotId, int32_t &csRadioTech)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetCsRadioTech WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetCsRadioTech Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_CS_RADIO_TECH), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetCsRadioTech failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    TELEPHONY_LOGI("GetCsRadioTech call end: result=%{public}d \n", result);
    if (result == TELEPHONY_ERR_SUCCESS) {
        csRadioTech = reply.ReadInt32();
    }
    return result;
}

std::u16string CoreServiceProxy::GetOperatorNumeric(int32_t slotId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetOperatorNumeric WriteInterfaceToken is false");
        return Str8ToStr16("");
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetOperatorNumeric Remote is null");
        return Str8ToStr16("");
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_OPERATOR_NUMERIC), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetCsRadioTech failed, error code is %{public}d \n", st);
        return Str8ToStr16("");
    }
    std::u16string result = reply.ReadString16();
    std::string str = Str16ToStr8(result);
    TELEPHONY_LOGI("CoreServiceProxy GetOperatorNumeric success");
    return result;
}

int32_t CoreServiceProxy::GetOperatorName(int32_t slotId, std::u16string &operatorName)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_OPERATOR_NAME), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGI("GetOperatorName failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        operatorName = reply.ReadString16();
    }
    TELEPHONY_LOGI("GetOperatorName call end");
    return result;
}

int32_t CoreServiceProxy::GetNetworkSearchInformation(int32_t slotId, const sptr<INetworkSearchCallback> &callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (callback != nullptr) {
        data.WriteRemoteObject(callback->AsObject().GetRefPtr());
    }
    auto remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(uint32_t(InterfaceID::GET_NETWORK_SEARCH_RESULT), data, reply, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("GetNetworkSearchInformation failed, error code is: %{public}d \n", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return reply.ReadInt32();
}

int32_t CoreServiceProxy::GetNetworkState(int32_t slotId, sptr<NetworkState> &networkState)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetNetworkState WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetNetworkState Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_NETWORK_STATE), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetNetworkState failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        networkState = NetworkState::Unmarshalling(reply);
    }
    return result;
}

int32_t CoreServiceProxy::GetSignalInfoList(int32_t slotId, std::vector<sptr<SignalInformation>> &signals)
{
    TELEPHONY_LOGI("CoreServiceProxy::GetSignalInfoList slotId : %{public}d", slotId);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetSignalInfoList WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSignalInfoList Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_SIGNAL_INFO_LIST), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetSignalInfoList failed, error code is %{public}d\n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        ProcessSignalInfo(reply, signals);
    }
    return result;
}

static void ProcessSignalInfoGsm(MessageParcel &reply, std::vector<sptr<SignalInformation>> &result)
{
    std::unique_ptr<GsmSignalInformation> signal = std::make_unique<GsmSignalInformation>();
    if (signal != nullptr) {
        signal->ReadFromParcel(reply);
        result.emplace_back(signal.release());
    }
}

static void ProcessSignalInfoCdma(MessageParcel &reply, std::vector<sptr<SignalInformation>> &result)
{
    std::unique_ptr<CdmaSignalInformation> signal = std::make_unique<CdmaSignalInformation>();
    if (signal != nullptr) {
        signal->ReadFromParcel(reply);
        result.emplace_back(signal.release());
    }
}

static void ProcessSignalInfoLte(MessageParcel &reply, std::vector<sptr<SignalInformation>> &result)
{
    std::unique_ptr<LteSignalInformation> signal = std::make_unique<LteSignalInformation>();
    if (signal != nullptr) {
        signal->ReadFromParcel(reply);
        result.emplace_back(signal.release());
    }
}

static void ProcessSignalInfoWcdma(MessageParcel &reply, std::vector<sptr<SignalInformation>> &result)
{
    std::unique_ptr<WcdmaSignalInformation> signal = std::make_unique<WcdmaSignalInformation>();
    if (signal != nullptr) {
        signal->ReadFromParcel(reply);
        result.emplace_back(signal.release());
    }
}

static void ProcessSignalInfoTdscdma(MessageParcel &reply, std::vector<sptr<SignalInformation>> &result)
{
    std::unique_ptr<TdScdmaSignalInformation> signal = std::make_unique<TdScdmaSignalInformation>();
    if (signal != nullptr) {
        signal->ReadFromParcel(reply);
        result.emplace_back(signal.release());
    }
}

static void ProcessSignalInfoNr(MessageParcel &reply, std::vector<sptr<SignalInformation>> &result)
{
    std::unique_ptr<NrSignalInformation> signal = std::make_unique<NrSignalInformation>();
    if (signal != nullptr) {
        signal->ReadFromParcel(reply);
        result.emplace_back(signal.release());
    }
}

void CoreServiceProxy::ProcessSignalInfo(MessageParcel &reply, std::vector<sptr<SignalInformation>> &result)
{
    int32_t size = reply.ReadInt32();
    TELEPHONY_LOGI("CoreServiceProxy::GetSignalInfoList size:%{public}d\n", size);
    if (size >= MAX_SIZE) {
        TELEPHONY_LOGE("CoreServiceProxy::GetSignalInfoList over max size");
        return;
    }
    SignalInformation::NetworkType type;
    for (int i = 0; i < size; ++i) {
        type = static_cast<SignalInformation::NetworkType>(reply.ReadInt32());
        switch (type) {
            case SignalInformation::NetworkType::GSM: {
                ProcessSignalInfoGsm(reply, result);
                break;
            }
            case SignalInformation::NetworkType::CDMA: {
                ProcessSignalInfoCdma(reply, result);
                break;
            }
            case SignalInformation::NetworkType::LTE: {
                ProcessSignalInfoLte(reply, result);
                break;
            }
            case SignalInformation::NetworkType::WCDMA: {
                ProcessSignalInfoWcdma(reply, result);
                break;
            }
            case SignalInformation::NetworkType::TDSCDMA: {
                ProcessSignalInfoTdscdma(reply, result);
                break;
            }
            case SignalInformation::NetworkType::NR: {
                ProcessSignalInfoNr(reply, result);
                break;
            }
            default:
                break;
        }
    }
}

int32_t CoreServiceProxy::SetRadioState(int32_t slotId, bool isOn, const sptr<INetworkSearchCallback> &callback)
{
    TELEPHONY_LOGI("CoreServiceProxy SetRadioState isOn:%{public}d", isOn);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SetRadioState WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    if (callback != nullptr) {
        data.WriteRemoteObject(callback->AsObject().GetRefPtr());
    }
    data.WriteBool(isOn);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SetRadioState Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(uint32_t(InterfaceID::SET_RADIO_STATE), data, reply, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("SetRadioState failed, error code is %{public}d \n", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return reply.ReadInt32();
}

int32_t CoreServiceProxy::GetRadioState(int32_t slotId, const sptr<INetworkSearchCallback> &callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetRadioState WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    if (callback != nullptr) {
        data.WriteRemoteObject(callback->AsObject().GetRefPtr());
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetRadioState Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_RADIO_STATE), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetRadioState failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return reply.ReadInt32();
}

int32_t CoreServiceProxy::GetIsoCountryCodeForNetwork(int32_t slotId, std::u16string &countryCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetIsoCountryCodeForNetwork WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetIsoCountryCodeForNetwork Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_ISO_COUNTRY_CODE_FOR_NETWORK), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetIsoCountryCodeForNetwork failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        countryCode = reply.ReadString16();
    }
    TELEPHONY_LOGI("GetIsoCountryCodeForNetwork call end");
    return result;
}

int32_t CoreServiceProxy::GetImei(int32_t slotId, std::u16string &imei)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetImei WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetImei Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(uint32_t(InterfaceID::GET_IMEI), data, reply, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("GetImei failed, error code is %{public}d \n", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        imei = reply.ReadString16();
    }
    TELEPHONY_LOGI("CoreServiceProxy::GetImei success");
    return result;
}

int32_t CoreServiceProxy::GetMeid(int32_t slotId, std::u16string &meid)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetMeid WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetMeid Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(uint32_t(InterfaceID::GET_MEID), data, reply, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("GetMeid failed, error code is %{public}d \n", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        meid = reply.ReadString16();
    }
    TELEPHONY_LOGI("CoreServiceProxy::GetMeid success");
    return result;
}

int32_t CoreServiceProxy::GetUniqueDeviceId(int32_t slotId, std::u16string &deviceId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetUniqueDeviceId WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetUniqueDeviceId Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(uint32_t(InterfaceID::GET_UNIQUE_DEVICE_ID), data, reply, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("GetUniqueDeviceId failed, error code is %{public}d \n", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        deviceId = reply.ReadString16();
    }
    return result;
}

int32_t CoreServiceProxy::HasSimCard(int32_t slotId, bool &hasSimCard)
{
    TELEPHONY_LOGI("CoreServiceProxy HasSimCard ::%{public}d", slotId);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("HasSimCard WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("HasSimCard Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::HAS_SIM_CARD), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("HasSimCard failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        hasSimCard = reply.ReadBool();
    }
    return result;
}

int32_t CoreServiceProxy::GetSimState(int32_t slotId, SimState &simState)
{
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    TELEPHONY_LOGI("CoreServiceProxy GetSimState ::%{public}d", slotId);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetSimState WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSimState Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_SIM_STATE), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetSimState failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        simState = static_cast<SimState>(reply.ReadInt32());
    }
    TELEPHONY_LOGI("GetSimState call end: result=%{public}d \n", result);
    return result;
}

int32_t CoreServiceProxy::GetCardType(int32_t slotId, CardType &cardType)
{
    TELEPHONY_LOGI("CoreServiceProxy GetCardType ::%{public}d", slotId);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetCardType WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetCardType Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_CARD_TYPE), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetCardType failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        cardType = static_cast<CardType>(reply.ReadInt32());
    }
    TELEPHONY_LOGI("GetCardType call end: result=%{public}d \n", result);
    return result;
}

int32_t CoreServiceProxy::GetISOCountryCodeForSim(int32_t slotId, std::u16string &countryCode)
{
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetISOCountryCodeForSim WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetISOCountryCodeForSim Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_ISO_COUNTRY_CODE), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetISOCountryCodeForSim failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        countryCode = reply.ReadString16();
    }
    TELEPHONY_LOGI("GetISOCountryCodeForSim call end");
    return result;
}

int32_t CoreServiceProxy::GetSimOperatorNumeric(int32_t slotId, std::u16string &operatorNumeric)
{
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetSimOperatorNumeric WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSimOperatorNumeric Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_SIM_OPERATOR_NUMERIC), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetSimOperatorNumeric failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        operatorNumeric = reply.ReadString16();
    }
    TELEPHONY_LOGI("GetSimOperatorNumeric call end");
    return result;
}

int32_t CoreServiceProxy::GetSimSpn(int32_t slotId, std::u16string &spn)
{
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetSimSpn WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSimSpn Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_SPN), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetSimSpn failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        spn = reply.ReadString16();
    }
    TELEPHONY_LOGI("GetSimSpn call end");
    return result;
}

int32_t CoreServiceProxy::GetSimIccId(int32_t slotId, std::u16string &iccId)
{
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetSimIccId WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSimIccId Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_ICCID), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetSimIccId failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        iccId = reply.ReadString16();
    }
    return result;
}

int32_t CoreServiceProxy::GetIMSI(int32_t slotId, std::u16string &imsi)
{
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetIMSI WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetIMSI Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_IMSI), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetIMSI failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        imsi = reply.ReadString16();
    }
    return result;
}

bool CoreServiceProxy::IsSimActive(int32_t slotId)
{
    TELEPHONY_LOGI("CoreServiceProxy IsSimActive ::%{public}d", slotId);
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CoreServiceProxy::IsSimActive invalid simId");
        return false;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("IsSimActive WriteInterfaceToken is false");
        return false;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("IsSimActive Remote is null");
        return false;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::IS_SIM_ACTIVE), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("IsSimActive failed, error code is %{public}d \n", st);
        return false;
    }
    return reply.ReadBool();
}

int32_t CoreServiceProxy::GetSlotId(int32_t simId)
{
    if (simId <= 0) {
        TELEPHONY_LOGE("CoreServiceProxy::GetSlotId invalid simId");
        return ERROR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetSlotId WriteInterfaceToken is false");
        return ERROR;
    }
    data.WriteInt32(simId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSlotId Remote is null");
        return ERROR;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_SIM_SLOTID), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetSlotId failed, error code is %{public}d", st);
        return ERROR;
    }
    return reply.ReadInt32();
}

int32_t CoreServiceProxy::GetSimId(int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CoreServiceProxy::GetSimId invalid slotId");
        return ERROR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetSimId WriteInterfaceToken is false");
        return ERROR;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSimId Remote is null");
        return ERROR;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_SIM_SIMID), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetSimId failed, error code is %{public}d", st);
        return ERROR;
    }
    return reply.ReadInt32();
}

int32_t CoreServiceProxy::GetNetworkSelectionMode(int32_t slotId, const sptr<INetworkSearchCallback> &callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetNetworkSelectionMode WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(slotId)) {
        TELEPHONY_LOGE("GetNetworkSelectionMode WriteInt32 slotId is false");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (callback != nullptr) {
        data.WriteRemoteObject(callback->AsObject().GetRefPtr());
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetNetworkSelectionMode Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_NETWORK_SELECTION_MODE), data, reply, option);
    if (st != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("GetNetworkSelectionMode failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return reply.ReadInt32();
}

int32_t CoreServiceProxy::SetNetworkSelectionMode(int32_t slotId, int32_t selectMode,
    const sptr<NetworkInformation> &networkInformation, bool resumeSelection,
    const sptr<INetworkSearchCallback> &callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SetNetworkSelectionMode WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(slotId)) {
        TELEPHONY_LOGE("SetNetworkSelectionMode WriteInt32 slotId is false");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (!data.WriteInt32(selectMode)) {
        TELEPHONY_LOGE("SetNetworkSelectionMode WriteInt32 selectMode is false");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (!data.WriteBool(resumeSelection)) {
        TELEPHONY_LOGE("SetNetworkSelectionMode WriteBool resumeSelection is false");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (callback != nullptr) {
        data.WriteRemoteObject(callback->AsObject().GetRefPtr());
    }
    if (networkInformation != nullptr) {
        networkInformation->Marshalling(data);
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SetNetworkSelectionMode Remote is null");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    int32_t error = remote->SendRequest(uint32_t(InterfaceID::SET_NETWORK_SELECTION_MODE), data, reply, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("SetNetworkSelectionMode failed, error code is %{public}d \n", error);
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return reply.ReadInt32();
}

std::u16string CoreServiceProxy::GetLocaleFromDefaultSim()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetLocaleFromDefaultSim WriteInterfaceToken is false");
        return Str8ToStr16("");
    }

    data.WriteInt32(DEFAULT_SIM_SLOT_ID);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetLocaleFromDefaultSim Remote is null");
        return Str8ToStr16("");
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_SIM_LANGUAGE), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetLocaleFromDefaultSim failed, error code is %{public}d \n", st);
        return Str8ToStr16("");
    }
    std::u16string result = reply.ReadString16();
    return result;
}

int32_t CoreServiceProxy::GetSimGid1(int32_t slotId, std::u16string &gid1)
{
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetSimGid1 WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSimGid1 Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_SIM_GID1), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetSimGid1 failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        gid1 = reply.ReadString16();
    }
    return result;
}

std::u16string CoreServiceProxy::GetSimGid2(int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        return Str8ToStr16("");
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetSimGid2 WriteInterfaceToken is false");
        return Str8ToStr16("");
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSimGid2 Remote is null");
        return Str8ToStr16("");
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_SIM_GID2), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetSimGid2 failed, error code is %{public}d", st);
        return Str8ToStr16("");
    }
    std::u16string result = reply.ReadString16();
    return result;
}

std::u16string CoreServiceProxy::GetSimEons(int32_t slotId, const std::string &plmn, int32_t lac, bool longNameRequired)
{
    if (!IsValidSlotId(slotId)) {
        return Str8ToStr16("");
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetSimEons WriteInterfaceToken is false");
        return Str8ToStr16("");
    }
    data.WriteInt32(slotId);
    data.WriteString(plmn);
    data.WriteInt32(lac);
    data.WriteBool(longNameRequired);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSimEons Remote is null");
        return Str8ToStr16("");
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_SIM_EONS), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetSimEons failed, error code is %{public}d", st);
        return Str8ToStr16("");
    }
    return reply.ReadString16();
}

int32_t CoreServiceProxy::GetSimAccountInfo(int32_t slotId, IccAccountInfo &info)
{
    TELEPHONY_LOGI("GetSimAccountInfo slotId = %{public}d", slotId);
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetSimAccountInfo WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSimAccountInfo Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_SIM_SUB_INFO), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetSimAccountInfo failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        info.ReadFromParcel(reply);
    }
    return result;
}

int32_t CoreServiceProxy::SetDefaultVoiceSlotId(int32_t slotId)
{
    TELEPHONY_LOGI("CoreServiceProxy::SetDefaultVoiceSlotId slotId = %{public}d", slotId);
    if (!IsValidSlotIdForDefault(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SetDefaultVoiceSlotId WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SetDefaultVoiceSlotId Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::SET_DEFAULT_VOICE_SLOTID), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("SetDefaultVoiceSlotId failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return reply.ReadInt32();
}

int32_t CoreServiceProxy::GetDefaultVoiceSlotId()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetDefaultVoiceSlotId WriteInterfaceToken is false");
        return ERROR;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetDefaultVoiceSlotId Remote is null");
        return ERROR;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_DEFAULT_VOICE_SLOTID), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetDefaultVoiceSlotId failed, error code is %{public}d \n", st);
        return ERROR;
    }
    int32_t result = reply.ReadInt32();
    TELEPHONY_LOGI("GetDefaultVoiceSlotId end: result=%{public}d \n", result);
    return result;
}

int32_t CoreServiceProxy::SetPrimarySlotId(int32_t slotId)
{
    TELEPHONY_LOGI("CoreServiceProxy::SetPrimarySlotId slotId = %{public}d", slotId);
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SetPrimarySlotId WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SetPrimarySlotId Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(uint32_t(InterfaceID::SET_PRIMARY_SLOTID), data, reply, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("SetPrimarySlotId failed, error code is %{public}d \n", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return reply.ReadInt32();
}

int32_t CoreServiceProxy::GetPrimarySlotId(int32_t &slotId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetPrimarySlotId WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetPrimarySlotId Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_PRIMARY_SLOTID), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetPrimarySlotId failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    TELEPHONY_LOGI("GetPrimarySlotId end: result=%{public}d \n", result);
    if (result == TELEPHONY_ERR_SUCCESS) {
        slotId = reply.ReadInt32();
    }
    return result;
}

int32_t CoreServiceProxy::SetShowNumber(int32_t slotId, const std::u16string &number)
{
    TELEPHONY_LOGI("CoreServiceProxy::SetShowNumber slotId = %{public}d", slotId);
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    if (!IsValidStringLength(number)) {
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SetShowNumber WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    data.WriteString16(number);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SetShowNumber Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::SET_SHOW_NUMBER), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("SetShowNumber failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    TELEPHONY_LOGI("SetShowNumber end: result=%{public}d \n", result);
    return result;
}

int32_t CoreServiceProxy::GetShowNumber(int32_t slotId, std::u16string &showNumber)
{
    TELEPHONY_LOGI("GetShowNumber slotId = %{public}d", slotId);
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetShowNumber WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetShowNumber Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_SHOW_NUMBER), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetShowNumber failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        showNumber = reply.ReadString16();
    }
    return result;
}

int32_t CoreServiceProxy::SetShowName(int32_t slotId, const std::u16string &name)
{
    TELEPHONY_LOGI("CoreServiceProxy::SetShowName slotId = %{public}d", slotId);
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    if (!IsValidStringLength(name)) {
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SetShowName WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    data.WriteString16(name);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SetShowName Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::SET_SHOW_NAME), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("SetShowName failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    TELEPHONY_LOGI("SetShowName end: result=%{public}d \n", result);
    return result;
}

int32_t CoreServiceProxy::GetShowName(int32_t slotId, std::u16string &showName)
{
    TELEPHONY_LOGI("GetShowName slotId = %{public}d", slotId);
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetShowName WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetShowName Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_SHOW_NAME), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetShowName failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        showName = reply.ReadString16();
    }
    return result;
}

int32_t CoreServiceProxy::GetActiveSimAccountInfoList(std::vector<IccAccountInfo> &iccAccountInfoList)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetActiveSimAccountInfoList WriteInterfaceToken is false");
        return false;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetActiveSimAccountInfoList Remote is null");
        return false;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_ACTIVE_ACCOUNT_INFO_LIST), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetActiveSimAccountInfoList failed, error code is %{public}d \n", st);
        return false;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        int32_t size = reply.ReadInt32();
        TELEPHONY_LOGI("CoreServiceProxy::GetActiveSimAccountInfoList size = %{public}d", size);
        if (size > MAX_VECTOR) {
            return false;
        }
        iccAccountInfoList.clear();
        for (int i = 0; i < size; i++) {
            IccAccountInfo accountInfo;
            accountInfo.ReadFromParcel(reply);
            TELEPHONY_LOGI("CoreServiceProxy::GetActiveSimAccountInfoList success");
            iccAccountInfoList.emplace_back(accountInfo);
        }
    }
    return result;
}

int32_t CoreServiceProxy::GetOperatorConfigs(int32_t slotId, OperatorConfig &poc)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CoreServiceProxy::OperatorConfig invalid slotId");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetOperatorConfigs WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetOperatorConfigs Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_OPERATOR_CONFIG), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetOperatorConfigs failed, error code is %{public}d", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        poc.ReadFromParcel(reply);
    }
    return result;
}

bool CoreServiceProxy::IsValidSlotId(int32_t slotId)
{
    int32_t count = GetMaxSimCount();
    if ((slotId >= DEFAULT_SIM_SLOT_ID) && (slotId < count)) {
        return true;
    }

    TELEPHONY_LOGE("SlotId is InValid = %{public}d", slotId);
    return false;
}

bool CoreServiceProxy::IsValidSlotIdForDefault(int32_t slotId)
{
    int32_t count = GetMaxSimCount();
    if ((slotId >= DEFAULT_SIM_SLOT_ID_REMOVE) && (slotId < count)) {
        return true;
    }

    TELEPHONY_LOGE("SlotId is InValid = %{public}d", slotId);
    return false;
}

bool CoreServiceProxy::IsValidStringLength(std::u16string str)
{
    int32_t length = static_cast<int32_t>(str.length());
    if ((length >= MIN_STRING_LE) && (length <= MAX_STRING_LE)) {
        return true;
    }
    TELEPHONY_LOGE("string length is InValid = %{public}d", length);
    return false;
}

bool CoreServiceProxy::IsValidServiceType(ImsServiceType serviceType)
{
    if (serviceType < ImsServiceType::TYPE_VOICE || serviceType > ImsServiceType::TYPE_SMS) {
        TELEPHONY_LOGE("ServiceType is InValid = %{public}d", serviceType);
        return false;
    }

    return true;
}

int32_t CoreServiceProxy::UnlockPin(const int32_t slotId, const std::u16string &pin, LockStatusResponse &response)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("UnlockPin WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    data.WriteString16(pin);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("UnlockPin Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::UNLOCK_PIN), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("UnlockPin failed, error code is %{public}d", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        response.result = reply.ReadInt32();
        if (response.result == UNLOCK_INCORRECT) {
            response.remain = reply.ReadInt32();
        }
    }
    return result;
}

int32_t CoreServiceProxy::UnlockPuk(
    const int32_t slotId, const std::u16string &newPin, const std::u16string &puk, LockStatusResponse &response)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("UnlockPuk WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    data.WriteString16(newPin);
    data.WriteString16(puk);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("UnlockPuk Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::UNLOCK_PUK), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("UnlockPuk failed, error code is %{public}d", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        response.result = reply.ReadInt32();
        if (response.result == UNLOCK_INCORRECT) {
            response.remain = reply.ReadInt32();
        }
    }
    return result;
}

int32_t CoreServiceProxy::AlterPin(
    const int32_t slotId, const std::u16string &newPin, const std::u16string &oldPin, LockStatusResponse &response)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("AlterPin WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    data.WriteString16(newPin);
    data.WriteString16(oldPin);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("AlterPin Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::ALTER_PIN), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("AlterPin failed, error code is %{public}d", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        response.result = reply.ReadInt32();
        if (response.result == UNLOCK_INCORRECT) {
            response.remain = reply.ReadInt32();
        }
    }
    return result;
}

int32_t CoreServiceProxy::UnlockPin2(const int32_t slotId, const std::u16string &pin2, LockStatusResponse &response)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("UnlockPin2 WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    data.WriteString16(pin2);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("UnlockPin2 Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::UNLOCK_PIN2), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("UnlockPin2 failed, error code is %{public}d", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        response.result = reply.ReadInt32();
        if (response.result == UNLOCK_INCORRECT) {
            response.remain = reply.ReadInt32();
        }
    }
    return result;
}

int32_t CoreServiceProxy::UnlockPuk2(
    const int32_t slotId, const std::u16string &newPin2, const std::u16string &puk2, LockStatusResponse &response)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("UnlockPuk2 WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    data.WriteString16(newPin2);
    data.WriteString16(puk2);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("UnlockPuk2 Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::UNLOCK_PUK2), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("UnlockPuk2 failed, error code is %{public}d", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        response.result = reply.ReadInt32();
        if (response.result == UNLOCK_INCORRECT) {
            response.remain = reply.ReadInt32();
        }
    }
    return result;
}

int32_t CoreServiceProxy::AlterPin2(
    const int32_t slotId, const std::u16string &newPin2, const std::u16string &oldPin2, LockStatusResponse &response)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("AlterPin2 WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    data.WriteString16(newPin2);
    data.WriteString16(oldPin2);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("AlterPin2 Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::ALTER_PIN2), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("AlterPin2 failed, error code is %{public}d", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        response.result = reply.ReadInt32();
        if (response.result == UNLOCK_INCORRECT) {
            response.remain = reply.ReadInt32();
        }
    }
    return result;
}

int32_t CoreServiceProxy::SetLockState(const int32_t slotId, const LockInfo &options, LockStatusResponse &response)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SetLockState WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    data.WriteInt32(static_cast<int32_t>(options.lockType));
    data.WriteInt32(static_cast<int32_t>(options.lockState));
    data.WriteString16(options.password);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SetLockState Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::SWITCH_LOCK), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("SetLockState failed, error code is %{public}d", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    TELEPHONY_LOGI("SetLockState successful, result:%{public}d", result);
    if (result == TELEPHONY_ERR_SUCCESS) {
        response.result = reply.ReadInt32();
        if (response.result == UNLOCK_INCORRECT) {
            response.remain = reply.ReadInt32();
        }
    }
    return result;
}

int32_t CoreServiceProxy::GetLockState(int32_t slotId, LockType lockType, LockState &lockState)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetLockState WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    TELEPHONY_LOGI("GetLockState WriteInterfaceToken is true");
    data.WriteInt32(slotId);
    data.WriteInt32(static_cast<int32_t>(lockType));
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetLockState Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    TELEPHONY_LOGI("GetLockState Remote is  != null");
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::CHECK_LOCK), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetLockState failed, error code is %{public}d", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        lockState = static_cast<LockState>(reply.ReadInt32());
    }
    TELEPHONY_LOGI("GetLockState call end: result=%{public}d", result);
    return result;
}

int32_t CoreServiceProxy::RefreshSimState(int32_t slotId)
{
    TELEPHONY_LOGI("CoreServiceProxy RefreshSimState ::%{public}d", slotId);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("RefreshSimState WriteInterfaceToken is false");
        return TELEPHONY_ERROR;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("RefreshSimState Remote is null");
        return TELEPHONY_ERROR;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::REFRESH_SIM_STATE), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("RefreshSimState failed, error code is %{public}d \n", st);
        return TELEPHONY_ERROR;
    }
    int32_t result = reply.ReadInt32();
    TELEPHONY_LOGI("RefreshSimState call end:: result = %{public}d", result);
    return result;
}

int32_t CoreServiceProxy::SetActiveSim(int32_t slotId, int32_t enable)
{
    TELEPHONY_LOGI("CoreServiceProxy SetActiveSim ::%{public}d", slotId);
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CoreServiceProxy::SetActiveSim invalid simId");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    static const int32_t DISABLE = 0;
    static const int32_t ENABLE = 1;
    if (enable != DISABLE && enable != ENABLE) {
        TELEPHONY_LOGE("CoreServiceProxy::SetActiveSim invalid enable status");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SetActiveSim WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    data.WriteInt32(enable);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SetActiveSim Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::SET_SIM_ACTIVE), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("SetActiveSim failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    TELEPHONY_LOGI("SetActiveSim call end:: result = %{public}d", result);
    return result;
}

int32_t CoreServiceProxy::GetPreferredNetwork(int32_t slotId, const sptr<INetworkSearchCallback> &callback)
{
    TELEPHONY_LOGI("CoreServiceProxy GetPreferredNetwork");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetPreferredNetwork WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(slotId)) {
        TELEPHONY_LOGE("GetPreferredNetwork WriteInt32 slotId is false");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (callback != nullptr) {
        data.WriteRemoteObject(callback->AsObject().GetRefPtr());
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetPreferredNetwork Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(uint32_t(InterfaceID::GET_PREFERRED_NETWORK_MODE), data, reply, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("GetPreferredNetwork failed, error code is %{public}d \n", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return reply.ReadInt32();
}

int32_t CoreServiceProxy::SetPreferredNetwork(
    int32_t slotId, int32_t networkMode, const sptr<INetworkSearchCallback> &callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SetPreferredNetwork WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(slotId)) {
        TELEPHONY_LOGE("SetPreferredNetwork WriteInt32 slotId is false");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (!data.WriteInt32(networkMode)) {
        TELEPHONY_LOGE("SetPreferredNetwork WriteInt32 networkMode is false");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (callback != nullptr) {
        data.WriteRemoteObject(callback->AsObject().GetRefPtr());
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SetPreferredNetwork Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(uint32_t(InterfaceID::SET_PREFERRED_NETWORK_MODE), data, reply, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("SetPreferredNetwork failed, error code is %{public}d \n", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return reply.ReadInt32();
}

int32_t CoreServiceProxy::GetSimTelephoneNumber(int32_t slotId, std::u16string &telephoneNumber)
{
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetSimTelephoneNumber WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSimTelephoneNumber Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_SIM_PHONE_NUMBER), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetSimTelephoneNumber failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        telephoneNumber = reply.ReadString16();
    }
    return result;
}

std::u16string CoreServiceProxy::GetSimTeleNumberIdentifier(const int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        return u"";
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetSimTeleNumberIdentifier WriteInterfaceToken is false");
        return Str8ToStr16("");
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSimTeleNumberIdentifier Remote is null");
        return Str8ToStr16("");
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_SIM_TELENUMBER_IDENTIFIER), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetSimTeleNumberIdentifier failed, error code is %{public}d \n", st);
        return Str8ToStr16("");
    }
    std::u16string result = reply.ReadString16();
    return result;
}

int32_t CoreServiceProxy::GetVoiceMailIdentifier(int32_t slotId, std::u16string &voiceMailIdentifier)
{
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetVoiceMailIdentifier WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetVoiceMailIdentifier Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_VOICE_MAIL_TAG), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetVoiceMailIdentifier failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        voiceMailIdentifier = reply.ReadString16();
    }
    return result;
}

int32_t CoreServiceProxy::GetVoiceMailNumber(int32_t slotId, std::u16string &voiceMailNumber)
{
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetVoiceMailNumber WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetVoiceMailNumber Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_VOICE_MAIL_NUMBER), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetVoiceMailNumber failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        voiceMailNumber = reply.ReadString16();
    }
    return result;
}

int32_t CoreServiceProxy::QueryIccDiallingNumbers(
    int slotId, int type, std::vector<std::shared_ptr<DiallingNumbersInfo>> &result)
{
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("QueryIccDiallingNumbers WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(slotId)) {
        TELEPHONY_LOGE("QueryIccDiallingNumbers WriteInt32 slotId is false");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(type)) {
        TELEPHONY_LOGE("QueryIccDiallingNumbers WriteInt32 type is false");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("QueryIccDiallingNumbers Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::ICC_DIALLING_NUMBERS_GET), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("QueryIccDiallingNumbers failed, error code is %{public}d", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t errorCode = reply.ReadInt32();
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        return errorCode;
    }
    int32_t size = reply.ReadInt32();
    TELEPHONY_LOGI("CoreServiceProxy::QueryIccDiallingNumbers size:%{public}d", size);
    if (size >= MAX_SIZE) {
        TELEPHONY_LOGE("CoreServiceProxy::QueryIccDiallingNumbers over max size");
        return TELEPHONY_ERR_READ_DATA_FAIL;
    }
    for (int i = 0; i < size; i++) {
        std::shared_ptr<DiallingNumbersInfo> diallingNumber = DiallingNumbersInfo::UnMarshalling(reply);
        result.emplace_back(diallingNumber);
    }
    return TELEPHONY_ERR_SUCCESS;
}

int32_t CoreServiceProxy::AddIccDiallingNumbers(
    int slotId, int type, const std::shared_ptr<DiallingNumbersInfo> &diallingNumber)
{
    TELEPHONY_LOGI("CoreServiceProxy AddIccDiallingNumbers ::%{public}d", slotId);
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("AddIccDiallingNumbers WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(slotId)) {
        TELEPHONY_LOGE("AddIccDiallingNumbers WriteInt32 slotId is false");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(type)) {
        TELEPHONY_LOGE("AddIccDiallingNumbers WriteInt32 type is false");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (diallingNumber != nullptr) {
        diallingNumber->Marshalling(data);
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("AddIccDiallingNumbers Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::ICC_DIALLING_NUMBERS_INSERT), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("AddIccDiallingNumbers failed, error code is %{public}d", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return reply.ReadInt32();
}

int32_t CoreServiceProxy::DelIccDiallingNumbers(
    int slotId, int type, const std::shared_ptr<DiallingNumbersInfo> &diallingNumber)
{
    TELEPHONY_LOGI("CoreServiceProxy DelIccDiallingNumbers ::%{public}d", slotId);
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("DelIccDiallingNumbers WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(slotId)) {
        TELEPHONY_LOGE("DelIccDiallingNumbers WriteInt32 slotId is false");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(type)) {
        TELEPHONY_LOGE("DelIccDiallingNumbers WriteInt32 type is false");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (diallingNumber != nullptr) {
        diallingNumber->Marshalling(data);
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("DelIccDiallingNumbers Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::ICC_DIALLING_NUMBERS_DELETE), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("DelIccDiallingNumbers failed, error code is %{public}d", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return reply.ReadInt32();
}

int32_t CoreServiceProxy::UpdateIccDiallingNumbers(
    int slotId, int type, const std::shared_ptr<DiallingNumbersInfo> &diallingNumber)
{
    TELEPHONY_LOGI("CoreServiceProxy UpdateIccDiallingNumbers ::%{public}d", slotId);
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("UpdateIccDiallingNumbers WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(slotId)) {
        TELEPHONY_LOGE("UpdateIccDiallingNumbers WriteInt32 slotId is false");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(type)) {
        TELEPHONY_LOGE("UpdateIccDiallingNumbers WriteInt32 type is false");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (diallingNumber != nullptr) {
        diallingNumber->Marshalling(data);
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("UpdateIccDiallingNumbers Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::ICC_DIALLING_NUMBERS_UPDATE), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("UpdateIccDiallingNumbers failed, error code is %{public}d", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return reply.ReadInt32();
}

int32_t CoreServiceProxy::SetVoiceMailInfo(
    const int32_t slotId, const std::u16string &mailName, const std::u16string &mailNumber)
{
    TELEPHONY_LOGI("CoreServiceProxy SetVoiceMailInfo ::%{public}d", slotId);
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    if (!IsValidStringLength(mailName) || !IsValidStringLength(mailNumber)) {
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SetVoiceMailInfo WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(slotId)) {
        TELEPHONY_LOGE("SetVoiceMailInfo WriteInt32 slotId is false");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteString16(mailName)) {
        TELEPHONY_LOGE("SetVoiceMailInfo WriteString16 mailName is false");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteString16(mailNumber)) {
        TELEPHONY_LOGE("SetVoiceMailInfo WriteString16 mailNumber is false");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SetVoiceMailInfo Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::SET_VOICE_MAIL), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("SetVoiceMailInfo failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return reply.ReadInt32();
}

int32_t CoreServiceProxy::GetOpKey(int32_t slotId, std::u16string &opkey)
{
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetOpKey WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetOpKey Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(uint32_t(InterfaceID::GET_OPKEY), data, reply, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("GetOpKey failed, error code is %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        opkey = reply.ReadString16();
    }
    return result;
}

int32_t CoreServiceProxy::GetOpKeyExt(int32_t slotId, std::u16string &opkeyExt)
{
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetOpKeyExt WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetOpKeyExt Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(uint32_t(InterfaceID::GET_OPKEY_EXT), data, reply, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("GetOpKeyExt failed, error code is %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        opkeyExt = reply.ReadString16();
    }
    return result;
}

int32_t CoreServiceProxy::GetOpName(int32_t slotId, std::u16string &opname)
{
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetOpName WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetOpName Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(uint32_t(InterfaceID::GET_OPNAME), data, reply, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("GetOpName failed, error code is %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        opname = reply.ReadString16();
    }
    return result;
}

int32_t CoreServiceProxy::GetMaxSimCount()
{
    char simSlotCount[SYSPARA_SIZE] = { 0 };
    GetParameter(TEL_SIM_SLOT_COUNT, DEFAULT_SLOT_COUNT, simSlotCount, SYSPARA_SIZE);
    int32_t slotCount = std::atoi(simSlotCount);
    return slotCount;
}

int32_t CoreServiceProxy::SendEnvelopeCmd(int32_t slotId, const std::string &cmd)
{
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    if (cmd.empty()) {
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SendEnvelopeCmd WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    data.WriteString(cmd);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SendEnvelopeCmd Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::STK_CMD_FROM_APP_ENVELOPE), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("SendEnvelopeCmd failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    TELEPHONY_LOGI("SendEnvelopeCmd end: result=%{public}d \n", result);
    return result;
}

int32_t CoreServiceProxy::SendTerminalResponseCmd(int32_t slotId, const std::string &cmd)
{
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    if (cmd.empty()) {
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SendTerminalResponseCmd WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    data.WriteString(cmd);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SendTerminalResponseCmd Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::STK_CMD_FROM_APP_TERMINAL_RESPONSE), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("SendTerminalResponseCmd failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    TELEPHONY_LOGI("SendTerminalResponseCmd end: result=%{public}d \n", result);
    return result;
}

int32_t CoreServiceProxy::SendCallSetupRequestResult(int32_t slotId, bool accept)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SendCallSetupRequestResult WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    data.WriteInt32(accept);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SendCallSetupRequestResult Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t st =
        remote->SendRequest(uint32_t(InterfaceID::STK_RESULT_FROM_APP_CALL_SETUP_REQUEST), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("SendCallSetupRequestResult failed, error code is %{public}d", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    TELEPHONY_LOGI("SendCallSetupRequestResult end: result=%{public}d", result);
    return result;
}

int32_t CoreServiceProxy::UnlockSimLock(int32_t slotId, const PersoLockInfo &lockInfo, LockStatusResponse &response)
{
    if (!IsValidSlotId(slotId)) {
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("UnlockSimLock WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    data.WriteInt32(static_cast<int32_t>(lockInfo.lockType));
    data.WriteString16(lockInfo.password);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("UnlockSimLock Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::UNLOCK_SIMLOCK), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("UnlockSimLock failed, error code is %{public}d", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        response.result = reply.ReadInt32();
        response.remain = reply.ReadInt32();
    }
    return result;
}

int32_t CoreServiceProxy::GetImsRegStatus(int32_t slotId, ImsServiceType imsSrvType, ImsRegInfo &info)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slotId!");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    TELEPHONY_LOGI("CoreServiceProxy GetImsRegStatus slotId:%{public}d", slotId);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetImsRegStatus WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(slotId)) {
        TELEPHONY_LOGE("GetImsRegStatus WriteInt32 slotId is false");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(imsSrvType)) {
        TELEPHONY_LOGE("GetImsRegStatus WriteInt32 imsSrvType is false");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetImsRegStatus Remote is null");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::GET_IMS_REG_STATUS), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("GetImsRegStatus failed, error code is %{public}d \n", st);
        return st;
    }
    int32_t ret = reply.ReadInt32();
    info.imsRegState = static_cast<ImsRegState>(reply.ReadInt32());
    info.imsRegTech = static_cast<ImsRegTech>(reply.ReadInt32());
    return ret;
}

int32_t CoreServiceProxy::GetCellInfoList(int32_t slotId, std::vector<sptr<CellInformation>> &cellInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetCellInfoList WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(slotId)) {
        TELEPHONY_LOGE("GetCellInfoList WriteInt32 imsSrvType is false");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetCellInfoList Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(uint32_t(InterfaceID::GET_CELL_INFO_LIST), data, reply, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("GetCellInfoList failed, error code is %{public}d\n", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        ProcessCellInfo(reply, cellInfo);
    }
    TELEPHONY_LOGI("CoreServiceProxy::GetCellInfoList cell size:%{public}zu\n", cellInfo.size());
    return result;
}

void CoreServiceProxy::ProcessCellInfo(MessageParcel &reply, std::vector<sptr<CellInformation>> &cells)
{
    int32_t size = reply.ReadInt32();
    TELEPHONY_LOGI("CoreServiceProxy::ProcessCellInfo size:%{public}d\n", size);
    if (size >= MAX_SIZE) {
        TELEPHONY_LOGE("CoreServiceProxy::ProcessCellInfo over max size");
        return;
    }
    CellInformation::CellType type;
    for (int i = 0; i < size; ++i) {
        type = static_cast<CellInformation::CellType>(reply.ReadInt32());
        switch (type) {
            case CellInformation::CellType::CELL_TYPE_GSM: {
                ProcessReply<GsmCellInformation>(reply, cells);
                break;
            }
            case CellInformation::CellType::CELL_TYPE_LTE: {
                ProcessReply<LteCellInformation>(reply, cells);
                break;
            }
            case CellInformation::CellType::CELL_TYPE_WCDMA: {
                ProcessReply<WcdmaCellInformation>(reply, cells);
                break;
            }
            case CellInformation::CellType::CELL_TYPE_CDMA: {
                ProcessReply<CdmaCellInformation>(reply, cells);
                break;
            }
            case CellInformation::CellType::CELL_TYPE_TDSCDMA: {
                ProcessReply<TdscdmaCellInformation>(reply, cells);
                break;
            }
            case CellInformation::CellType::CELL_TYPE_NR: {
                ProcessReply<NrCellInformation>(reply, cells);
                break;
            }
            default:
                break;
        }
    }
}

int32_t CoreServiceProxy::SendUpdateCellLocationRequest(int32_t slotId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SendUpdateCellLocationRequest WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SendUpdateCellLocationRequest Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    data.WriteInt32(slotId);
    int32_t error = remote->SendRequest(uint32_t(InterfaceID::GET_CELL_LOCATION), data, reply, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("SendUpdateCellLocationRequest failed, error code is %{public}d \n", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }

    return reply.ReadInt32();
}

int32_t CoreServiceProxy::HasOperatorPrivileges(const int32_t slotId, bool &hasOperatorPrivileges)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("HasOperatorPrivileges WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("HasOperatorPrivileges Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::HAS_OPERATOR_PRIVILEGES), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("HasOperatorPrivileges failed, error code is %{public}d \n", st);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    TELEPHONY_LOGI("HasOperatorPrivileges end: result=%{public}d \n", result);
    if (result == TELEPHONY_ERR_SUCCESS) {
        hasOperatorPrivileges = reply.ReadBool();
    }
    return result;
}

int32_t CoreServiceProxy::SimAuthentication(
    int32_t slotId, const std::string &aid, const std::string &authData, SimAuthenticationResponse &response)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SimAuthentication WriteInterfaceToken is false");
        return ERROR;
    }
    data.WriteInt32(slotId);
    data.WriteString(aid);
    data.WriteString(authData);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SimAuthentication Remote is null");
        return ERROR;
    }
    int32_t st = remote->SendRequest(uint32_t(InterfaceID::SIM_AUTHENTICATION), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("SimAuthentication failed, error code is %{public}d", st);
        return ERROR;
    }
    int32_t ret = reply.ReadInt32();
    response.sw1 = reply.ReadInt32();
    response.sw2 = reply.ReadInt32();
    response.response = reply.ReadString();
    TELEPHONY_LOGI("SimAuthentication end: result=%{public}d", ret);
    return ret;
}

bool CoreServiceProxy::IsNrSupported(int32_t slotId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("IsNrSupported WriteInterfaceToken is false");
        return false;
    }
    auto remote = Remote();
    data.WriteInt32(slotId);
    if (remote == nullptr) {
        TELEPHONY_LOGE("SimAuthentication Remote is null");
        return ERROR;
    }
    int32_t st = remote->SendRequest(static_cast<uint32_t>(InterfaceID::IS_NR_SUPPORTED), data, reply, option);
    if (st != ERR_NONE) {
        TELEPHONY_LOGE("IsNrSupported failed, error code is %{public}d \n", st);
        return false;
    }
    bool result = reply.ReadBool();
    return result;
}

int32_t CoreServiceProxy::GetNrOptionMode(int32_t slotId, NrMode &mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("GetNrOptionMode WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetNrOptionMode Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(InterfaceID::GET_NR_OPTION_MODE), data, reply, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("GetNrOptionMode failed, error code is %{public}d \n", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = reply.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        mode = static_cast<NrMode>(reply.ReadInt32());
    }
    return result;
}

int32_t CoreServiceProxy::RegisterImsRegInfoCallback(
    int32_t slotId, ImsServiceType imsSrvType, const sptr<ImsRegInfoCallback> &callback)
{
    if (callback == nullptr) {
        TELEPHONY_LOGE("callback is nullptr!");
        return TELEPHONY_ERR_ARGUMENT_NULL;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = SerializeImsRegInfoData(slotId, imsSrvType, data);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("serialize data failed, result is %{public}d", ret);
        return ret;
    }
    if (!data.WriteRemoteObject(callback->AsObject().GetRefPtr())) {
        TELEPHONY_LOGE("write remote object failed!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<OHOS::IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("remote is nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(InterfaceID::REG_IMS_CALLBACK), data, reply, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("error! errCode:%{public}d", error);
        return error;
    }
    return reply.ReadInt32();
}

int32_t CoreServiceProxy::UnregisterImsRegInfoCallback(int32_t slotId, ImsServiceType imsSrvType)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = SerializeImsRegInfoData(slotId, imsSrvType, data);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("serialize data failed, result is %{public}d", ret);
        return ret;
    }
    sptr<OHOS::IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("remote is nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(InterfaceID::UN_REG_IMS_CALLBACK), data, reply, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("error! errCode:%{public}d", error);
        return error;
    }
    return reply.ReadInt32();
}

int32_t CoreServiceProxy::SerializeImsRegInfoData(int32_t slotId, ImsServiceType imsSrvType, MessageParcel &data)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slotId");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    if (!IsValidServiceType(imsSrvType)) {
        TELEPHONY_LOGE("invalid serviceType!");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("write interface token failed!");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(slotId)) {
        TELEPHONY_LOGE("write slotId failed!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(static_cast<int32_t>(imsSrvType))) {
        TELEPHONY_LOGE("write imsSrvType failed!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
