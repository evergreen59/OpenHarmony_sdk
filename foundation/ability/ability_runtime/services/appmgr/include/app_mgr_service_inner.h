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

#ifndef OHOS_ABILITY_RUNTIME_APP_MGR_SERVICE_INNER_H
#define OHOS_ABILITY_RUNTIME_APP_MGR_SERVICE_INNER_H

#include <list>
#include <map>
#include <vector>
#include <regex>
#include <unordered_map>

#include "iremote_object.h"
#include "refbase.h"

#include "ability_info.h"
#include "appexecfwk_errors.h"
#include "app_death_recipient.h"
#include "app_mgr_constants.h"
#include "app_record_id.h"
#include "app_running_record.h"
#include "app_scheduler_interface.h"
#include "app_spawn_client.h"
#include "app_task_info.h"
#include "iapp_state_callback.h"
#include "iapplication_state_observer.h"
#include "iconfiguration_observer.h"
#include "app_process_manager.h"
#include "remote_client_manager.h"
#include "app_running_manager.h"
#include "record_query_result.h"
#include "running_process_info.h"
#include "bundle_info.h"
#include "istart_specified_ability_response.h"

#include "want.h"
#include "window_focus_changed_listener.h"

namespace OHOS {
namespace AppExecFwk {
using OHOS::AAFwk::Want;
class AppMgrServiceInner : public std::enable_shared_from_this<AppMgrServiceInner> {
public:
    AppMgrServiceInner();
    virtual ~AppMgrServiceInner();

    /**
     * Initialize the object.
     */
    void Init();
    /**
     * LoadAbility, load the ability that needed to be started.
     *
     * @param token, the unique identification to start the ability.
     * @param preToken, the unique identification to call the ability.
     * @param abilityInfo, the ability information.
     * @param appInfo, the app information.
     * @param want the ability want.
     *
     * @return
     */
    virtual void LoadAbility(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
        const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<ApplicationInfo> &appInfo,
        const std::shared_ptr<AAFwk::Want> &want);

    /**
     * TerminateAbility, terminate the token ability.
     *
     * @param token, he unique identification to terminate the ability.
     * @param clearMissionFlag, indicates whether terminate the ability when clearMission.
     * @return
     */
    virtual void TerminateAbility(const sptr<IRemoteObject> &token, bool clearMissionFlag);

    /**
     * UpdateAbilityState, update the ability status.
     *
     * @param token, the unique identification to update the ability.
     * @param state, ability status that needs to be updated.
     *
     * @return
     */
    virtual void UpdateAbilityState(const sptr<IRemoteObject> &token, const AbilityState state);

    /**
     * UpdateExtensionState, call UpdateExtensionState() through the proxy object, update the extension status.
     *
     * @param token, the unique identification to update the extension.
     * @param state, extension status that needs to be updated.
     */
    virtual void UpdateExtensionState(const sptr<IRemoteObject> &token, const ExtensionState state);

    /**
     * StateChangedNotifyObserver, Call ability state change.
     *
     * @param ability, the ability info.
     * @param state, the ability state.
     *
     * @return
     */
    void StateChangedNotifyObserver(const AbilityStateData abilityStateData, bool isAbility);

    /**
     * RegisterAppStateCallback, register the callback.
     *
     * @param callback, Ams register the callback.
     *
     * @return
     */
    virtual void RegisterAppStateCallback(const sptr<IAppStateCallback> &callback);

    /**
     * AbilityBehaviorAnalysis, ability behavior analysis assistant process optimization.
     *
     * @param token, the unique identification to start the ability.
     * @param preToken, the unique identification to call the ability.
     * @param visibility, the visibility information about windows info.
     * @param perceptibility, the Perceptibility information about windows info.
     * @param connectionState, the service ability connection state.
     * @return
     */
    virtual void AbilityBehaviorAnalysis(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
        const int32_t visibility, const int32_t perceptibility, const int32_t connectionState);

