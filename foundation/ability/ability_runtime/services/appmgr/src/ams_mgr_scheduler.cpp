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

#include "ams_mgr_scheduler.h"
#include <sys/types.h>

#include "datetime_ex.h"
#include "ipc_skeleton.h"
#include "system_ability_definition.h"

#include "app_death_recipient.h"
#include "app_mgr_constants.h"
#include "hilog_wrapper.h"
#include "perf_profile.h"
#include "permission_constants.h"
#include "permission_verification.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string TASK_LOAD_ABILITY = "LoadAbilityTask";
const std::string TASK_TERMINATE_ABILITY = "TerminateAbilityTask";
const std::string TASK_UPDATE_ABILITY_STATE = "UpdateAbilityStateTask";
const std::string TASK_UPDATE_EXTENSION_STATE = "UpdateExtensionStateTask";
const std::string TASK_REGISTER_APP_STATE_CALLBACK = "RegisterAppStateCallbackTask";
const std::string TASK_STOP_ALL_PROCESS = "StopAllProcessTask";
const std::string TASK_ABILITY_BEHAVIOR_ANALYSIS = "AbilityBehaviorAnalysisTask";
const std::string TASK_KILL_PROCESS_BY_ABILITY_TOKEN = "KillProcessByAbilityTokenTask";
const std::string TASK_KILL_PROCESSES_BY_USERID = "KillProcessesByUserIdTask";
const std::string TASK_KILL_APPLICATION = "KillApplicationTask";
};  // namespace

AmsMgrScheduler::AmsMgrScheduler(
    const std::shared_ptr<AppMgrServiceInner> &mgrServiceInner_, const std::shared_ptr<AMSEventHandler> &handler_)
    : amsMgrServiceInner_(mgrServiceInner_), amsHandler_(handler_)
{}

AmsMgrScheduler::~AmsMgrScheduler()
{
    HILOG_INFO("AmsMgrScheduler instance destroyed");
}

void AmsMgrScheduler::LoadAbility(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
    const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<ApplicationInfo> &appInfo,
    const std::shared_ptr<AAFwk::Want> &want)
{
    if (!abilityInfo || !appInfo) {
        HILOG_ERROR("param error");
        return;
    }

    if (!IsReady()) {
        return;
    }

    if (amsMgrServiceInner_->VerifyRequestPermission() != ERR_OK) {
        HILOG_ERROR("Permission verification failed.");
        return;
    }
    PerfProfile::GetInstance().SetAbilityLoadStartTime(GetTickCount());
    std::function<void()> loadAbilityFunc =
        std::bind(&AppMgrServiceInner::LoadAbility, amsMgrServiceInner_, token, preToken, abilityInfo, appInfo, want);

    amsHandler_->PostTask(loadAbilityFunc, TASK_LOAD_ABILITY);
}

void AmsMgrScheduler::UpdateAbilityState(const sptr<IRemoteObject> &token, const AbilityState state)
{
    if (!IsReady()) {
        return;
    }

    if (amsMgrServiceInner_->VerifyRequestPermission() != ERR_OK) {
        HILOG_ERROR("Permission verification failed.");
        return;
    }
    std::function<void()> updateAbilityStateFunc =
        std::bind(&AppMgrServiceInner::UpdateAbilityState, amsMgrServiceInner_, token, state);
    amsHandler_->PostTask(updateAbilityStateFunc, TASK_UPDATE_ABILITY_STATE);
}

void AmsMgrScheduler::UpdateExtensionState(const sptr<IRemoteObject> &token, const ExtensionState state)
{
    if (!IsReady()) {
        return;
    }

    if (amsMgrServiceInner_->VerifyRequestPermission() != ERR_OK) {
        HILOG_ERROR("Permission verification failed.");
        return;
    }
    std::function<void()> updateExtensionStateFunc =
        std::bind(&AppMgrServiceInner::UpdateExtensionState, amsMgrServiceInner_, token, state);
    amsHandler_->PostTask(updateExtensionStateFunc, TASK_UPDATE_EXTENSION_STATE);
}

void AmsMgrScheduler::TerminateAbility(const sptr<IRemoteObject> &token, bool clearMissionFlag)
{
    if (!IsReady()) {
        return;
    }

    if (amsMgrServiceInner_->VerifyRequestPermission() != ERR_OK) {
        HILOG_ERROR("Permission verification failed.");
        return;
    }
    std::function<void()> terminateAbilityFunc =
        std::bind(&AppMgrServiceInner::TerminateAbility, amsMgrServiceInner_, token, clearMissionFlag);
    amsHandler_->PostTask(terminateAbilityFunc, TASK_TERMINATE_ABILITY);
}

void AmsMgrScheduler::RegisterAppStateCallback(const sptr<IAppStateCallback> &callback)
{
    if (!IsReady()) {
        return;
    }
    std::function<void()> registerAppStateCallbackFunc =
        std::bind(&AppMgrServiceInner::RegisterAppStateCallback, amsMgrServiceInner_, callback);
    amsHandler_->PostTask(registerAppStateCallbackFunc, TASK_REGISTER_APP_STATE_CALLBACK);
}

void AmsMgrScheduler::AbilityBehaviorAnalysis(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
    const int32_t visibility, const int32_t perceptibility, const int32_t connectionState)
{
    if (!IsReady()) {
        return;
    }

    if (amsMgrServiceInner_->VerifyRequestPermission() != ERR_OK) {
        HILOG_ERROR("Permission verification failed.");
        return;
    }
    std::function<void()> abilityBehaviorAnalysisFunc = std::bind(&AppMgrServiceInner::AbilityBehaviorAnalysis,
        amsMgrServiceInner_, token, preToken, visibility, perceptibility, connectionState);
    amsHandler_->PostTask(abilityBehaviorAnalysisFunc, TASK_ABILITY_BEHAVIOR_ANALYSIS);
}

void AmsMgrScheduler::KillProcessByAbilityToken(const sptr<IRemoteObject> &token)
{
    if (!IsReady()) {
        return;
    }

    if (amsMgrServiceInner_->VerifyProcessPermission(token) != ERR_OK) {
        HILOG_ERROR("%{public}s: Permission verification failed", __func__);
        return;
    }

    std::function<void()> killProcessByAbilityTokenFunc =
        std::bind(&AppMgrServiceInner::KillProcessByAbilityToken, amsMgrServiceInner_, token);
    amsHandler_->PostTask(killProcessByAbilityTokenFunc, TASK_KILL_PROCESS_BY_ABILITY_TOKEN);
}

void AmsMgrScheduler::KillProcessesByUserId(int32_t userId)
{
    if (!IsReady()) {
        return;
    }

    auto permission = AAFwk::PermissionConstants::PERMISSION_CLEAN_BACKGROUND_PROCESSES;
    if (amsMgrServiceInner_->VerifyAccountPermission(permission, userId) == ERR_PERMISSION_DENIED) {
        HILOG_ERROR("%{public}s: Permission verification failed", __func__);
        return;
    }

    std::function<void()> killProcessesByUserIdFunc =
        std::bind(&AppMgrServiceInner::KillProcessesByUserId, amsMgrServiceInner_, userId);
    amsHandler_->PostTask(killProcessesByUserIdFunc, TASK_KILL_PROCESSES_BY_USERID);
}

int32_t AmsMgrScheduler::KillProcessWithAccount(const std::string &bundleName, const int accountId)
{
    HILOG_INFO("bundleName = %{public}s, accountId = %{public}d", bundleName.c_str(), accountId);
    if (!IsReady()) {
        return ERR_INVALID_OPERATION;
    }
    return amsMgrServiceInner_->KillApplicationByUserId(bundleName, accountId);
}

void AmsMgrScheduler::AbilityAttachTimeOut(const sptr<IRemoteObject> &token)
{
    HILOG_INFO("AmsMgrScheduler AttachTimeOut begin");
    if (!IsReady()) {
        return;
    }

    if (amsMgrServiceInner_->VerifyRequestPermission() != ERR_OK) {
        HILOG_ERROR("Permission verification failed.");
        return;
    }
    auto task = [=]() { amsMgrServiceInner_->HandleAbilityAttachTimeOut(token); };
    amsHandler_->PostTask(task);
}

