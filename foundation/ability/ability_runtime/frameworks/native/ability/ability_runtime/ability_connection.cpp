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

#include "ability_connection.h"

#include <unistd.h>

#include "connection_manager.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AbilityRuntime {
namespace {
constexpr int32_t DIED = -1;
} // namespace

void AbilityConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    HILOG_DEBUG("%{public}s begin.", __func__);
    mutex_.lock();
    if (abilityConnectCallbackList_.empty()) {
        HILOG_ERROR("%{public}s abilityConnectCallbackList is empty.", __func__);
        mutex_.unlock();
        return;
    }

    SetRemoteObject(remoteObject);
    SetResultCode(resultCode);
    SetConnectionState(CONNECTION_STATE_CONNECTED);
    
    std::vector<sptr<AbilityConnectCallback>> callbacks = GetCallbackList();
    mutex_.unlock();

    auto item = callbacks.begin();
    while (item != callbacks.end()) {
        (*item)->OnAbilityConnectDone(element, remoteObject, resultCode);
        item++;
    }
    HILOG_DEBUG("%{public}s end, bundleName:%{public}s, abilityName:%{public}s.",
        __func__, element.GetBundleName().c_str(), element.GetAbilityName().c_str());
}

void AbilityConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    HILOG_DEBUG("%{public}s begin, resultCode:%{public}d.", __func__, resultCode);
    mutex_.lock();
    SetConnectionState(CONNECTION_STATE_DISCONNECTED);
    if (abilityConnectCallbackList_.empty()) {
        HILOG_ERROR("%{public}s abilityConnectCallback is empty.", __func__);
        mutex_.unlock();
        return;
    }
    // if resultCode < 0 that means the connectReceiver is died
    if (resultCode == DIED) {
        bool ret = ConnectionManager::GetInstance().DisconnectReceiver(element);
        if (ret) {
            ConnectionManager::GetInstance().ReportConnectionLeakEvent(getpid(), gettid());
            HILOG_INFO("The service connection is not disconnected.");
        }
        resultCode = DIED + 1;
    }

    std::vector<sptr<AbilityConnectCallback>> callbacks = GetCallbackList();
    mutex_.unlock();

    auto item = callbacks.begin();
    while (item != callbacks.end()) {
        (*item)->OnAbilityDisconnectDone(element, resultCode);
        item++;
    }
    HILOG_DEBUG("%{public}s end, bundleName:%{public}s, abilityName:%{public}s.",
        __func__, element.GetBundleName().c_str(), element.GetAbilityName().c_str());
}

void AbilityConnection::AddConnectCallback(const sptr<AbilityConnectCallback> &abilityConnectCallback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto item = abilityConnectCallbackList_.begin();
    while (item != abilityConnectCallbackList_.end()) {
        if (*item == abilityConnectCallback) {
            return;
        }
        item++;
    }
    abilityConnectCallbackList_.push_back(abilityConnectCallback);
}

void AbilityConnection::RemoveConnectCallback(const sptr<AbilityConnectCallback> &abilityConnectCallback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto item = abilityConnectCallbackList_.begin();
    while (item != abilityConnectCallbackList_.end()) {
        if (*item == abilityConnectCallback) {
            abilityConnectCallbackList_.erase(item);
            break;
        } else {
            item++;
        }
    }
}

void AbilityConnection::SetRemoteObject(const sptr<IRemoteObject> &remoteObject)
{
    remoteObject_ = remoteObject;
}

void AbilityConnection::SetResultCode(int resultCode)
{
    resultCode_ = resultCode;
}

void AbilityConnection::SetConnectionState(int connectionState)
{
    connectionState_ = connectionState;
}

sptr<IRemoteObject> AbilityConnection::GetRemoteObject() const
{
    return remoteObject_;
}

int AbilityConnection::GetResultCode() const
{
    return resultCode_;
}

int AbilityConnection::GetConnectionState() const
{
    return connectionState_;
}

std::vector<sptr<AbilityConnectCallback>> AbilityConnection::GetCallbackList()
{
    return abilityConnectCallbackList_;
}
} // namespace AbilityRuntime
} // namespace OHOS
