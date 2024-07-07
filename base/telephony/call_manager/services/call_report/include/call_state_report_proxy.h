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

#ifndef REPORT_CALL_STATE_CLIENT_H
#define REPORT_CALL_STATE_CLIENT_H

#include "if_system_ability_manager.h"
#include "rwlock.h"

#include "call_state_listener_base.h"

namespace OHOS {
namespace Telephony {
class CallStateReportProxy : public CallStateListenerBase {
public:
    CallStateReportProxy();
    ~CallStateReportProxy();
    void CallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState);
    int32_t ReportCallState(int32_t slotId, int32_t callState, std::u16string phoneNumber);
    int32_t ReportCallStateForCallId(
        int32_t slotId, int32_t callId, int32_t callState, std::u16string incomingNumber);
};
} // namespace Telephony
} // namespace OHOS

#endif // REPORT_CALL_STATE_CLIENT_H
