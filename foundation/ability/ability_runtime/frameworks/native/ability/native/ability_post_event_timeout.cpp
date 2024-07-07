/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "ability_post_event_timeout.h"

#include "ability_handler.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
const int64_t AbilityPostEventTimeout::defalutDelayTime = 5000;

std::atomic<uint32_t> AbilityPostEventTimeout::allocationId_ = 0;

AbilityPostEventTimeout::AbilityPostEventTimeout(std::string str, std::shared_ptr<AbilityHandler> &eventHandler)
{
    uint32_t taskId = allocationId_++;
    std::string strId = std::to_string(taskId);
    task_ = str + strId;
    taskExec_ = false;
    handler_ = eventHandler;
}
AbilityPostEventTimeout::~AbilityPostEventTimeout()
{
    handler_.reset();
}

void AbilityPostEventTimeout::TimingBegin(int64_t delaytime)
{
    HILOG_INFO("AbilityPostEventTimeout::TimingBegin() call %{public}s", task_.c_str());
    if (handler_ == nullptr) {
        HILOG_ERROR("AbilityPostEventTimeout::TimingBegin %{public}s handler_ is nullptr", task_.c_str());
        return;
    }

    auto task = [timeOutTask = shared_from_this()]() { timeOutTask->TimeOutProc(); };
    handler_->PostTask(task, task_, delaytime);
}
void AbilityPostEventTimeout::TimeEnd()
{
    if (handler_ == nullptr) {
        HILOG_ERROR("AbilityPostEventTimeout::TimeEnd %{public}s handler_ is nullptr", task_.c_str());
        return;
    }

    std::lock_guard<std::mutex> lck(mtx_);
    if (!taskExec_) {
        taskExec_ = true;
        handler_->RemoveTask(task_);
    }
}

void AbilityPostEventTimeout::TimeOutProc()
{
    HILOG_INFO("AbilityPostEventTimeout::TimeOutProc() call %{public}s", task_.c_str());
    if (handler_ == nullptr) {
        HILOG_ERROR("AbilityPostEventTimeout::TimeEnd %{public}s handler_ is nullptr", task_.c_str());
        return;
    }

    std::lock_guard<std::mutex> lck(mtx_);
    if (!taskExec_) {
        taskExec_ = true;
        HILOG_WARN("TimeOutProc %{public}s Event TimeOut", task_.c_str());
        handler_->RemoveTask(task_);
    } else {
        HILOG_WARN("AbilityPostEventTimeout::TimeOutProc Exec Failed, The Event is %{public}s", task_.c_str());
    }
}
}  // namespace AppExecFwk
}  // namespace OHOS