    /**
     * KillProcessByAbilityToken, kill the process by ability token.
     *
     * @param token, the unique identification to the ability.
     * @return
     */
    virtual void KillProcessByAbilityToken(const sptr<IRemoteObject> &token);

    /**
     * KillProcessesByUserId, kill the processes by userId.
     *
     * @param userId, the user id.
     * @return
     */
    virtual void KillProcessesByUserId(int32_t userId);

    /**
     * AttachApplication, get all the information needed to start the Application
     * (data related to the Application ).
     *
     * @param appScheduler, information needed to start the Application.
     *
     * @return
     */
    virtual void AttachApplication(const pid_t pid, const sptr<IAppScheduler> &appScheduler);

    /**
     * ApplicationForegrounded, set the application to Foreground State.
     *
     * @param recordId, a unique record that identifies this Application from others.
     *
     * @return
     */
    virtual void ApplicationForegrounded(const int32_t recordId);

    /**
     * ApplicationBackgrounded, set the application to Backgrounded State.
     *
     * @param recordId, a unique record that identifies this Application from others.
     *
     * @return
     */
    virtual void ApplicationBackgrounded(const int32_t recordId);

    /**
     * ApplicationTerminated, terminate the application.
     *
     * @param recordId, a unique record that identifies this Application from others.
     *
     * @return
     */
    virtual void ApplicationTerminated(const int32_t recordId);

    /**
     * AbilityTerminated, terminate the ability.
     *
     * @param token, the unique identification to terminated the ability.
     *
     * @return
     */
    virtual void AbilityTerminated(const sptr<IRemoteObject> &token);

    /**
     * UpdateApplicationInfoInstalled, update the application info after new module installed.
     *
     * @param bundleName, bundle name in Application record.
     * @param  uid, uid.
     * @return ERR_OK, return back success, others fail.
     */
    virtual int32_t UpdateApplicationInfoInstalled(const std::string &bundleName, const int uid);

    /**
     * KillApplication, kill the application.
     *
     * @param  bundleName, bundle name in Application record.
     *
     * @return ERR_OK, return back success, others fail.
     */
    virtual int32_t KillApplication(const std::string &bundleName);

    /**
     * KillApplicationByUid, call KillApplicationByUid() through proxy object, kill the application.
     *
     * @param  bundleName, bundle name in Application record.
     * @param  uid, uid.
     * @return ERR_OK, return back success, others fail.
     */
    virtual int32_t KillApplicationByUid(const std::string &bundleName, const int uid);

    virtual int32_t KillApplicationSelf();

    /**
     * KillApplicationByUserId, kill the application by user ID.
     *
     * @param bundleName, bundle name in Application record.
     * @param userId, user ID.
     *
     * @return ERR_OK, return back success, others fail.
     */
    virtual int32_t KillApplicationByUserId(const std::string &bundleName, const int userId);

    /**
     * ClearUpApplicationData, clear the application data.
     *
     * @param bundleName, bundle name in Application record.
     * @param callerUid, app uid in Application record.
     * @param callerPid, app pid in Application record.
     *
     * @return
     */
    virtual void ClearUpApplicationData(const std::string &bundleName, const int32_t callerUid, const pid_t callerPid);

    /**
     * GetAllRunningProcesses, Obtains information about application processes that are running on the device.
     *
     * @param info, app name in Application record.
     *
     * @return ERR_OK ,return back success，others fail.
     */
    virtual int32_t GetAllRunningProcesses(std::vector<RunningProcessInfo> &info);

    /**
     * GetProcessRunningInfosByUserId, Obtains information about application processes that are running on the device.
     *
     * @param info, app name in Application record.
     * @param userId, userId.
     *
     * @return ERR_OK ,return back success，others fail.
     */
    virtual int32_t GetProcessRunningInfosByUserId(std::vector<RunningProcessInfo> &info, int32_t userId);

