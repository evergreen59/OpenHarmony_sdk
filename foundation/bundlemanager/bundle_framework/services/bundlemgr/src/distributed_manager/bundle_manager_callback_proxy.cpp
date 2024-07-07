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

#include "bundle_manager_callback_proxy.h"

#include "message_parcel.h"

namespace OHOS {
namespace AppExecFwk {
using OHOS::AppExecFwk::IBundleManagerCallback;

BundleManagerCallbackProxy::BundleManagerCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IBundleManagerCallback>(impl)
{
}

int32_t BundleManagerCallbackProxy::OnQueryRpcIdFinished(const std::string &installResult)
{
    return 0;
}
}  // namespace AppExecFwk
}  // namespace OHOS
