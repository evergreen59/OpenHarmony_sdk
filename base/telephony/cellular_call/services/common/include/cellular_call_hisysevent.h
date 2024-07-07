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

#ifndef CELLULAR_CALL_HISYSEVENT_H
#define CELLULAR_CALL_HISYSEVENT_H

#include <string>

#include "telephony_hisysevent.h"

namespace OHOS {
namespace Telephony {
static const int64_t NORMAL_INCOMING_TIME = 500; // incoming time (ms)

enum class AnswerCallType {
    IMS_VOICE_ANSWER = 0,
    IMS_VIDEO_TO_VOICE_ANSWER,
    IMS_VIDEO_TO_VIDEO_ANSWER,
    CS_VOICE_ANSWER,
};

enum class DialCallType {
    IMS_VOICE_DIAL = 0,
    IMS_VIDEO_DIAL,
    CS_VOICE_DIAL,
};

enum class HangUpCallType {
    IMS_VOICE_CALL = 0,
    IMS_VIDEO_CALL,
    CS_VOICE_CALL,
};

enum class CallResponseResult {
    COMMAND_FAILURE = 0,
    COMMAND_SUCCESS,
};

struct CallBehaviorParameterInfo {
    int32_t slotId;
    int32_t callType;
    int32_t videoState;
    int32_t incomingCallType;
    int32_t incomingVideoState;
};

class CellularCallHiSysEvent : public TelephonyHiSysEvent {
public:
    static void WriteFoundationRestartFaultEvent(const int32_t count);
    static void WriteCallEndBehaviorEvent(const int32_t slotId, const int32_t cause);
    static void WriteDialCallBehaviorEvent(const CallBehaviorParameterInfo &info, const CallResponseResult &result);
    static void WriteHangUpCallBehaviorEvent(const CallBehaviorParameterInfo &info, const CallResponseResult &result);
    static void WriteAnswerCallBehaviorEvent(const CallBehaviorParameterInfo &info, const CallResponseResult &result);
    static void WriteDialCallFaultEvent(const int32_t slotId, const int32_t callType, const int32_t videoState,
        const int32_t errorCode, const std::string &desc);
    static void WriteAnswerCallFaultEvent(const int32_t slotId, const int32_t callId, const int32_t videoState,
        const int32_t errorCode, const std::string &desc);
    static void WriteHangUpFaultEvent(
        const int32_t slotId, const int32_t callId, const int32_t errorCode, const std::string &desc);
    static void WriteIncomingCallFaultEvent(const int32_t slotId, const int32_t callType, const int32_t videoState,
        const int32_t errCode, const std::string &desc);
    void SetCallParameterInfo(const int32_t slotId, const int32_t callType, const int32_t videoState);
    void SetIncomingCallParameterInfo(const int32_t incomingCallType, const int32_t incomingVideoState);
    void GetCallParameterInfo(CallBehaviorParameterInfo &info);
    void SetIncomingStartTime();
    void JudgingIncomingTimeOut(const int32_t slotId, const int32_t callType, const int32_t videoState);

private:
    template<typename... Types>
    static void CallWriteEvent(const std::string &eventName, Types... args);
    static int32_t ErrorCodeConversion(const int32_t errCode, CallErrorCode &eventValue);
    static int32_t CallDataErrorCodeConversion(const int32_t errCode, CallErrorCode &eventValue);
    static int32_t CallInterfaceErrorCodeConversion(const int32_t errCode, CallErrorCode &eventValue);
    static int32_t TelephonyErrorCodeConversion(const int32_t errCode, CallErrorCode &eventValue);

private:
    int32_t dfxSlotId_;
    int32_t dfxCallId_;
    int32_t dfxCallType_;
    int32_t dfxVideoState_;
    int32_t dfxIncomingCallType_;
    int32_t dfxIncomingVideoState_;
    int64_t incomingStartTime_ = 0L;
};
} // namespace Telephony
} // namespace OHOS

#endif // CELLULAR_CALL_HISYSEVENT_H
