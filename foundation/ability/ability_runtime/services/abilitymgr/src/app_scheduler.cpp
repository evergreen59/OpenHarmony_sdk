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

#include "app_scheduler.h"

#include "ability_manager_errors.h"
#include "ability_manager_service.h"
#include "ability_record.h"
#include "ability_util.h"
#include "appmgr/app_mgr_constants.h"
#include "hitrace_meter.h"
#include "hilog_wrapper.h"
#include "in_process_call_wrapper.h"

namespace OHOS {
namespace AAFwk {
const std::map<AppState, std::string> appStateToStrMap_ = {
    std::map<AppState, std::string>::value_type(AppState::BEGIN, "BEGIN"),
    std::map<AppState, std::string>::value_type(AppState::READY, "READY"),
    std::map<AppState, std::string>::value_type(AppState::FOREGROUND, "FOREGROUND"),
    std::map<AppState, std::string>::value_type(AppState::BACKGROUND, "BACKGROUND"),
    std::map<AppState, std::string>::value_type(AppState::SUSPENDED, "SUSPENDED"),
    std::map<AppState, std::string>::value_type(AppState::TERMINATED, "TERMINATED"),
    std::map<AppState, std::string>::value_type(AppState::END, "END"),
    std::map<AppState, std::string>::value_type(AppState::FOCUS, "FOCUS"),
};
AppScheduler::AppScheduler() : appMgrClient_(std::make_unique<AppExecFwk::AppMgrClient>())
{}

AppScheduler::~AppScheduler()
{}

bool AppScheduler::Init(const std::weak_ptr<AppStateCallback> &callback)
{
    CHECK_POINTER_RETURN_BOOL(callback.lock());
    CHECK_POINTER_RETURN_BOOL(appMgrClient_);

    std::lock_guard<std::recursive_mutex> guard(lock_);
    if (isInit_) {
        return true;
    }

    callback_ = callback;
    /* because the errcode type of AppMgr Client API will be changed to int,
     * so must to covert the return result  */
    int result = static_cast<int>(appMgrClient_->ConnectAppMgrService());
    if (result != ERR_OK) {
        HILOG_ERROR("failed to ConnectAppMgrService");
        return false;
    }
    this->IncStrongRef(this);
    result = static_cast<int>(appMgrClient_->RegisterAppStateCallback(sptr<AppScheduler>(this)));
    if (result != ERR_OK) {
        HILOG_ERROR("failed to RegisterAppStateCallback");
        return false;
    }

    startSpecifiedAbilityResponse_ = new (std::nothrow) StartSpecifiedAbilityResponse();
    if (startSpecifiedAbilityResponse_ == nullptr) {
        HILOG_ERROR("startSpecifiedAbilityResponse_ is nullptr.");
        return false;
    }
    appMgrClient_->RegisterStartSpecifiedAbilityResponse(startSpecifiedAbilityResponse_);

    HILOG_INFO("success to ConnectAppMgrService");
    isInit_ = true;
    return true;
}

int AppScheduler::LoadAbility(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
    const AppExecFwk::AbilityInfo &abilityInfo, const AppExecFwk::ApplicationInfo &applicationInfo, const Want &want)
{
    HITRACE_METER_NAME(HITRACE_TAG_ABILITY_MANAGER, __PRETTY_FUNCTION__);
    HILOG_DEBUG("Send load ability to AppMgrService.");
    CHECK_POINTER_AND_RETURN(appMgrClient_, INNER_ERR);
    /* because the errcode type of AppMgr Client API will be changed to int,
     * so must to covert the return result  */
    int ret = static_cast<int>(IN_PROCESS_CALL(
        appMgrClient_->LoadAbility(token, preToken, abilityInfo, applicationInfo, want)));
    if (ret != ERR_OK) {
        HILOG_ERROR("AppScheduler fail to LoadAbility. ret %d", ret);
        return INNER_ERR;
    }
    return ERR_OK;
}

int AppScheduler::TerminateAbility(const sptr<IRemoteObject> &token, bool clearMissionFlag)
{
    HITRACE_METER_NAME(HITRACE_TAG_ABILITY_MANAGER, __PRETTY_FUNCTION__);
    HILOG_DEBUG("Terminate ability.");
    CHECK_POINTER_AND_RETURN(appMgrClient_, INNER_ERR);
    /* because the errcode type of AppMgr Client API will be changed to int,
     * so must to covert the return result  */
    int ret = static_cast<int>(IN_PROCESS_CALL(appMgrClient_->TerminateAbility(token, clearMissionFlag)));
    if (ret != ERR_OK) {
        HILOG_ERROR("AppScheduler fail to TerminateAbility. ret %d", ret);
        return INNER_ERR;
    }
    return ERR_OK;
}

int AppScheduler::UpdateApplicationInfoInstalled(const std::string &bundleName, const int32_t uid)
{
    CHECK_POINTER_AND_RETURN(appMgrClient_, INNER_ERR);
    HILOG_DEBUG("Start to update the application info after new module installed.");
    int ret = (int)appMgrClient_->UpdateApplicationInfoInstalled(bundleName, uid);
    if (ret != ERR_OK) {
        HILOG_ERROR("Fail to UpdateApplicationInfoInstalled.");
        return INNER_ERR;
    }

    return ERR_OK;
}

void AppScheduler::MoveToForeground(const sptr<IRemoteObject> &token)
{
    HITRACE_METER_NAME(HITRACE_TAG_ABILITY_MANAGER, __PRETTY_FUNCTION__);
    HILOG_DEBUG("Start to move the ability to foreground.");
    CHECK_POINTER(appMgrClient_);
    IN_PROCESS_CALL_WITHOUT_RET(
        appMgrClient_->UpdateAbilityState(token, AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND));
}

void AppScheduler::MoveToBackground(const sptr<IRemoteObject> &token)
{
    HITRACE_METER_NAME(HITRACE_TAG_ABILITY_MANAGER, __PRETTY_FUNCTION__);
    HILOG_DEBUG("Move the app to background.");
    CHECK_POINTER(appMgrClient_);
    IN_PROCESS_CALL_WITHOUT_RET(
        appMgrClient_->UpdateAbilityState(token, AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND));
}

void AppScheduler::UpdateAbilityState(const sptr<IRemoteObject> &token, const AppExecFwk::AbilityState state)
{
    HILOG_DEBUG("UpdateAbilityState.");
    CHECK_POINTER(appMgrClient_);
    IN_PROCESS_CALL_WITHOUT_RET(appMgrClient_->UpdateAbilityState(token, state));
}

void AppScheduler::UpdateExtensionState(const sptr<IRemoteObject> &token, const AppExecFwk::ExtensionState state)
{
    HILOG_DEBUG("UpdateExtensionState.");
    CHECK_POINTER(appMgrClient_);
    IN_PROCESS_CALL_WITHOUT_RET(appMgrClient_->UpdateExtensionState(token, state));
}

void AppScheduler::AbilityBehaviorAnalysis(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
    const int32_t visibility, const int32_t perceptibility, const int32_t connectionState)
{
    HILOG_DEBUG("Ability behavior analysis.");
    CHECK_POINTER(appMgrClient_);
    IN_PROCESS_CALL_WITHOUT_RET(
        appMgrClient_->AbilityBehaviorAnalysis(token, preToken, visibility, perceptibility, connectionState));
}

void AppScheduler::KillProcessByAbilityToken(const sptr<IRemoteObject> &token)
{
    HILOG_DEBUG("Kill process by ability token.");
    CHECK_POINTER(appMgrClient_);
    appMgrClient_->KillProcessByAbilityToken(token);
}

void AppScheduler::KillProcessesByUserId(int32_t userId)
{
    HILOG_DEBUG("Kill process by user id.");
    CHECK_POINTER(appMgrClient_);
    appMgrClient_->KillProcessesByUserId(userId);
}

AppAbilityState AppScheduler::ConvertToAppAbilityState(const int32_t state)
{
    AppExecFwk::AbilityState abilityState = static_cast<AppExecFwk::AbilityState>(state);
    switch (abilityState) {
        case AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND: {
            return AppAbilityState::ABILITY_STATE_FOREGROUND;
        }
        case AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND: {
            return AppAbilityState::ABILITY_STATE_BACKGROUND;
        }
        default:
            return AppAbilityState::ABILITY_STATE_UNDEFINED;
    }
}

AppAbilityState AppScheduler::GetAbilityState() const
{
    return appAbilityState_;
}

void AppScheduler::OnAbilityRequestDone(const sptr<IRemoteObject> &token, const AppExecFwk::AbilityState state)
{
    HITRACE_METER_NAME(HITRACE_TAG_ABILITY_MANAGER, __PRETTY_FUNCTION__);
    HILOG_INFO("On ability request done, state:%{public}d", static_cast<int32_t>(state));
    auto callback = callback_.lock();
    CHECK_POINTER(callback);
    appAbilityState_ = ConvertToAppAbilityState(static_cast<int32_t>(state));
    callback->OnAbilityRequestDone(token, static_cast<int32_t>(state));
}

int AppScheduler::KillApplication(const std::string &bundleName)
{
    HILOG_INFO("[%{public}s(%{public}s)] enter", __FILE__, __FUNCTION__);
    CHECK_POINTER_AND_RETURN(appMgrClient_, INNER_ERR);
    int ret = (int)appMgrClient_->KillApplication(bundleName);
    if (ret != ERR_OK) {
        HILOG_ERROR("Fail to kill application.");
        return INNER_ERR;
    }

    return ERR_OK;
}

int AppScheduler::KillApplicationByUid(const std::string &bundleName, int32_t uid)
{
    HILOG_INFO("[%{public}s(%{public}s)] enter", __FILE__, __FUNCTION__);
    CHECK_POINTER_AND_RETURN(appMgrClient_, INNER_ERR);
    int ret = (int)appMgrClient_->KillApplicationByUid(bundleName, uid);
    if (ret != ERR_OK) {
        HILOG_ERROR("Fail to kill application by uid.");
        return INNER_ERR;
    }

    return ERR_OK;
}

int AppScheduler::ClearUpApplicationData(const std::string &bundleName)
{
    CHECK_POINTER_AND_RETURN(appMgrClient_, INNER_ERR);
    int ret = (int)appMgrClient_->ClearUpApplicationData(bundleName);
    if (ret != ERR_OK) {
        HILOG_ERROR("Fail to clear application data.");
        return INNER_ERR;
    }
    return ERR_OK;
}

void AppScheduler::AttachTimeOut(const sptr<IRemoteObject> &token)
{
    CHECK_POINTER(appMgrClient_);
    IN_PROCESS_CALL_WITHOUT_RET(appMgrClient_->AbilityAttachTimeOut(token));
}

void AppScheduler::PrepareTerminate(const sptr<IRemoteObject> &token)
{
    CHECK_POINTER(appMgrClient_);
    IN_PROCESS_CALL_WITHOUT_RET(appMgrClient_->PrepareTerminate(token));
}

void AppScheduler::OnAppStateChanged(const AppExecFwk::AppProcessData &appData)
{
    HITRACE_METER_NAME(HITRACE_TAG_ABILITY_MANAGER, __PRETTY_FUNCTION__);
    auto callback = callback_.lock();
    CHECK_POINTER(callback);
    AppInfo info;
    for (const auto &list : appData.appDatas) {
        AppData data;
        data.appName = list.appName;
        data.uid = list.uid;
        info.appData.push_back(data);
    }
    info.processName = appData.processName;
    info.state = static_cast<AppState>(appData.appState);
    callback->OnAppStateChanged(info);
}

void AppScheduler::GetRunningProcessInfoByToken(const sptr<IRemoteObject> &token, AppExecFwk::RunningProcessInfo &info)
{
    CHECK_POINTER(appMgrClient_);
    IN_PROCESS_CALL_WITHOUT_RET(appMgrClient_->GetRunningProcessInfoByToken(token, info));
}

void AppScheduler::GetRunningProcessInfoByPid(const pid_t pid, OHOS::AppExecFwk::RunningProcessInfo &info) const
{
    CHECK_POINTER(appMgrClient_);
    IN_PROCESS_CALL_WITHOUT_RET(appMgrClient_->GetRunningProcessInfoByPid(pid, info));
}

void AppScheduler::StartupResidentProcess(const std::vector<AppExecFwk::BundleInfo> &bundleInfos)
{
    CHECK_POINTER(appMgrClient_);
    appMgrClient_->StartupResidentProcess(bundleInfos);
}

void AppScheduler::StartSpecifiedAbility(const AAFwk::Want &want, const AppExecFwk::AbilityInfo &abilityInfo)
{
    CHECK_POINTER(appMgrClient_);
    IN_PROCESS_CALL_WITHOUT_RET(appMgrClient_->StartSpecifiedAbility(want, abilityInfo));
}

void StartSpecifiedAbilityResponse::OnAcceptWantResponse(
    const AAFwk::Want &want, const std::string &flag)
{
    DelayedSingleton<AbilityManagerService>::GetInstance()->OnAcceptWantResponse(want, flag);
}

void StartSpecifiedAbilityResponse::OnTimeoutResponse(const AAFwk::Want &want)
{
    DelayedSingleton<AbilityManagerService>::GetInstance()->OnStartSpecifiedAbilityTimeoutResponse(want);
}
int AppScheduler::GetProcessRunningInfos(std::vector<AppExecFwk::RunningProcessInfo> &info)
{
    CHECK_POINTER_AND_RETURN(appMgrClient_, INNER_ERR);
    return static_cast<int>(appMgrClient_->GetAllRunningProcesses(info));
}

int AppScheduler::GetProcessRunningInfosByUserId(std::vector<AppExecFwk::RunningProcessInfo> &info, int32_t userId)
{
    CHECK_POINTER_AND_RETURN(appMgrClient_, INNER_ERR);
    return static_cast<int>(appMgrClient_->GetProcessRunningInfosByUserId(info, userId));
}

std::string AppScheduler::ConvertAppState(const AppState &state)
{
    auto it = appStateToStrMap_.find(state);
    if (it != appStateToStrMap_.end()) {
        return it->second;
    }
    return "INVALIDSTATE";
}

int AppScheduler::StartUserTest(
    const Want &want, const sptr<IRemoteObject> &observer, const AppExecFwk::BundleInfo &bundleInfo, int32_t userId)
{
    CHECK_POINTER_AND_RETURN(appMgrClient_, INNER_ERR);
    int ret = appMgrClient_->StartUserTestProcess(want, observer, bundleInfo, userId);
    if (ret != ERR_OK) {
        HILOG_ERROR("Fail to start user test.");
        return INNER_ERR;
    }
    return ERR_OK;
}

int AppScheduler::FinishUserTest(const std::string &msg, const int64_t &resultCode, const std::string &bundleName)
{
    CHECK_POINTER_AND_RETURN(appMgrClient_, INNER_ERR);
    int ret = appMgrClient_->FinishUserTest(msg, resultCode, bundleName);
    if (ret != ERR_OK) {
        HILOG_ERROR("Fail to start user test.");
        return INNER_ERR;
    }
    return ERR_OK;
}

int AppScheduler::UpdateConfiguration(const AppExecFwk::Configuration &config)
{
    CHECK_POINTER_AND_RETURN(appMgrClient_, INNER_ERR);
    auto ret = static_cast<int>(appMgrClient_->UpdateConfiguration(config));
    if (ret != ERR_OK) {
        HILOG_ERROR("UpdateConfiguration failed.");
        return INNER_ERR;
    }

    return ERR_OK;
}

int AppScheduler::GetConfiguration(AppExecFwk::Configuration &config)
{
    CHECK_POINTER_AND_RETURN(appMgrClient_, INNER_ERR);
    auto ret = static_cast<int>(appMgrClient_->GetConfiguration(config));
    if (ret != ERR_OK) {
        HILOG_ERROR("GetConfiguration failed.");
        return INNER_ERR;
    }

    return ERR_OK;
}

int AppScheduler::GetAbilityRecordsByProcessID(const int pid, std::vector<sptr<IRemoteObject>> &tokens)
{
    CHECK_POINTER_AND_RETURN(appMgrClient_, INNER_ERR);
    auto ret = static_cast<int>(appMgrClient_->GetAbilityRecordsByProcessID(pid, tokens));
    if (ret != ERR_OK) {
        HILOG_ERROR("GetAbilityRecordsByProcessID failed.");
        return INNER_ERR;
    }

    return ERR_OK;
}

int AppScheduler::GetApplicationInfoByProcessID(const int pid, AppExecFwk::ApplicationInfo &application, bool &debug)
{
    CHECK_POINTER_AND_RETURN(appMgrClient_, INNER_ERR);
    auto ret = static_cast<int>(appMgrClient_->GetApplicationInfoByProcessID(pid, application, debug));
    if (ret != ERR_OK) {
        HILOG_ERROR("GetApplicationInfoByProcessID failed.");
        return ret;
    }

    return ERR_OK;
}

#ifdef ABILITY_COMMAND_FOR_TEST
int AppScheduler::BlockAppService()
{
    HILOG_INFO("[%{public}s(%{public}s)] enter", __FILE__, __FUNCTION__);
    CHECK_POINTER_AND_RETURN(appMgrClient_, INNER_ERR);
    auto ret = static_cast<int>(IN_PROCESS_CALL(appMgrClient_->BlockAppService()));
    if (ret != ERR_OK) {
        HILOG_ERROR("BlockAppService failed.");
        return INNER_ERR;
    }
    return ERR_OK;
}
#endif

void AppScheduler::SetCurrentUserId(const int32_t userId)
{
    CHECK_POINTER(appMgrClient_);
    IN_PROCESS_CALL_WITHOUT_RET(appMgrClient_->SetCurrentUserId(userId));
}
}  // namespace AAFwk
}  // namespace OHOS
