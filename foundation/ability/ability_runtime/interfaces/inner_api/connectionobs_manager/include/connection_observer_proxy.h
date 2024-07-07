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

#ifndef OHOS_ABILITYRUNTIME_CONNECTION_OBSERVER_PROXY_H
#define OHOS_ABILITYRUNTIME_CONNECTION_OBSERVER_PROXY_H

#include <string>

#include "iconnection_observer.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace AbilityRuntime {
/**
 * interface for connection observer.
 */
class ConnectionObserverProxy : public IRemoteProxy<IConnectionObserver> {
public:
    explicit ConnectionObserverProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IConnectionObserver>(impl) {}

    virtual ~ConnectionObserverProxy() = default;

    virtual void OnExtensionConnected(const ConnectionData &connectionData) override;

    virtual void OnExtensionDisconnected(const ConnectionData &connectionData) override;

    virtual void OnDlpAbilityOpened(const DlpStateData &dlpData) override;

    virtual void OnDlpAbilityClosed(const DlpStateData &dlpData) override;

private:
    static inline BrokerDelegator<ConnectionObserverProxy> delegator_;
};
}  // namespace AbilityRuntime
}  // namespace OHOS
#endif  // OHOS_ABILITYRUNTIME_CONNECTION_OBSERVER_PROXY_H
