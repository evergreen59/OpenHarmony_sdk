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

#include "ims_call_proxy.h"

#include "cellular_call_hisysevent.h"
#include "message_option.h"
#include "message_parcel.h"
#include "telephony_errors.h"
#include "telephony_permission.h"

namespace OHOS {
namespace Telephony {
int32_t ImsCallProxy::Dial(const ImsCallInfo &callInfo, CLIRMode mode)
{
    if (!TelephonyPermission::CheckPermission(Permission::PLACE_CALL)) {
        TELEPHONY_LOGE("[slot%{public}d]Permission denied!", callInfo.slotId);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    MessageParcel in;
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("[slot%{public}d]Write descriptor token fail!", callInfo.slotId);
        CellularCallHiSysEvent::WriteDialCallFaultEvent(callInfo.slotId, INVALID_PARAMETER, callInfo.videoState,
            TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL, "ims call proxy write descriptor token fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(ImsCallInfo))) {
        TELEPHONY_LOGE("[slot%{public}d]Write callInfo fail!", callInfo.slotId);
        CellularCallHiSysEvent::WriteDialCallFaultEvent(callInfo.slotId, INVALID_PARAMETER, callInfo.videoState,
            TELEPHONY_ERR_WRITE_DATA_FAIL, "ims call proxy write data fail");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(mode)) {
        TELEPHONY_LOGE("[slot%{public}d]Write mode fail!", callInfo.slotId);
        CellularCallHiSysEvent::WriteDialCallFaultEvent(callInfo.slotId, INVALID_PARAMETER, callInfo.videoState,
            TELEPHONY_ERR_WRITE_DATA_FAIL, "ims call proxy write data fail");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("[slot%{public}d]Remote is null", callInfo.slotId);
        CellularCallHiSysEvent::WriteDialCallFaultEvent(callInfo.slotId, INVALID_PARAMETER, callInfo.videoState,
            TELEPHONY_ERR_LOCAL_PTR_NULL, "ims call proxy remote is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    MessageParcel out;
    MessageOption option;
    int32_t error = remote->SendRequest(IMS_DIAL, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    TELEPHONY_LOGE("[slot%{public}d]SendRequest fail, error:%{public}d", callInfo.slotId, error);
    CellularCallHiSysEvent::WriteDialCallFaultEvent(callInfo.slotId, INVALID_PARAMETER, callInfo.videoState,
        static_cast<int32_t>(CallErrorCode::CALL_ERROR_SEND_REQUEST_FAIL), "ims call proxy send request fail");
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t ImsCallProxy::HangUp(const ImsCallInfo &callInfo)
{
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("[slot%{public}d]Permission denied!", callInfo.slotId);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    MessageParcel in;
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("[slot%{public}d]Write descriptor token fail!", callInfo.slotId);
        CellularCallHiSysEvent::WriteHangUpFaultEvent(callInfo.slotId, INVALID_PARAMETER,
            TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL, "HangUp ims call proxy write descriptor token fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(ImsCallInfo))) {
        TELEPHONY_LOGE("[slot%{public}d]Write callInfo fail!", callInfo.slotId);
        CellularCallHiSysEvent::WriteHangUpFaultEvent(callInfo.slotId, INVALID_PARAMETER,
            TELEPHONY_ERR_WRITE_DATA_FAIL, "HangUp ims call proxy write data fail");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("[slot%{public}d]Remote is null", callInfo.slotId);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    MessageParcel out;
    MessageOption option;
    int32_t error = remote->SendRequest(IMS_HANG_UP, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    TELEPHONY_LOGE("[slot%{public}d]SendRequest fail, error:%{public}d", callInfo.slotId, error);
    CellularCallHiSysEvent::WriteHangUpFaultEvent(callInfo.slotId, INVALID_PARAMETER,
        static_cast<int32_t>(CallErrorCode::CALL_ERROR_SEND_REQUEST_FAIL), "HangUp ims call proxy send request fail");
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t ImsCallProxy::RejectWithReason(const ImsCallInfo &callInfo, const ImsRejectReason &reason)
{
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("[slot%{public}d]Permission denied!", callInfo.slotId);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    MessageParcel in;
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("[slot%{public}d]Write descriptor token fail!", callInfo.slotId);
        CellularCallHiSysEvent::WriteHangUpFaultEvent(callInfo.slotId, INVALID_PARAMETER,
            TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL, "Reject ims call proxy write descriptor token fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(ImsCallInfo))) {
        TELEPHONY_LOGE("[slot%{public}d]Write callInfo fail!", callInfo.slotId);
        CellularCallHiSysEvent::WriteHangUpFaultEvent(callInfo.slotId, INVALID_PARAMETER,
            TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL, "Reject ims call proxy write data fail");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(reason)) {
        TELEPHONY_LOGE("[slot%{public}d]Write reason fail!", callInfo.slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("[slot%{public}d]Remote is null", callInfo.slotId);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    MessageParcel out;
    MessageOption option;
    int32_t error = remote->SendRequest(IMS_REJECT_WITH_REASON, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    TELEPHONY_LOGE("[slot%{public}d]SendRequest fail, error:%{public}d", callInfo.slotId, error);
    CellularCallHiSysEvent::WriteHangUpFaultEvent(callInfo.slotId, INVALID_PARAMETER,
        static_cast<int32_t>(CallErrorCode::CALL_ERROR_SEND_REQUEST_FAIL), "Reject ims call proxy send request fail");
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t ImsCallProxy::Answer(const ImsCallInfo &callInfo)
{
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("[slot%{public}d]Permission denied!", callInfo.slotId);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    MessageParcel in;
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("[slot%{public}d]Write descriptor token fail!", callInfo.slotId);
        CellularCallHiSysEvent::WriteAnswerCallFaultEvent(callInfo.slotId, INVALID_PARAMETER, callInfo.videoState,
            TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL, "ims call proxy write descriptor token fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(ImsCallInfo))) {
        TELEPHONY_LOGE("[slot%{public}d]Write callInfo fail!", callInfo.slotId);
        CellularCallHiSysEvent::WriteAnswerCallFaultEvent(callInfo.slotId, INVALID_PARAMETER, callInfo.videoState,
            TELEPHONY_ERR_WRITE_DATA_FAIL, "ims call proxy write data fail");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("[slot%{public}d]Remote is null", callInfo.slotId);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    MessageParcel out;
    MessageOption option;
    int32_t error = remote->SendRequest(IMS_ANSWER, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    TELEPHONY_LOGE("[slot%{public}d]SendRequest fail, error:%{public}d", callInfo.slotId, error);
    CellularCallHiSysEvent::WriteAnswerCallFaultEvent(callInfo.slotId, INVALID_PARAMETER, callInfo.videoState,
        static_cast<int32_t>(CallErrorCode::CALL_ERROR_SEND_REQUEST_FAIL), "ims call proxy send request fail");
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t ImsCallProxy::HoldCall(int32_t slotId, int32_t callType)
{
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("[slot%{public}d]Permission denied!", slotId);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId, callType);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    return SendRequest(in, slotId, IMS_HOLD);
}

int32_t ImsCallProxy::UnHoldCall(int32_t slotId, int32_t callType)
{
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("[slot%{public}d]Permission denied!", slotId);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId, callType);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    return SendRequest(in, slotId, IMS_UN_HOLD);
}

int32_t ImsCallProxy::SwitchCall(int32_t slotId, int32_t callType)
{
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("[slot%{public}d]Permission denied!", slotId);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId, callType);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    return SendRequest(in, slotId, IMS_SWITCH);
}

int32_t ImsCallProxy::CombineConference(int32_t slotId)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    return SendRequest(in, slotId, IMS_COMBINE_CONFERENCE);
}

int32_t ImsCallProxy::InviteToConference(int32_t slotId, const std::vector<std::string> &numberList)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteStringVector(numberList)) {
        TELEPHONY_LOGE("[slot%{public}d]Write numberList fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_INVITE_TO_CONFERENCE);
}

int32_t ImsCallProxy::KickOutFromConference(int32_t slotId, const std::vector<std::string> &numberList)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteStringVector(numberList)) {
        TELEPHONY_LOGE("[slot%{public}d]Write numberList fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_KICK_OUT_CONFERENCE);
}

int32_t ImsCallProxy::UpdateImsCallMode(const ImsCallInfo &callInfo, ImsCallMode mode)
{
    MessageParcel in;
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("[slot%{public}d]Write descriptor token fail!", callInfo.slotId);
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(ImsCallInfo))) {
        TELEPHONY_LOGE("[slot%{public}d]Write callInfo fail!", callInfo.slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(mode)) {
        TELEPHONY_LOGE("[slot%{public}d]Write mode fail!", callInfo.slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, callInfo.slotId, IMS_UPDATE_CALL_MEDIA_MODE);
}

int32_t ImsCallProxy::GetImsCallsDataRequest(int32_t slotId, int64_t lastCallsDataFlag)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteInt64(lastCallsDataFlag)) {
        TELEPHONY_LOGE("[slot%{public}d]Write lastCallsDataFlag fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_GET_CALL_DATA);
}

int32_t ImsCallProxy::GetLastCallFailReason(int32_t slotId)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    return SendRequest(in, slotId, IMS_GET_LAST_CALL_FAIL_REASON);
}

int32_t ImsCallProxy::StartDtmf(int32_t slotId, char cDtmfCode, int32_t index)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteInt8(cDtmfCode)) {
        TELEPHONY_LOGE("[slot%{public}d]Write cDtmfCode fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(index)) {
        TELEPHONY_LOGE("[slot%{public}d]Write index fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_START_DTMF);
}

int32_t ImsCallProxy::SendDtmf(int32_t slotId, char cDtmfCode, int32_t index)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteInt8(cDtmfCode)) {
        TELEPHONY_LOGE("[slot%{public}d]Write cDtmfCode fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(index)) {
        TELEPHONY_LOGE("[slot%{public}d]Write index fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_SEND_DTMF);
}

int32_t ImsCallProxy::StopDtmf(int32_t slotId, int32_t index)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteInt32(index)) {
        TELEPHONY_LOGE("[slot%{public}d]Write index fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_STOP_DTMF);
}

int32_t ImsCallProxy::StartRtt(int32_t slotId, const std::string &msg)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteString(msg)) {
        TELEPHONY_LOGE("[slot%{public}d]Write msg fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_START_RTT);
}

int32_t ImsCallProxy::StopRtt(int32_t slotId)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    return SendRequest(in, slotId, IMS_STOP_RTT);
}

int32_t ImsCallProxy::SetDomainPreferenceMode(int32_t slotId, int32_t mode)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("[slot%{public}d]Permission denied!", slotId);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteInt32(mode)) {
        TELEPHONY_LOGE("[slot%{public}d]Write mode fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_SET_DOMAIN_PREFERENCE_MODE);
}

int32_t ImsCallProxy::GetDomainPreferenceMode(int32_t slotId)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    return SendRequest(in, slotId, IMS_GET_DOMAIN_PREFERENCE_MODE);
}

int32_t ImsCallProxy::SetImsSwitchStatus(int32_t slotId, int32_t active)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteInt32(active)) {
        TELEPHONY_LOGE("[slot%{public}d]Write active fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_SET_SWITCH_STATUS);
}

int32_t ImsCallProxy::GetImsSwitchStatus(int32_t slotId)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    return SendRequest(in, slotId, IMS_GET_SWITCH_STATUS);
}

int32_t ImsCallProxy::SetImsConfig(ImsConfigItem item, const std::string &value)
{
    MessageParcel in;
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("Write descriptor token fail!");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(item)) {
        TELEPHONY_LOGE("Write item fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteString(value)) {
        TELEPHONY_LOGE("Write value fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, IMS_SET_IMS_CONFIG_STRING);
}

int32_t ImsCallProxy::SetImsConfig(ImsConfigItem item, int32_t value)
{
    MessageParcel in;
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("Write descriptor token fail!");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(item)) {
        TELEPHONY_LOGE("Write item fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(value)) {
        TELEPHONY_LOGE("Write value fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, IMS_SET_IMS_CONFIG_INT);
}

int32_t ImsCallProxy::GetImsConfig(ImsConfigItem item)
{
    MessageParcel in;
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("Write descriptor token fail!");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(item)) {
        TELEPHONY_LOGE("Write item fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, IMS_GET_IMS_CONFIG);
}

int32_t ImsCallProxy::SetImsFeatureValue(FeatureType type, int32_t value)
{
    MessageParcel in;
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("Write descriptor token fail!");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(type)) {
        TELEPHONY_LOGE("Write type fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(value)) {
        TELEPHONY_LOGE("Write value fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, IMS_SET_IMS_FEATURE);
}

int32_t ImsCallProxy::GetImsFeatureValue(FeatureType type, int32_t &value)
{
    MessageParcel in;
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("Write descriptor token fail!");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(type)) {
        TELEPHONY_LOGE("Write type fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, IMS_GET_IMS_FEATURE);
}

int32_t ImsCallProxy::SetMute(int32_t slotId, int32_t mute)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteInt32(mute)) {
        TELEPHONY_LOGE("[slot%{public}d]Write mute fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_SET_MUTE);
}

int32_t ImsCallProxy::GetMute(int32_t slotId)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    return SendRequest(in, slotId, IMS_GET_MUTE);
}

int32_t ImsCallProxy::CtrlCamera(const std::u16string &cameraId, int32_t callingUid, int32_t callingPid)
{
    MessageParcel in;
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("Write descriptor token fail!");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteString16(cameraId)) {
        TELEPHONY_LOGE("Write cameraId fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(callingUid)) {
        TELEPHONY_LOGE("Write callingUid fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(callingPid)) {
        TELEPHONY_LOGE("Write callingPid fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, IMS_CTRL_CAMERA);
}

int32_t ImsCallProxy::SetPreviewWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height)
{
    MessageParcel in;
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("Write descriptor token fail!");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(x)) {
        TELEPHONY_LOGE("Write x fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(y)) {
        TELEPHONY_LOGE("Write y fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(z)) {
        TELEPHONY_LOGE("Write z fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(width)) {
        TELEPHONY_LOGE("Write width fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(height)) {
        TELEPHONY_LOGE("Write height fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, IMS_SET_PREVIEW_WINDOW);
}

int32_t ImsCallProxy::SetDisplayWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height)
{
    MessageParcel in;
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("Write descriptor token fail!");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(x)) {
        TELEPHONY_LOGE("Write x fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(y)) {
        TELEPHONY_LOGE("Write y fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(z)) {
        TELEPHONY_LOGE("Write z fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(width)) {
        TELEPHONY_LOGE("Write width fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(height)) {
        TELEPHONY_LOGE("Write height fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, IMS_SET_DISPLAY_WINDOW);
}

int32_t ImsCallProxy::SetCameraZoom(float zoomRatio)
{
    MessageParcel in;
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("Write descriptor token fail!");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteFloat(zoomRatio)) {
        TELEPHONY_LOGE("Write zoomRatio fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, IMS_SET_CAMERA_ZOOM);
}

int32_t ImsCallProxy::SetPauseImage(const std::u16string &path)
{
    MessageParcel in;
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("Write descriptor token fail!");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteString16(path)) {
        TELEPHONY_LOGE("Write path fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, IMS_SET_PAUSE_IMAGE);
}

int32_t ImsCallProxy::SetDeviceDirection(int32_t rotation)
{
    MessageParcel in;
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("Write descriptor token fail!");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(rotation)) {
        TELEPHONY_LOGE("Write rotation fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, IMS_SET_DEVICE_DIRECTION);
}

int32_t ImsCallProxy::SetClip(int32_t slotId, int32_t action)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteInt32(action)) {
        TELEPHONY_LOGE("[slot%{public}d]Write action fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_SET_CLIP);
}

int32_t ImsCallProxy::GetClip(int32_t slotId)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    return SendRequest(in, slotId, IMS_GET_CLIP);
}

int32_t ImsCallProxy::SetClir(int32_t slotId, int32_t action)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteInt32(action)) {
        TELEPHONY_LOGE("[slot%{public}d]Write action fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_SET_CLIR);
}

int32_t ImsCallProxy::GetClir(int32_t slotId)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    return SendRequest(in, slotId, IMS_GET_CLIR);
}

int32_t ImsCallProxy::SetCallTransfer(
    int32_t slotId, int32_t reason, int32_t mode, const std::string &transferNum, int32_t classType)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("[slot%{public}d]Permission denied!", slotId);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteInt32(reason)) {
        TELEPHONY_LOGE("[slot%{public}d]Write reason fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(mode)) {
        TELEPHONY_LOGE("[slot%{public}d]Write mode fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteString(transferNum)) {
        TELEPHONY_LOGE("[slot%{public}d]Write transferNum fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(classType)) {
        TELEPHONY_LOGE("[slot%{public}d]Write classType fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_SET_CALL_TRANSFER);
}

int32_t ImsCallProxy::GetCallTransfer(int32_t slotId, int32_t reason)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("[slot%{public}d]Permission denied!", slotId);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteInt32(reason)) {
        TELEPHONY_LOGE("[slot%{public}d]Write reason fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_GET_CALL_TRANSFER);
}

int32_t ImsCallProxy::SetCallRestriction(int32_t slotId, const std::string &fac, int32_t mode, const std::string &pw)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("[slot%{public}d]Permission denied!", slotId);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteString(fac)) {
        TELEPHONY_LOGE("[slot%{public}d]Write fac fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(mode)) {
        TELEPHONY_LOGE("[slot%{public}d]Write mode fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteString(pw)) {
        TELEPHONY_LOGE("[slot%{public}d]Write pw fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_SET_CALL_RESTRICTION);
}

int32_t ImsCallProxy::GetCallRestriction(int32_t slotId, const std::string &fac)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("[slot%{public}d]Permission denied!", slotId);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteString(fac)) {
        TELEPHONY_LOGE("[slot%{public}d]Write fac fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_GET_CALL_RESTRICTION);
}

int32_t ImsCallProxy::SetCallWaiting(int32_t slotId, bool activate, int32_t classType)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("[slot%{public}d]Permission denied!", slotId);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteBool(activate)) {
        TELEPHONY_LOGE("[slot%{public}d]Write activate fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(classType)) {
        TELEPHONY_LOGE("[slot%{public}d]Write classType fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_SET_CALL_WAITING);
}

int32_t ImsCallProxy::GetCallWaiting(int32_t slotId)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("[slot%{public}d]Permission denied!", slotId);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    return SendRequest(in, slotId, IMS_GET_CALL_WAITING);
}

int32_t ImsCallProxy::SetColr(int32_t slotId, int32_t presentation)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteInt32(presentation)) {
        TELEPHONY_LOGE("[slot%{public}d]Write presentation fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_SET_COLR);
}

int32_t ImsCallProxy::GetColr(int32_t slotId)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    return SendRequest(in, slotId, IMS_GET_COLR);
}

int32_t ImsCallProxy::SetColp(int32_t slotId, int32_t action)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteInt32(action)) {
        TELEPHONY_LOGE("[slot%{public}d]Write action fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_SET_COLP);
}

int32_t ImsCallProxy::GetColp(int32_t slotId)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    return SendRequest(in, slotId, IMS_GET_COLP);
}

int32_t ImsCallProxy::RegisterImsCallCallback(const sptr<ImsCallCallbackInterface> &callback)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callback == nullptr) {
        TELEPHONY_LOGE("callback is nullptr!");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    MessageParcel in;
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("Write descriptor token fail!");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteRemoteObject(callback->AsObject().GetRefPtr())) {
        TELEPHONY_LOGE("Write ImsCallCallbackInterface fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, IMS_CALL_REGISTER_CALLBACK);
}

int32_t ImsCallProxy::UpdateImsCapabilities(int32_t slotId, const ImsCapabilityList &imsCapabilityList)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteRawData((const void *)&imsCapabilityList, sizeof(ImsCapabilityList))) {
        TELEPHONY_LOGE("[slot%{public}d]Write imsCapabilityList fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_UPDATE_CAPABILITY);
}

int32_t ImsCallProxy::WriteCommonInfo(std::string funcName, MessageParcel &in, int32_t slotId)
{
    if (!in.WriteInterfaceToken(ImsCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("[slot%{public}d] %{public}s Write descriptor token fail!", slotId, funcName.c_str());
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        TELEPHONY_LOGE("[slot%{public}d] %{public}s Write slotId fail!", slotId, funcName.c_str());
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t ImsCallProxy::WriteCommonInfo(std::string funcName, MessageParcel &in, int32_t slotId, int32_t callType)
{
    int32_t ret = WriteCommonInfo(funcName, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteInt32(callType)) {
        TELEPHONY_LOGE("[slot%{public}d] %{public}s Write callType fail!", slotId, funcName.c_str());
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t ImsCallProxy::SendRequest(MessageParcel &in, int32_t eventId)
{
    MessageParcel out;
    MessageOption option;

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("Remote is null, eventId:%{public}d", eventId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }

    int32_t error = remote->SendRequest(eventId, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    TELEPHONY_LOGE("SendRequest fail, eventId:%{public}d, error:%{public}d", eventId, error);
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t ImsCallProxy::SendRequest(MessageParcel &in, int32_t slotId, int32_t eventId)
{
    MessageParcel out;
    MessageOption option;

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("[slot%{public}d]Remote is null, eventId:%{public}d", slotId, eventId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }

    int32_t error = remote->SendRequest(eventId, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    TELEPHONY_LOGE("[slot%{public}d]SendRequest fail, eventId:%{public}d, error:%{public}d", slotId, eventId, error);
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}
} // namespace Telephony
} // namespace OHOS