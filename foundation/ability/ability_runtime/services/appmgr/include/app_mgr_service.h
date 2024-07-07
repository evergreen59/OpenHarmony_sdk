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

#ifndef OHOS_ABILITY_RUNTIME_APP_MGR_SERVICE_H
#define OHOS_ABILITY_RUNTIME_APP_MGR_SERVICE_H

#include <list>
#include <string>
#include <vector>

#include "if_system_ability_manager.h"
#include "nocopyable.h"
#include "system_ability.h"

#include "ability_info.h"
#include "ability_running_record.h"
#include "appexecfwk_errors.h"
#include "application_info.h"
#include "app_mgr_constants.h"
#include "app_mgr_stub.h"
#include "app_mgr_service_event_handler.h"
#include "app_mgr_service_inner.h"
#include "app_record_id.h"
#include "app_running_record.h"
#include "app_scheduler_proxy.h"
#include "ams_mgr_scheduler.h"
#include "ams_mgr_scheduler.h"

namespace OHOS {
namespace AppExecFwk {
enum class ServiceRunningState { STATE_NOT_START, STATE_RUNNING };

struct AppMgrServiceState {
    ServiceRunningState serviceRunningState = ServiceRunningState::STATE_NOT_START;
    SpawnConnectionState connectionState = SpawnConnectionState::STATE_NOT_CONNECT;
};

class AMSEventHandler;

class AppMgrService : public SystemAbility, public AppMgrStub {
public:
    DECLEAR_SYSTEM_ABILITY(AppMgrService);

    AppMgrService();
    AppMgrService(const int32_t serviceId, bool runOnCreate = false);
    virtual ~AppMgrService() override;

    // the function about application
    // attach the application to ams, then ams can control it.
    /**
     * AttachApplication, call AttachApplication() through proxy object,
     * get all the information needed to start the Application (data related to the Application ).
     *
     * @param app, information needed to start the Application.
     * @return
     */
    virtual void AttachApplication(const sptr<IRemoteObject> &app) override;

    // notify the ams update the state of an app, when it entered foreground.

    /**
     * ApplicationForegrounded, call ApplicationForegrounded() through proxy object,
     * set the application to Foreground State.
     *
     * @param recordId, a unique record that identifies this Application from others.
     * @return
     */
    virtual void ApplicationForegrounded(const int32_t recordId) override;

    /**
     * ApplicationBackgrounded, call ApplicationBackgrounded() through proxy object,
     * set the application to Backgrounded State.
     *
     * @param recordId, a unique record that identifies this Application from others.
     * @return
     */
    virtual void ApplicationBackgrounded(const int32_t recordId) override;

    /**
     * ApplicationTerminated, call ApplicationTerminated() through proxy object,
     * terminate the application.
     *
     * @param recordId, a unique record that identifies this Application from others.
     * @return
     */
    virtual void ApplicationTerminated(const int32_t recordId) override;

    /**
     * AbilityCleaned,call through AbilityCleaned() proxy project, clean Ability record.
     *
     * @param token, a unique record that identifies AbilityCleaned from others.
     * @return
     */
    virtual void AbilityCleaned(const sptr<IRemoteObject> &token) override;

    /**
     * ClearUpApplicationData, call ClearUpApplicationData() through proxy project,
     * clear the application data.
     *
     * @param bundleName, bundle name in Application record.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int32_t ClearUpApplicationData(const std::string &bundleName) override;

    /**
     * GetAllRunningProcesses, call GetAllRunningProcesses() through proxy project.
     * Obtains information about application processes that are running on the device.
     *
     * @param info, app name in Application record.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int32_t GetAllRunningProcesses(std::vector<RunningProcessInfo> &info) override;

    /**
     * GetProcessRunningInfosByUserId, call GetProcessRunningInfosByUserId() through proxy project.
     * Obtains information about application processes that are running on the device.
     *
     * @param info, app name in Application record.
     * @param userId, userId.
     *
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int32_t GetProcessRunningInfosByUserId(std::vector<RunningProcessInfo> &info, int32_t userId) override;

    /**
     * GetProcessRunningInformation, call GetProcessRunningInformation() through proxy project.
     * Obtains information about current application process which is running on the device.
     *
     * @param info, app name in Application record.
     *
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int32_t GetProcessRunningInformation(RunningProcessInfo &info) override;

    /**
     * NotifyMemoryLevel, call NotifyMemoryLevel() through proxy project.
     * Notify applications background the current memory level.
     *
     * @param level, current memory level.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int32_t NotifyMemoryLevel(int32_t level) override;

    // the function about system
    /**
     * CheckPermission, call CheckPermission() through proxy object, check the permission.
     *
     * @param recordId, a unique record that identifies this Application from others.
     * @param permission, check the permissions.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int32_t CheckPermission(const int32_t recordId, const std::string &permission) override;

    // the function about service running info
    /**
     * QueryServiceState, Query application service status.
     *
     * @return the application service status.
     */
    AppMgrServiceState QueryServiceState();

    /**
     * GetAmsMgr, call GetAmsMgr() through proxy object, get AMS interface instance.
     *
     * @return sptr<IAmsMgr>, return to AMS interface instance.
     */
    virtual sptr<IAmsMgr> GetAmsMgr() override;

