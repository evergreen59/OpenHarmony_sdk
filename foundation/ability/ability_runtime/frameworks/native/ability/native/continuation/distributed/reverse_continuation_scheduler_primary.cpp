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

#include "reverse_continuation_scheduler_primary.h"
#include "continuation_handler.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
ReverseContinuationSchedulerPrimary::ReverseContinuationSchedulerPrimary(
    const std::weak_ptr<IReverseContinuationSchedulerPrimaryHandler> &continuationHandler,
    const std::shared_ptr<AbilityHandler> &mainHandler)
    : continuationHandler_(continuationHandler), mainHandler_(mainHandler)
{}

/**
 * @brief Replica call this method when it terminated.
 */
void ReverseContinuationSchedulerPrimary::NotifyReplicaTerminated()
{
    HILOG_INFO("%{public}s called begin", __func__);
    auto task = [reverseContinuationSchedulerPrimary = this]() {
        reverseContinuationSchedulerPrimary->HandlerNotifyReplicaTerminated();
    };

    if (mainHandler_ == nullptr) {
        HILOG_ERROR("ReverseContinuationSchedulerPrimary::NotifyReplicaTerminated mainHandler_ == nullptr");
        return;
    }

    bool ret = mainHandler_->PostTask(task);
    if (!ret) {
        HILOG_ERROR("ReverseContinuationSchedulerPrimary::NotifyReplicaTerminated PostTask error");
        return;
    }
    HILOG_INFO("%{public}s called end", __func__);
}

/**
 * @brief Replica call this method to notify primary go on.
 *
 * @param want Contains data to be restore.
 * @return True if success, otherwise false.
 */
bool ReverseContinuationSchedulerPrimary::ContinuationBack(const AAFwk::Want &want)
{
    HILOG_INFO("%{public}s called begin", __func__);
    auto task = [reverseContinuationSchedulerPrimary = this, want]() {
        reverseContinuationSchedulerPrimary->HandlerContinuationBack(want);
    };

    if (mainHandler_ == nullptr) {
        HILOG_ERROR("ReverseContinuationSchedulerPrimary::ContinuationBack mainHandler_ == nullptr");
        return false;
    }

    bool ret = mainHandler_->PostTask(task);
    if (!ret) {
        HILOG_ERROR("ReverseContinuationSchedulerPrimary::ContinuationBack PostTask error");
        return false;
    }
    HILOG_INFO("%{public}s called end", __func__);
    return true;
}

void ReverseContinuationSchedulerPrimary::HandlerNotifyReplicaTerminated()
{
    HILOG_INFO("%{public}s called begin", __func__);
    std::shared_ptr<IReverseContinuationSchedulerPrimaryHandler> continuationHandler = nullptr;
    continuationHandler = continuationHandler_.lock();
    if (continuationHandler == nullptr) {
        HILOG_ERROR(
            "ReverseContinuationSchedulerPrimary::HandlerNotifyReplicaTerminated get continuationHandler is nullptr");
        return;
    }
    continuationHandler->NotifyReplicaTerminated();
    HILOG_INFO("%{public}s called end", __func__);
}

void ReverseContinuationSchedulerPrimary::HandlerContinuationBack(const AAFwk::Want &want)
{
    HILOG_INFO("%{public}s called begin", __func__);
    std::shared_ptr<IReverseContinuationSchedulerPrimaryHandler> continuationHandler = nullptr;
    continuationHandler = continuationHandler_.lock();
    if (continuationHandler == nullptr) {
        HILOG_ERROR("ReverseContinuationSchedulerPrimary::HandlerContinuationBack get continuationHandler is nullptr");
        return;
    }
    continuationHandler->ContinuationBack(want);
    HILOG_INFO("%{public}s called end", __func__);
}
}  // namespace AppExecFwk
}  // namespace OHOS
