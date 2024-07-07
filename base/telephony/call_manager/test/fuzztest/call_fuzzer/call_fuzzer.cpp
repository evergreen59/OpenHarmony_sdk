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

#include "call_fuzzer.h"

#include <cstddef>
#include <cstdint>
#define private public
#include "addcalltoken_fuzzer.h"
#include "call_manager_service.h"
#include "cs_call.h"
#include "ims_call.h"
#include "ott_call.h"
#include "system_ability_definition.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
constexpr int32_t SLOT_NUM = 2;
constexpr int32_t BOOL_NUM = 2;
constexpr int32_t DIAL_TYPE = 3;
constexpr int32_t CALL_TYPE_NUM = 4;
constexpr int32_t VIDIO_TYPE_NUM = 2;
constexpr int32_t TEL_CALL_STATE_NUM = 9;
constexpr int32_t TEL_CONFERENCE_STATE_NUM = 4;
constexpr int32_t CALL_RUNNING_STATE_NUM = 8;
constexpr int32_t CALL_ENDED_TYPE_NUM = 4;
constexpr int32_t CALL_ANSWER_TYPE_NUM = 3;
constexpr int32_t INVALID_CALL_ID = -1;

bool IsServiceInited()
{
    if (!g_isInited) {
        DelayedSingleton<CallManagerService>::GetInstance()->OnStart();
        if (DelayedSingleton<CallManagerService>::GetInstance()->GetServiceRunningState() ==
            static_cast<int32_t>(CallManagerService::ServiceRunningState::STATE_RUNNING)) {
            g_isInited = true;
        }
    }
    return g_isInited;
}

void CSCallFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(size % DIAL_TYPE);
    paraInfo.callType = static_cast<CallType>(size % CALL_TYPE_NUM);
    paraInfo.videoState = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    paraInfo.callState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    int32_t videoState = static_cast<int32_t>(size % VIDIO_TYPE_NUM);
    int32_t mute = static_cast<int32_t>(size % BOOL_NUM);
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    TelCallState nextState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    TelConferenceState telConferenceState = static_cast<TelConferenceState>(size % TEL_CONFERENCE_STATE_NUM);
    VideoStateType mediaType = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    PolicyFlag flag = static_cast<PolicyFlag>(size);

    callObjectPtr->AnswerCall(videoState);
    callObjectPtr->SetMute(mute, slotId);
    callObjectPtr->StartDtmf(static_cast<char>(*data));
    callObjectPtr->StopDtmf();
    callObjectPtr->GetSlotId();
    callObjectPtr->DialCallBase();
    callObjectPtr->IncomingCallBase();
    callObjectPtr->AnswerCallBase();
    callObjectPtr->RejectCallBase();
    callObjectPtr->GetCallID();
    callObjectPtr->GetCallType();
    callObjectPtr->GetCallRunningState();
    callObjectPtr->SetTelCallState(nextState);
    callObjectPtr->GetTelCallState();
    callObjectPtr->SetTelConferenceState(telConferenceState);
    callObjectPtr->GetTelConferenceState();
    callObjectPtr->GetVideoStateType();
    callObjectPtr->SetVideoStateType(mediaType);
    callObjectPtr->SetPolicyFlag(flag);
    callObjectPtr->GetPolicyFlag();
}

void DialingProcess(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(size % DIAL_TYPE);
    paraInfo.callType = static_cast<CallType>(size % CALL_TYPE_NUM);
    paraInfo.videoState = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    paraInfo.callState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();

    callObjectPtr->StartDtmf(static_cast<char>(*data));
    callObjectPtr->RejectCall();
    callObjectPtr->HangUpCall();
    callObjectPtr->HoldCall();
    callObjectPtr->UnHoldCall();
    callObjectPtr->SwitchCall();
    callObjectPtr->DialingProcess();
    callObjectPtr->CombineConference(); // merge calls
    callObjectPtr->SeparateConference();
    callObjectPtr->CanCombineConference();
    callObjectPtr->CanSeparateConference();
    callObjectPtr->LaunchConference();
    callObjectPtr->ExitConference();
    callObjectPtr->HoldConference();
    int32_t mainCallId = INVALID_CALL_ID;
    callObjectPtr->GetMainCallId(mainCallId);
    std::vector<std::u16string> subCallIdList;
    callObjectPtr->GetSubCallIdList(subCallIdList);
    std::vector<std::u16string> callIdList;
    callObjectPtr->GetCallIdListForConference(callIdList);
    callObjectPtr->IsSupportConferenceable();
    callObjectPtr->GetEmergencyState();
}

