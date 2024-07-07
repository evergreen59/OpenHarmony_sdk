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

#include <gtest/gtest.h>
#include "gmock/gmock.h"
#include "semaphore_ex.h"
#include "app_scheduler_interface.h"
#include "app_mgr_stub.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
class MockAppMgrService : public AppMgrStub {
public:
    MOCK_METHOD5(LoadAbility,
        void(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
            const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<ApplicationInfo> &appInfo,
            const std::shared_ptr<AAFwk::Want> &want));
    MOCK_METHOD2(TerminateAbility, void(const sptr<IRemoteObject> &token, bool isClearMissionFlag));
    MOCK_METHOD2(UpdateAbilityState, void(const sptr<IRemoteObject> &token, const AbilityState state));
    MOCK_METHOD1(SetAppFreezingTime, void(int time));
    MOCK_METHOD1(GetAppFreezingTime, void(int &time));
    MOCK_METHOD1(AddAbilityStageDone, void(const int32_t recordId));
    MOCK_METHOD1(StartupResidentProcess, void(const std::vector<AppExecFwk::BundleInfo> &bundleInfos));
    MOCK_METHOD1(NotifyMemoryLevel, int(int32_t level));
    MOCK_METHOD2(GetProcessRunningInfosByUserId, int(std::vector<RunningProcessInfo> &info, int32_t userId));
    MOCK_METHOD4(StartUserTestProcess, int(const AAFwk::Want &want, const sptr<IRemoteObject> &observer,
        const BundleInfo &bundleInfo, int32_t userId));
    MOCK_METHOD3(FinishUserTest, int(const std::string &msg, const int64_t &resultCode,
        const std::string &bundleName));
    MOCK_METHOD3(ScheduleAcceptWantDone, void(const int32_t recordId, const AAFwk::Want &want,
        const std::string &flag));
    MOCK_METHOD2(GetAbilityRecordsByProcessID, int(const int pid, std::vector<sptr<IRemoteObject>> &tokens));
    #ifdef ABILITY_COMMAND_FOR_TEST
    MOCK_METHOD0(BlockAppService, int());
    #endif
    MOCK_METHOD0(PreStartNWebSpawnProcess, int());
    MOCK_METHOD4(StartRenderProcess, int(const std::string &renderParam, int32_t ipcFd,
        int32_t sharedFd, pid_t &renderPid));
    MOCK_METHOD1(AttachRenderProcess, void(const sptr<IRemoteObject> &renderScheduler));
    MOCK_METHOD2(GetRenderProcessTerminationStatus, int(pid_t renderPid, int &status));
    MOCK_METHOD1(GetConfiguration, int32_t(Configuration& config));
    MOCK_METHOD1(UpdateConfiguration, int32_t(const Configuration &config));
    MOCK_METHOD1(RegisterConfigurationObserver, int32_t(const sptr<IConfigurationObserver> &observer));
    MOCK_METHOD1(UnregisterConfigurationObserver, int32_t(const sptr<IConfigurationObserver> &observer));
    MOCK_METHOD1(GetAppRunningStateByBundleName, bool(const std::string &bundleName));
    MOCK_METHOD2(NotifyLoadRepairPatch, int32_t(const std::string &bundleName,
        const sptr<IQuickFixCallback> &callback));
    MOCK_METHOD2(NotifyHotReloadPage, int32_t(const std::string &bundleName, const sptr<IQuickFixCallback> &callback));
    MOCK_METHOD2(NotifyUnLoadRepairPatch, int32_t(const std::string &bundleName,
        const sptr<IQuickFixCallback> &callback));

    void AttachApplication(const sptr<IRemoteObject> &app)
    {
        GTEST_LOG_(INFO) << "MockAppMgrService::AttachApplication called";
        Attached_ = true;
        EXPECT_TRUE(Attached_);
        Appthread_ = iface_cast<IAppScheduler>(app);
    }

    virtual void ApplicationForegrounded(const int32_t recordId)
    {
        GTEST_LOG_(INFO) << "MockAppMgrService::ApplicationForegrounded called";
        Foregrounded_ = true;
        EXPECT_TRUE(Foregrounded_);
    }

