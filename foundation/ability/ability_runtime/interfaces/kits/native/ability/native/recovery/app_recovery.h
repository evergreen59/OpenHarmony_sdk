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

#ifndef OHOS_ABILITY_APP_RECOVERY_H
#define OHOS_ABILITY_APP_RECOVERY_H

#include <memory>
#include <mutex>
#include <vector>

#include "ability.h"
#include "ability_info.h"
#include "ability_recovery.h"
#include "application_info.h"
#include "event_handler.h"
#include "iremote_object.h"
#include "parcel.h"
#include "want.h"
#include "want_params.h"

namespace OHOS {
namespace AppExecFwk {
class AppRecovery {
public:
    static AppRecovery& GetInstance();
    void EnableAppRecovery(uint16_t restartFlag, uint16_t saveFlag, uint16_t saveMode);
    bool InitApplicationInfo(const std::shared_ptr<AppExecFwk::EventHandler>& mainHandler,
        const std::shared_ptr<ApplicationInfo>& applicationInfo);
    bool AddAbility(const std::shared_ptr<Ability>& ability,
        const std::shared_ptr<AbilityInfo>& abilityInfo, const sptr<IRemoteObject>& token);

    bool IsEnabled() const;
    bool ScheduleRecoverApp(StateReason reason);
    bool ScheduleSaveAppState(StateReason reason);
    bool TryRecoverApp(StateReason reason);
    bool PersistAppState();

    uint16_t GetRestartFlag() const;
    uint16_t GetSaveOccasionFlag() const;
    uint16_t GetSaveModeFlag() const;

private:
    AppRecovery();
    ~AppRecovery();
    bool ShouldSaveAppState(StateReason reason);
    bool ShouldRecoverApp(StateReason reason);

    void DoRecoverApp(StateReason reason);
    void DoSaveAppState(StateReason reason);

    bool isEnable_;
    uint16_t restartFlag_;
    uint16_t saveOccasion_;
    uint16_t saveMode_;
    std::weak_ptr<AppExecFwk::EventHandler> mainHandler_;
    std::weak_ptr<AppExecFwk::ApplicationInfo> applicationInfo_;
    std::vector<std::shared_ptr<AbilityRecovery>> abilityRecoverys_;
};
}  // namespace AbilityRuntime
}  // namespace OHOS
#endif // OHOS_ABILITY_APP_RECOVERY_H