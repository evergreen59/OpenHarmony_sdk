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

#ifndef OHOS_ABILITY_RUNTIME_MAIN_THREAD_H
#define OHOS_ABILITY_RUNTIME_MAIN_THREAD_H

#include <string>
#include <mutex>
#include "event_handler.h"
#include "extension_config_mgr.h"
#include "idle_time.h"
#include "inner_event.h"
#include "app_scheduler_host.h"
#include "app_mgr_interface.h"
#include "ability_record_mgr.h"
#include "application_impl.h"
#include "resource_manager.h"
#include "foundation/ability/ability_runtime/interfaces/inner_api/runtime/include/runtime.h"
#include "foundation/ability/ability_runtime/interfaces/inner_api/runtime/include/source_map.h"
#include "ipc_singleton.h"
#include "native_engine/native_engine.h"
#include "watchdog.h"
#define ABILITY_LIBRARY_LOADER

class Runtime;
class ModSourceMap;
namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::Global;
using OHOS::AbilityRuntime::Runtime;
using OHOS::AbilityRuntime::ModSourceMap;
enum class MainThreadState { INIT, ATTACH, READY, RUNNING };
struct BundleInfo;
class ContextDeal;
// class Global::Resource::ResourceManager;
class AppMgrDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    /**
     *
     * @brief Notify the AppMgrDeathRecipient that the remote is dead.
     *
     * @param remote The remote which is dead.
     */
    virtual void OnRemoteDied(const wptr<IRemoteObject> &remote) override;
    AppMgrDeathRecipient() = default;
    ~AppMgrDeathRecipient() override = default;
};

class MainThread : public AppSchedulerHost {
    DECLARE_DELAYED_IPCSINGLETON(MainThread);

public:
    /**
     *
     * @brief Get the current MainThreadState.
     *
     * @return Returns the current MainThreadState.
     */
    MainThreadState GetMainThreadState() const;

    /**
     *
     * @brief Get the runner state of mainthread.
     *
     * @return Returns the runner state of mainthread.
     */
    bool GetRunnerStarted() const;

    /**
     *
     * @brief Get the newThreadId.
     *
     * @return Returns the newThreadId.
     */
    int GetNewThreadId();

    /**
     *
     * @brief Get the application.
     *
     * @return Returns the application.
     */
    std::shared_ptr<OHOSApplication> GetApplication() const;

    /**
     *
     * @brief Get the applicationInfo.
     *
     * @return Returns the applicationInfo.
     */
    std::shared_ptr<ApplicationInfo> GetApplicationInfo() const;

    /**
     *
     * @brief Get the applicationImpl.
     *
     * @return Returns the applicationImpl.
     */
    std::shared_ptr<ApplicationImpl> GetApplicationImpl();

    /**
     *
     * @brief Get the eventHandler of mainthread.
     *
     * @return Returns the eventHandler of mainthread.
     */
    std::shared_ptr<EventHandler> GetMainHandler() const;

    /**
     *
     * @brief Schedule the foreground lifecycle of application.
     *
     */
    void ScheduleForegroundApplication() override;

    /**
     *
     * @brief Schedule the background lifecycle of application.
     *
     */
    void ScheduleBackgroundApplication() override;

    /**
     *
     * @brief Schedule the terminate lifecycle of application.
     *
     */
    void ScheduleTerminateApplication() override;

    /**
     *
     * @brief Shrink the memory which used by application.
     *
     * @param level Indicates the memory trim level, which shows the current memory usage status.
     */
    void ScheduleShrinkMemory(const int level) override;

    /**
     *
     * @brief Notify the current memory.
     *
     * @param level Indicates the memory trim level, which shows the current memory usage status.
     */
    void ScheduleMemoryLevel(const int level) override;

    /**
     *
     * @brief Low the memory which used by application.
     *
     */
    void ScheduleLowMemory() override;

