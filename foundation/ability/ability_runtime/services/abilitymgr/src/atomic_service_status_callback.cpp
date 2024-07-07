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

#include "atomic_service_status_callback.h"

#include "ability_util.h"
#include "free_install_manager.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {
AtomicServiceStatusCallback::AtomicServiceStatusCallback(const std::weak_ptr<FreeInstallManager> &server,
    const int64_t startInstallTime)
    : server_(server), startInstallTime_(startInstallTime)
{
}

void AtomicServiceStatusCallback::OnInstallFinished(int resultCode, const Want &want, int32_t userId)
{
    auto server = server_.lock();
    CHECK_POINTER(server);
    server->OnInstallFinished(resultCode, want, userId, startInstallTime_);
}

void AtomicServiceStatusCallback::OnRemoteInstallFinished(int resultCode, const Want &want, int32_t userId)
{
    auto server = server_.lock();
    CHECK_POINTER(server);
    server->OnRemoteInstallFinished(resultCode, want, userId, startInstallTime_);
}
}  // namespace AAFwk
}  // namespace OHOS
