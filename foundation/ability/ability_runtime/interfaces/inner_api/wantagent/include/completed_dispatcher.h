/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ABILITY_RUNTIME_COMPLETED_DISPATCHER_H
#define OHOS_ABILITY_RUNTIME_COMPLETED_DISPATCHER_H

#include <string>
#include <memory>
#include "completed_callback.h"
#include "event_handler.h"
#include "want.h"
#include "want_params.h"
#include "want_receiver_stub.h"

namespace OHOS::AbilityRuntime::WantAgent {
class PendingWant;
class CompletedDispatcher : public AAFwk::WantReceiverStub {
public:
    CompletedDispatcher(const std::shared_ptr<PendingWant> &pendingWant,
        const std::shared_ptr<CompletedCallback> &callback, const std::shared_ptr<AppExecFwk::EventHandler> &handler);
    virtual ~CompletedDispatcher() = default;

    void Send(const int32_t resultCode) override;

    void PerformReceive(const AAFwk::Want &want, int resultCode, const std::string &data,
        const AAFwk::WantParams &extras, bool serialized, bool sticky, int sendingUser) override;

    void Run();

private:
    const std::shared_ptr<PendingWant> pendingWant_;
    const std::shared_ptr<CompletedCallback> callback_;
    const std::shared_ptr<AppExecFwk::EventHandler> handler_;
    AAFwk::Want want_;
    int resultCode_ = 0;
    std::string resultData_;
    AAFwk::WantParams resultExtras_;
};
}  // namespace OHOS::AbilityRuntime::WantAgent
#endif  // OHOS_ABILITY_RUNTIME_COMPLETED_DISPATCHER_H
