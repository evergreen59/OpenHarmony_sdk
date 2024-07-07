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

#include "cellular_call_connection_ims.h"

#include "cellular_call_hisysevent.h"
#include "cellular_call_service.h"
#include "ims_call_client.h"
#include "radio_event.h"
#include "securec.h"

namespace OHOS {
namespace Telephony {
int32_t CellularCallConnectionIMS::DialRequest(int32_t slotId, const ImsDialInfoStruct &dialRequest)
{
    TELEPHONY_LOGI("call ims service");
    ImsCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        TELEPHONY_LOGE("return, memset_s error.");
        CellularCallHiSysEvent::WriteDialCallFaultEvent(
            slotId, INVALID_PARAMETER, dialRequest.videoState, TELEPHONY_ERR_MEMSET_FAIL, "memset_s error");
        return TELEPHONY_ERR_MEMSET_FAIL;
    }
    size_t cpyLen = strlen(dialRequest.phoneNum.c_str()) + 1;
    if (cpyLen > static_cast<size_t>(kMaxNumberLength)) {
        return TELEPHONY_ERR_STRCPY_FAIL;
    }
    if (strcpy_s(callInfo.phoneNum, cpyLen, dialRequest.phoneNum.c_str()) != EOK) {
        TELEPHONY_LOGE("return, strcpy_s fail.");
        CellularCallHiSysEvent::WriteDialCallFaultEvent(
            slotId, INVALID_PARAMETER, dialRequest.videoState, TELEPHONY_ERR_STRCPY_FAIL, "strcpy_s fail");
        return TELEPHONY_ERR_STRCPY_FAIL;
    }
    callInfo.videoState = dialRequest.videoState;
    callInfo.slotId = slotId;
    if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
        TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
        CellularCallHiSysEvent::WriteDialCallFaultEvent(slotId, INVALID_PARAMETER, dialRequest.videoState,
            CALL_ERR_RESOURCE_UNAVAILABLE, "ims vendor service does not exist");
        return CALL_ERR_RESOURCE_UNAVAILABLE;
    }
    return DelayedSingleton<ImsCallClient>::GetInstance()->Dial(callInfo, dialRequest.clirMode);
}

int32_t CellularCallConnectionIMS::HangUpRequest(int32_t slotId, const std::string &phoneNum, int32_t index)
{
    if (moduleUtils_.NeedCallImsService()) {
        TELEPHONY_LOGI("call ims service");
        ImsCallInfo callInfo;
        if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
            TELEPHONY_LOGE("return, memset_s error.");
            return TELEPHONY_ERR_MEMSET_FAIL;
        }
        if (static_cast<int32_t>(phoneNum.length() + 1) > kMaxNumberLength) {
            return TELEPHONY_ERR_STRCPY_FAIL;
        }
        if (strcpy_s(callInfo.phoneNum, strlen(phoneNum.c_str()) + 1, phoneNum.c_str()) != EOK) {
            TELEPHONY_LOGE("return, strcpy_s fail.");
            return TELEPHONY_ERR_STRCPY_FAIL;
        }
        callInfo.slotId = slotId;
        callInfo.index = index;
        if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
            TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
            return CALL_ERR_RESOURCE_UNAVAILABLE;
        }
        return DelayedSingleton<ImsCallClient>::GetInstance()->HangUp(callInfo);
    }
    TELEPHONY_LOGE("ims vendor service does not exist.");
    CellularCallHiSysEvent::WriteHangUpFaultEvent(slotId, INVALID_PARAMETER,
        static_cast<int32_t>(CallErrorCode::CALL_ERROR_IMS_SERVICE_NOT_EXIST),
        "HangUpRequest ims vendor service does not exist");
    return TELEPHONY_ERROR;
}

