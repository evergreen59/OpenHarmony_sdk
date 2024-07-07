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

#ifndef MOCK_OHOS_ABILITY_RUNTIME_MOCK_APP_MGR_SERVICE_H
#define MOCK_OHOS_ABILITY_RUNTIME_MOCK_APP_MGR_SERVICE_H

#include "gmock/gmock.h"
#include "semaphore_ex.h"
#include "app_mgr_stub.h"

namespace OHOS {
namespace AppExecFwk {
class MockAppMgrService : public AppMgrStub {
public:
    MOCK_METHOD5(LoadAbility,
        void(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
            const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<ApplicationInfo> &appInfo,
            const std::shared_ptr<AAFwk::Want> &want));
    MOCK_METHOD2(TerminateAbility, void(const sptr<IRemoteObject> &token, bool clearMissionFlag));
    MOCK_METHOD2(UpdateAbilityState, void(const sptr<IRemoteObject> &token, const AbilityState state));
    MOCK_METHOD1(AttachApplication, void(const sptr<IRemoteObject> &app));
    MOCK_METHOD1(NotifyMemoryLevel, int(int32_t level));
    MOCK_METHOD1(ApplicationForegrounded, void(const int32_t recordId));
    MOCK_METHOD1(ApplicationBackgrounded, void(const int32_t recordId));
    MOCK_METHOD1(ApplicationTerminated, void(const int32_t recordId));
    MOCK_METHOD2(CheckPermission, int32_t(const int32_t recordId, const std::string &permission));
    MOCK_METHOD1(AbilityCleaned, void(const sptr<IRemoteObject> &token));
    MOCK_METHOD2(UpdateApplicationInfoInstalled, int(const std::string&, const int uid));
    MOCK_METHOD1(KillApplication, int32_t(const std::string &appName));
    MOCK_METHOD2(KillApplicationByUid, int(const std::string &, const int uid));
    MOCK_METHOD1(IsBackgroundRunningRestricted, int(const std::string &bundleName));
    MOCK_METHOD1(GetAllRunningProcesses, int(std::vector<RunningProcessInfo> &info));
    MOCK_METHOD2(GetProcessRunningInfosByUserId, int(std::vector<RunningProcessInfo> &info, int32_t userId));
    MOCK_METHOD0(GetAmsMgr, sptr<IAmsMgr>());
    MOCK_METHOD1(GetAppFreezingTime, void(int &time));
    MOCK_METHOD1(SetAppFreezingTime, void(int time));
    MOCK_METHOD1(ClearUpApplicationData, int32_t(const std::string &bundleName));
    MOCK_METHOD1(StartupResidentProcess, void(const std::vector<AppExecFwk::BundleInfo> &bundleInfos));
    MOCK_METHOD1(AddAbilityStageDone, void(const int32_t recordId));
    MOCK_METHOD0(PreStartNWebSpawnProcess, int());
    MOCK_METHOD4(StartRenderProcess, int(const std::string&, int32_t, int32_t, pid_t&));
    MOCK_METHOD1(AttachRenderProcess, void(const sptr<IRemoteObject> &renderScheduler));
    MOCK_METHOD2(GetRenderProcessTerminationStatus, int(pid_t renderPid, int &status));
    MOCK_METHOD2(RegisterApplicationStateObserver, int32_t(const sptr<IApplicationStateObserver> &observer,
        const std::vector<std::string> &bundleNameList));
    MOCK_METHOD1(UnregisterApplicationStateObserver, int32_t(const sptr<IApplicationStateObserver> &observer));
    MOCK_METHOD3(ScheduleAcceptWantDone,
        void(const int32_t recordId, const AAFwk::Want &want, const std::string &flag));
    MOCK_METHOD2(GetAbilityRecordsByProcessID, int(const int pid, std::vector<sptr<IRemoteObject>> &tokens));
    MOCK_METHOD1(GetConfiguration, int32_t(Configuration& config));
    MOCK_METHOD1(UpdateConfiguration, int32_t(const Configuration &config));
    MOCK_METHOD1(RegisterConfigurationObserver, int32_t(const sptr<IConfigurationObserver> &observer));
    MOCK_METHOD1(UnregisterConfigurationObserver, int32_t(const sptr<IConfigurationObserver> &observer));
    #ifdef ABILITY_COMMAND_FOR_TEST
    MOCK_METHOD0(BlockAppService, int());
    #endif
    MOCK_METHOD1(GetAppRunningStateByBundleName, bool(const std::string &bundleName));
    MOCK_METHOD2(NotifyLoadRepairPatch, int32_t(const std::string &bundleName,
        const sptr<IQuickFixCallback> &callback));
    MOCK_METHOD2(NotifyHotReloadPage, int32_t(const std::string &bundleName, const sptr<IQuickFixCallback> &callback));
    MOCK_METHOD2(NotifyUnLoadRepairPatch, int32_t(const std::string &bundleName,
        const sptr<IQuickFixCallback> &callback));

    virtual int StartUserTestProcess(
        const AAFwk::Want &want, const sptr<IRemoteObject> &observer, const BundleInfo &bundleInfo, int32_t userId)
    {
        return 0;
    }

    virtual int FinishUserTest(const std::string &msg, const int64_t &resultCode, const std::string &bundleName)
    {
        return 0;
    }

    virtual int GetProcessRunningInformation(RunningProcessInfo &info)
    {
        return 0;
    }

    virtual void RegisterAppStateCallback(const sptr<IAppStateCallback> &callback)
    {
        callback_ = callback;
    }

    int32_t CheckPermissionImpl([[maybe_unused]] const int32_t recordId, const std::string &data)
    {
        data_ = data;
        return 0;
    }

    void KillApplicationImpl(const std::string &data)
    {
        data_ = data;
    }

    const std::string &GetData() const
    {
        return data_;
    }

    void Wait()
    {
        sem_.Wait();
    }

    void Post()
    {
        sem_.Post();
    }

    void UpdateState() const
    {
        if (!callback_) {
            return;
        }
        AppProcessData processData;
        processData.pid = 1;
        processData.appState = ApplicationState::APP_STATE_BEGIN;
        callback_->OnAppStateChanged(processData);
    }

    void Terminate(const sptr<IRemoteObject> &token) const
    {
        if (!callback_) {
            return;
        }
        AbilityState st = AbilityState::ABILITY_STATE_BEGIN;
        callback_->OnAbilityRequestDone(token, st);
    }

    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel &, MessageParcel &, MessageOption &));

    int InvokeSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        code_ = code;

        return 0;
    }

    int code_;

private:
    Semaphore sem_;
    std::string data_;
    sptr<IAppStateCallback> callback_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // MOCK_OHOS_ABILITY_RUNTIME_MOCK_APP_MGR_SERVICE_H
