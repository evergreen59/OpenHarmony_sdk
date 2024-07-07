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

#ifndef CALL_ABILITY_CALLBACK_H
#define CALL_ABILITY_CALLBACK_H

#include <memory>
#include <mutex>

#include "ipc_types.h"

#include "call_ability_callback_stub.h"
#include "call_manager_callback.h"

namespace OHOS {
namespace Telephony {
class CallAbilityCallback : public CallAbilityCallbackStub {
public:
    explicit CallAbilityCallback();
    ~CallAbilityCallback() override;
    int32_t SetProcessCallback(std::unique_ptr<CallManagerCallback> callback);
    int32_t OnCallDetailsChange(const CallAttributeInfo &info) override;
    int32_t OnCallEventChange(const CallEventInfo &info) override;
    int32_t OnCallDisconnectedCause(const DisconnectedDetails &details) override;
    int32_t OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo) override;
    int32_t OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info) override;
    int32_t OnReportMmiCodeResult(const MmiCodeInfo &info) override;

private:
    std::unique_ptr<CallManagerCallback> callbackPtr_;
    std::mutex mutex_;
};
} // namespace Telephony
} // namespace OHOS

#endif