    /**
     * GetProcessRunningInformation, Obtains information about current application process
     * which is running on the device.
     * @param info, app name in Application record.
     *
     * @return ERR_OK ,return back success，others fail.
     */
    virtual int32_t GetProcessRunningInformation(RunningProcessInfo &info);

    /**
     * NotifyMemoryLevel, Notify applications background the current memory level.
     *
     * @param level, current memory level.
     *
     * @return ERR_OK ,return back success，others fail.
     */
    virtual int32_t NotifyMemoryLevel(int32_t level);

    std::shared_ptr<AppRunningRecord> CreateAppRunningRecord(
        const sptr<IRemoteObject> &token,
        const sptr<IRemoteObject> &preToken,
        const std::shared_ptr<ApplicationInfo> &appInfo,
        const std::shared_ptr<AbilityInfo> &abilityInfo,
        const std::string &processName,
        const BundleInfo &bundleInfo,
        const HapModuleInfo &hapModuleInfo,
        const std::shared_ptr<AAFwk::Want> &want);

    /**
     * OnStop, Application management service stopped.
     *
     * @return
     */
    void OnStop();

    /**
     * OpenAppSpawnConnection, Open connection with appspawn.
     *
     * @return ERR_OK ,return back success，others fail.
     */
    virtual ErrCode OpenAppSpawnConnection();

    /**
     * CloseAppSpawnConnection, Close connection with appspawn.
     *
     * @return
     */
    virtual void CloseAppSpawnConnection() const;

    /**
     * QueryAppSpawnConnectionState, Query the connection status with appspawn.
     *
     * @return Returns the connection status with appspawn.
     */
    virtual SpawnConnectionState QueryAppSpawnConnectionState() const;

    /**
     * SetAppSpawnClient, Setting the client to connect with appspawn.
     *
     * @param spawnClient, the client to connect with appspawn.
     *
     * @return
     */
    void SetAppSpawnClient(std::shared_ptr<AppSpawnClient> spawnClient);

    // Schedule launch application with specified |appRecord|

    /**
     * LaunchApplication, Notify application to launch application.
     *
     * @param appRecord, the application record.
     *
     * @return
     */
    void LaunchApplication(const std::shared_ptr<AppRunningRecord> &appRecord);

    /**
     * Notice of AddAbilityStageInfo()
     *
     * @param recordId, the application record.
     */
    virtual void AddAbilityStageDone(const int32_t recordId);

    /**
     * GetAppRunningRecordByPid, Get process record by application pid.
     *
     * @param pid, the application pid.
     *
     * @return process record.
     */
    std::shared_ptr<AppRunningRecord> GetAppRunningRecordByPid(const pid_t pid) const;

    /**
     * GetAppRunningRecordByAbilityToken, Get process record by ability token.
     *
     * @param abilityToken, the ability token.
     *
     * @return process record.
     */
    std::shared_ptr<AppRunningRecord> GetAppRunningRecordByAbilityToken(const sptr<IRemoteObject> &abilityToken) const;

    /**
     * GetAppRunningRecordByAppRecordId, Get process record by application id.
     *
     * @param recordId, the application id.
     *
     * @return process record.
     */
    std::shared_ptr<AppRunningRecord> GetAppRunningRecordByAppRecordId(const int32_t recordId) const;

    /**
     * OnAbilityStateChanged, Call ability state change.
     *
     * @param ability, the ability info.
     * @param state, the ability state.
     *
     * @return
     */
    void OnAbilityStateChanged(const std::shared_ptr<AbilityRunningRecord> &ability, const AbilityState state);

    /**
     * GetRecentAppList, Get a list of recent applications.
     *
     * @return a list of recent applications.
     */
    const std::list<const std::shared_ptr<AppTaskInfo>> &GetRecentAppList() const;

    /**
     * GetRecentAppList, Remove the corresponding latest application list data by applying the name.
     *
     * @param appName, the application name.
     * @param processName, the process name.
     *
     * @return
     */
    void RemoveAppFromRecentList(const std::string &appName, const std::string &processName);

