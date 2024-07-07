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

#include "ability_controller_proxy.h"

#include "hilog_wrapper.h"
#include "ipc_types.h"


namespace OHOS {
namespace AppExecFwk {
AbilityControllerProxy::AbilityControllerProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAbilityController>(impl)
{}

bool AbilityControllerProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(AbilityControllerProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

bool AbilityControllerProxy::AllowAbilityStart(const Want &want, const std::string &bundleName)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return true;
    }
    data.WriteParcelable(&want);
    data.WriteString(bundleName);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return true;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IAbilityController::Message::TRANSACT_ON_ALLOW_ABILITY_START),
        data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
        return true;
    }
    return reply.ReadBool();
}

bool AbilityControllerProxy::AllowAbilityBackground(const std::string &bundleName)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return true;
    }
    data.WriteString(bundleName);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return true;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IAbilityController::Message::TRANSACT_ON_ALLOW_ABILITY_BACKGROUND),
        data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
        return true;
    }
    return reply.ReadBool();
}
}  // namespace AppExecFwk
}  // namespace OHOS
