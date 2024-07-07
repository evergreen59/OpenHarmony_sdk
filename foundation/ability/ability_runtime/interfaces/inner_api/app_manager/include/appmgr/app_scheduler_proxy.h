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

#ifndef OHOS_ABILITY_RUNTIME_APP_SCHEDULER_PROXY_H
#define OHOS_ABILITY_RUNTIME_APP_SCHEDULER_PROXY_H

#include "iremote_proxy.h"
#include "app_scheduler_interface.h"

namespace OHOS {
namespace AppExecFwk {
class AppSchedulerProxy : public IRemoteProxy<IAppScheduler> {
public:
    explicit AppSchedulerProxy(const sptr<IRemoteObject> &impl);
    virtual ~AppSchedulerProxy() = default;

    /**
     * ScheduleForegroundApplication, call ScheduleForegroundApplication() through proxy project,
     * Notify application to switch to foreground.
     *
     * @return
     */
    virtual void ScheduleForegroundApplication() override;

    /**
     * ScheduleBackgroundApplication, call ScheduleBackgroundApplication() through proxy project,
     * Notify application to switch to background.
     *
     * @return
     */
    virtual void ScheduleBackgroundApplication() override;

    /**
     * ScheduleTerminateApplication, call ScheduleTerminateApplication() through proxy project,
     * Notify application to terminate.
     *
     * @return
     */
    virtual void ScheduleTerminateApplication() override;

    /**
     * ScheduleShrinkMemory, call ScheduleShrinkMemory() through proxy project,
     * Notifies the application of the memory seen.
     *
     * @param The memory value.
     *
     * @return
     */
    virtual void ScheduleShrinkMemory(const int32_t level) override;

    /**
     * ScheduleLowMemory, call ScheduleLowMemory() through proxy project,
     * Notify application to low memory.
     *
     * @return
     */
    virtual void ScheduleLowMemory() override;

    /**
     * ScheduleMemoryLevel, call ScheduleMemoryLevel() through proxy project,
     * Notify applications background the current memory level.
     *
     * @return
     */
    virtual void ScheduleMemoryLevel(int32_t level) override;

    /**
     * ScheduleLaunchApplication, call ScheduleLaunchApplication() through proxy project,
     * Notify application to launch application.
     *
     * @param The app data value.
     *
     * @return
     */
    virtual void ScheduleLaunchApplication(const AppLaunchData &launchData, const Configuration &config) override;

    /**
     * ScheduleUpdateApplicationInfoInstalled, call ScheduleUpdateApplicationInfoInstalled() through proxy object,
     * update the application info after new module installed.
     *
     * @param appInfo The latest application info obtained from bms for update abilityRuntimeContext.
     *
     * @return
     */
    virtual void ScheduleUpdateApplicationInfoInstalled(const ApplicationInfo &) override;

    /**
     * Notify application to launch ability stage.
     *
     * @param The resident process data value.
     */
    virtual void ScheduleAbilityStage(const HapModuleInfo &abilityStage) override;

    /**
     * ScheduleLaunchAbility, call ScheduleLaunchAbility() through proxy project,
     * Notify application to launch ability.
     *
     * @param The ability info.
     * @param The ability token.
     * @param The ability want.
     * @return
     */
    virtual void ScheduleLaunchAbility(const AbilityInfo &, const sptr<IRemoteObject> &,
        const std::shared_ptr<AAFwk::Want> &want) override;

    /**
     * ScheduleCleanAbility, call ScheduleCleanAbility() through proxy project,
     * Notify application to clean ability.
     *
     * @param The ability token.
     * @return
     */
    virtual void ScheduleCleanAbility(const sptr<IRemoteObject> &token) override;

    /**
     * ScheduleProfileChanged, call ScheduleProfileChanged() through proxy project,
     * Notify application to profile update.
     *
     * @param The profile data.
     * @return
     */
    virtual void ScheduleProfileChanged(const Profile &profile) override;

    /**
     * ScheduleConfigurationUpdated, call ScheduleConfigurationUpdated() through proxy project,
     * Notify application to configuration update.
     *
     * @param The configuration data.
     * @return
     */
    virtual void ScheduleConfigurationUpdated(const Configuration &config) override;

    /**
     * ScheduleProcessSecurityExit, call ScheduleProcessSecurityExit() through proxy project,
     * Notify application process exit safely.
     *
     * @return
     */
    virtual void ScheduleProcessSecurityExit() override;

    virtual void ScheduleAcceptWant(const AAFwk::Want &want, const std::string &moduleName) override;

    int32_t ScheduleNotifyLoadRepairPatch(const std::string &bundleName,
        const sptr<IQuickFixCallback> &callback, const int32_t recordId) override;

    int32_t ScheduleNotifyHotReloadPage(const sptr<IQuickFixCallback> &callback, const int32_t recordId) override;

    int32_t ScheduleNotifyUnLoadRepairPatch(const std::string &bundleName,
        const sptr<IQuickFixCallback> &callback, const int32_t recordId) override;

private:
    bool WriteInterfaceToken(MessageParcel &data);
    void ScheduleMemoryCommon(const int32_t level, const uint32_t operation);
    static inline BrokerDelegator<AppSchedulerProxy> delegator_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_APP_SCHEDULER_PROXY_H