    /**
     * GetRecentAppList, Clear recent application list.
     *
     * @return
     */
    void ClearRecentAppList();

    /**
     * OnRemoteDied, Equipment death notification.
     *
     * @param remote, Death client.
     * @param isRenderProcess is render process died.
     * @return
     */
    void OnRemoteDied(const wptr<IRemoteObject> &remote, bool isRenderProcess = false);

    /**
     * AddAppDeathRecipient, Add monitoring death application record.
     *
     * @param pid, the application pid.
     * @param appDeathRecipient, Application death recipient list.
     *
     * @return
     */

    virtual void AddAppDeathRecipient(const pid_t pid, const sptr<AppDeathRecipient> &appDeathRecipient) const;

    void HandleTimeOut(const InnerEvent::Pointer &event);

    void SetEventHandler(const std::shared_ptr<AMSEventHandler> &handler);

    void HandleAbilityAttachTimeOut(const sptr<IRemoteObject> &token);

    void PrepareTerminate(const sptr<IRemoteObject> &token);

    void OnAppStateChanged(const std::shared_ptr<AppRunningRecord> &appRecord, const ApplicationState state,
        bool needNotifyApp);

    void GetRunningProcessInfoByToken(const sptr<IRemoteObject> &token, AppExecFwk::RunningProcessInfo &info);

    void GetRunningProcessInfoByPid(const pid_t pid, OHOS::AppExecFwk::RunningProcessInfo &info) const;

	 /**
     * UpdateConfiguration, ANotify application update system environment changes.
     *
     * @param config, System environment change parameters.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t UpdateConfiguration(const Configuration &config);

    std::shared_ptr<AppExecFwk::Configuration> GetConfiguration();

    int32_t RegisterConfigurationObserver(const sptr<IConfigurationObserver>& observer);

    int32_t UnregisterConfigurationObserver(const sptr<IConfigurationObserver>& observer);

    /**
     * Start empty process
     */
    void LoadResidentProcess(const std::vector<BundleInfo> &infos);

    void StartResidentProcess(const std::vector<BundleInfo> &infos,  int restartCount, bool isEmptyKeepAliveApp);

    bool CheckRemoteClient();

    /**
     * Register application or process state observer.
     * @param observer, ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t RegisterApplicationStateObserver(const sptr<IApplicationStateObserver> &observer,
        const std::vector<std::string> &bundleNameList = {});

    /**
     * Unregister application or process state observer.
     * @param observer, ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t UnregisterApplicationStateObserver(const sptr<IApplicationStateObserver> &observer);

    /**
     * Get Foreground Applications.
     *
     * @return Foreground Applications.
     */
    int32_t GetForegroundApplications(std::vector<AppStateData> &list);

    /**
     * Start user test process.
     * @param want, want object.
     * @param observer, test observer remote object.
     * @param bundleInfo, bundle info.
     * @param userId the user id.
     * @return Returns ERR_OK on success, others on failure.
     */
    int StartUserTestProcess(const AAFwk::Want &want, const sptr<IRemoteObject> &observer,
        const AppExecFwk::BundleInfo &bundleInfo, int32_t userId);

    /**
     * @brief Finish user test.
     * @param msg user test message.
     * @param resultCode user test result Code.
     * @param bundleName user test bundleName.
     * @param pid the user test process id.
     *
     * @return Returns ERR_OK on success, others on failure.
     */
    int FinishUserTest(
        const std::string &msg, const int64_t &resultCode, const std::string &bundleName, const pid_t &pid);

    void StartSpecifiedAbility(const AAFwk::Want &want, const AppExecFwk::AbilityInfo &abilityInfo);

    void RegisterStartSpecifiedAbilityResponse(const sptr<IStartSpecifiedAbilityResponse> &response);

    void ScheduleAcceptWantDone(const int32_t recordId, const AAFwk::Want &want, const std::string &flag);