int32_t CellularCallConnectionIMS::AnswerRequest(
    int32_t slotId, const std::string &phoneNum, int32_t videoState, int32_t index)
{
    if (moduleUtils_.NeedCallImsService()) {
        TELEPHONY_LOGI("call ims service");
        ImsCallInfo callInfo;
        if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
            TELEPHONY_LOGE("return, memset_s error.");
            return TELEPHONY_ERR_MEMSET_FAIL;
        }
        if (static_cast<int32_t>(phoneNum.length() + 1) > kMaxNumberLength) {
            return TELEPHONY_ERR_STRCPY_FAIL;
        }
        if (strcpy_s(callInfo.phoneNum, strlen(phoneNum.c_str()) + 1, phoneNum.c_str()) != EOK) {
            TELEPHONY_LOGE("return, strcpy_s fail.");
            return TELEPHONY_ERR_STRCPY_FAIL;
        }
        callInfo.videoState = videoState;
        callInfo.slotId = slotId;
        callInfo.index = index;
        if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
            TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
            return CALL_ERR_RESOURCE_UNAVAILABLE;
        }
        return DelayedSingleton<ImsCallClient>::GetInstance()->Answer(callInfo);
    }
    TELEPHONY_LOGE("ims vendor service does not exist.");
    CellularCallHiSysEvent::WriteAnswerCallFaultEvent(slotId, INVALID_PARAMETER, INVALID_PARAMETER,
        static_cast<int32_t>(CallErrorCode::CALL_ERROR_IMS_SERVICE_NOT_EXIST), "ims vendor service does not exist");
    return TELEPHONY_ERROR;
}

int32_t CellularCallConnectionIMS::RejectRequest(int32_t slotId, const std::string &phoneNum, int32_t index)
{
    if (moduleUtils_.NeedCallImsService()) {
        TELEPHONY_LOGI("call ims service");
        ImsCallInfo callInfo;
        if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
            TELEPHONY_LOGE("return, memset_s error.");
            return TELEPHONY_ERR_MEMSET_FAIL;
        }
        if (static_cast<int32_t>(phoneNum.length() + 1) > kMaxNumberLength) {
            return TELEPHONY_ERR_STRCPY_FAIL;
        }
        if (strcpy_s(callInfo.phoneNum, strlen(phoneNum.c_str()) + 1, phoneNum.c_str()) != EOK) {
            TELEPHONY_LOGE("return, strcpy_s fail.");
            return TELEPHONY_ERR_STRCPY_FAIL;
        }
        callInfo.slotId = slotId;
        callInfo.index = index;
        if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
            TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
            return CALL_ERR_RESOURCE_UNAVAILABLE;
        }
        return DelayedSingleton<ImsCallClient>::GetInstance()->Reject(callInfo);
    }
    TELEPHONY_LOGE("ims vendor service does not exist.");
    CellularCallHiSysEvent::WriteHangUpFaultEvent(slotId, INVALID_PARAMETER,
        static_cast<int32_t>(CallErrorCode::CALL_ERROR_IMS_SERVICE_NOT_EXIST),
        "RejectRequest ims vendor service does not exist");
    return TELEPHONY_ERROR;
}

int32_t CellularCallConnectionIMS::HoldCallRequest(int32_t slotId)
{
    if (moduleUtils_.NeedCallImsService()) {
        TELEPHONY_LOGI("call ims service");
        if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
            TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
            return CALL_ERR_RESOURCE_UNAVAILABLE;
        }
        int32_t callType = static_cast<int32_t>(GetCallReportInfo().callMode);
        return DelayedSingleton<ImsCallClient>::GetInstance()->HoldCall(slotId, callType);
    }
    TELEPHONY_LOGE("ims vendor service does not exist.");
    return TELEPHONY_ERROR;
}

int32_t CellularCallConnectionIMS::UnHoldCallRequest(int32_t slotId)
{
    if (moduleUtils_.NeedCallImsService()) {
        TELEPHONY_LOGI("call ims service");
        if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
            TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
            return CALL_ERR_RESOURCE_UNAVAILABLE;
        }
        int32_t callType = static_cast<int32_t>(GetCallReportInfo().callMode);
        return DelayedSingleton<ImsCallClient>::GetInstance()->UnHoldCall(slotId, callType);
    }
    TELEPHONY_LOGE("ims vendor service does not exist.");
    return TELEPHONY_ERROR;
}

int32_t CellularCallConnectionIMS::SwitchCallRequest(int32_t slotId)
{
    if (moduleUtils_.NeedCallImsService()) {
        TELEPHONY_LOGI("call ims service");
        if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
            TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
            return CALL_ERR_RESOURCE_UNAVAILABLE;
        }
        int32_t callType = static_cast<int32_t>(GetCallReportInfo().callMode);
        return DelayedSingleton<ImsCallClient>::GetInstance()->SwitchCall(slotId, callType);
    }
    TELEPHONY_LOGE("ims vendor service does not exist.");
    return TELEPHONY_ERROR;
}