    /**
     *
     * @brief Launch the application.
     *
     * @param data The launchdata of the application witch launced.
     *
     */
    void ScheduleLaunchApplication(const AppLaunchData &data, const Configuration &config) override;

    /**
     *
     * @brief update the application info after new module installed.
     *
     * @param appInfo The latest application info obtained from bms for update abilityRuntimeContext.
     *
     */
    void ScheduleUpdateApplicationInfoInstalled(const ApplicationInfo &appInfo) override;

    /**
     * Notify application to launch ability stage.
     *
     * @param The resident process data value.
     */
    void ScheduleAbilityStage(const HapModuleInfo &abilityStage) override;

    void ScheduleLaunchAbility(const AbilityInfo &info, const sptr<IRemoteObject> &token,
        const std::shared_ptr<AAFwk::Want> &want) override;

    /**
     *
     * @brief clean the ability by token.
     *
     * @param token The token belong to the ability which want to be cleaned.
     *
     */
    void ScheduleCleanAbility(const sptr<IRemoteObject> &token) override;

    /**
     *
     * @brief send the new profile.
     *
     * @param profile The updated profile.
     *
     */
    void ScheduleProfileChanged(const Profile &profile) override;

    /**
     *
     * @brief send the new config to the application.
     *
     * @param config The updated config.
     *
     */
    void ScheduleConfigurationUpdated(const Configuration &config) override;

    /**
     *
     * @brief Starts the mainthread.
     *
     */
    static void Start();

    /**
     *
     * @brief Schedule the application process exit safely.
     *
     */
    void ScheduleProcessSecurityExit() override;

    void ScheduleAcceptWant(const AAFwk::Want &want, const std::string &moduleName) override;

    /**
     *
     * @brief Check the App main thread state.
     *
     */
    void CheckMainThreadIsAlive();

    int32_t ScheduleNotifyLoadRepairPatch(const std::string &bundleName, const sptr<IQuickFixCallback> &callback,
        const int32_t recordId) override;

    int32_t ScheduleNotifyHotReloadPage(const sptr<IQuickFixCallback> &callback, const int32_t recordId) override;

    int32_t ScheduleNotifyUnLoadRepairPatch(const std::string &bundleName,
        const sptr<IQuickFixCallback> &callback, const int32_t recordId) override;

private:
    /**
     *
     * @brief Terminate the application but don't notify ams.
     *
     */
    void HandleTerminateApplicationLocal();

    void HandleScheduleAcceptWant(const AAFwk::Want &want, const std::string &moduleName);

    /**
     *
     * @brief Schedule the application process exit safely.
     *
     */
    void HandleProcessSecurityExit();

    /**
     *
     * @brief Clean the ability but don't notify ams.
     *
     * @param token The token which belongs to the ability launched.
     *
     */
    void HandleCleanAbilityLocal(const sptr<IRemoteObject> &token);

    /**
     *
     * @brief Launch the application.
     *
     * @param appLaunchData The launchdata of the application witch launced.
     *
     */
    void HandleLaunchApplication(const AppLaunchData &appLaunchData, const Configuration &config);

    /**
     *
     * @brief update the application info after new module installed.
     *
     * @param appInfo The latest application info obtained from bms for update abilityRuntimeContext.
     *
     */
    void HandleUpdateApplicationInfoInstalled(const ApplicationInfo &appInfo);

    /**
     *
     * @brief Launch the application.
     *
     * @param appLaunchData The launchdata of the application witch launced.
     *
     */
    void HandleAbilityStage(const HapModuleInfo &abilityStage);

    /**
     *
     * @brief Launch the ability.
     *
     * @param abilityRecord The abilityRecord which belongs to the ability launched.
     *
     */
    void HandleLaunchAbility(const std::shared_ptr<AbilityLocalRecord> &abilityRecord);

    /**
     *
     * @brief Clean the ability.
     *
     * @param token The token which belongs to the ability launched.
     *
     */
    void HandleCleanAbility(const sptr<IRemoteObject> &token);

    /**
     *
     * @brief Foreground the application.
     *
     */
    void HandleForegroundApplication();

    /**
     *
     * @brief Background the application.
     *
     */
    void HandleBackgroundApplication();

    /**
     *
     * @brief Terminate the application.
     *
     */
    void HandleTerminateApplication();

    /**
     *
     * @brief Shrink the memory which used by application.
     *
     * @param level Indicates the memory trim level, which shows the current memory usage status.
     *
     */
    void HandleShrinkMemory(const int level);

    /**
     *
     * @brief Notify the memory.
     *
     * @param level Indicates the memory trim level, which shows the current memory usage status.
     *
     */
    void HandleMemoryLevel(int level);

    /**
     *
     * @brief send the new config to the application.
     *
     * @param config The updated config.
     *
     */
    void HandleConfigurationUpdated(const Configuration &config);

    /**
     *
     * @brief remove the deathRecipient from appMgr.
     *
     */
    void RemoveAppMgrDeathRecipient();

    /**
     *
     * @brief Attach the mainthread to the AppMgr.
     *
     */
    void Attach();

    /**
     *
     * @brief Set the runner state of mainthread.
     *
     * @param runnerStart whether the runner is started.
     */
    void SetRunnerStarted(bool runnerStart);

    /**
     *
     * @brief Connect the mainthread to the AppMgr.
     *
     */
    bool ConnectToAppMgr();

    /**
     *
     * @brief Check whether the appLaunchData is legal.
     *
     * @param appLaunchData The appLaunchData should be checked.
     *
     * @return if the appLaunchData is legal, return true. else return false.
     */
    bool CheckLaunchApplicationParam(const AppLaunchData &appLaunchData) const;

    /**
     *
     * @brief Check whether the record is legal.
     *
     * @param record The record should be checked.
     *
     * @return if the record is legal, return true. else return false.
     */
    bool CheckAbilityItem(const std::shared_ptr<AbilityLocalRecord> &record) const;

    /**
     *
     * @brief Init the mainthread.
     *
     * @param runner the runner belong to the mainthread.
     *
     */
    void Init(const std::shared_ptr<EventRunner> &runner);

    /**
     *
     * @brief Task in event handler timeout detected.
     *
     * @param runner the runner belong to the mainthread.
     *
     */
    void TaskTimeoutDetected(const std::shared_ptr<EventRunner>& runner);

    /**
     *
     * @brief Check whether the OHOSApplication is ready.
     *
     * @return if the OHOSApplication is ready, return true. else return false.
     *
     */
    bool IsApplicationReady() const;

    /**
     * @brief Load all extension so
     *
     * @param nativeEngine nativeEngine
     * @param filePath extension so file path
     */
    void LoadAllExtensions(NativeEngine &nativeEngine, const std::string &filePath);

    /**
     *
     * @brief Ability Delegator Prepare.
     *
     * @param record User Test info.
     *
     */
    bool PrepareAbilityDelegator(const std::shared_ptr<UserTestRecord> &record, bool isStageBased,
        BundleInfo& bundleInfo);

    /**
     * @brief Update current process extension type
     *
     * @param abilityRecord current running ability record
     */
    void UpdateProcessExtensionType(const std::shared_ptr<AbilityLocalRecord> &abilityRecord);

    /**
     * @brief Add Extension block item
     *
     * @param extensionName extension name
     * @param type extension type
     */
    void AddExtensionBlockItem(const std::string &extensionName, int32_t type);

    /**
     * @brief Update extension block list to nativeEngine
     *
     * @param nativeEngine nativeEngine instance
     */
    void UpdateEngineExtensionBlockList(NativeEngine &nativeEngine);

    static void HandleDumpHeap(bool isPrivate);
    static void HandleSignal(int signal);

    bool Timer();
    bool WaitForDuration(uint32_t duration);
    void reportEvent();
    bool IsStopWatchdog();