    /**
     *  Get the token of ability records by process ID.
     *
     * @param pid The process id.
     * @param tokens The token of ability records.
     * @return Returns true on success, others on failure.
     */
    int GetAbilityRecordsByProcessID(const int pid, std::vector<sptr<IRemoteObject>> &tokens);

    virtual int32_t PreStartNWebSpawnProcess(const pid_t hostPid);

    virtual int32_t StartRenderProcess(const pid_t hostPid, const std::string &renderParam,
        int32_t ipcFd, int32_t sharedFd, pid_t &renderPid);

    virtual void AttachRenderProcess(const pid_t pid, const sptr<IRenderScheduler> &scheduler);

    virtual int GetRenderProcessTerminationStatus(pid_t renderPid, int &status);

    int VerifyProcessPermission(const sptr<IRemoteObject> &token) const;

    int VerifyAccountPermission(const std::string &permissionName, const int userId) const;

    int VerifyRequestPermission() const;

    void ClearAppRunningData(const std::shared_ptr<AppRunningRecord> &appRecord, bool containsApp);

    void TerminateApplication(const std::shared_ptr<AppRunningRecord> &appRecord);

    int GetApplicationInfoByProcessID(const int pid, AppExecFwk::ApplicationInfo &application, bool &debug);
    /**
     * Notify application status.
     *
     * @param bundleName Indicates the name of the bundle.
     * @param eventData Indicates the event defined by CommonEventSupport
     *
     * @return
     */
    void NotifyAppStatus(const std::string &bundleName, const std::string &eventData);

    /**
     * KillProcessByPid, Kill process by PID.
     *
     * @param pid_t, the app record pid.
     *
     * @return ERR_OK, return back success，others fail.
     */
    int32_t KillProcessByPid(const pid_t pid) const;

    bool GetAppRunningStateByBundleName(const std::string &bundleName);

    int32_t NotifyLoadRepairPatch(const std::string &bundleName, const sptr<IQuickFixCallback> &callback);

    int32_t NotifyHotReloadPage(const std::string &bundleName, const sptr<IQuickFixCallback> &callback);

    int32_t NotifyUnLoadRepairPatch(const std::string &bundleName, const sptr<IQuickFixCallback> &callback);

    void HandleFocused(const sptr<OHOS::Rosen::FocusChangeInfo> &focusChangeInfo);
    void HandleUnfocused(const sptr<OHOS::Rosen::FocusChangeInfo> &focusChangeInfo);

    /**
     * Set the current userId, only used by abilityMgr.
     *
     * @param userId the user id.
     *
     * @return
     */
    void SetCurrentUserId(const int32_t userId);

#ifdef BGTASKMGR_CONTINUOUS_TASK_ENABLE
    int32_t SetContinuousTaskProcess(int32_t pid, bool isContinuousTask);
#endif

private:

    void StartEmptyResidentProcess(const BundleInfo &info, const std::string &processName, int restartCount,
        bool isEmptyKeepAliveApp);

    void RestartResidentProcess(std::shared_ptr<AppRunningRecord> appRecord);

    bool CheckLoadAbilityConditions(const sptr<IRemoteObject> &token,
        const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<ApplicationInfo> &appInfo);

    bool GetBundleInfo(const std::string &bundleName, BundleInfo &bundleInfo);

    void MakeProcessName(const std::shared_ptr<AbilityInfo> &abilityInfo,
        const std::shared_ptr<ApplicationInfo> &appInfo,
        const HapModuleInfo &hapModuleInfo, int32_t appIndex, std::string &processName);

    void MakeProcessName(
        const std::shared_ptr<ApplicationInfo> &appInfo, const HapModuleInfo &hapModuleInfo, std::string &processName);
    /**
     * StartAbility, load the ability that needed to be started(Start on the basis of the original process).
     *  Start on a new boot process
     * @param token, the unique identification to start the ability.
     * @param preToken, the unique identification to call the ability.
     * @param abilityInfo, the ability information.
     * @param appInfo, the app information.
     *
     * @return
     */
    void StartAbility(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
        const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<AppRunningRecord> &appRecord,
        const HapModuleInfo &hapModuleInfo, const std::shared_ptr<AAFwk::Want> &want);

    /**
     * StartProcess, load the ability that needed to be started(Start on a new boot process).
     *
     * @param appName, the app name.
     * @param processName, the process name.
     * @param appRecord, the app information.
     * @param uid, the process uid.
     * @param bundleName, the app bundleName.
     *
     * @return
     */
    void StartProcess(const std::string &appName, const std::string &processName, uint32_t startFlags,
                      const std::shared_ptr<AppRunningRecord> &appRecord, const int uid,
                      const std::string &bundleName, const int32_t bundleIndex);

    /**
     * PushAppFront, Adjust the latest application record to the top level.
     *
     * @param recordId, the app record id.
     *
     * @return
     */
    void PushAppFront(const int32_t recordId);

    /**
     * RemoveAppFromRecentListById, Remove the specified recent application record by application record id.
     *
     * @param recordId, the app record id.
     *
     * @return
     */
    void RemoveAppFromRecentListById(const int32_t recordId);

    /**
     * AddAppToRecentList, Add application to recent list.
     *
     * @param appName, the app name.
     * @param processName, the process name.
     * @param pid, the app pid.
     * @param recordId, the app record id.
     *
     * @return
     */
    void AddAppToRecentList(
        const std::string &appName, const std::string &processName, const pid_t pid, const int32_t recordId);

    /**
     * AddAppToRecentList, Get application task information through ID.
     *
     * @param recordId, the app record id.
     *
     * @return application task information.
     */
    const std::shared_ptr<AppTaskInfo> GetAppTaskInfoById(const int32_t recordId) const;

    /**
     * KillApplicationByUserId, kill the application by user ID.
     *
     * @param bundleName, bundle name in Application record.
     * @param userId, user ID.
     *
     * @return ERR_OK, return back success, others fail.
     */
    int32_t KillApplicationByUserIdLocked(const std::string &bundleName, const int userId);

    /**
     * WaitForRemoteProcessExit, Wait for the process to exit normally.
     *
     * @param pids, process number collection to exit.
     * @param startTime, execution process security exit start time.
     *
     * @return true, return back success，others fail.
     */
    bool WaitForRemoteProcessExit(std::list<pid_t> &pids, const int64_t startTime);

    /**
     * GetAllPids, Get the corresponding pid collection.
     *
     * @param pids, process number collection to exit.
     *
     * @return true, return back success，others fail.
     */
    bool GetAllPids(std::list<pid_t> &pids);

    /**
     * ProcessExist, Judge whether the process exists.
     *
     * @param pids, process number collection to exit.
     *
     * @return true, return back existed，others non-existent.
     */
    bool ProcessExist(pid_t &pid);

    /**
     * CheckAllProcessExist, Determine whether all processes exist .
     *
     * @param pids, process number collection to exit.
     *
     * @return true, Returns that a process exists and all other processes do not exist.
     */
    bool CheckAllProcessExist(std::list<pid_t> &pids);

    /**
     * SystemTimeMillisecond, Get system time.
     *
     * @return the system time.
     */
    int64_t SystemTimeMillisecond();

    // Test add the bundle manager instance.
    void SetBundleManager(sptr<IBundleMgr> bundleManager);

    void HandleTerminateApplicationTimeOut(const int64_t eventId);

    void HandleAddAbilityStageTimeOut(const int64_t eventId);

    void ClipStringContent(const std::regex &re, const std::string &source, std::string &afterCutStr);

    bool GetBundleAndHapInfo(const AbilityInfo &abilityInfo, const std::shared_ptr<ApplicationInfo> &appInfo,
        BundleInfo &bundleInfo, HapModuleInfo &hapModuleInfo, int32_t appIndex = 0);
    AppProcessData WrapAppProcessData(const std::shared_ptr<AppRunningRecord> &appRecord,
        const ApplicationState state);

