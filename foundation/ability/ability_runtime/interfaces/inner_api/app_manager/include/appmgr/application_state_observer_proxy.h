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

#ifndef OHOS_ABILITY_RUNTIME_APPLICATION_STATE_OBSERVER_PROXY_H
#define OHOS_ABILITY_RUNTIME_APPLICATION_STATE_OBSERVER_PROXY_H

#include "iremote_proxy.h"

#include "app_mgr_constants.h"
#include "iapplication_state_observer.h"

namespace OHOS {
namespace AppExecFwk {
class ApplicationStateObserverProxy : public IRemoteProxy<IApplicationStateObserver> {
public:
    explicit ApplicationStateObserverProxy(const sptr<IRemoteObject> &impl);
    virtual ~ApplicationStateObserverProxy() = default;

    /**
     * Application foreground state changed callback.
     *
     * @param appStateData Application Process data.
     */
    virtual void OnForegroundApplicationChanged(const AppStateData &appStateData) override;

    /**
     * Will be called when the ability state changes.
     *
     * @param abilityStateData Ability state data.
     */
    virtual void OnAbilityStateChanged(const AbilityStateData &abilityStateData) override;

    /**
     * Will be called when the extension state changes.
     *
     * @param abilityStateData Extension state data.
     */
    virtual void OnExtensionStateChanged(const AbilityStateData &abilityStateData) override;

    /**
     * Will be called when the process start.
     *
     * @param processData Process data.
     */
    virtual void OnProcessCreated(const ProcessData &processData) override;

    /**
     * Will be called when the process state change.
     *
     * @param processData Process data.
     */
    virtual void OnProcessStateChanged(const ProcessData &processData) override;

    /**
     * Will be called when the process die.
     *
     * @param processData Process data.
     */
    virtual void OnProcessDied(const ProcessData &processData) override;

    /**
     * Application state changed callback.
     *
     * @param appStateData Application state data.
     */
    virtual void OnApplicationStateChanged(const AppStateData &appStateData) override;

    virtual void OnAppStateChanged(const AppStateData &appStateData) override;

    virtual void OnProcessReused(const ProcessData &processData) override;

private:
    bool WriteInterfaceToken(MessageParcel &data);
    static inline BrokerDelegator<ApplicationStateObserverProxy> delegator_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif
