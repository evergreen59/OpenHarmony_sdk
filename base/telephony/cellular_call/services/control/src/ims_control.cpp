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

#include "ims_control.h"

#include "cellular_call_hisysevent.h"
#include "cellular_call_register.h"
#include "emergency_utils.h"
#include "module_service_utils.h"
#include "securec.h"
#include "standardize_utils.h"

namespace OHOS {
namespace Telephony {
IMSControl::~IMSControl()
{
    TELEPHONY_LOGI("~IMSControl start");
    ReleaseAllConnection();
}

int32_t IMSControl::Dial(const CellularCallInfo &callInfo)
{
    TELEPHONY_LOGI("Dial start");
    DelayedSingleton<CellularCallHiSysEvent>::GetInstance()->SetCallParameterInfo(
        callInfo.slotId, static_cast<int32_t>(callInfo.callType), callInfo.videoState);
    int32_t ret = DialPreJudgment(callInfo);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }

    // sip uri needs to remove separator
    std::string newPhoneNum(callInfo.phoneNum);
    StandardizeUtils standardizeUtils;
    if (newPhoneNum.find('@') != std::string::npos || newPhoneNum.find("%40") != std::string::npos) {
        newPhoneNum = standardizeUtils.RemoveSeparatorsPhoneNumber(newPhoneNum);
    }

    CLIRMode clirMode = CLIRMode::DEFAULT;
    if (IsNeedExecuteMMI(callInfo.slotId, newPhoneNum, clirMode)) {
        TELEPHONY_LOGI("Dial return, mmi code type.");
        return RETURN_TYPE_MMI;
    }
    return DialJudgment(callInfo.slotId, newPhoneNum, clirMode, callInfo.videoState);
}

int32_t IMSControl::DialJudgment(int32_t slotId, const std::string &phoneNum, CLIRMode &clirMode, int32_t videoState)
{
    TELEPHONY_LOGI("DialJudgment entry.");
    if (!CanCall(connectionMap_)) {
        TELEPHONY_LOGE("DialJudgment return, error type: call state error.");
        CellularCallHiSysEvent::WriteDialCallFaultEvent(
            slotId, INVALID_PARAMETER, videoState, CALL_ERR_CALL_COUNTS_EXCEED_LIMIT, "ims dial call state error");
        return CALL_ERR_CALL_COUNTS_EXCEED_LIMIT;
    }

    // Calls can be put on hold, recovered, released, added to conversation,
    // and transferred similarly as defined in 3GPP TS 22.030 [19].
    if (IsInState(connectionMap_, TelCallState::CALL_STATUS_ACTIVE)) {
        // New calls must be active, so other calls need to be hold
        TELEPHONY_LOGI("DialJudgment, have connection in active state.");
        CellularCallConnectionIMS connection;
        // - a call can be temporarily disconnected from the ME but the connection is retained by the network
        connection.HoldCallRequest(slotId);
    }
    return EncapsulateDial(slotId, phoneNum, clirMode, videoState);
}

int32_t IMSControl::EncapsulateDial(
    int32_t slotId, const std::string &phoneNum, CLIRMode &clirMode, int32_t videoState) const
{
    TELEPHONY_LOGI("EncapsulateDial start");

    ImsDialInfoStruct dialInfo;
    dialInfo.videoState = videoState;
    dialInfo.bEmergencyCall = false;
    EmergencyUtils emergencyUtils;
    emergencyUtils.IsEmergencyCall(slotId, phoneNum, dialInfo.bEmergencyCall);

    /**
     * <idx>: integer type;
     * call identification number as described in 3GPP TS 22.030 [19] subclause 4.5.5.1
     * this number can be used in +CHLD command operations
     * <dir>:
     */
    dialInfo.phoneNum = phoneNum;
    /**
     * <n> (parameter sets the adjustment for outgoing calls):
     *  0	presentation indicator is used according to the subscription of the CLIR service
     *  1	CLIR invocation
     *  2	CLIR suppression
     */
    dialInfo.clirMode = clirMode;
    /**
     * An example of voice group call service request usage:
     * ATD*17*753#500; (originate voice group call with the priority level 3)
     * OK (voice group call setup was successful)
     */

    CellularCallConnectionIMS cellularCallConnectionIms;
    return cellularCallConnectionIms.DialRequest(slotId, dialInfo);
}

int32_t IMSControl::HangUp(const CellularCallInfo &callInfo, CallSupplementType type)
{
    TELEPHONY_LOGI("HangUp start");
    switch (type) {
        case CallSupplementType::TYPE_DEFAULT: {
            auto pConnection =
                GetConnectionData<ImsConnectionMap &, CellularCallConnectionIMS *>(connectionMap_, callInfo.phoneNum);
            if (pConnection == nullptr) {
                TELEPHONY_LOGI("HangUp: connection cannot be matched, use index directly");
                pConnection = FindConnectionByIndex<ImsConnectionMap &, CellularCallConnectionIMS *>(
                    connectionMap_, callInfo.index);
            }
            if (pConnection == nullptr) {
                TELEPHONY_LOGE("HangUp return, error type: connection is null");
                return CALL_ERR_CALL_CONNECTION_NOT_EXIST;
            }

            if (DelayedSingleton<CellularCallRegister>::GetInstance() != nullptr) {
                DelayedSingleton<CellularCallRegister>::GetInstance()->ReportSingleCallInfo(
                    pConnection->GetCallReportInfo(), TelCallState::CALL_STATUS_DISCONNECTING);
            }
            return pConnection->HangUpRequest(callInfo.slotId, callInfo.phoneNum, callInfo.index);
        }
        case CallSupplementType::TYPE_HANG_UP_HOLD_WAIT:
            // release the second (active) call and recover the first (held) call
        case CallSupplementType::TYPE_HANG_UP_ACTIVE: {
            CellularCallConnectionIMS connection;
            return connection.CallSupplementRequest(callInfo.slotId, type);
        }
        case CallSupplementType::TYPE_HANG_UP_ALL: {
            TELEPHONY_LOGI("HangUp, hang up all call");
            CellularCallConnectionIMS connection;
            // The AT command for hanging up all calls is the same as the AT command for rejecting calls,
            // so the reject interface is reused.
            return connection.RejectRequest(callInfo.slotId, callInfo.phoneNum, callInfo.index);
        }
        default: {
            TELEPHONY_LOGE("HangUp warring, type is invalid");
            return TELEPHONY_ERR_ARGUMENT_INVALID;
        }
    }
}

int32_t IMSControl::Answer(const CellularCallInfo &callInfo)
{
    TELEPHONY_LOGI("IMSControl::Answer start");
    auto pConnection =
        GetConnectionData<ImsConnectionMap &, CellularCallConnectionIMS *>(connectionMap_, callInfo.phoneNum);
    if (pConnection == nullptr) {
        TELEPHONY_LOGE("IMSControl::Answer, error type: connection is null");
        return CALL_ERR_CALL_CONNECTION_NOT_EXIST;
    }
    if (IsInState(connectionMap_, TelCallState::CALL_STATUS_ACTIVE) &&
        pConnection->GetStatus() == TelCallState::CALL_STATUS_WAITING) {
        TELEPHONY_LOGI("Answer there is an active call when you call, or third party call waiting");
        auto con = FindConnectionByState<ImsConnectionMap &, CellularCallConnectionIMS *>(
            connectionMap_, TelCallState::CALL_STATUS_ACTIVE);
        if (con == nullptr) {
            TELEPHONY_LOGE("Answer return, error type: con is null, there are no active calls");
            return CALL_ERR_CALL_CONNECTION_NOT_EXIST;
        }
        return con->SwitchCallRequest(callInfo.slotId);
    }
    if (pConnection->GetStatus() == TelCallState::CALL_STATUS_ALERTING ||
        pConnection->GetStatus() == TelCallState::CALL_STATUS_INCOMING ||
        pConnection->GetStatus() == TelCallState::CALL_STATUS_WAITING) {
        return pConnection->AnswerRequest(callInfo.slotId, callInfo.phoneNum, callInfo.videoState, callInfo.index);
    }
    TELEPHONY_LOGE("IMSControl::Answer return, error type: call state error, phone not ringing.");
    return CALL_ERR_CALL_STATE;
}

int32_t IMSControl::Reject(const CellularCallInfo &callInfo)
{
    TELEPHONY_LOGI("IMSControl::Reject start");
    auto pConnection =
        GetConnectionData<ImsConnectionMap &, CellularCallConnectionIMS *>(connectionMap_, callInfo.phoneNum);
    if (pConnection == nullptr) {
        TELEPHONY_LOGI("Reject: connection cannot be matched, use index directly");
        pConnection =
            FindConnectionByIndex<ImsConnectionMap &, CellularCallConnectionIMS *>(connectionMap_, callInfo.index);
    }
    if (pConnection == nullptr) {
        TELEPHONY_LOGE("IMSControl::Reject, error type: connection is null");
        return CALL_ERR_CALL_CONNECTION_NOT_EXIST;
    }
    if (!pConnection->IsRingingState()) {
        TELEPHONY_LOGE("IMSControl::Reject return, error type: call state error, phone not ringing.");
        return CALL_ERR_CALL_STATE;
    }
    if (DelayedSingleton<CellularCallRegister>::GetInstance() != nullptr) {
        DelayedSingleton<CellularCallRegister>::GetInstance()->ReportSingleCallInfo(
            pConnection->GetCallReportInfo(), TelCallState::CALL_STATUS_DISCONNECTING);
    }
    return pConnection->RejectRequest(callInfo.slotId, callInfo.phoneNum, callInfo.index);
}

int32_t IMSControl::HoldCall(int32_t slotId)
{
    TELEPHONY_LOGI("HoldCall start");
    if (IsInState(connectionMap_, TelCallState::CALL_STATUS_INCOMING)) {
        TELEPHONY_LOGE("HoldCall return, error type: call state error.");
        return CALL_ERR_CALL_STATE;
    }
    CellularCallConnectionIMS cellularCallConnectionIms;
    return cellularCallConnectionIms.HoldCallRequest(slotId);
}

int32_t IMSControl::UnHoldCall(int32_t slotId)
{
    TELEPHONY_LOGI("UnHoldCall start");
    CellularCallConnectionIMS cellularCallConnectionIms;
    return cellularCallConnectionIms.UnHoldCallRequest(slotId);
}

int32_t IMSControl::SwitchCall(int32_t slotId)
{
    TELEPHONY_LOGI("SwitchCall start");
    CellularCallConnectionIMS cellularCallConnectionIms;
    return cellularCallConnectionIms.SwitchCallRequest(slotId);
}

/**
 * Add another remote party, to which a private communication has been established using
 * the same procedures as in Section 1.3.8.1, if the number of remote parties does not then
 * exceed the maximum number allowed, which results in an active multiParty call.
 */
int32_t IMSControl::CombineConference(int32_t slotId)
{
    TELEPHONY_LOGI("CombineConference entry");
    CellularCallConnectionIMS connection;
    int32_t voiceCall = 0;
    return connection.CombineConferenceRequest(slotId, voiceCall);
}

int32_t IMSControl::HangUpAllConnection(int32_t slotId)
{
    TELEPHONY_LOGI("HangUpAllConnection entry");
    CellularCallConnectionIMS connection;
    int32_t index = connectionMap_.begin()->second.GetIndex();
    // The AT command for hanging up all calls is the same as the AT command for rejecting calls,
    // so the reject interface is reused.
    return connection.RejectRequest(slotId, connectionMap_.begin()->first, index);
}

int32_t IMSControl::InviteToConference(int32_t slotId, const std::vector<std::string> &numberList)
{
    TELEPHONY_LOGI("InviteToConference entry");
    CellularCallConnectionIMS connection;
    return connection.InviteToConferenceRequest(slotId, numberList);
}

int32_t IMSControl::KickOutFromConference(int32_t slotId, const std::vector<std::string> &numberList)
{
    TELEPHONY_LOGI("KickOutFromConference entry");
    CellularCallConnectionIMS connection;
    return connection.KickOutFromConferenceRequest(slotId, numberList);
}

int32_t IMSControl::UpdateImsCallMode(const CellularCallInfo &callInfo, ImsCallMode mode)
{
    TELEPHONY_LOGI("UpdateImsCallMode entry");
    auto pConnection =
        GetConnectionData<ImsConnectionMap &, CellularCallConnectionIMS *>(connectionMap_, callInfo.phoneNum);
    if (pConnection == nullptr) {
        TELEPHONY_LOGI("UpdateImsCallMode: connection cannot be matched, use index directly");
        pConnection =
            FindConnectionByIndex<ImsConnectionMap &, CellularCallConnectionIMS *>(connectionMap_, callInfo.index);
    }
    if (pConnection == nullptr) {
        TELEPHONY_LOGE("IMSControl::UpdateImsCallMode, error type: connection is null");
        return CALL_ERR_CALL_CONNECTION_NOT_EXIST;
    }
    bool bContinue = pConnection->GetStatus() == TelCallState::CALL_STATUS_ALERTING ||
        pConnection->GetStatus() == TelCallState::CALL_STATUS_ACTIVE;
    if (!bContinue) {
        TELEPHONY_LOGE("IMSControl::UpdateImsCallMode return, error type: call state error.");
        return CALL_ERR_CALL_STATE;
    }
    return pConnection->UpdateCallMediaModeRequest(callInfo, mode);
}

int32_t IMSControl::StartRtt(int32_t slotId, const std::string &msg)
{
    TELEPHONY_LOGI("StartRtt entry");
    CellularCallConnectionIMS connection;
    return connection.StartRttRequest(slotId, msg);
}

int32_t IMSControl::StopRtt(int32_t slotId)
{
    TELEPHONY_LOGI("StopRtt entry");
    CellularCallConnectionIMS connection;
    return connection.StopRttRequest(slotId);
}

void IMSControl::ReleaseAllConnection()
{
    connectionMap_.clear();
}

ImsConnectionMap IMSControl::GetConnectionMap()
{
    return connectionMap_;
}

int32_t IMSControl::ReportImsCallsData(int32_t slotId, const ImsCurrentCallList &callInfoList)
{
    if (callInfoList.callSize <= 0 && !connectionMap_.empty()) {
        return ReportHungUpInfo(slotId);
    } else if (callInfoList.callSize > 0 && connectionMap_.empty()) {
        return ReportIncomingInfo(slotId, callInfoList);
    } else if (callInfoList.callSize > 0 && !connectionMap_.empty()) {
        return ReportUpdateInfo(slotId, callInfoList);
    }
    return TELEPHONY_ERROR;
}

int32_t IMSControl::ReportCallsData(int32_t slotId, const CallInfoList &callInfoList)
{
    return TELEPHONY_ERROR;
}

int32_t IMSControl::ReportHungUpInfo(int32_t slotId)
{
    TELEPHONY_LOGI("ReportHungUpInfo entry");
    CallsReportInfo callsReportInfo;
    for (auto &it : connectionMap_) {
        CallReportInfo reportInfo = it.second.GetCallReportInfo();
        reportInfo.state = TelCallState::CALL_STATUS_DISCONNECTED;
        reportInfo.accountId = slotId;
        callsReportInfo.callVec.push_back(reportInfo);
        GetCallFailReason(slotId, connectionMap_);
    }
    if (DelayedSingleton<CellularCallRegister>::GetInstance() == nullptr) {
        TELEPHONY_LOGE("ReportHungUpInfo return, GetInstance() is nullptr.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callsReportInfo.slotId = slotId;
    if (isIgnoredHangupReport_) {
        SetHangupReportIgnoredFlag(false);
    } else if (isIgnoredIncomingCall_) {
        isIgnoredIncomingCall_ = false;
    } else {
        DelayedSingleton<CellularCallRegister>::GetInstance()->ReportCallsInfo(callsReportInfo);
    }
    ReleaseAllConnection();
    return TELEPHONY_SUCCESS;
}

int32_t IMSControl::ReportIncomingInfo(int32_t slotId, const ImsCurrentCallList &imsCurrentCallInfoList)
{
    TELEPHONY_LOGI("ReportIncomingInfo entry");
    CallsReportInfo callsReportInfo;
    for (int32_t i = 0; i < imsCurrentCallInfoList.callSize; ++i) {
        CallReportInfo reportInfo = EncapsulationCallReportInfo(slotId, imsCurrentCallInfoList.calls[i]);

        CellularCallConnectionIMS connection;
        connection.SetStatus(static_cast<TelCallState>(imsCurrentCallInfoList.calls[i].state));
        connection.SetIndex(imsCurrentCallInfoList.calls[i].index);
        connection.SetOrUpdateCallReportInfo(reportInfo);
        SetConnectionData(connectionMap_, imsCurrentCallInfoList.calls[i].number, connection);

        callsReportInfo.callVec.push_back(reportInfo);
    }
    if (DelayedSingleton<CellularCallRegister>::GetInstance() == nullptr) {
        TELEPHONY_LOGE("ReportIncomingInfo return, GetInstance() is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    callsReportInfo.slotId = slotId;
    if (!DelayedSingleton<CellularCallRegister>::GetInstance()->IsCallManagerCallBackRegistered() &&
        callsReportInfo.callVec.size() != 0 && callsReportInfo.callVec[0].state == TelCallState::CALL_STATUS_INCOMING) {
        isIgnoredIncomingCall_ = true;
    } else {
        DelayedSingleton<CellularCallRegister>::GetInstance()->ReportCallsInfo(callsReportInfo);
    }
    return TELEPHONY_SUCCESS;
}

int32_t IMSControl::ReportUpdateInfo(int32_t slotId, const ImsCurrentCallList &callInfoList)
{
    TELEPHONY_LOGI("ReportUpdateInfo entry");
    CallsReportInfo callsReportInfo;
    for (int32_t i = 0; i < callInfoList.callSize; ++i) {
        CallReportInfo reportInfo = EncapsulationCallReportInfo(slotId, callInfoList.calls[i]);

        auto pConnection = GetConnectionData<ImsConnectionMap &, CellularCallConnectionIMS *>(
            connectionMap_, callInfoList.calls[i].number);
        if (pConnection == nullptr) {
            CellularCallConnectionIMS connection;
            connection.SetOrUpdateCallReportInfo(reportInfo);
            connection.SetFlag(true);
            connection.SetIndex(callInfoList.calls[i].index);
            SetConnectionData(connectionMap_, callInfoList.calls[i].number, connection);
        } else {
            pConnection->SetFlag(true);
            pConnection->SetIndex(callInfoList.calls[i].index);
            pConnection->SetOrUpdateCallReportInfo(reportInfo);
        }
        callsReportInfo.callVec.push_back(reportInfo);
    }
    callsReportInfo.slotId = slotId;
    DeleteConnection(callsReportInfo, callInfoList);
    if (DelayedSingleton<CellularCallRegister>::GetInstance() == nullptr) {
        TELEPHONY_LOGE("ReportUpdateInfo return, GetInstance() is nullptr.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (isIgnoredIncomingCall_) {
        isIgnoredIncomingCall_ = false;
    } else {
        DelayedSingleton<CellularCallRegister>::GetInstance()->ReportCallsInfo(callsReportInfo);
    }
    return TELEPHONY_SUCCESS;
}

CallReportInfo IMSControl::EncapsulationCallReportInfo(int32_t slotId, const ImsCurrentCall &callInfo)
{
    TELEPHONY_LOGI("EncapsulationCallReportInfo entry");
    CallReportInfo callReportInfo;
    if (memset_s(&callReportInfo, sizeof(callReportInfo), 0, sizeof(callReportInfo)) != EOK) {
        TELEPHONY_LOGE("EncapsulationCallReportInfo return, memset_s fail.");
        return callReportInfo;
    }

    size_t cpyLen = strlen(callInfo.number.c_str()) + 1;
    if (cpyLen > static_cast<size_t>(kMaxNumberLen + 1)) {
        TELEPHONY_LOGE("EncapsulationCallReportInfo return, strcpy_s fail.");
        return callReportInfo;
    }
    if (strcpy_s(callReportInfo.accountNum, cpyLen, callInfo.number.c_str()) != EOK) {
        TELEPHONY_LOGE("EncapsulationCallReportInfo return, strcpy_s fail.");
        return callReportInfo;
    }
    callReportInfo.index = callInfo.index;
    callReportInfo.accountId = slotId;
    callReportInfo.state = static_cast<TelCallState>(callInfo.state);
    callReportInfo.voiceDomain = callInfo.voiceDomain;
    callReportInfo.callType = CallType::TYPE_IMS;
    callReportInfo.callMode = callInfo.callType ? VideoStateType::TYPE_VIDEO : VideoStateType::TYPE_VOICE;
    return callReportInfo;
}

void IMSControl::DeleteConnection(CallsReportInfo &callsReportInfo, const ImsCurrentCallList &callInfoList)
{
    TELEPHONY_LOGI("DeleteConnection entry");
    auto it = connectionMap_.begin();
    while (it != connectionMap_.end()) {
        if (!it->second.GetFlag()) {
            CallReportInfo callReportInfo = it->second.GetCallReportInfo();
            callReportInfo.state = TelCallState::CALL_STATUS_DISCONNECTED;
            callsReportInfo.callVec.push_back(callReportInfo);
            connectionMap_.erase(it++);
            GetCallFailReason(callsReportInfo.slotId, connectionMap_);
        } else {
            it->second.SetFlag(false);
            ++it;
        }
    }
}
} // namespace Telephony
} // namespace OHOS
