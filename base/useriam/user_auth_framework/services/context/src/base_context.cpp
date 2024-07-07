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
#include "base_context.h"

#include <sstream>

#include "iam_check.h"
#include "iam_logger.h"
#include "iam_para2str.h"

#define LOG_LABEL UserIam::Common::LABEL_USER_AUTH_SA
namespace OHOS {
namespace UserIam {
namespace UserAuth {
BaseContext::BaseContext(const std::string &type, uint64_t contextId, std::shared_ptr<ContextCallback> callback)
    : callback_(callback),
      contextId_(contextId)
{
    std::ostringstream ss;
    ss << "Context(type:" << type << ", contextId:" << GET_MASKED_STRING(contextId_) << ")";
    description_ = ss.str();
}

void BaseContext::SetLatestError(int32_t error)
{
    if (error != ResultCode::SUCCESS) {
        latestError_ = error;
    }
}

int32_t BaseContext::GetLatestError() const
{
    return latestError_;
}

uint64_t BaseContext::GetContextId() const
{
    return contextId_;
}

bool BaseContext::Start()
{
    std::lock_guard<std::mutex> guard(mutex_);
    IAM_LOGI("%{public}s start", GetDescription());
    if (hasStarted_) {
        IAM_LOGI("%{public}s context has started, cannot start again", GetDescription());
        return false;
    }
    hasStarted_ = true;
    return OnStart();
}

bool BaseContext::Stop()
{
    IAM_LOGI("%{public}s start", GetDescription());
    return OnStop();
}

std::shared_ptr<ScheduleNode> BaseContext::GetScheduleNode(uint64_t scheduleId) const
{
    for (auto const &schedule : scheduleList_) {
        if (schedule == nullptr) {
            continue;
        }
        if (schedule->GetScheduleId() == scheduleId) {
            return schedule;
        }
    }
    return nullptr;
}

void BaseContext::OnScheduleStarted()
{
    IAM_LOGI("%{public}s start", GetDescription());
}

void BaseContext::OnScheduleProcessed(ExecutorRole src, int32_t moduleType, const std::vector<uint8_t> &acquireMsg)
{
    IAM_LOGI("%{public}s start", GetDescription());
    IF_FALSE_LOGE_AND_RETURN(callback_ != nullptr);
    callback_->OnAcquireInfo(src, moduleType, acquireMsg);
}

void BaseContext::OnScheduleStoped(int32_t resultCode, const std::shared_ptr<Attributes> &finalResult)
{
    OnResult(resultCode, finalResult);
    return;
}

const char *BaseContext::GetDescription() const
{
    return description_.c_str();
}
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS
