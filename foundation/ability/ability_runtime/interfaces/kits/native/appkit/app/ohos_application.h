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

#ifndef OHOS_ABILITY_RUNTIME_OHOS_APPLICATION_H
#define OHOS_ABILITY_RUNTIME_OHOS_APPLICATION_H

#include <string>
#include <list>
#include <memory>

#include "ability_lifecycle_callbacks.h"
#include "foundation/ability/ability_runtime/interfaces/kits/native/appkit/ability_runtime/context/context.h"
#include "ability_stage.h"
#include "app_context.h"
#include "element_callback.h"

namespace OHOS {
namespace AbilityRuntime {
class Runtime;
} // namespace AbilityRuntime
namespace AppExecFwk {
class ElementsCallback;
class ApplicationImpl;
class Configuration;
class AbilityRecordMgr;
class OHOSApplication : public AppContext,
                        public AbilityLifecycleCallbacks,
                        public std::enable_shared_from_this<OHOSApplication> {
public:
    OHOSApplication();
    virtual ~OHOSApplication();

    /**
     * @brief dump OHOSApplication info
     *
     * @param extra dump OHOSApplication info
     */
    void DumpApplication();

    /**
     * @brief Set Runtime
     *
     * @param runtime Runtime instance.
     */
    void SetRuntime(std::unique_ptr<AbilityRuntime::Runtime>&& runtime);

    /**
     * @brief Set ApplicationContext
     *
     * @param context ApplicationContext instance.
     */
    void SetApplicationContext(const std::shared_ptr<AbilityRuntime::ApplicationContext> &abilityRuntimeContext);

    /**
     *
     * @brief Set the abilityRecordMgr to the OHOSApplication.
     *
     * @param abilityRecordMgr
     */
    void SetAbilityRecordMgr(const std::shared_ptr<AbilityRecordMgr> &abilityRecordMgr);

    /**
     *
     * @brief Register AbilityLifecycleCallbacks with OHOSApplication
     *
     * @param callBack callBack When the life cycle of the ability in the application changes,
     */
    void RegisterAbilityLifecycleCallbacks(const std::shared_ptr<AbilityLifecycleCallbacks> &callBack);

    /**
     *
     * @brief Unregister AbilityLifecycleCallbacks with OHOSApplication
     *
     * @param callBack RegisterAbilityLifecycleCallbacks`s callBack
     */
    void UnregisterAbilityLifecycleCallbacks(const std::shared_ptr<AbilityLifecycleCallbacks> &callBack);

    /**
     *
     * @brief Will be called when the given ability calls Ability->onStart
     *
     * @param Ability Indicates the ability object that calls the onStart() method.
     */
    void OnAbilityStart(const std::shared_ptr<Ability> &ability);

    /**
     *
     * @brief Will be called when the given ability calls Ability->onInactive
     *
     * @param Ability Indicates the Ability object that calls the onInactive() method.
     */
    void OnAbilityInactive(const std::shared_ptr<Ability> &ability);

    /**
     *
     * @brief Will be called when the given ability calls Ability->onBackground
     *
     * @param Ability Indicates the Ability object that calls the onBackground() method.
     */
    void OnAbilityBackground(const std::shared_ptr<Ability> &ability);

    /**
     *
     * @brief Will be called when the given ability calls Ability->onForeground
     *
     * @param Ability Indicates the Ability object that calls the onForeground() method.
     */
    void OnAbilityForeground(const std::shared_ptr<Ability> &ability);

    /**
     *
     * @brief Will be called when the given ability calls Ability->onActive
     *
     * @param Ability Indicates the Ability object that calls the onActive() method.
     */
    void OnAbilityActive(const std::shared_ptr<Ability> &ability);

    /**
     *
     * @brief Will be called when the given ability calls Ability->onStop
     *
     * @param Ability Indicates the Ability object that calls the onStop() method.
     */
    void OnAbilityStop(const std::shared_ptr<Ability> &ability);

    /**
     *
     * Called when Ability#onSaveAbilityState(PacMap) was called on an ability.
     *
     * @param outState Indicates the PacMap object passed to Ability#onSaveAbilityState(PacMap)
     * for storing user data and states. This parameter cannot be null.
     */
    void DispatchAbilitySavedState(const PacMap &outState);

    /**
     *
     * @brief Called when an ability calls Ability#onSaveAbilityState(PacMap).
     * You can implement your own logic in this method.
     * @param outState IIndicates the {@link PacMap} object passed to the onSaveAbilityState() callback.
     *
     */
    void OnAbilitySaveState(const PacMap &outState);

    /**
     *
     * @brief Register ElementsCallback with OHOSApplication
     *
     * @param callBack callBack when the system configuration of the device changes.
     */
    void RegisterElementsCallbacks(const std::shared_ptr<ElementsCallback> &callback);

    /**
     *
     * @brief Unregister ElementsCallback with OHOSApplication
     *
     * @param callback RegisterElementsCallbacks`s callback
     */
    void UnregisterElementsCallbacks(const std::shared_ptr<ElementsCallback> &callback);

    /**
     *
     * @brief Will be Called when the system configuration of the device changes.
     *
     * @param config Indicates the new Configuration object.
     */
    virtual void OnConfigurationUpdated(const Configuration &config);

    /**
     *
     * @brief Called when the system has determined to trim the memory, for example,
     * when the ability is running in the background and there is no enough memory for
     * running as many background processes as possible.
     *
     * @param level Indicates the memory trim level, which shows the current memory usage status.
     */
    virtual void OnMemoryLevel(int level);

    /**
     *
     * @brief Will be called the application foregrounds
     *
     */
    virtual void OnForeground();

    /**
     *
     * @brief Will be called the application backgrounds
     *
     */
    virtual void OnBackground();

    /**
     *
     * @brief Will be called the application starts
     *
     */
    virtual void OnStart();

    /**
     *
     * @brief Will be called the application ends
     *
     */
    virtual void OnTerminate();

    /**
     * @brief add the ability stage when a hap first load
     *
     * @param abilityRecord
     * @return abilityStage context
     */
    std::shared_ptr<AbilityRuntime::Context> AddAbilityStage(
        const std::shared_ptr<AbilityLocalRecord> &abilityRecord);

    /**
     *
     * @brief update the application info after new module installed.
     *
     * @param appInfo The latest application info obtained from bms for update abilityRuntimeContext.
     *
     */
    void UpdateApplicationInfoInstalled(const AppExecFwk::ApplicationInfo &appInfo);

    /**
     * @brief add the ability stage when a hap first load
     *
     * @param hapModuleInfo
     * @return Returns true on success, false on failure
     */
    bool AddAbilityStage(const AppExecFwk::HapModuleInfo &hapModuleInfo);

    /**
     * @brief remove the ability stage when all of the abilities in the hap have been removed
     *
     * @param abilityInfo
     */
    void CleanAbilityStage(const sptr<IRemoteObject> &token, const std::shared_ptr<AbilityInfo> &abilityInfo);

    /**
     * @brief return the application context
     *
     * @param context
     */
    std::shared_ptr<AbilityRuntime::Context> GetAppContext() const;

    /**
     * @brief return the application runtime
     *
     * @param runtime
     */
    const std::unique_ptr<AbilityRuntime::Runtime>& GetRuntime();

    /*
     *
     * @brief Will be called the application ends
     *
     */
    virtual void SetConfiguration(const Configuration &config);

    void ScheduleAcceptWant(const AAFwk::Want &want, const std::string &moduleName, std::string &flag);

    virtual std::shared_ptr<Configuration> GetConfiguration();

    void GetExtensionNameByType(int32_t type, std::string &name)
    {
        std::map<int32_t, std::string>::iterator it = extensionTypeMap_.find(type);
        if (it == extensionTypeMap_.end()) {
            return;
        }
        name = it->second;
    }

    /**
     * @brief Set extension types.
     *
     * @param map The extension types.
     */
    void SetExtensionTypeMap(std::map<int32_t, std::string> map);

    bool NotifyLoadRepairPatch(const std::string &hqfFile, const std::string &hapPath);

    bool NotifyHotReloadPage();

    bool NotifyUnLoadRepairPatch(const std::string &hqfFile);

private:
    std::list<std::shared_ptr<AbilityLifecycleCallbacks>> abilityLifecycleCallbacks_;
    std::list<std::shared_ptr<ElementsCallback>> elementsCallbacks_;
    std::shared_ptr<AbilityRecordMgr> abilityRecordMgr_ = nullptr;
    std::shared_ptr<AbilityRuntime::ApplicationContext> abilityRuntimeContext_ = nullptr;
    std::unordered_map<std::string, std::shared_ptr<AbilityRuntime::AbilityStage>> abilityStages_;
    std::unique_ptr<AbilityRuntime::Runtime> runtime_;
    std::shared_ptr<Configuration> configuration_ = nullptr;
    std::map<int32_t, std::string> extensionTypeMap_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_OHOS_APPLICATION_H
