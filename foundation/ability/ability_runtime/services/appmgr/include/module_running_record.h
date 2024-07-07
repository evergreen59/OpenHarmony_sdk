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

#ifndef OHOS_ABILITY_RUNTIME_MODULE_RUNNING_RECORD_H
#define OHOS_ABILITY_RUNTIME_MODULE_RUNNING_RECORD_H

#include <string>
#include <list>
#include <map>
#include <memory>
#include "iremote_object.h"

#include "ability_info.h"
#include "application_info.h"
#include "ability_running_record.h"
#include "app_mgr_constants.h"
#include "app_lifecycle_deal.h"
#include "app_mgr_service_event_handler.h"

namespace OHOS {
namespace AppExecFwk {
enum class ModuleRecordState {
    UNKNOWN_STATE,
    INITIALIZED_STATE,
    RUNNING_STATE,
};

class AppMgrServiceInner;
class AppRunningRecord;
class ModuleRunningRecord {
public:
    ModuleRunningRecord(
        const std::shared_ptr<ApplicationInfo> &info, const std::shared_ptr<AMSEventHandler> &eventHandler);
    virtual ~ModuleRunningRecord();

    /**
     * @brief Obtains module moduleName.
     *
     * @return Returns module moduleName.
     */
    const std::string &GetModuleName() const;

    /**
     * @param name, the module main ability name.
     *
     * @return
     */
    void GetMainAbilityName(const std::string &name);

    void Init(const HapModuleInfo &info);

    const HapModuleInfo GetModuleInfo();

    /**
     * GetAbilityRunningRecordByToken, Obtaining the ability record through token.
     *
     * @param token, the unique identification to the ability.
     *
     * @return
     */
    std::shared_ptr<AbilityRunningRecord> GetAbilityRunningRecordByToken(const sptr<IRemoteObject> &token) const;

    std::shared_ptr<AbilityRunningRecord> AddAbility(const sptr<IRemoteObject> &token,
        const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<AAFwk::Want> &want);

    bool IsLastAbilityRecord(const sptr<IRemoteObject> &token);

    int32_t GetPageAbilitySize();

    // Get abilities_ for this process
    /**
     * @brief Obtains the abilities info for the application record.
     *
     * @return Returns the abilities info for the application record.
     */
    const std::map<const sptr<IRemoteObject>, std::shared_ptr<AbilityRunningRecord>> &GetAbilities() const;

    std::shared_ptr<AbilityRunningRecord> GetAbilityByTerminateLists(const sptr<IRemoteObject> &token) const;

    std::shared_ptr<AbilityRunningRecord> GetAbilityRunningRecord(const int64_t eventId) const;

    /**
     * LaunchAbility, Notify application to launch ability.
     *
     * @param ability, the ability record.
     *
     * @return
     */
    void LaunchAbility(const std::shared_ptr<AbilityRunningRecord> &ability);

    /**
     * LaunchPendingAbilities, Launch Pending Abilities.
     *
     * @return
     */
    void LaunchPendingAbilities();

    /**
     * TerminateAbility, terminate the token ability.
     *
     * @param token, he unique identification to terminate the ability.
     *
     * @return
     */
    void TerminateAbility(const std::shared_ptr<AppRunningRecord> &appRecord,
        const sptr<IRemoteObject> &token, const bool isForce);

    /**
     * AbilityTerminated, terminate the ability.
     *
     * @param token, the unique identification to terminated the ability.
     *
     * @return
     */
    void AbilityTerminated(const sptr<IRemoteObject> &token);

    /**
     * @brief Setting application service internal handler instance.
     *
     * @param serviceInner, application service internal handler instance.
     */
    void SetAppMgrServiceInner(const std::weak_ptr<AppMgrServiceInner> &inner);

    // drive application state changes when ability state changes.
    /**
     * OnAbilityStateChanged, Call ability state change.
     *
     * @param ability, the ability info.
     * @param state, the ability state.
     *
     * @return
     */
    void OnAbilityStateChanged(const std::shared_ptr<AbilityRunningRecord> &ability, const AbilityState state);

    ModuleRecordState GetModuleRecordState();

    void SetModuleRecordState(const ModuleRecordState &state);

    void GetHapModuleInfo(HapModuleInfo &info);

    void SetApplicationClient(std::shared_ptr<AppLifeCycleDeal> &appLifeCycleDeal);

    const std::shared_ptr<ApplicationInfo> GetAppInfo();

    bool RemoveTerminateAbilityTimeoutTask(const sptr<IRemoteObject>& token) const;

private:
    void SendEvent(uint32_t msg, int64_t timeOut, const std::shared_ptr<AbilityRunningRecord> &abilityRecord);

    ModuleRecordState GetState() const;

private:
    std::map<const sptr<IRemoteObject>, std::shared_ptr<AbilityRunningRecord>> abilities_;
    std::map<const sptr<IRemoteObject>, std::shared_ptr<AbilityRunningRecord>> terminateAbilities_;
    std::weak_ptr<AppMgrServiceInner> appMgrServiceInner_;
    std::weak_ptr<AppRunningRecord> appRunningRecord_;
    std::shared_ptr<AppLifeCycleDeal> appLifeCycleDeal_;
    std::shared_ptr<ApplicationInfo> appInfo_;  // the application's info
    std::shared_ptr<AMSEventHandler> eventHandler_;
    HapModuleInfo owenInfo_;
    ModuleRecordState owenState_ = ModuleRecordState::UNKNOWN_STATE;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_MODULE_RUNNING_RECORD_H
