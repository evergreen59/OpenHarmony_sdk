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

#ifndef OHOS_ABILITY_RUNTIME_ABILITY_PROCESS_H
#define OHOS_ABILITY_RUNTIME_ABILITY_PROCESS_H

#include <mutex>
#include "feature_ability.h"
#include "napi_context.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * @class AbilityProcess
 * Provides the feature ability function.
 */
class AbilityProcess {
public:
    AbilityProcess();
    virtual ~AbilityProcess();
    static std::shared_ptr<AbilityProcess> GetInstance();

    ErrCode StartAbility(Ability *ability, CallAbilityParam param, CallbackInfo callback);
    void OnAbilityResult(Ability *ability, int requestCode, int resultCode, const Want &resultData);
    void AddAbilityResultCallback(Ability *ability, CallAbilityParam &param, int32_t errCode, CallbackInfo &callback);

    void RequestPermissionsFromUser(Ability *ability, CallAbilityPermissionParam &param, CallbackInfo callbackInfo);

private:
    bool CaullFunc(int requestCode, const std::vector<std::string> &permissions,
        const std::vector<int> &permissionsState, CallbackInfo &callbackInfo);

private:
    static std::mutex mutex_;
    static std::shared_ptr<AbilityProcess> instance_;
    static std::map<Ability *, std::map<int, CallbackInfo>> abilityResultMap_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_ABILITY_PROCESS_H
