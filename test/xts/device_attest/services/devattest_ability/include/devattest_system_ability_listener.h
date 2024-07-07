/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef DEVATTEST_SYSTEM_ABILITY_LISTENER_H
#define DEVATTEST_SYSTEM_ABILITY_LISTENER_H


#include "system_ability_status_change_stub.h"

namespace OHOS {
namespace DevAttest {
class DevAttestSystemAbilityListener : public SystemAbilityStatusChangeStub {
public:
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    bool AddDevAttestSystemAbilityListener(int32_t systemAbilityId);
private:
    bool RemoveDevAttestSystemAbilityListener(int32_t systemAbilityId);
    bool CheckInputSysAbilityId(int32_t systemAbilityId);
};
} // DevAttest
} // OHOS
#endif // DEVATTEST_SYSTEM_ABILITY_LISTENER_H