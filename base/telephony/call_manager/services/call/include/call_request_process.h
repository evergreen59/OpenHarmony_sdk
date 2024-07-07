/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef CALL_REQUEST_PROCESS_H
#define CALL_REQUEST_PROCESS_H

#include "call_object_manager.h"

namespace OHOS {
namespace Telephony {
class CallRequestProcess : public CallObjectManager {
public:
    CallRequestProcess();
    ~CallRequestProcess();

    void DialRequest();
    void AnswerRequest(int32_t callId, int32_t videoState);
    void RejectRequest(int32_t callId, bool isSendSms, std::string &content);
    void HangUpRequest(int32_t callId);
    void HoldRequest(int32_t callId);
    void UnHoldRequest(int32_t callId);
    void SwitchRequest(int32_t callId);
    void CombineConferenceRequest(int32_t mainCallId);
    void SeparateConferenceRequest(int32_t callId);
    void UpdateCallMediaModeRequest(int32_t callId, ImsCallMode mode);
    void StartRttRequest(int32_t callId, std::u16string &msg);
    void StopRttRequest(int32_t callId);
    void JoinConference(int32_t callId, std::vector<std::string> &numberList);

private:
    void CarrierDialProcess(DialParaInfo &info);
    void VoiceMailDialProcess(DialParaInfo &info);
    void OttDialProcess(DialParaInfo &info);
    int32_t UpdateImsCallMode(int32_t callId, ImsCallMode mode);
    int32_t PackCellularCallInfo(DialParaInfo &info, CellularCallInfo &callInfo);
    bool IsFdnNumber(std::vector<std::u16string> fdnNumberList, std::string phoneNumber);
};
} // namespace Telephony
} // namespace OHOS

#endif // CALL_REQUEST_PROCESS_H