    int UserTestAbnormalFinish(const sptr<IRemoteObject> &observer, const std::string &msg);
    int GetHapModuleInfoForTestRunner(const AAFwk::Want &want, const sptr<IRemoteObject> &observer,
        const BundleInfo &bundleInfo, HapModuleInfo &hapModuleInfo);
    int StartEmptyProcess(const AAFwk::Want &want, const sptr<IRemoteObject> &observer, const BundleInfo &info,
        const std::string &processName, const int userId);

    void HandleStartSpecifiedAbilityTimeOut(const int64_t eventId);

    void InitGlobalConfiguration();

    void GetRunningProcesses(const std::shared_ptr<AppRunningRecord> &appRecord, std::vector<RunningProcessInfo> &info);
    void GetRunningProcess(const std::shared_ptr<AppRunningRecord> &appRecord, RunningProcessInfo &info);

    int StartRenderProcessImpl(const std::shared_ptr<RenderRecord> &renderRecord,
        const std::shared_ptr<AppRunningRecord> appRecord, pid_t &renderPid);

    void OnRenderRemoteDied(const wptr<IRemoteObject> &remote);

    void AddWatchParameter();

    bool VerifyAPL() const;

    void InitFocusListener();
    void RegisterFocusListener();

    static void PointerDeviceEventCallback(const char *key, const char *value, void *context);

    int VerifyProcessPermission(const std::string &bundleName) const;

private:
    /**
     * ClearUpApplicationData, clear the application data.
     *
     * @param bundleName, bundle name in Application record.
     * @param uid, app uid in Application record.
     * @param pid, app pid in Application record.
     * @param userId, userId.
     *
     * @return
     */
    void ClearUpApplicationDataByUserId(const std::string &bundleName,
        int32_t callerUid, pid_t callerPid, const int userId);

    uint32_t BuildStartFlags(const AAFwk::Want &want, const AbilityInfo &abilityInfo);

    bool CheckGetRunningInfoPermission() const;

    int32_t KillApplicationByBundleName(const std::string &bundleName);

private:
    /**
     * Notify application status.
     *
     * @param bundleName Indicates the name of the bundle.
     * @param bundleName Indicates the name of the bundle.
     * @param eventData Indicates the event defined by CommonEventSupport
     *
     * @return
     */
    void NotifyAppStatusByCallerUid(const std::string &bundleName, const int32_t userId, const int32_t callerUid,
        const std::string &eventData);
    void KillApplicationByRecord(const std::shared_ptr<AppRunningRecord> &appRecord);
    void SendHiSysEvent(const int32_t innerEventId, const int64_t eventId);
    int FinishUserTestLocked(
        const std::string &msg, const int64_t &resultCode, const std::shared_ptr<AppRunningRecord> &appRecord);
    const std::string TASK_ON_CALLBACK_DIED = "OnCallbackDiedTask";
    std::vector<const sptr<IAppStateCallback>> appStateCallbacks_;
    std::shared_ptr<AppProcessManager> appProcessManager_;
    std::shared_ptr<RemoteClientManager> remoteClientManager_;
    std::shared_ptr<AppRunningManager> appRunningManager_;
    std::shared_ptr<AMSEventHandler> eventHandler_;
    std::shared_ptr<Configuration> configuration_;
    std::mutex userTestLock_;
    sptr<IStartSpecifiedAbilityResponse> startSpecifiedAbilityResponse_;
    std::recursive_mutex configurationObserverLock_;
    std::vector<sptr<IConfigurationObserver>> configurationObservers_;
    sptr<WindowFocusChangedListener> focusListener_;
    std::vector<std::shared_ptr<AppRunningRecord>> restartResedentTaskList_;
    int32_t currentUserId_ = 0;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_APP_MGR_SERVICE_INNER_H