void AmsMgrScheduler::PrepareTerminate(const sptr<IRemoteObject> &token)
{
    HILOG_INFO("Notify AppMgrService to prepare to terminate the ability.");
    if (!IsReady()) {
        return;
    }

    if (amsMgrServiceInner_->VerifyRequestPermission() != ERR_OK) {
        HILOG_ERROR("Permission verification failed.");
        return;
    }
    auto task = [=]() { amsMgrServiceInner_->PrepareTerminate(token); };
    amsHandler_->PostTask(task);
}

int32_t AmsMgrScheduler::UpdateApplicationInfoInstalled(const std::string &bundleName, const int uid)
{
    if (!IsReady()) {
        return ERR_INVALID_OPERATION;
    }

    return amsMgrServiceInner_->UpdateApplicationInfoInstalled(bundleName, uid);
}

int32_t AmsMgrScheduler::KillApplication(const std::string &bundleName)
{
    HILOG_INFO("bundleName = %{public}s", bundleName.c_str());
    if (!IsReady()) {
        return ERR_INVALID_OPERATION;
    }

    return amsMgrServiceInner_->KillApplication(bundleName);
}

int32_t AmsMgrScheduler::KillApplicationByUid(const std::string &bundleName, const int uid)
{
    HILOG_INFO("bundleName = %{public}s, uid = %{public}d", bundleName.c_str(), uid);
    if (!IsReady()) {
        return ERR_INVALID_OPERATION;
    }
    return amsMgrServiceInner_->KillApplicationByUid(bundleName, uid);
}

int32_t AmsMgrScheduler::KillApplicationSelf()
{
    if (!IsReady()) {
        return ERR_INVALID_OPERATION;
    }
    return amsMgrServiceInner_->KillApplicationSelf();
}

bool AmsMgrScheduler::IsReady() const
{
    if (!amsMgrServiceInner_) {
        HILOG_ERROR("amsMgrServiceInner_ is null");
        return false;
    }
    if (!amsHandler_) {
        HILOG_ERROR("amsHandler_ is null");
        return false;
    }
    return true;
}

void AmsMgrScheduler::GetRunningProcessInfoByToken(
    const sptr<IRemoteObject> &token, AppExecFwk::RunningProcessInfo &info)
{
    if (!IsReady()) {
        return;
    }

    amsMgrServiceInner_->GetRunningProcessInfoByToken(token, info);
}

void AmsMgrScheduler::GetRunningProcessInfoByPid(const pid_t pid, OHOS::AppExecFwk::RunningProcessInfo &info)
{
    if (!IsReady()) {
        return;
    }

    if (amsMgrServiceInner_->VerifyRequestPermission() != ERR_OK) {
        HILOG_ERROR("Permission verification failed.");
        return;
    }
    amsMgrServiceInner_->GetRunningProcessInfoByPid(pid, info);
}

void AmsMgrScheduler::StartSpecifiedAbility(const AAFwk::Want &want, const AppExecFwk::AbilityInfo &abilityInfo)
{
    if (!IsReady()) {
        return;
    }

    if (amsMgrServiceInner_->VerifyRequestPermission() != ERR_OK) {
        HILOG_ERROR("Permission verification failed.");
        return;
    }
    auto task = [=]() { amsMgrServiceInner_->StartSpecifiedAbility(want, abilityInfo); };
    amsHandler_->PostTask(task);
}

void AmsMgrScheduler::RegisterStartSpecifiedAbilityResponse(const sptr<IStartSpecifiedAbilityResponse> &response)
{
    if (!IsReady()) {
        return;
    }
    auto task = [=]() { amsMgrServiceInner_->RegisterStartSpecifiedAbilityResponse(response); };
    amsHandler_->PostTask(task);
}

int AmsMgrScheduler::GetApplicationInfoByProcessID(const int pid, AppExecFwk::ApplicationInfo &application, bool &debug)
{
    if (!IsReady()) {
        return ERR_INVALID_OPERATION;
    }
    return amsMgrServiceInner_->GetApplicationInfoByProcessID(pid, application, debug);
}

void AmsMgrScheduler::SetCurrentUserId(const int32_t userId)
{
    if (!IsReady()) {
        return;
    }
    amsMgrServiceInner_->SetCurrentUserId(userId);
}
}  // namespace AppExecFwk
}  // namespace OHOS
