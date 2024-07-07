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

#ifndef TELEPHONY_CALL_STATE_PROCESSOR_H
#define TELEPHONY_CALL_STATE_PROCESSOR_H

#include <set>
#include <string>

#include "singleton.h"

#include "call_manager_inner_type.h"

namespace OHOS {
namespace Telephony {
constexpr uint16_t EMPTY_VALUE = 0;
constexpr uint16_t EXIST_ONLY_ONE_CALL = 1;

class CallStateProcessor : public std::enable_shared_from_this<CallStateProcessor> {
    DECLARE_DELAYED_SINGLETON(CallStateProcessor)
public:
    void AddCall(const std::string &phoneNum, TelCallState state);
    void DeleteCall(const std::string &phoneNum, TelCallState state);
    bool UpdateCurrentCallState();
    std::string GetCurrentActiveCall() const;
    int32_t GetCallNumber(TelCallState state);
    bool ShouldSwitchState(TelCallState callState);

private:
    std::set<std::string> activeCalls_;
    std::set<std::string> holdingCalls_;
    std::set<std::string> alertingCalls_;
    std::set<std::string> incomingCalls_;
    std::set<std::string> dialingCalls_;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_CALL_STATE_PROCESSOR_H