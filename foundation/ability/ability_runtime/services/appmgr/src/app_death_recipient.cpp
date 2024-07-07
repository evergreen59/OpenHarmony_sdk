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

#include "app_death_recipient.h"
#include "app_mgr_service_inner.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string TASK_ON_REMOTE_DIED = "OnRemoteDiedTask";
} // namespace

void AppDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    if (remote == nullptr) {
        HILOG_ERROR("remote is null");
        return;
    }

    auto handler = handler_.lock();
    if (!handler) {
        HILOG_ERROR("handler is null");
        return;
    }
    auto serviceInner = appMgrServiceInner_.lock();
    if (!serviceInner) {
        HILOG_ERROR("serviceInner is null");
        return;
    }

    auto onRemoteDiedFunc = std::bind(&AppMgrServiceInner::OnRemoteDied, serviceInner, remote, isRenderProcess_);
    handler->PostTask(onRemoteDiedFunc, TASK_ON_REMOTE_DIED);
}

void AppDeathRecipient::SetEventHandler(const std::shared_ptr<AMSEventHandler> &handler)
{
    handler_ = handler;
}

void AppDeathRecipient::SetAppMgrServiceInner(const std::shared_ptr<AppMgrServiceInner> &serviceInner)
{
    appMgrServiceInner_ = serviceInner;
}

void AppDeathRecipient::SetIsRenderProcess(bool isRenderProcess)
{
    isRenderProcess_ = isRenderProcess;
}
}  // namespace AppExecFwk
}  // namespace OHOS
