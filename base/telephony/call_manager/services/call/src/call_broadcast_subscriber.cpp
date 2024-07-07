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

#include "call_broadcast_subscriber.h"

#include <string_ex.h>

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace OHOS::EventFwk;
CallBroadcastSubscriber::CallBroadcastSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo &subscriberInfo)
    : CommonEventSubscriber(subscriberInfo)
{
    memberFuncMap_[UNKNOWN_BROADCAST_EVENT] = &CallBroadcastSubscriber::UnknownBroadcast;
    memberFuncMap_[SIM_STATE_BROADCAST_EVENT] = &CallBroadcastSubscriber::SimStateBroadcast;
}

void CallBroadcastSubscriber::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    uint32_t code = UNKNOWN_BROADCAST_EVENT;
    OHOS::EventFwk::Want want = data.GetWant();
    std::string action = data.GetWant().GetAction();
    TELEPHONY_LOGI("receive one broadcast:%{public}s", action.c_str());
    if (action == EventFwk::CommonEventSupport::COMMON_EVENT_SIM_STATE_CHANGED) {
        code = SIM_STATE_BROADCAST_EVENT;
    } else {
        code = UNKNOWN_BROADCAST_EVENT;
    }
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data);
        }
    }
}

void CallBroadcastSubscriber::UnknownBroadcast(const EventFwk::CommonEventData &data)
{
    TELEPHONY_LOGI("you receive one unknown broadcast!");
}

void CallBroadcastSubscriber::SimStateBroadcast(const EventFwk::CommonEventData &data)
{
    TELEPHONY_LOGI("sim state broadcast code:%{public}d", data.GetCode());
}
} // namespace Telephony
} // namespace OHOS