int32_t CellularCallConnectionIMS::CombineConferenceRequest(int32_t slotId, int32_t voiceCall)
{
    if (moduleUtils_.NeedCallImsService()) {
        TELEPHONY_LOGI("call ims service");
        if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
            TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
            return CALL_ERR_RESOURCE_UNAVAILABLE;
        }
        return DelayedSingleton<ImsCallClient>::GetInstance()->CombineConference(slotId);
    }
    TELEPHONY_LOGE("ims vendor service does not exist.");
    return TELEPHONY_ERROR;
}

int32_t CellularCallConnectionIMS::InviteToConferenceRequest(
    int32_t slotId, const std::vector<std::string> &numberList)
{
    if (moduleUtils_.NeedCallImsService()) {
        TELEPHONY_LOGI("call ims service");
        if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
            TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
            return CALL_ERR_RESOURCE_UNAVAILABLE;
        }
        return DelayedSingleton<ImsCallClient>::GetInstance()->InviteToConference(slotId, numberList);
    }
    TELEPHONY_LOGE("ims vendor service does not exist.");
    return TELEPHONY_ERROR;
}

int32_t CellularCallConnectionIMS::KickOutFromConferenceRequest(
    int32_t slotId, const std::vector<std::string> &numberList)
{
    if (moduleUtils_.NeedCallImsService()) {
        TELEPHONY_LOGI("call ims service");
        if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
            TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
            return CALL_ERR_RESOURCE_UNAVAILABLE;
        }
        return DelayedSingleton<ImsCallClient>::GetInstance()->KickOutFromConference(slotId, numberList);
    }
    TELEPHONY_LOGE("ims vendor service does not exist.");
    return TELEPHONY_ERROR;
}

int32_t CellularCallConnectionIMS::CallSupplementRequest(int32_t slotId, CallSupplementType type)
{
    TELEPHONY_LOGI("start");
    if (DelayedSingleton<CellularCallService>::GetInstance() == nullptr) {
        TELEPHONY_LOGE("return, error type: GetInstance() is nullptr.");
        return CALL_ERR_RESOURCE_UNAVAILABLE;
    }
    auto handle = DelayedSingleton<CellularCallService>::GetInstance()->GetHandler(slotId);
    if (handle == nullptr) {
        TELEPHONY_LOGE("return, error type: handle is nullptr.");
        return CALL_ERR_RESOURCE_UNAVAILABLE;
    }
    CoreManagerInner::GetInstance().CallSupplement(slotId, RadioEvent::RADIO_CALL_SUPPLEMENT, type, handle);
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnectionIMS::UpdateCallMediaModeRequest(const CellularCallInfo &callInfo, ImsCallMode mode)
{
    if (moduleUtils_.NeedCallImsService()) {
        TELEPHONY_LOGI("call ims service");
        ImsCallInfo imsCallInfo;
        if (memset_s(&imsCallInfo, sizeof(imsCallInfo), 0, sizeof(imsCallInfo)) != EOK) {
            TELEPHONY_LOGE("return, memset_s error.");
            return TELEPHONY_ERR_MEMSET_FAIL;
        }
        if (static_cast<int32_t>(strlen(callInfo.phoneNum) + 1) > kMaxNumberLength) {
            return TELEPHONY_ERR_STRCPY_FAIL;
        }
        errno_t result = strcpy_s(imsCallInfo.phoneNum, strlen(callInfo.phoneNum) + 1,
                                  callInfo.phoneNum);
        if (result != EOK) {
            TELEPHONY_LOGE("return, strcpy_s fail.");
            return TELEPHONY_ERR_STRCPY_FAIL;
        }
        imsCallInfo.slotId = callInfo.slotId;
        imsCallInfo.index = callInfo.index;
        if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
            TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
            return CALL_ERR_RESOURCE_UNAVAILABLE;
        }
        return DelayedSingleton<ImsCallClient>::GetInstance()->UpdateImsCallMode(imsCallInfo, mode);
    }
    TELEPHONY_LOGE("ims vendor service does not exist.");
    return TELEPHONY_ERROR;
}

