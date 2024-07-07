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

#ifndef OHOS_ABILITY_RUNTIME_APP_SCHEDULER_H
#define OHOS_ABILITY_RUNTIME_APP_SCHEDULER_H

#include <memory>
#include <unordered_set>

#include "ability_info.h"
#include "appmgr/app_mgr_client.h"
#include "appmgr/app_state_callback_host.h"
#include "appmgr/start_specified_ability_response_stub.h"
#include "application_info.h"
#include "bundle_info.h"
#include "iremote_object.h"
#include "refbase.h"
#include "singleton.h"
#include "system_memory_attr.h"
#include "running_process_info.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
class Configuration;
}
namespace AAFwk {
/**
 * @enum AppAbilityState
 * AppAbilityState defines the life cycle state of app ability.
 */
enum class AppAbilityState {
    ABILITY_STATE_UNDEFINED = 0,
    ABILITY_STATE_FOREGROUND,
    ABILITY_STATE_BACKGROUND,
    ABILITY_STATE_END,
};

enum class AppState {
    BEGIN = 0,
    READY,
    FOREGROUND,
    FOCUS,
    BACKGROUND,
    TERMINATED,
    END,
    SUSPENDED,
};

struct AppData {
    std::string appName;
    int32_t uid;
};

struct AppInfo {
    std::vector<AppData> appData;
    std::string processName;
    AppState state;
};
/**
 * @class AppStateCallback
 * AppStateCallback.
 */
class AppStateCallback {
public:
    AppStateCallback()
    {}
    virtual ~AppStateCallback()
    {}

    virtual void OnAbilityRequestDone(const sptr<IRemoteObject> &token, const int32_t state) = 0;

    virtual void OnAppStateChanged(const AppInfo &info) = 0;
};

class StartSpecifiedAbilityResponse : public AppExecFwk::StartSpecifiedAbilityResponseStub {
public:
    StartSpecifiedAbilityResponse() = default;
    virtual ~StartSpecifiedAbilityResponse() = default;

    virtual void OnAcceptWantResponse(const AAFwk::Want &want, const std::string &flag) override;
    virtual void OnTimeoutResponse(const AAFwk::Want &want) override;
};

/**
 * @class AppScheduler
 * AppScheduler , access app manager service.
 */
class AppScheduler : virtual RefBase, public AppExecFwk::AppStateCallbackHost {
    DECLARE_DELAYED_SINGLETON(AppScheduler)
public:
    /**
     * init app scheduler.
     * @param callback, app state call back.
     * @return true on success ,false on failure.
     */
    bool Init(const std::weak_ptr<AppStateCallback> &callback);

    /**
     * load ability with token, ability info and application info.
     *
     * @param token, the token of ability.
     * @param preToken, the token of ability's caller.
     * @param abilityInfo, ability info.
     * @param applicationInfo, application info.
     * @param want ability want
     * @return true on success ,false on failure.
     */
    int LoadAbility(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
        const AppExecFwk::AbilityInfo &abilityInfo, const AppExecFwk::ApplicationInfo &applicationInfo,
        const Want &want);

    /**
     * terminate ability with token.
     *
     * @param token, the token of ability.
     * @param clearMissionFlag, indicates whether terminate the ability when clearMission.
     * @return true on success ,false on failure.
     */
    int TerminateAbility(const sptr<IRemoteObject> &token, bool clearMissionFlag);

    /**
     * move ability to foreground.
     *
     * @param token, the token of ability.
     */
    void MoveToForeground(const sptr<IRemoteObject> &token);

    /**
     * move ability to background.
     *
     * @param token, the token of ability.
     */
    void MoveToBackground(const sptr<IRemoteObject> &token);

    /**
     * Update ability state.
     *
     * @param token, the token of ability.
     * @param state, ability state.
     */
    void UpdateAbilityState(const sptr<IRemoteObject> &token, const AppExecFwk::AbilityState state);

    /**
     * UpdateExtensionState, call UpdateExtensionState() through the proxy object, update the extension status.
     *
     * @param token, the unique identification to update the extension.
     * @param state, extension status that needs to be updated.
     * @return
     */
    void UpdateExtensionState(const sptr<IRemoteObject> &token, const AppExecFwk::ExtensionState state);

    /**
     * AbilityBehaviorAnalysis, ability behavior analysis assistant process optimization.
     *
     * @param token, the unique identification to start the ability.
     * @param preToken, the unique identification to call the ability.
     * @param visibility, the visibility information about windows info.
     * @param perceptibility, the Perceptibility information about windows info.
     * @param connectionState, the service ability connection state.
     * @return Returns RESULT_OK on success, others on failure.
     */
    void AbilityBehaviorAnalysis(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
        const int32_t visibility, const int32_t perceptibility, const int32_t connectionState);

    /**
     * KillProcessByAbilityToken, call KillProcessByAbilityToken() through proxy object,
     * kill the process by ability token.
     *
     * @param token, the unique identification to the ability.
     */
    void KillProcessByAbilityToken(const sptr<IRemoteObject> &token);

