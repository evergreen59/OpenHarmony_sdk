/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <gtest/gtest.h>
#include "gmock/gmock.h"

#include <thread>
#include <chrono>

#define private public
#define protected public
#include "system_ability_definition.h"
#include "lifecycle_test_base.h"
#include "mock_bundle_manager.h"
#include "sa_mgr_client.h"
#undef private
#undef protected

using namespace testing::ext;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AAFwk {
namespace {
const std::string NAME_BUNDLE_MGR_SERVICE = "BundleMgrService";
}

static void WaitUntilTaskFinished()
{
    const uint32_t maxRetryCount = 1000;
    const uint32_t sleepTime = 1000;
    uint32_t count = 0;
    auto handler = OHOS::DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (handler == nullptr) {
        GTEST_LOG_(ERROR) << "handler is nullptr";
        return;
    }
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };
    if (handler->PostTask(f)) {
        while (!taskCalled.load()) {
            ++count;
            if (count >= maxRetryCount) {
                break;
            }
            usleep(sleepTime);
        }
    }
}

class LifecycleTest : public testing::Test, public LifecycleTestBase {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp() override;

    void TearDown() override;

    bool StartNextAbility() override;

    int AttachAbility(const OHOS::sptr<OHOS::AAFwk::AbilityScheduler> &scheduler,
        const OHOS::sptr<OHOS::IRemoteObject> &token) override;

    void OnStartabilityAms();
public:
    int startLancherFlag_ = false;

    std::shared_ptr<AbilityManagerService> abilityMs_ = OHOS::DelayedSingleton<AbilityManagerService>::GetInstance();
    std::shared_ptr<OHOS::AAFwk::AbilityRecord> launcherAbilityRecord_ {nullptr};  // launcher ability
    OHOS::sptr<OHOS::IRemoteObject> launcherToken_ {nullptr};                      // token of launcher ability
    std::shared_ptr<OHOS::AAFwk::AbilityRecord> nextAbilityRecord_ {nullptr};      // ability being launched
    OHOS::sptr<OHOS::IRemoteObject> nextToken_ {nullptr};                          // token of ability being launched
    OHOS::sptr<OHOS::AAFwk::AbilityScheduler> launcherScheduler_ {nullptr};        // launcher ability thread interface
    OHOS::sptr<OHOS::AAFwk::AbilityScheduler> nextScheduler_ {nullptr};            // next ability thread interface
    std::unique_ptr<LifeTestCommand> command_ {nullptr};                           // test command_ interact with ams_
};

void LifecycleTest::OnStartabilityAms()
{
    if (abilityMs_) {
        if (abilityMs_->state_ == ServiceRunningState::STATE_RUNNING) {
            return;
        }

        abilityMs_->state_ = ServiceRunningState::STATE_RUNNING;
        abilityMs_->eventLoop_ = AppExecFwk::EventRunner::Create(AbilityConfig::NAME_ABILITY_MGR_SERVICE);
        EXPECT_TRUE(abilityMs_->eventLoop_);
        abilityMs_->handler_ = std::make_shared<AbilityEventHandler>(abilityMs_->eventLoop_, abilityMs_);
        abilityMs_->connectManager_ = std::make_shared<AbilityConnectManager>(0);
        abilityMs_->connectManagers_.emplace(0, abilityMs_->connectManager_);
        EXPECT_TRUE(abilityMs_->handler_);
        EXPECT_TRUE(abilityMs_->connectManager_);
        abilityMs_->connectManager_->SetEventHandler(abilityMs_->handler_);
        abilityMs_->dataAbilityManager_ = std::make_shared<DataAbilityManager>();
        abilityMs_->dataAbilityManagers_.emplace(0, abilityMs_->dataAbilityManager_);
        EXPECT_TRUE(abilityMs_->dataAbilityManager_);
        abilityMs_->amsConfigResolver_ = std::make_shared<AmsConfigurationParameter>();
        EXPECT_TRUE(abilityMs_->amsConfigResolver_);
        abilityMs_->amsConfigResolver_->Parse();
        abilityMs_->currentMissionListManager_ = std::make_shared<MissionListManager>(0);
        abilityMs_->currentMissionListManager_->Init();
        abilityMs_->pendingWantManager_ = std::make_shared<PendingWantManager>();
        EXPECT_TRUE(abilityMs_->pendingWantManager_);
        abilityMs_->eventLoop_->Run();
        return;
    }

    GTEST_LOG_(INFO) << "OnStart fail";
}

void LifecycleTest::SetUpTestCase(void)
{
    OHOS::DelayedSingleton<SaMgrClient>::GetInstance()->RegisterSystemAbility(
        OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, new BundleMgrService());
}

void LifecycleTest::TearDownTestCase(void)
{
    OHOS::DelayedSingleton<SaMgrClient>::DestroyInstance();
}