    /**
     * Notify that the ability stage has been updated
     * @param recordId, the app record.
     */
    virtual void AddAbilityStageDone(const int32_t recordId) override;

    /**
     * Start all resident process
     */
    virtual void StartupResidentProcess(const std::vector<AppExecFwk::BundleInfo> &bundleInfos) override;

    /**
     * Start user test process.
     * @param want, want object.
     * @param observer, test observer remote object.
     * @param bundleInfo, bundle info.
     * @param userId the user id.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int StartUserTestProcess(const AAFwk::Want &want, const sptr<IRemoteObject> &observer,
        const AppExecFwk::BundleInfo &bundleInfo, int32_t userId) override;

    /**
     * @brief Finish user test.
     * @param msg user test message.
     * @param resultCode user test result Code.
     * @param bundleName user test bundleName.
     *
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int FinishUserTest(
        const std::string &msg, const int64_t &resultCode, const std::string &bundleName) override;

    /**
     * @brief Application hidumper.
     * @param fd Indicates the fd.
     * @param args Indicates the params.
     * @return Returns the dump result.
     */
    int Dump(int fd, const std::vector<std::u16string>& args) override;

    virtual void ScheduleAcceptWantDone(
        const int32_t recordId, const AAFwk::Want &want, const std::string &flag) override;

    virtual int GetAbilityRecordsByProcessID(const int pid, std::vector<sptr<IRemoteObject>> &tokens) override;

    virtual int PreStartNWebSpawnProcess() override;

    virtual int StartRenderProcess(const std::string &renderParam, int32_t ipcFd,
        int32_t sharedFd, pid_t &renderPid) override;

    virtual void AttachRenderProcess(const sptr<IRemoteObject> &shceduler) override;

    virtual int GetRenderProcessTerminationStatus(pid_t renderPid, int &status) override;

    virtual int32_t GetConfiguration(Configuration& config) override;

    virtual int32_t UpdateConfiguration(const Configuration &config) override;

    virtual int32_t RegisterConfigurationObserver(const sptr<IConfigurationObserver> &observer) override;

    virtual int32_t UnregisterConfigurationObserver(const sptr<IConfigurationObserver> &observer) override;

    #ifdef ABILITY_COMMAND_FOR_TEST
    /**
     * Block app service.
     *
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int BlockAppService() override;
    #endif

    bool GetAppRunningStateByBundleName(const std::string &bundleName) override;

    int32_t NotifyLoadRepairPatch(const std::string &bundleName, const sptr<IQuickFixCallback> &callback) override;

    int32_t NotifyHotReloadPage(const std::string &bundleName, const sptr<IQuickFixCallback> &callback) override;

    int32_t NotifyUnLoadRepairPatch(const std::string &bundleName, const sptr<IQuickFixCallback> &callback) override;

#ifdef BGTASKMGR_CONTINUOUS_TASK_ENABLE
    int32_t SetContinuousTaskProcess(int32_t pid, bool isContinuousTask) override;
#endif

private:
    /**
     * Init, Initialize application services.
     *
     * @return ERR_OK, return back success, others fail.
     */
    ErrCode Init();

    // the function that overrode from SystemAbility
    /**
     * OnStart, Start application service.
     *
     * @return
     */
    virtual void OnStart() override;

    /**
     * OnStop, Stop application service.
     *
     * @return
     */
    virtual void OnStop() override;

    /**
     * @brief Judge whether the application service is ready.
     *
     * @return Returns true means service is ready, otherwise service is not ready.
     */
    bool IsReady() const;

    /**
     * AddAppDeathRecipient, Add monitoring death application record.
     *
     * @param pid, the application pid.
     * @param appDeathRecipient, Application death recipient list.
     *
     * @return
     */
    void AddAppDeathRecipient(const pid_t pid) const;

    /**
     * SetInnerService, Setting application service Inner instance.
     *
     * @return
     */
    void SetInnerService(const std::shared_ptr<AppMgrServiceInner> &innerService);

    /**
     * Register application or process state observer.
     * @param observer, ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int32_t RegisterApplicationStateObserver(const sptr<IApplicationStateObserver> &observer,
        const std::vector<std::string> &bundleNameList = {}) override;

    /**
     * Unregister application or process state observer.
     * @param observer, ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int32_t UnregisterApplicationStateObserver(const sptr<IApplicationStateObserver> &observer) override;

    /**
     * Get foreground applications.
     * @param list, foreground apps.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int32_t GetForegroundApplications(std::vector<AppStateData> &list) override;

    void Dump(const std::vector<std::u16string>& args, std::string& result) const;
    void ShowHelp(std::string& result) const;

    bool JudgeSelfCalledByRecordId(int32_t recordId);

private:
    std::shared_ptr<AppMgrServiceInner> appMgrServiceInner_;
    AppMgrServiceState appMgrServiceState_;
    std::shared_ptr<EventRunner> runner_;
    std::shared_ptr<AMSEventHandler> handler_;
    sptr<ISystemAbilityManager> systemAbilityMgr_;
    sptr<IAmsMgr> amsMgrScheduler_;

    DISALLOW_COPY_AND_MOVE(AppMgrService);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_APP_MGR_SERVICE_H
