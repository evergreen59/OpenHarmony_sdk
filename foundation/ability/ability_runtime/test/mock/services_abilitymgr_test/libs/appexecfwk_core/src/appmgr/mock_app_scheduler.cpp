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

#include "hilog_wrapper.h"
#include "ability_util.h"
#include "ability_manager_errors.h"
#include "ability_record.h"
#include "appmgr/app_mgr_constants.h"

namespace OHOS {
namespace AAFwk {
AppScheduler::AppScheduler()
{
    HILOG_INFO(" Test AppScheduler::AppScheduler()");
}

AppScheduler::~AppScheduler()
{
    HILOG_INFO("Test AppScheduler::~AppScheduler()");
}

bool AppScheduler::Init(const std::weak_ptr<AppStateCallback> &callback)
{
    HILOG_INFO("Test AppScheduler::Init()");
    if (!callback.lock()) {
        return false;
    }
    return true;
}

int AppScheduler::LoadAbility(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
    const AppExecFwk::AbilityInfo &abilityInfo, const AppExecFwk::ApplicationInfo &applicationInfo,
    const AAFwk::Want &want)
{
    HILOG_INFO("Test AppScheduler::LoadAbility()");
    if (applicationInfo.bundleName.find("com.ix.First.Test") != std::string::npos) {
        return INNER_ERR;
    }
    return ERR_OK;
}

int AppScheduler::TerminateAbility(const sptr<IRemoteObject> &token, bool isClearMissionFlag)
{
    HILOG_INFO("Test AppScheduler::TerminateAbility()");
    return ERR_OK;
}

void AppScheduler::MoveToForeground(const sptr<IRemoteObject> &token)
{
    HILOG_INFO("Test AppScheduler::MoveToForeground()");
}

void AppScheduler::MoveToBackground(const sptr<IRemoteObject> &token)
{
    HILOG_INFO("Test AppScheduler::MoveToBackground()");
}

void AppScheduler::AbilityBehaviorAnalysis(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
    const int32_t visibility, const int32_t perceptibility, const int32_t connectionState)
{
    HILOG_INFO("Test AppScheduler::AbilityBehaviorAnalysis()");
}

void AppScheduler::KillProcessByAbilityToken(const sptr<IRemoteObject> &token)
{
    HILOG_INFO("Test AppScheduler::KillProcessByAbilityToken()");
}

void AppScheduler::KillProcessesByUserId(int32_t userId)
{
    HILOG_INFO("Test AppScheduler::KillProcessesByUserId()");
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
    HILOG_INFO("Test AppScheduler::OnAbilityRequestDone()");
}

int AppScheduler::KillApplication(const std::string &bundleName)
{
    HILOG_INFO("Test AppScheduler::KillApplication()");
    return ERR_OK;
}

void AppScheduler::AttachTimeOut(const sptr<IRemoteObject> &token)
{
    HILOG_INFO("Test AppScheduler::AttachTimeOut()");
}

void AppScheduler::PrepareTerminate(const sptr<IRemoteObject> &token)
{
    HILOG_INFO("Test AppScheduler::PrepareTerminate()");
}

void AppScheduler::OnAppStateChanged(const AppExecFwk::AppProcessData &appData)
{
    HILOG_INFO("Test AppScheduler::OnAppStateChanged()");
}

int AppScheduler::ClearUpApplicationData(const std::string &bundleName)
{
    HILOG_INFO("Test AppScheduler::ClearUpApplicationData()");
    return ERR_OK;
}

void AppScheduler::UpdateAbilityState(const sptr<IRemoteObject> &token, const AppExecFwk::AbilityState state)
{
    HILOG_INFO("Test AppScheduler::UpdateAbilityState()");
}

void AppScheduler::UpdateExtensionState(const sptr<IRemoteObject> &token, const AppExecFwk::ExtensionState state)
{
    HILOG_INFO("Test AppScheduler::UpdateExtensionState()");
}

void AppScheduler::StartupResidentProcess(const std::vector<AppExecFwk::BundleInfo> &bundleInfos)
{
    HILOG_INFO("Test AppScheduler::StartupResidentProcess()");
}

int AppScheduler::GetProcessRunningInfos(std::vector<AppExecFwk::RunningProcessInfo> &info)
{
    HILOG_INFO("Test AppScheduler::GetProcessRunningInfos()");
    return 0;
}

void AppScheduler::GetRunningProcessInfoByToken(const sptr<IRemoteObject> &token, AppExecFwk::RunningProcessInfo &info)
{
    HILOG_INFO("Test AppScheduler::GetRunningProcessInfoByToken()");
}

void AppScheduler::GetRunningProcessInfoByPid(const pid_t pid, OHOS::AppExecFwk::RunningProcessInfo &info) const
{
    HILOG_INFO("Test AppScheduler::GetRunningProcessInfoByPid()");
}

void AppScheduler::StartSpecifiedAbility(const AAFwk::Want &want, const AppExecFwk::AbilityInfo &abilityInfo)
{}

int AppScheduler::StartUserTest(
    const Want &want, const sptr<IRemoteObject> &observer, const AppExecFwk::BundleInfo &bundleInfo, int32_t userId)
{
    return 0;
}

int AppScheduler::GetApplicationInfoByProcessID(const int pid, AppExecFwk::ApplicationInfo &application, bool &debug)
{
    if (pid < 0) {
        return -1;
    }

    return 0;
}

int AppScheduler::GetAbilityRecordsByProcessID(const int pid, std::vector<sptr<IRemoteObject>> &tokens)
{
    return 0;
}
}  // namespace AAFwk
}  // namespace OHOS
