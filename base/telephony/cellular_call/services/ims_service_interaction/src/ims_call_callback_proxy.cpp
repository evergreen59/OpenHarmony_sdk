/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "ims_call_callback_proxy.h"

#include "message_option.h"
#include "message_parcel.h"

namespace OHOS {
namespace Telephony {
ImsCallCallbackProxy::ImsCallCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<ImsCallCallbackInterface>(impl) {}

int32_t ImsCallCallbackProxy::DialResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_DIAL, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::HangUpResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_HANG_UP, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::RejectWithReasonResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_REJECT, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::AnswerResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_ANSWER, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::HoldCallResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_HOLD, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::UnHoldCallResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_UN_HOLD, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::SwitchCallResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_SWITCH, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::StartDtmfResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_START_DTMF, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::SendDtmfResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_SEND_DTMF, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::StopDtmfResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_STOP_DTMF, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::CallStateChangeReport(int32_t slotId)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_CALL_STATE_CHANGE, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::GetImsCallsDataResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_GET_CALLS_DATA, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::GetImsCallsDataResponse(int32_t slotId, const ImsCurrentCallList &callList)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&callList, sizeof(ImsCurrentCallList))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_GET_CALLS_DATA, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::SetImsSwitchResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_SET_SWITCH_STATUS, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::GetImsSwitchResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_GET_SWITCH_STATUS, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::GetImsSwitchResponse(int32_t slotId, int32_t active)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(active)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_GET_SWITCH_STATUS, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::CallRingBackReport(int32_t slotId, const RingbackVoice &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(RingbackVoice))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_CALL_CRING, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::SetMuteResponse(int32_t slotId, const MuteControlResponse &response)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&response, sizeof(MuteControlResponse))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_SET_MUTE, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::LastCallFailReasonResponse(int32_t slotId, const DisconnectedDetails &details)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&details, sizeof(DisconnectedDetails))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_LAST_CALL_FAIL_REASON, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::SetClipResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_SET_CALL_CLIP, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::GetClipResponse(int32_t slotId, const ClipResponse &response)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&response, sizeof(ClipResponse))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_GET_CALL_CLIP, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::GetClirResponse(int32_t slotId, const ClirResponse &response)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&response, sizeof(ClirResponse))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_GET_CALL_CLIR, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::SetClirResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_SET_CALL_CLIR, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::GetCallTransferResponse(int32_t slotId, const CallTransferResponse &response)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&response, sizeof(CallTransferResponse))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_GET_CALL_FORWARD, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::SetCallTransferResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_SET_CALL_FORWARD, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::GetCallRestrictionResponse(int32_t slotId, const CallRestrictionResponse &response)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&response, sizeof(CallRestrictionResponse))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_GET_CALL_RESTRICTION, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::SetCallRestrictionResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_SET_CALL_RESTRICTION, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::GetCallWaitingResponse(int32_t slotId, const CallWaitResponse &response)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&response, sizeof(CallWaitResponse))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_GET_CALL_WAIT, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::SetCallWaitingResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_SET_CALL_WAIT, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::SetColrResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_SET_CALL_COLR, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::GetColrResponse(int32_t slotId, const ColrResponse &response)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&response, sizeof(ColrResponse))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_GET_CALL_COLR, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::SetColpResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_SET_CALL_COLP, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsCallCallbackProxy::GetColpResponse(int32_t slotId, const ColpResponse &response)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsCallCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&response, sizeof(ColpResponse))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t error = remote->SendRequest(IMS_GET_CALL_COLP, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}
} // namespace Telephony
} // namespace OHOS