    virtual void ApplicationBackgrounded(const int32_t recordId)
    {
        GTEST_LOG_(INFO) << "MockAppMgrService::ApplicationBackgrounded called";
        Backgrounded_ = true;
        EXPECT_TRUE(Backgrounded_);
    }

    virtual void ApplicationTerminated(const int32_t recordId)
    {
        GTEST_LOG_(INFO) << "MockAppMgrService::ApplicationTerminated called";
        Terminated_ = true;
        EXPECT_TRUE(Terminated_);
    }
    MOCK_METHOD2(CheckPermission, int32_t(const int32_t recordId, const std::string &permission));

    virtual void AbilityCleaned(const sptr<IRemoteObject> &token)
    {
        GTEST_LOG_(INFO) << "MockAppMgrService::AbilityCleaned called";
        Cleaned_ = true;
        EXPECT_TRUE(Cleaned_);
    }

    MOCK_METHOD2(UpdateApplicationInfoInstalled, int(const std::string&, const int uid));

    MOCK_METHOD1(KillApplication, int(const std::string &appName));
    MOCK_METHOD2(KillApplicationByUid, int(const std::string &, const int uid));

    virtual sptr<IAmsMgr> GetAmsMgr() override
    {
        return nullptr;
    };
    virtual int32_t ClearUpApplicationData(const std::string &appName) override
    {
        return 0;
    }

    virtual int GetProcessRunningInformation(RunningProcessInfo &info)
    {
        return 0;
    }

    int IsBackgroundRunningRestricted(const std::string &appName)
    {
        return 0;
    };
    virtual int GetAllRunningProcesses(std::vector<RunningProcessInfo> &info) override
    {
        return 0;
    };

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

    void ScheduleTerminateApplication()
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleTerminateApplication();
        }
    }

    void ScheduleLaunchApplication(const AppLaunchData &lanchdata, const Configuration & config)
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleLaunchApplication(lanchdata, config);
        }
    }

    void ScheduleForegroundApplication()
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleForegroundApplication();
        }
    }

    void ScheduleBackgroundApplication()
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleBackgroundApplication();
        }
    }

    void ScheduleShrinkMemory(const int32_t level)
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleShrinkMemory(level);
        }
    }

    void ScheduleLowMemory()
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleLowMemory();
        }
    }

    void ScheduleLaunchAbility(const AbilityInfo &abilityinf, const sptr<IRemoteObject> &token,
        const std::shared_ptr<AAFwk::Want> &want)
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleLaunchAbility(abilityinf, token, want);
        }
    }

    void ScheduleCleanAbility(const sptr<IRemoteObject> &token)
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleCleanAbility(token);
        }
    }

    void ScheduleProfileChanged(const Profile &profile)
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleProfileChanged(profile);
        }
    }

    void ScheduleConfigurationUpdated(const Configuration &config)
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleConfigurationUpdated(config);
        }
    }

    sptr<IAppScheduler> GetAppthread()
    {
        return Appthread_;
    }

    bool IsAttached()
    {
        HILOG_INFO("MockAppMgrService::IsAttached Attached_ = %{public}d", Attached_);
        return Attached_;
    }

    bool IsForegrounded()
    {
        HILOG_INFO("MockAppMgrService::IsForegrounded Foregrounded_ = %{public}d", Foregrounded_);
        return Foregrounded_;
    }

    bool IsBackgrounded()
    {
        HILOG_INFO("MockAppMgrService::IsBackgrounded Backgrounded_ = %{public}d", Backgrounded_);
        return Backgrounded_;
    }

    bool IsTerminated()
    {
        HILOG_INFO("MockAppMgrService::IsTerminated Terminated_ = %{public}d", Terminated_);
        return Terminated_;
    }

    void init()
    {
        HILOG_INFO("MockAppMgrService::init called");
        Attached_ = false;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }

private:
    bool Attached_ = false;
    bool Foregrounded_ = false;
    bool Backgrounded_ = false;
    bool Terminated_ = false;
    bool Cleaned_ = false;
    sptr<IAppScheduler> Appthread_ = nullptr;
    Semaphore sem_;
    std::string data_;
    sptr<IAppStateCallback> callback_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // MOCK_OHOS_ABILITY_RUNTIME_MOCK_APP_MGR_SERVICE_H