void LifecycleTest::SetUp(void)
{
    OnStartabilityAms();
    WaitUntilTaskFinished();
    command_ = std::make_unique<LifeTestCommand>();
}

void LifecycleTest::TearDown(void)
{
    abilityMs_->OnStop();
    launcherAbilityRecord_.reset();
    launcherToken_ = nullptr;
    nextAbilityRecord_.reset();
    nextToken_ = nullptr;
    launcherScheduler_ = nullptr;
    nextScheduler_ = nullptr;
    command_.reset();
    startLancherFlag_ = false;
}

bool LifecycleTest::StartNextAbility()
{
    return true;
}

int LifecycleTest::AttachAbility(
    const OHOS::sptr<OHOS::AAFwk::AbilityScheduler> &scheduler, const OHOS::sptr<OHOS::IRemoteObject> &token)
{
    return abilityMs_->AttachAbilityThread(scheduler, token);
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AttachAbilityThread
 * EnvConditions:NA
 * CaseDescription: verify AttachAbilityThread parameters.
 * AttachAbilityThread fail if IAbilityScheduler or token is nullptr.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_StartLauncherAbilityLifeCycle_001, TestSize.Level1)
{
    if (startLancherFlag_) {
        EXPECT_TRUE(abilityMs_);
        EXPECT_TRUE(launcherAbilityRecord_);
        EXPECT_NE(abilityMs_->AttachAbilityThread(nullptr, launcherToken_), 0);
        EXPECT_NE(abilityMs_->AttachAbilityThread(launcherScheduler_, nullptr), 0);
        EXPECT_EQ(launcherAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVE);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AttachAbilityThread
 * EnvConditions:NA
 * CaseDescription: verify launcher AbilityRecord state_ when AttachAbilityThread success.
 * 1. AbilityState transferred from INITIAL to ACTIVATING.
 * 2. AbilityRecord is attached.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_StartLauncherAbilityLifeCycle_002, TestSize.Level1)
{
    if (startLancherFlag_) {
        EXPECT_TRUE(abilityMs_);
        EXPECT_TRUE(launcherAbilityRecord_);
        EXPECT_EQ(launcherAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVE);
        EXPECT_TRUE(launcherScheduler_);
        EXPECT_TRUE(launcherToken_);
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        EXPECT_EQ(launcherAbilityRecord_->IsReady(), true);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AttachAbilityThread
 * EnvConditions:NA
 * CaseDescription: verify AbilityRecord transition timeout handler.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_StartLauncherAbilityLifeCycle_003, TestSize.Level1)
{
    if (startLancherFlag_) {
        command_->callback_ = false;
        command_->expectState_ = OHOS::AAFwk::AbilityState::ACTIVE;
        command_->state_ = OHOS::AAFwk::AbilityState::INITIAL;
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        pthread_t tid = 0;
        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret =
            LifecycleTest::SemTimedWaitMillis(AbilityManagerService::LOAD_TIMEOUT + DELAY_TEST_TIME, command_->sem_);
        EXPECT_NE(ret, 0);
        // check timeout handler
        EXPECT_EQ(launcherAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVE);
        pthread_join(tid, nullptr);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AttachAbilityThread
 * EnvConditions:NA
 * CaseDescription: verify AbilityTransitionDone parameters.
 * AbilityTransitionDone fail if launcher schedules incorrect Life state_.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_StartLauncherAbilityLifeCycle_004, TestSize.Level1)
{
    if (startLancherFlag_) {
        // AttachAbilityThread done and success
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);

        command_->callback_ = true;
        command_->expectState_ = OHOS::AAFwk::AbilityState::ACTIVE;
        command_->abnormalState_ = OHOS::AAFwk::AbilityState::INACTIVE;
        pthread_t tid = 0;
        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret =
            LifecycleTest::SemTimedWaitMillis(AbilityManagerService::LOAD_TIMEOUT + DELAY_TEST_TIME, command_->sem_);
        if (ret != 0) {
            // check timeout handler
            GTEST_LOG_(INFO) << "timeout. It shouldn't happen.";
            pthread_join(tid, nullptr);
            return;
        }
        pthread_join(tid, nullptr);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AttachAbilityThread
 * EnvConditions:NA
 * CaseDescription: AttachAbilityThread done, verify AbilityRecord state_ when AbilityStartThread success.
 * 1. Life transition from UNDEFINED to ACTIVATING to ACTIVE.
 * 2. AbilityRecord is attached.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_StartLauncherAbilityLifeCycle_005, TestSize.Level1)
{
    if (startLancherFlag_) {
        // AttachAbilityThread done and success
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        command_->callback_ = true;
        command_->expectState_ = OHOS::AAFwk::AbilityState::ACTIVE;
        command_->state_ = OHOS::AAFwk::AbilityState::INITIAL;
        pthread_t tid = 0;

        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret =
            LifecycleTest::SemTimedWaitMillis(AbilityManagerService::LOAD_TIMEOUT + DELAY_TEST_TIME, command_->sem_);
        if (ret != 0) {
            // check timeout handler. It won't happen normally.
            GTEST_LOG_(INFO) << "timeout. It shouldn't happen.";
            pthread_join(tid, nullptr);
            return;
        }
        PacMap saveData;
        abilityMs_->AbilityTransitionDone(launcherToken_, command_->state_, saveData);
        if (launcherAbilityRecord_->GetAbilityState() != OHOS::AAFwk::AbilityState::ACTIVE) {
            WaitUntilTaskFinished();
            EXPECT_EQ(launcherAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVE);
        }
        EXPECT_EQ(launcherAbilityRecord_->IsReady(), true);
        pthread_join(tid, nullptr);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AttachAbilityThread
 * EnvConditions:NA
 * CaseDescription:  hnadeler is timeout
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_StartLauncherAbilityLifeCycle_006, TestSize.Level1)
{
    if (startLancherFlag_) {
        command_->callback_ = false;
        command_->expectState_ = OHOS::AAFwk::AbilityState::ACTIVE;
        command_->state_ = OHOS::AAFwk::AbilityState::INITIAL;
        pthread_t tid = 0;
        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret = LifecycleTest::SemTimedWaitMillis(AbilityManagerService::ACTIVE_TIMEOUT, command_->sem_);
        EXPECT_NE(ret, 0);
        // check AttachAbilityThread timeout handler
        EXPECT_EQ(launcherAbilityRecord_->IsReady(), false);
        pthread_join(tid, nullptr);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AbilityTransitionDone
 * EnvConditions:NA
 * CaseDescription: launcher OnInactive timeout, verify launcher AbilityTransitionDone timeout handler.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_startAbilityLifeCycle_001, TestSize.Level1)
{
    if (startLancherFlag_) {
        command_->callback_ = false;
        command_->expectState_ = OHOS::AAFwk::AbilityState::INACTIVE;
        command_->state_ = OHOS::AAFwk::AbilityState::INITIAL;
        // launcher is in inactivating process.
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        EXPECT_TRUE(StartNextAbility());
        launcherAbilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVATING);
        pthread_t tid = 0;
        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret = LifecycleTest::SemTimedWaitMillis(AbilityManagerService::INACTIVE_TIMEOUT, command_->sem_);
        EXPECT_NE(ret, 0);
        // check AbilityTransitionDone timeout handler
        EXPECT_NE(nextAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::INACTIVATING);
        pthread_join(tid, nullptr);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AbilityTransitionDone
 * EnvConditions:NA
 * CaseDescription: verify AbilityTransitionDone parameters.
 * AbilityTransitionDone fail if life state_ is incompatible with
 * OnInactive process. Or launcher schedules incorrect life state_.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_startAbilityLifeCycle_002, TestSize.Level1)
{
    if (startLancherFlag_) {
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        EXPECT_TRUE(StartNextAbility());
        // launcher is in inactivating process.
        PacMap saveData;
        EXPECT_NE(abilityMs_->AbilityTransitionDone(launcherToken_, OHOS::AAFwk::AbilityState::ACTIVE, saveData), 0);
        WaitUntilTaskFinished();
        EXPECT_EQ(launcherAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::INACTIVATING);
        EXPECT_EQ(nextAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::INITIAL);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AttachAbilityThread
 * EnvConditions:NA
 * CaseDescription: launcher OnInactive done, verify new ability AttachAbilityThread timeout handler.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_startAbilityLifeCycle_003, TestSize.Level1)
{
    if (startLancherFlag_) {
        command_->callback_ = false;
        command_->expectState_ = OHOS::AAFwk::AbilityState::ACTIVE;
        command_->state_ = OHOS::AAFwk::AbilityState::INITIAL;
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        WaitUntilTaskFinished();
        EXPECT_TRUE(StartNextAbility());
        launcherAbilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVATING);
        PacMap saveData;
        EXPECT_EQ(abilityMs_->AbilityTransitionDone(launcherToken_, OHOS::AAFwk::AbilityState::INACTIVE, saveData), 0);
        // launcher oninactive done.
        pthread_t tid = 0;
        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret = LifecycleTest::SemTimedWaitMillis(
            AbilityManagerService::INACTIVE_TIMEOUT + DELAY_TEST_TIME, command_->sem_);
        EXPECT_NE(ret, 0);
        // check timeout handler
        EXPECT_EQ(nextAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVATING);
        pthread_join(tid, nullptr);

        WaitUntilTaskFinished();
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AbilityTransitionDone
 * EnvConditions:NA
 * CaseDescription: launcher OnInactive done, verify AbilityTransitionDone parameter.
 * AbilityTransitionDone fail if new ability
 * IAbilityScheduler is nullptr.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_startAbilityLifeCycle_004, TestSize.Level1)
{
    if (startLancherFlag_) {
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        EXPECT_TRUE(StartNextAbility());
        launcherAbilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVATING);
        PacMap saveData;
        EXPECT_EQ(abilityMs_->AbilityTransitionDone(launcherToken_, OHOS::AAFwk::AbilityState::INACTIVE, saveData), 0);
        // launcher oninactive done.
        nextAbilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INITIAL);
        EXPECT_EQ(AttachAbility(nextScheduler_, nextToken_), 0);
        EXPECT_NE(abilityMs_->AbilityTransitionDone(nullptr, OHOS::AAFwk::AbilityState::ACTIVE, saveData), 0);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AbilityTransitionDone
 * EnvConditions:NA
 * CaseDescription: launcher OnInactive done. verify AbilityTransitionDone parameter.
 * AbilityTransitionDone fail if new ability
 * schedules incorrect state_.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_startAbilityLifeCycle_005, TestSize.Level1)
{
    if (startLancherFlag_) {
        command_->callback_ = true;
        command_->expectState_ = OHOS::AAFwk::AbilityState::ACTIVE;
        command_->abnormalState_ = OHOS::AAFwk::AbilityState::INACTIVE;
        command_->state_ = OHOS::AAFwk::AbilityState::INITIAL;
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        EXPECT_TRUE(StartNextAbility());
        launcherAbilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVATING);
        PacMap saveData;
        EXPECT_EQ(abilityMs_->AbilityTransitionDone(launcherToken_, OHOS::AAFwk::AbilityState::INACTIVE, saveData), 0);
        WaitUntilTaskFinished();
        // launcher oninactive done.
        nextAbilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INITIAL);
        EXPECT_EQ(AttachAbility(nextScheduler_, nextToken_), 0);
        pthread_t tid = 0;
        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret =
            LifecycleTest::SemTimedWaitMillis(AbilityManagerService::LOAD_TIMEOUT + DELAY_TEST_TIME, command_->sem_);
        if (ret != 0) {
            // check timeout handler
            pthread_join(tid, nullptr);
            return;
        }
        pthread_join(tid, nullptr);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AbilityTransitionDone
 * EnvConditions:NA
 * CaseDescription: launcher OnInactive done. verify new ability AbilityTransitionDone timeout handler.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_startAbilityLifeCycle_006, TestSize.Level1)
{
    if (startLancherFlag_) {
        command_->callback_ = false;
        command_->expectState_ = OHOS::AAFwk::AbilityState::ACTIVE;
        command_->state_ = OHOS::AAFwk::AbilityState::INITIAL;
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        EXPECT_TRUE(StartNextAbility());
        launcherAbilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVATING);
        // launcher oninactive done.
        EXPECT_EQ(AttachAbility(nextScheduler_, nextToken_), 0);
        pthread_t tid = 0;
        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret =
            LifecycleTest::SemTimedWaitMillis(AbilityManagerService::ACTIVE_TIMEOUT + DELAY_TEST_TIME, command_->sem_);
        EXPECT_NE(ret, 0);
        pthread_join(tid, nullptr);
        return;
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AttachAbilityThread AbilityTransitionDone
 * EnvConditions:NA
 * CaseDescription: launcher OnInactive done and starts new ability success. verify new AbilityRecord.
 * 1. Launcher oninactive done and is INACTIVE.
 * 2. new ability is ACTIVE.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_startAbilityLifeCycle_007, TestSize.Level1)
{
    if (startLancherFlag_) {
        command_->callback_ = true;
        command_->expectState_ = OHOS::AAFwk::AbilityState::ACTIVE;
        command_->state_ = OHOS::AAFwk::AbilityState::INITIAL;
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        EXPECT_TRUE(StartNextAbility());
        launcherAbilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVATING);
        PacMap saveData;
        EXPECT_EQ(abilityMs_->AbilityTransitionDone(
            launcherToken_, OHOS::AAFwk::AbilityState::INACTIVE, saveData), OHOS::ERR_OK);
        // launcher oninactive done.
        WaitUntilTaskFinished();
        EXPECT_EQ(launcherAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::INACTIVE);
        EXPECT_EQ(AttachAbility(nextScheduler_, nextToken_), 0);
        pthread_t tid = 0;
        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret = LifecycleTest::SemTimedWaitMillis(AbilityManagerService::ACTIVE_TIMEOUT * 2, command_->sem_);
        if (ret != 0) {
            // check timeout handler
            pthread_join(tid, nullptr);
            return;
        }
        pthread_join(tid, nullptr);
    }
}
}  // namespace AAFwk
}  // namespace OHOS