int32_t CellularCallConnectionIMS::GetImsCallsDataRequest(int32_t slotId, int64_t lastCallsDataFlag)
{
    if (moduleUtils_.NeedCallImsService()) {
        TELEPHONY_LOGI("call ims service");
        if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
            TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
            return CALL_ERR_RESOURCE_UNAVAILABLE;
        }
        return DelayedSingleton<ImsCallClient>::GetInstance()->GetImsCallsDataRequest(slotId, lastCallsDataFlag);
    }
    TELEPHONY_LOGE("ims vendor service does not exist.");
    return TELEPHONY_ERROR;
}

int32_t CellularCallConnectionIMS::SendDtmfRequest(int32_t slotId, char cDtmfCode, int32_t index) const
{
    if (moduleUtils_.NeedCallImsService()) {
        TELEPHONY_LOGI("call ims service");
        if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
            TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
            return CALL_ERR_RESOURCE_UNAVAILABLE;
        }
        return DelayedSingleton<ImsCallClient>::GetInstance()->SendDtmf(slotId, cDtmfCode, index);
    }
    TELEPHONY_LOGE("ims vendor service does not exist.");
    return TELEPHONY_ERROR;
}

int32_t CellularCallConnectionIMS::StartDtmfRequest(int32_t slotId, char cDtmfCode, int32_t index) const
{
    if (moduleUtils_.NeedCallImsService()) {
        TELEPHONY_LOGI("call ims service");
        if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
            TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
            return CALL_ERR_RESOURCE_UNAVAILABLE;
        }
        return DelayedSingleton<ImsCallClient>::GetInstance()->StartDtmf(slotId, cDtmfCode, index);
    }
    TELEPHONY_LOGE("ims vendor service does not exist.");
    return TELEPHONY_ERROR;
}

int32_t CellularCallConnectionIMS::StopDtmfRequest(int32_t slotId, int32_t index) const
{
    if (moduleUtils_.NeedCallImsService()) {
        TELEPHONY_LOGI("call ims service");
        if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
            TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
            return CALL_ERR_RESOURCE_UNAVAILABLE;
        }
        return DelayedSingleton<ImsCallClient>::GetInstance()->StopDtmf(slotId, index);
    }
    TELEPHONY_LOGE("ims vendor service does not exist.");
    return TELEPHONY_ERROR;
}

int32_t CellularCallConnectionIMS::StartRttRequest(int32_t slotId, const std::string &msg)
{
    if (moduleUtils_.NeedCallImsService()) {
        TELEPHONY_LOGI("call ims service");
        if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
            TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
            return CALL_ERR_RESOURCE_UNAVAILABLE;
        }
        return DelayedSingleton<ImsCallClient>::GetInstance()->StartRtt(slotId, msg);
    }
    TELEPHONY_LOGE("ims vendor service does not exist.");
    return TELEPHONY_ERROR;
}

int32_t CellularCallConnectionIMS::StopRttRequest(int32_t slotId)
{
    if (moduleUtils_.NeedCallImsService()) {
        TELEPHONY_LOGI("call ims service");
        if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
            TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
            return CALL_ERR_RESOURCE_UNAVAILABLE;
        }
        return DelayedSingleton<ImsCallClient>::GetInstance()->StopRtt(slotId);
    }
    TELEPHONY_LOGE("ims vendor service does not exist.");
    return TELEPHONY_ERROR;
}

int32_t CellularCallConnectionIMS::GetCallFailReasonRequest(int32_t slotId) const
{
    if (moduleUtils_.NeedCallImsService()) {
        TELEPHONY_LOGI("call ims service");
        if (DelayedSingleton<ImsCallClient>::GetInstance() == nullptr) {
            TELEPHONY_LOGE("return, ImsCallClient is nullptr.");
            return CALL_ERR_RESOURCE_UNAVAILABLE;
        }
        return DelayedSingleton<ImsCallClient>::GetInstance()->GetLastCallFailReason(slotId);
    }
    TELEPHONY_LOGE("ims vendor service does not exist.");
    return TELEPHONY_ERROR;
}
} // namespace Telephony
} // namespace OHOS
