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
#include <gtest/gtest.h>
#include "sys_mgr_client.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
SysMrgClient::SysMrgClient() : abilityManager_(nullptr)
{}

SysMrgClient::~SysMrgClient()
{}

sptr<IRemoteObject> SysMrgClient::GetSystemAbility(const int32_t systemAbilityId)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_GetHapModuleInfo_0100 SysMrgClient::GetSystemAbility( "
        << systemAbilityId << " )";
    if (servicesMap_[systemAbilityId] == nullptr) {
        OHOS::sptr<ISystemAbilityManager> abilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (abilityManager == nullptr) {
            HILOG_ERROR("%s:fail to get Registry", __func__);
            return nullptr;
        }
        OHOS::sptr<OHOS::IRemoteObject> object = abilityManager->GetSystemAbility(systemAbilityId);
        servicesMap_[systemAbilityId] = object;
    }
    auto ret = servicesMap_[systemAbilityId];
    GTEST_LOG_(INFO) << "AaFwk_Ability_GetHapModuleInfo_0100 SysMrgClient::GetSystemAbility( "
        << ret.GetRefPtr() << " )";
    return ret;
}

void SysMrgClient::RegisterSystemAbility(const int32_t systemAbilityId, sptr<IRemoteObject> broker)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_GetHapModuleInfo_0100 SysMrgClient::RegisterSystemAbility( "
        << broker.GetRefPtr() << " )";
    servicesMap_[systemAbilityId] = broker;
}
}  // namespace AppExecFwk
}  // namespace OHOS