    class MainHandler : public EventHandler {
    public:
        MainHandler(const std::shared_ptr<EventRunner> &runner, const sptr<MainThread> &thread);
        virtual ~MainHandler() = default;

        /**
         *
         * @brief Process the event.
         *
         * @param event the event want to be processed.
         *
         */
        void ProcessEvent(const OHOS::AppExecFwk::InnerEvent::Pointer &event) override;

    private:
        sptr<MainThread> mainThreadObj_ = nullptr;
    };

    bool isRunnerStarted_ = false;
    int newThreadId_ = -1;
    std::shared_ptr<Profile> appProfile_ = nullptr;
    std::shared_ptr<ApplicationInfo> applicationInfo_ = nullptr;
    std::shared_ptr<ProcessInfo> processInfo_ = nullptr;
    std::shared_ptr<OHOSApplication> application_ = nullptr;
    std::shared_ptr<ApplicationImpl> applicationImpl_ = nullptr;
    static std::shared_ptr<MainHandler> mainHandler_;
    std::shared_ptr<AbilityRecordMgr> abilityRecordMgr_ = nullptr;
    std::shared_ptr<Watchdog> watchdog_ = nullptr;
    std::unique_ptr<AbilityRuntime::ExtensionConfigMgr> extensionConfigMgr_ = nullptr;
    MainThreadState mainThreadState_ = MainThreadState::INIT;
    sptr<IAppMgr> appMgr_ = nullptr;  // appMgrService Handler
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
    std::string aceApplicationName_ = "AceApplication";
    std::string pathSeparator_ = "/";
    std::string abilityLibraryType_ = ".so";
    static std::shared_ptr<EventHandler> signalHandler_;
    static std::shared_ptr<OHOSApplication> applicationForDump_;

#ifdef ABILITY_LIBRARY_LOADER
    /**
     *
     * @brief Load the ability library.
     *
     * @param libraryPaths the library paths.
     *
     */
    void LoadAbilityLibrary(const std::vector<std::string> &libraryPaths);

    void LoadNativeLiabrary(std::string &nativeLibraryPath);

    void LoadAppDetailAbilityLibrary(std::string &nativeLibraryPath);

    void LoadAppLibrary();

    void ChangeToLocalPath(const std::string &bundleName,
        const std::vector<std::string> &sourceDirs, std::vector<std::string> &localPath);

    bool ScanDir(const std::string &dirPath, std::vector<std::string> &files);

    /**
     *
     * @brief Check the fileType.
     *
     * @param fileName The fileName of the lib.
     * @param extensionName The extensionName of the lib.
     *
     * @return if the FileType is legal, return true. else return false.
     *
     */
    bool CheckFileType(const std::string &fileName, const std::string &extensionName);

    bool InitCreate(std::shared_ptr<ContextDeal> &contextDeal, ApplicationInfo &appInfo, ProcessInfo &processInfo,
        Profile &appProfile);
    bool CheckForHandleLaunchApplication(const AppLaunchData &appLaunchData);
    bool InitResourceManager(std::shared_ptr<Global::Resource::ResourceManager> &resourceManager,
        BundleInfo& bundleInfo, const Configuration &config);

    bool GetHqfFileAndHapPath(const std::string &bundleName,
        std::vector<std::pair<std::string, std::string>> &fileMap);

    std::vector<std::string> fileEntries_;
    std::vector<std::string> nativeFileEntries_;
    std::vector<void *> handleAbilityLib_;  // the handler of ACE Library.
    std::shared_ptr<IdleTime> idleTime_ = nullptr;
#endif                                      // ABILITY_LIBRARY_LOADER
#ifdef APPLICATION_LIBRARY_LOADER
    void *handleAppLib_ = nullptr;  // the handler of ACE Library.
    constexpr static std::string applicationLibraryPath = "/hos/lib/libapplication_native.z.so";
#endif  // APPLICATION_LIBRARY_LOADER
    DISALLOW_COPY_AND_MOVE(MainThread);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_MAIN_THREAD_H
