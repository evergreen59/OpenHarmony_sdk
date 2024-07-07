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

#ifndef SERVICES_EDM_INCLUDE_ABILITY_MANAGER_DEATH_RECIPIENT_H
#define SERVICES_EDM_INCLUDE_ABILITY_MANAGER_DEATH_RECIPIENT_H

#include "iremote_object.h"

namespace OHOS {
namespace EDM {
class AbilityManagerDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    /**
     * Called back when the remote object is died.
     *
     * @param wptrDeath Indicates the died object.
     */
    void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
};
}  // namespace EDM
}  // namespace OHOS

#endif  // SERVICES_EDM_INCLUDE_ABILITY_MANAGER_DEATH_RECIPIENT_H