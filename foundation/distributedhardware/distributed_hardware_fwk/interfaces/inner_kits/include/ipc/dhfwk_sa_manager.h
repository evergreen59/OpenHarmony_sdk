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

#ifndef OHOS_DHFWK_SA_MANAGER_H
#define OHOS_DHFWK_SA_MANAGER_H

#include <atomic>
#include <functional>
#include <mutex>

#include "refbase.h"
#include "system_ability_status_change_stub.h"

#include "single_instance.h"
#include "idistributed_hardware.h"

namespace OHOS {
namespace DistributedHardware {
// DHFWK sa state callback, param true for started, false for stopped.
using DHFWKSAStateCb = std::function<void(bool)>;
class DHFWKSAManager {
DECLARE_SINGLE_INSTANCE_BASE(DHFWKSAManager);
public:
    DHFWKSAManager();
    virtual ~DHFWKSAManager();
    void RegisterAbilityListener();
    sptr<IDistributedHardware> GetDHFWKProxy();
    void RegisterSAStateCallback(DHFWKSAStateCb callback);

public:
class SystemAbilityListener : public SystemAbilityStatusChangeStub {
public:
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
};

private:
    std::atomic<bool> dhfwkOnLine_;
    std::atomic<bool> isSubscribeDHFWKSAChangeListener;
    std::mutex proxyMutex_;
    sptr<IDistributedHardware> dhfwkProxy_;
    sptr<SystemAbilityListener> saListener_;
    std::mutex saStatCbMutex_;
    DHFWKSAStateCb saStateCallback;
};
} // DistributedHardware
} // OHOS
#endif // OHOS_DHFWK_SA_MANAGER_H