void GetCallerInfo(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(size % DIAL_TYPE);
    paraInfo.callType = static_cast<CallType>(size % CALL_TYPE_NUM);
    paraInfo.videoState = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    paraInfo.callState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    ContactInfo contactInfo;
    CallRunningState callRunningState = static_cast<CallRunningState>(size % CALL_RUNNING_STATE_NUM);
    bool speakerphoneOn = static_cast<bool>(size % BOOL_NUM);
    std::string phoneNumber(reinterpret_cast<const char *>(data), size);
    int32_t callId = static_cast<int32_t>(size);
    CallEndedType callEndedType = static_cast<CallEndedType>(size % CALL_ENDED_TYPE_NUM);
    CallAnswerType answerType = static_cast<CallAnswerType>(size % CALL_ANSWER_TYPE_NUM);
    int64_t startTime = static_cast<int64_t>(size);
    time_t callBeginTime = static_cast<time_t>(size);
    time_t callEndTime = static_cast<time_t>(size);
    time_t ringBeginTime = static_cast<time_t>(size);
    time_t ringEndTime = static_cast<time_t>(size);
    callObjectPtr->GetCallerInfo();
    callObjectPtr->SetCallerInfo(contactInfo);
    callObjectPtr->SetCallRunningState(callRunningState);
    callObjectPtr->SetStartTime(startTime);
    callObjectPtr->SetCallBeginTime(callBeginTime);
    callObjectPtr->SetCallEndTime(callEndTime);
    callObjectPtr->SetRingBeginTime(ringBeginTime);
    callObjectPtr->SetRingEndTime(ringEndTime);
    callObjectPtr->SetAnswerType(answerType);
    callObjectPtr->GetCallEndedType();
    callObjectPtr->SetCallEndedType(callEndedType);
    callObjectPtr->SetCallId(callId);
    callObjectPtr->IsSpeakerphoneEnabled();
    callObjectPtr->IsCurrentRinging();
    callObjectPtr->GetAccountNumber();
    callObjectPtr->SetSpeakerphoneOn(speakerphoneOn);
    callObjectPtr->IsSpeakerphoneOn();
    callObjectPtr->SetAudio();
    callObjectPtr->CheckVoicemailNumber(phoneNumber);
    callObjectPtr->IsAliveState();
}

void IMSCallFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(size % DIAL_TYPE);
    paraInfo.callType = static_cast<CallType>(size % CALL_TYPE_NUM);
    paraInfo.videoState = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    paraInfo.callState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    sptr<IMSCall> callObjectPtr = std::make_unique<IMSCall>(paraInfo).release();
    int32_t videoState = static_cast<int32_t>(size % VIDIO_TYPE_NUM);
    int32_t mute = static_cast<int32_t>(size % BOOL_NUM);
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    std::string msg(reinterpret_cast<const char *>(data), size);
    std::u16string msgU16 = Str8ToStr16(msg);
    CallMediaModeResponse response;
    response.result = static_cast<int32_t>(size % BOOL_NUM);
    int32_t length = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    if (memcpy_s(response.phoneNum, kMaxNumberLen, msg.c_str(), length) != EOK) {
        return;
    }

    callObjectPtr->InitVideoCall();
    callObjectPtr->DialingProcess();
    callObjectPtr->AnswerCall(videoState);
    callObjectPtr->RejectCall();
    callObjectPtr->HangUpCall();
    callObjectPtr->HoldCall();
    callObjectPtr->UnHoldCall();
    callObjectPtr->SwitchCall();
    callObjectPtr->CombineConference();
    callObjectPtr->SeparateConference();
    callObjectPtr->CanCombineConference();
    callObjectPtr->CanSeparateConference();
    callObjectPtr->LaunchConference();
    callObjectPtr->ExitConference();
    callObjectPtr->HoldConference();
    int32_t mainCallId = INVALID_CALL_ID;
    callObjectPtr->GetMainCallId(mainCallId);
    std::vector<std::u16string> subCallIdList;
    callObjectPtr->GetSubCallIdList(subCallIdList);
    std::vector<std::u16string> callIdList;
    callObjectPtr->GetCallIdListForConference(callIdList);
    callObjectPtr->IsSupportConferenceable();
    callObjectPtr->StartRtt(msgU16);
    callObjectPtr->StopRtt();
    callObjectPtr->SetMute(mute, slotId);
    callObjectPtr->AcceptVideoCall();
    callObjectPtr->RefuseVideoCall();
    callObjectPtr->IsSupportVideoCall();
}

void OttCallFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(size % DIAL_TYPE);
    paraInfo.callType = static_cast<CallType>(size % CALL_TYPE_NUM);
    paraInfo.videoState = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    paraInfo.callState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    sptr<OTTCall> callObjectPtr = std::make_unique<OTTCall>(paraInfo).release();
    int32_t videoState = static_cast<int32_t>(size % VIDIO_TYPE_NUM);
    int32_t mute = static_cast<int32_t>(size % BOOL_NUM);
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    std::string msg(reinterpret_cast<const char *>(data), size);
    CallMediaModeResponse response;
    response.result = static_cast<int32_t>(size % BOOL_NUM);
    int32_t length = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    if (memcpy_s(response.phoneNum, kMaxNumberLen, msg.c_str(), length) != EOK) {
        return;
    }

    callObjectPtr->DialingProcess();
    callObjectPtr->AnswerCall(videoState);
    callObjectPtr->RejectCall();
    callObjectPtr->HangUpCall();
    callObjectPtr->HoldCall();
    callObjectPtr->UnHoldCall();
    callObjectPtr->SwitchCall();
    callObjectPtr->GetEmergencyState();
    callObjectPtr->StopDtmf();
    callObjectPtr->GetSlotId();
    callObjectPtr->CombineConference();
    callObjectPtr->SeparateConference();
    callObjectPtr->CanCombineConference();
    callObjectPtr->CanSeparateConference();
    callObjectPtr->LaunchConference();
    callObjectPtr->ExitConference();
    callObjectPtr->HoldConference();
    int32_t mainCallId = INVALID_CALL_ID;
    callObjectPtr->GetMainCallId(mainCallId);
    std::vector<std::u16string> subCallIdList;
    callObjectPtr->GetSubCallIdList(subCallIdList);
    std::vector<std::u16string> callIdList;
    callObjectPtr->GetCallIdListForConference(callIdList);
    callObjectPtr->IsSupportConferenceable();
    callObjectPtr->ReceiveUpdateCallMediaModeResponse(response);
    callObjectPtr->SetMute(mute, slotId);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    CSCallFunc(data, size);
    DialingProcess(data, size);
    GetCallerInfo(data, size);
    IMSCallFunc(data, size);
    OttCallFunc(data, size);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::AddCallTokenFuzzer token;
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
