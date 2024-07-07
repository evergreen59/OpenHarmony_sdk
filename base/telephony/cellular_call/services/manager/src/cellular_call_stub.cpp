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

#include "cellular_call_stub.h"
#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"
#include "emergency_utils.h"
#include "ipc_skeleton.h"
#include "i_call_status_callback.h"

namespace OHOS {
namespace Telephony {
const int32_t MAX_SIZE = 10;
const int32_t MAX_ECC_SIZE = 1000;

int32_t CellularCallStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string myDescriptor = CellularCallStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (myDescriptor != remoteDescriptor) {
        TELEPHONY_LOGE("descriptor checked fail");
        return TELEPHONY_ERR_DESCRIPTOR_MISMATCH;
    }

    auto itFunc = requestFuncMap_.find(static_cast<OperationType>(code));
    if (itFunc != requestFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }
    TELEPHONY_LOGI("CellularCallStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

CellularCallStub::CellularCallStub()
{
    TELEPHONY_LOGI("CellularCallStub::CellularCallStub");
    requestFuncMap_[OperationType::DIAL] = &CellularCallStub::OnDialInner;
    requestFuncMap_[OperationType::HANG_UP] = &CellularCallStub::OnHangUpInner;
    requestFuncMap_[OperationType::REJECT] = &CellularCallStub::OnRejectInner;
    requestFuncMap_[OperationType::ANSWER] = &CellularCallStub::OnAnswerInner;
    requestFuncMap_[OperationType::EMERGENCY_CALL] = &CellularCallStub::OnIsEmergencyPhoneNumberInner;
    requestFuncMap_[OperationType::SET_EMERGENCY_CALL_LIST] = &CellularCallStub::OnSetEmergencyCallList;
    requestFuncMap_[OperationType::HOLD_CALL] = &CellularCallStub::OnHoldCallInner;
    requestFuncMap_[OperationType::UN_HOLD_CALL] = &CellularCallStub::OnUnHoldCallInner;
    requestFuncMap_[OperationType::SWITCH_CALL] = &CellularCallStub::OnSwitchCallInner;
    requestFuncMap_[OperationType::COMBINE_CONFERENCE] = &CellularCallStub::OnCombineConferenceInner;
    requestFuncMap_[OperationType::SEPARATE_CONFERENCE] = &CellularCallStub::OnSeparateConferenceInner;
    requestFuncMap_[OperationType::INVITE_TO_CONFERENCE] = &CellularCallStub::OnInviteToConferenceInner;
    requestFuncMap_[OperationType::KICK_OUT_CONFERENCE] = &CellularCallStub::OnKickOutFromConferenceInner;
    requestFuncMap_[OperationType::HANG_UP_ALL_CONNECTION] = &CellularCallStub::OnHangUpAllConnectionInner;
    requestFuncMap_[OperationType::UPDATE_CALL_MEDIA_MODE] = &CellularCallStub::OnUpdateCallMediaModeInner;
    requestFuncMap_[OperationType::REGISTER_CALLBACK] = &CellularCallStub::OnRegisterCallBackInner;
    requestFuncMap_[OperationType::UNREGISTER_CALLBACK] = &CellularCallStub::OnUnRegisterCallBackInner;
    requestFuncMap_[OperationType::START_DTMF] = &CellularCallStub::OnStartDtmfInner;
    requestFuncMap_[OperationType::STOP_DTMF] = &CellularCallStub::OnStopDtmfInner;
    requestFuncMap_[OperationType::SEND_DTMF] = &CellularCallStub::OnSendDtmfInner;
    requestFuncMap_[OperationType::START_RTT] = &CellularCallStub::OnStartRttInner;
    requestFuncMap_[OperationType::STOP_RTT] = &CellularCallStub::OnStopRttInner;
    requestFuncMap_[OperationType::SET_CALL_TRANSFER] = &CellularCallStub::OnSetCallTransferInner;
    requestFuncMap_[OperationType::GET_CALL_TRANSFER] = &CellularCallStub::OnGetCallTransferInner;
    requestFuncMap_[OperationType::SET_CALL_WAITING] = &CellularCallStub::OnSetCallWaitingInner;
    requestFuncMap_[OperationType::GET_CALL_WAITING] = &CellularCallStub::OnGetCallWaitingInner;
    requestFuncMap_[OperationType::SET_CALL_RESTRICTION] = &CellularCallStub::OnSetCallRestrictionInner;
    requestFuncMap_[OperationType::GET_CALL_RESTRICTION] = &CellularCallStub::OnGetCallRestrictionInner;
    requestFuncMap_[OperationType::SET_DOMAIN_PREFERENCE_MODE] = &CellularCallStub::OnSetDomainPreferenceModeInner;
    requestFuncMap_[OperationType::GET_DOMAIN_PREFERENCE_MODE] = &CellularCallStub::OnGetDomainPreferenceModeInner;
    requestFuncMap_[OperationType::SET_IMS_SWITCH_STATUS] = &CellularCallStub::OnSetImsSwitchStatusInner;
    requestFuncMap_[OperationType::GET_IMS_SWITCH_STATUS] = &CellularCallStub::OnGetImsSwitchStatusInner;
    requestFuncMap_[OperationType::SET_IMS_CONFIG_STRING] = &CellularCallStub::OnSetImsConfigStringInner;
    requestFuncMap_[OperationType::SET_IMS_CONFIG_INT] = &CellularCallStub::OnSetImsConfigIntInner;
    requestFuncMap_[OperationType::GET_IMS_CONFIG] = &CellularCallStub::OnGetImsConfigInner;
    requestFuncMap_[OperationType::SET_IMS_FEATURE] = &CellularCallStub::OnSetImsFeatureValueInner;
    requestFuncMap_[OperationType::GET_IMS_FEATURE] = &CellularCallStub::OnGetImsFeatureValueInner;
    requestFuncMap_[OperationType::CTRL_CAMERA] = &CellularCallStub::OnCtrlCameraInner;
    requestFuncMap_[OperationType::SET_PREVIEW_WINDOW] = &CellularCallStub::OnSetPreviewWindowInner;
    requestFuncMap_[OperationType::SET_DISPLAY_WINDOW] = &CellularCallStub::OnSetDisplayWindowInner;
    requestFuncMap_[OperationType::SET_CAMERA_ZOOM] = &CellularCallStub::OnSetCameraZoomInner;
    requestFuncMap_[OperationType::SET_PAUSE_IMAGE] = &CellularCallStub::OnSetPauseImageInner;
    requestFuncMap_[OperationType::SET_DEVICE_DIRECTION] = &CellularCallStub::OnSetDeviceDirectionInner;
    requestFuncMap_[OperationType::SET_MUTE] = &CellularCallStub::OnSetMuteInner;
    requestFuncMap_[OperationType::GET_MUTE] = &CellularCallStub::OnGetMuteInner;
}

CellularCallStub::~CellularCallStub()
{
    TELEPHONY_LOGI("CellularCallStub::~CellularCallStub");
    requestFuncMap_.clear();
}

int32_t CellularCallStub::OnDialInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnDialInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnRemoteRequest data size error");
        return TELEPHONY_ERR_FAIL;
    }
    auto pCallInfo = (CellularCallInfo *)data.ReadRawData(sizeof(CellularCallInfo));
    if (pCallInfo == nullptr) {
        TELEPHONY_LOGE("OnDialInner return, pCallInfo is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    reply.WriteInt32(Dial(*pCallInfo));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnHangUpInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnHangUpInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnRemoteRequest data size error");
        return TELEPHONY_ERR_FAIL;
    }
    auto pCallInfo = (CellularCallInfo *)data.ReadRawData(sizeof(CellularCallInfo));
    if (pCallInfo == nullptr) {
        TELEPHONY_LOGE("OnHangUpInner return, pCallInfo is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    auto type = static_cast<CallSupplementType>(data.ReadInt32());

    reply.WriteInt32(HangUp(*pCallInfo, type));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnRejectInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnRejectInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnRemoteRequest data size error");
        return TELEPHONY_ERR_FAIL;
    }
    auto pCallInfo = (CellularCallInfo *)data.ReadRawData(sizeof(CellularCallInfo));
    if (pCallInfo == nullptr) {
        TELEPHONY_LOGE("OnRejectInner return, pCallInfo is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    reply.WriteInt32(Reject(*pCallInfo));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnAnswerInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnAnswerInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnRemoteRequest data size error");
        return TELEPHONY_ERR_FAIL;
    }
    auto pCallInfo = (CellularCallInfo *)data.ReadRawData(sizeof(CellularCallInfo));
    if (pCallInfo == nullptr) {
        TELEPHONY_LOGE("OnAnswerInner return, pCallInfo is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    reply.WriteInt32(Answer(*pCallInfo));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnIsEmergencyPhoneNumberInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnIsEmergencyPhoneNumberInner entry.");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnIsEmergencyPhoneNumberInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    std::string phoneNum = data.ReadString();
    bool enabled = false;
    int32_t ret = IsEmergencyPhoneNumber(slotId, phoneNum, enabled);
    if (!reply.WriteInt32(ret)) {
        TELEPHONY_LOGE("fail to write ret");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!reply.WriteBool(enabled)) {
        TELEPHONY_LOGE("fail to write enabled");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSetEmergencyCallList(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSetEmergencyCallList entry.");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnSetEmergencyCallList data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_ECC_SIZE) {
        TELEPHONY_LOGE("CellularCallStub::OnSetEmergencyCallList ecc size error");
        return TELEPHONY_ERR_FAIL;
    }
    std::vector<EmergencyCall> eccVec;
    for (int i = 0; i < len; i++) {
        auto value = reinterpret_cast<const EmergencyCall *>(data.ReadRawData(sizeof(EmergencyCall)));
        if (value == nullptr) {
            TELEPHONY_LOGE("CellularCallStub::OnSetEmergencyCallList call data error");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        eccVec.push_back(*value);
    }
    for (auto ecc : eccVec) {
        TELEPHONY_LOGE("OnSetEmergencyCallList, data: eccNum %{public}s mcc %{public}s",
            ecc.eccNum.c_str(), ecc.mcc.c_str());
    }
    reply.WriteInt32(SetEmergencyCallList(slotId, eccVec));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnRegisterCallBackInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnRegisterCallBackInner entry.");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnRemoteRequest data size error");
        return TELEPHONY_ERR_FAIL;
    }

    int32_t result = TELEPHONY_ERR_LOCAL_PTR_NULL;
    auto remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        TELEPHONY_LOGE("CellularCallStub::OnRegisterCallBackInner return, remote is nullptr.");
        reply.WriteInt32(result);
        return result;
    }
    result = RegisterCallManagerCallBack(iface_cast<ICallStatusCallback>(remote));

    reply.WriteInt32(result);
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnUnRegisterCallBackInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnUnRegisterCallBackInner entry.");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnRemoteRequest data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t result = UnRegisterCallManagerCallBack();

    reply.WriteInt32(result);
    return result;
}

int32_t CellularCallStub::OnHoldCallInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnHoldCallInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnRemoteRequest data size error");
        return TELEPHONY_ERR_FAIL;
    }
    auto pCallInfo = (CellularCallInfo *)data.ReadRawData(sizeof(CellularCallInfo));
    if (pCallInfo == nullptr) {
        TELEPHONY_LOGE("OnHoldCallInner return, pCallInfo is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    reply.WriteInt32(HoldCall(*pCallInfo));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnUnHoldCallInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnUnHoldCallInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnRemoteRequest data size error");
        return TELEPHONY_ERR_FAIL;
    }
    auto pCallInfo = (CellularCallInfo *)data.ReadRawData(sizeof(CellularCallInfo));
    if (pCallInfo == nullptr) {
        TELEPHONY_LOGE("OnUnHoldCallInner return, pCallInfo is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    reply.WriteInt32(UnHoldCall(*pCallInfo));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSwitchCallInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSwitchCallInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnRemoteRequest data size error");
        return TELEPHONY_ERR_FAIL;
    }
    auto pCallInfo = (CellularCallInfo *)data.ReadRawData(sizeof(CellularCallInfo));
    if (pCallInfo == nullptr) {
        TELEPHONY_LOGE("OnSwitchCallInner return, pCallInfo is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    reply.WriteInt32(SwitchCall(*pCallInfo));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnCombineConferenceInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnCombineConferenceInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnCombineConferenceInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    auto pCallInfo = (CellularCallInfo *)data.ReadRawData(sizeof(CellularCallInfo));
    if (pCallInfo == nullptr) {
        TELEPHONY_LOGE("OnCombineConferenceInner return, pCallInfo is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    reply.WriteInt32(CombineConference(*pCallInfo));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSeparateConferenceInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSeparateConferenceInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnSeparateConferenceInner data size error");
        return TELEPHONY_ERR_FAIL;
    }

    auto pCallInfo = (CellularCallInfo *)data.ReadRawData(sizeof(CellularCallInfo));
    if (pCallInfo == nullptr) {
        TELEPHONY_LOGE("OnSeparateConferenceInner return, pCallInfo is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    reply.WriteInt32(SeparateConference(*pCallInfo));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnInviteToConferenceInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnInviteToConferenceInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnInviteToConferenceInner data size error");
        return TELEPHONY_ERR_FAIL;
    }

    int32_t slotId = data.ReadInt32();
    std::vector<std::string> numberList;
    bool bRead = data.ReadStringVector(&numberList);
    if (!bRead) {
        TELEPHONY_LOGE("InviteToConferenceInner return, read fail.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    reply.WriteInt32(InviteToConference(slotId, numberList));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnKickOutFromConferenceInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnKickOutFromConferenceInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnKickOutFromConferenceInner data size error");
        return TELEPHONY_ERR_FAIL;
    }

    int32_t slotId = data.ReadInt32();
    std::vector<std::string> numberList;
    bool bRead = data.ReadStringVector(&numberList);
    if (!bRead) {
        TELEPHONY_LOGE("OnKickOutFromConferenceInner return, read fail.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    reply.WriteInt32(KickOutFromConference(slotId, numberList));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnHangUpAllConnectionInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnHangUpAllConnectionInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("OnHangUpAllConnectionInner data size error");
        return TELEPHONY_ERR_FAIL;
    }

    reply.WriteInt32(HangUpAllConnection());
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnUpdateCallMediaModeInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnUpdateCallMediaModeInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("OnUpdateCallMediaModeInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    auto pCallInfo = (CellularCallInfo *)data.ReadRawData(sizeof(CellularCallInfo));
    if (pCallInfo == nullptr) {
        TELEPHONY_LOGE("OnUpdateCallMediaModeInner return, pCallInfo is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    auto mode = static_cast<ImsCallMode>(data.ReadInt32());

    reply.WriteInt32(UpdateImsCallMode(*pCallInfo, mode));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnStartDtmfInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnStartDtmfInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnStartDtmfInner data size error");
        return TELEPHONY_ERR_FAIL;
    }

    char pDtmf = data.ReadInt8();
    auto pCallInfo = (CellularCallInfo *)data.ReadRawData(sizeof(CellularCallInfo));
    if (pCallInfo == nullptr) {
        TELEPHONY_LOGE("OnStartDtmfInner return, pCallInfo is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    reply.WriteInt32(StartDtmf(pDtmf, *pCallInfo));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnStopDtmfInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnStopDtmfInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnStopDtmfInner data size error");
        return TELEPHONY_ERR_FAIL;
    }

    auto pCallInfo = (CellularCallInfo *)data.ReadRawData(sizeof(CellularCallInfo));
    if (pCallInfo == nullptr) {
        TELEPHONY_LOGE("OnStopDtmfInner return, pCallInfo is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    reply.WriteInt32(StopDtmf(*pCallInfo));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSendDtmfInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSendDtmfInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnSendDtmfInner data size error");
        return TELEPHONY_ERR_FAIL;
    }

    char pDtmf = data.ReadInt8();
    auto pCallInfo = (CellularCallInfo *)data.ReadRawData(sizeof(CellularCallInfo));
    if (pCallInfo == nullptr) {
        TELEPHONY_LOGE("OnSendDtmfInner return, pCallInfo is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    reply.WriteInt32(SendDtmf(pDtmf, *pCallInfo));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnStartRttInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnStartRttInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnStartRttInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    std::string msg = data.ReadString();

    reply.WriteInt32(StartRtt(slotId, msg));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnStopRttInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnStopRttInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnStopRttInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();

    reply.WriteInt32(StopRtt(slotId));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSetCallTransferInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSetCallTransferInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnSetCallTransferInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    auto pCTInfo = (CallTransferInfo *)data.ReadRawData(sizeof(CallTransferInfo));
    if (pCTInfo == nullptr) {
        TELEPHONY_LOGE("OnSetCallTransferInner return, pCTInfo is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    reply.WriteInt32(SetCallTransferInfo(slotId, *pCTInfo));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnGetCallTransferInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnGetCallTransferInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnGetCallTransferInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    auto type = static_cast<CallTransferType>(data.ReadInt32());

    reply.WriteInt32(GetCallTransferInfo(slotId, type));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSetCallWaitingInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSetCallWaitingInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnSetCallWaitingInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    bool enable = data.ReadBool();

    reply.WriteInt32(SetCallWaiting(slotId, enable));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnGetCallWaitingInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnGetCallWaitingInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnGetCallWaitingInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    TELEPHONY_LOGI("CellularCallStub::OnGetCallWaitingInner data.ReadInt32()");
    reply.WriteInt32(GetCallWaiting(slotId));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSetCallRestrictionInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSetCallRestrictionInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnSetCallRestrictionInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    auto pCRInfo = (CallRestrictionInfo *)data.ReadRawData(sizeof(CallRestrictionInfo));
    if (pCRInfo == nullptr) {
        TELEPHONY_LOGE("OnSetCallRestrictionInner return, pCRInfo is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    reply.WriteInt32(SetCallRestriction(slotId, *pCRInfo));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnGetCallRestrictionInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnGetCallRestrictionInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnGetCallRestrictionInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    auto facType = static_cast<CallRestrictionType>(data.ReadInt32());

    reply.WriteInt32(GetCallRestriction(slotId, facType));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSetDomainPreferenceModeInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSetDomainPreferenceModeInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnSetDomainPreferenceModeInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    int32_t mode = data.ReadInt32();

    reply.WriteInt32(SetDomainPreferenceMode(slotId, mode));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnGetDomainPreferenceModeInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnGetDomainPreferenceModeInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnGetDomainPreferenceModeInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();

    reply.WriteInt32(GetDomainPreferenceMode(slotId));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSetImsSwitchStatusInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSetImsSwitchStatusInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnSetImsSwitchStatusInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    bool active = data.ReadBool();

    reply.WriteInt32(SetImsSwitchStatus(slotId, active));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnGetImsSwitchStatusInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnGetImsSwitchStatusInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnGetImsSwitchStatusInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    bool enabled;
    int32_t result = GetImsSwitchStatus(slotId, enabled);
    reply.WriteBool(enabled);
    reply.WriteInt32(result);
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSetImsConfigStringInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSetImsConfigStringInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnSetImsConfigStringInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    auto item = static_cast<ImsConfigItem>(data.ReadInt32());
    std::string value = data.ReadString();

    reply.WriteInt32(SetImsConfig(slotId, item, value));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSetImsConfigIntInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSetImsConfigIntInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnSetImsConfigIntInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    auto item = static_cast<ImsConfigItem>(data.ReadInt32());
    int32_t value = data.ReadInt32();

    reply.WriteInt32(SetImsConfig(slotId, item, value));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnGetImsConfigInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnGetImsConfigInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnGetImsConfigInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    auto item = static_cast<ImsConfigItem>(data.ReadInt32());

    reply.WriteInt32(GetImsConfig(slotId, item));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSetImsFeatureValueInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSetImsFeatureValueInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnSetImsFeatureValueInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    auto type = static_cast<FeatureType>(data.ReadInt32());
    int32_t value = data.ReadInt32();

    reply.WriteInt32(SetImsFeatureValue(slotId, type, value));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnGetImsFeatureValueInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnGetImsFeatureValueInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnGetImsFeatureValueInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    auto type = static_cast<FeatureType>(data.ReadInt32());

    reply.WriteInt32(GetImsFeatureValue(slotId, type));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnCtrlCameraInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnCtrlCameraInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnCtrlCameraInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    std::u16string cameraId = data.ReadString16();
    auto callingPid = IPCSkeleton::GetCallingPid();
    auto callingUid = IPCSkeleton::GetCallingUid();

    reply.WriteInt32(CtrlCamera(cameraId, callingUid, callingPid));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSetPreviewWindowInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSetPreviewWindowInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnSetPreviewWindowInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t x = data.ReadInt32();
    int32_t y = data.ReadInt32();
    int32_t z = data.ReadInt32();
    int32_t width = data.ReadInt32();
    int32_t height = data.ReadInt32();

    reply.WriteInt32(SetPreviewWindow(x, y, z, width, height));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSetDisplayWindowInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSetDisplayWindowInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnSetDisplayWindowInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t x = data.ReadInt32();
    int32_t y = data.ReadInt32();
    int32_t z = data.ReadInt32();
    int32_t width = data.ReadInt32();
    int32_t height = data.ReadInt32();

    reply.WriteInt32(SetDisplayWindow(x, y, z, width, height));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSetCameraZoomInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSetCameraZoomInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnSetCameraZoomInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    float zoomRatio = data.ReadFloat();

    reply.WriteInt32(SetCameraZoom(zoomRatio));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSetPauseImageInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSetPauseImageInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnSetPauseImageInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    std::u16string imagePath = data.ReadString16();

    reply.WriteInt32(SetPauseImage(imagePath));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSetDeviceDirectionInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSetDeviceDirectionInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnSetDeviceDirectionInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t rotation = data.ReadInt32();

    reply.WriteInt32(SetDeviceDirection(rotation));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnSetMuteInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnSetMuteInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnSetMuteInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();
    int32_t mute = data.ReadInt32();

    reply.WriteInt32(SetMute(slotId, mute));
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallStub::OnGetMuteInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("CellularCallStub::OnGetMuteInner entry");
    int32_t size = data.ReadInt32();
    size = ((size > MAX_SIZE) ? 0 : size);
    if (size <= 0) {
        TELEPHONY_LOGE("CellularCallStub::OnGetMuteInner data size error");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t slotId = data.ReadInt32();

    reply.WriteInt32(GetMute(slotId));
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