    /**
     * KillProcessesByUserId, call KillProcessesByUserId() through proxy object,
     * kill the process by user id.
     *
     * @param userId, the user id.
     */
    void KillProcessesByUserId(int32_t userId);

    /**
     * convert ability state to app ability state.
     *
     * @param state, the state of ability.
     */
    AppAbilityState ConvertToAppAbilityState(const int32_t state);

    /**
     * get ability state.
     *
     * @return state, the state of app ability.
     */
    AppAbilityState GetAbilityState() const;

    /**
     * kill the application
     *
     * @param bundleName.
     */
    int KillApplication(const std::string &bundleName);

    /**
     * kill the application by uid
     *
     * @param bundleName name of bundle.
     * @param uid uid of bundle.
     * @return 0 if success.
     */
    int KillApplicationByUid(const std::string &bundleName, int32_t uid);

    /**
     * update the application info after new module installed.
     *
     * @param bundleName, bundle name in Application record.
     * @param  uid, uid.
     * @return 0 if success.
     */
    int UpdateApplicationInfoInstalled(const std::string &bundleName, const int32_t uid);

    /**
     * clear the application data
     *
     * @param bundleName.
     */
    int ClearUpApplicationData(const std::string &bundleName);

    void AttachTimeOut(const sptr<IRemoteObject> &token);

    void PrepareTerminate(const sptr<IRemoteObject> &token);

    void GetRunningProcessInfoByToken(const sptr<IRemoteObject> &token, AppExecFwk::RunningProcessInfo &info);

    void GetRunningProcessInfoByPid(const pid_t pid, OHOS::AppExecFwk::RunningProcessInfo &info) const;

    /**
     * Start a resident process
     */
    void StartupResidentProcess(const std::vector<AppExecFwk::BundleInfo> &bundleInfos);

    void StartSpecifiedAbility(const AAFwk::Want &want, const AppExecFwk::AbilityInfo &abilityInfo);
    int GetProcessRunningInfos(std::vector<AppExecFwk::RunningProcessInfo> &info);

    /**
     * Start a user test
     */
    int StartUserTest(const Want &want, const sptr<IRemoteObject> &observer, const AppExecFwk::BundleInfo &bundleInfo,
        int32_t userId);

    /**
     * @brief Finish user test.
     * @param msg user test message.
     * @param resultCode user test result Code.
     * @param bundleName user test bundleName.
     *
     * @return Returns ERR_OK on success, others on failure.
     */
    int FinishUserTest(const std::string &msg, const int64_t &resultCode, const std::string &bundleName);

    int GetProcessRunningInfosByUserId(std::vector<AppExecFwk::RunningProcessInfo> &info, int32_t userId);
    std::string ConvertAppState(const AppState &state);

    /**
     *  ANotify application update system environment changes.
     *
     * @param config System environment change parameters.
     * @return Returns ERR_OK on success, others on failure.
     */
    int UpdateConfiguration(const AppExecFwk::Configuration &config);

    int GetConfiguration(AppExecFwk::Configuration &config);

    /**
     *  Get the token of ability records by process ID.
     *
     * @param pid The process id.
     * @param tokens The token of ability records.
     * @return Returns ERR_OK on success, others on failure.
     */
    int GetAbilityRecordsByProcessID(const int pid, std::vector<sptr<IRemoteObject>> &tokens);

    /**
     *  Get the application info by process ID.
     *
     * @param pid The process id.
     * @param application The application info.
     * @param debug The app is or not debug.
     * @return Returns ERR_OK on success, others on failure.
     */
    int GetApplicationInfoByProcessID(const int pid, AppExecFwk::ApplicationInfo &application, bool &debug);

    /**
     * Set the current userId of appMgr, only used by abilityMgr.
     *
     * @param userId the user id.
     *
     * @return
     */
    void SetCurrentUserId(int32_t userId);

    #ifdef ABILITY_COMMAND_FOR_TEST
    /**
     * Block app service.
     *
     * @return Returns ERR_OK on success, others on failure.
     */
    int BlockAppService();
    #endif

protected:
    /**
     * OnAbilityRequestDone, app manager service call this interface after ability request done.
     *
     * @param token,ability's token.
     * @param state,the state of ability lift cycle.
     */
    virtual void OnAbilityRequestDone(const sptr<IRemoteObject> &token, const AppExecFwk::AbilityState state) override;

    /**
     * Application state changed callback.
     *
     * @param appProcessData Process data
     */
    virtual void OnAppStateChanged(const AppExecFwk::AppProcessData &appData) override;

private:
    std::recursive_mutex lock_;
    bool isInit_  {false};
    std::weak_ptr<AppStateCallback> callback_;
    std::unique_ptr<AppExecFwk::AppMgrClient> appMgrClient_;
    AppAbilityState appAbilityState_ = AppAbilityState::ABILITY_STATE_UNDEFINED;
    sptr<StartSpecifiedAbilityResponse> startSpecifiedAbilityResponse_;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_APP_SCHEDULER_H
