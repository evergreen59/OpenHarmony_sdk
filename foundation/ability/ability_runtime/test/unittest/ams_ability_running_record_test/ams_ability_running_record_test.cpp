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

#include <gtest/gtest.h>
#include "app_mgr_service_inner.h"
#include "ability_running_record.h"
#include "app_running_record.h"
#include "app_scheduler_host.h"
#include "hilog_wrapper.h"
#include "mock_ability_token.h"
#include "mock_application.h"

using namespace testing::ext;
using testing::_;
namespace OHOS {
namespace AppExecFwk {
class AmsAbilityRunningRecordTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    static const std::string GetTestAppName()
    {
        return "test_app_name";
    }
    static const std::string GetTestAbilityName()
    {
        return "test_ability_name";
    }
    static const std::string GetAnotherTestAbilityName()
    {
        return "another_test_ability_name";
    }
    static const std::string GetProcessName()
    {
        return "AmsAbilityRunningRecordTest";
    }

    std::shared_ptr<AppRunningRecord> GetTestAppRunningRecord();
    sptr<IAppScheduler> GetMockedAppSchedulerClient();

protected:
    sptr<IAppScheduler> client_;
    sptr<MockApplication> mockedAppClient_;
    std::shared_ptr<AppRunningRecord> testAppRecord_;
    std::shared_ptr<AppMgrServiceInner> serviceInner_ = nullptr;
    std::shared_ptr<AMSEventHandler> handler_ = nullptr;
};

void AmsAbilityRunningRecordTest::SetUpTestCase()
{}

void AmsAbilityRunningRecordTest::TearDownTestCase()
{}

void AmsAbilityRunningRecordTest::SetUp()
{
    mockedAppClient_ = new MockApplication();
    auto runner = EventRunner::Create("AmsAppLifeCycleModuleTest");
    serviceInner_ = std::make_shared<OHOS::AppExecFwk::AppMgrServiceInner>();
    handler_ = std::make_shared<AMSEventHandler>(runner, serviceInner_);
}

void AmsAbilityRunningRecordTest::TearDown()
{
    testAppRecord_.reset();
}

sptr<IAppScheduler> AmsAbilityRunningRecordTest::GetMockedAppSchedulerClient()
{
    if (!client_) {
        client_ = iface_cast<IAppScheduler>(mockedAppClient_.GetRefPtr());
    }
    return client_;
}

std::shared_ptr<AppRunningRecord> AmsAbilityRunningRecordTest::GetTestAppRunningRecord()
{
    if (!testAppRecord_) {
        auto appInfo = std::make_shared<ApplicationInfo>();
        appInfo->name = GetTestAppName();
        testAppRecord_ = std::make_shared<AppRunningRecord>(appInfo, AppRecordId::Create(), GetProcessName());
        testAppRecord_->SetApplicationClient(GetMockedAppSchedulerClient());
        testAppRecord_->SetEventHandler(handler_);
    }
    return testAppRecord_;
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Create AbilityRunningRecord using correct args.
 * EnvConditions: NA
 * CaseDescription: Verify the function AddAbility can create AbilityRunningRecord add add to AppRunningRecord.
 */
HWTEST_F(AmsAbilityRunningRecordTest, CreateAbilityRunningRecord_001, TestSize.Level1)
{
    HILOG_DEBUG("CreateAbilityRunningRecord_001 start.");
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    auto appRunningRecord = GetTestAppRunningRecord();
    sptr<IRemoteObject> token = new MockAbilityToken();
    HapModuleInfo hapModuleInfo;
    hapModuleInfo.moduleName = "module789";

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();

    appRunningRecord->AddModule(appInfo, abilityInfo, token, hapModuleInfo, nullptr);
    auto moduleRecord = appRunningRecord->GetModuleRecordByModuleName(appInfo->bundleName, hapModuleInfo.moduleName);
    EXPECT_TRUE(moduleRecord);
    auto abilityRunningRecord = moduleRecord->GetAbilityRunningRecordByToken(token);

    EXPECT_TRUE(abilityRunningRecord != nullptr);
    EXPECT_EQ(abilityRunningRecord, appRunningRecord->GetAbilityRunningRecordByToken(token));
    HILOG_DEBUG("CreateAbilityRunningRecord_001 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Create AbilityRunningRecord using null args.
 * EnvConditions: NA
 * CaseDescription: Verify the function AddAbility works but does not take effect using nullptr parameter.
 */
HWTEST_F(AmsAbilityRunningRecordTest, CreateAbilityRunningRecord_002, TestSize.Level1)
{
    HILOG_DEBUG("CreateAbilityRunningRecord_002 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    sptr<IRemoteObject> token = new MockAbilityToken();
    HapModuleInfo hapModuleInfo;
    hapModuleInfo.moduleName = "module789";

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();

    appRunningRecord->AddModule(appInfo, nullptr, token, hapModuleInfo, nullptr);
    auto moduleRecord = appRunningRecord->GetModuleRecordByModuleName(appInfo->bundleName, hapModuleInfo.moduleName);
    EXPECT_TRUE(moduleRecord);
    auto abilityRunningRecord = moduleRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_TRUE(abilityRunningRecord == nullptr);
    HILOG_DEBUG("CreateAbilityRunningRecord_002 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Create AbilityRunningRecord that already exists.
 * EnvConditions: NA
 * CaseDescription: Verify the function AddAbility does not take effect when abilityRunningRecord already exists.
 */
HWTEST_F(AmsAbilityRunningRecordTest, CreateAbilityRunningRecord_003, TestSize.Level1)
{
    HILOG_DEBUG("CreateAbilityRunningRecord_003 start.");
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    auto appRunningRecord = GetTestAppRunningRecord();
    sptr<IRemoteObject> token = new MockAbilityToken();

    HapModuleInfo hapModuleInfo;
    hapModuleInfo.moduleName = "module789";

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();

    appRunningRecord->AddModule(appInfo, abilityInfo, token, hapModuleInfo, nullptr);
    auto moduleRecord = appRunningRecord->GetModuleRecordByModuleName(appInfo->bundleName, hapModuleInfo.moduleName);
    EXPECT_TRUE(moduleRecord);
    auto abilityRunningRecordFirst = moduleRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_TRUE(abilityRunningRecordFirst != nullptr);
    EXPECT_EQ(abilityRunningRecordFirst, appRunningRecord->GetAbilityRunningRecordByToken(token));

    appRunningRecord->AddModule(appInfo, abilityInfo, token, hapModuleInfo, nullptr);
    moduleRecord = appRunningRecord->GetModuleRecordByModuleName(appInfo->bundleName, hapModuleInfo.moduleName);
    EXPECT_TRUE(moduleRecord);
    auto abilityRunningRecordSecond = moduleRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_TRUE(abilityRunningRecordSecond == abilityRunningRecordFirst);
    HILOG_DEBUG("CreateAbilityRunningRecord_003 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Update the state of AbilityRunningRecord using correct args.
 * EnvConditions: NA
 * CaseDescription: Verify the function UpdateAbilityState can update the state of AbilityRunningRecord correctly.
 */
HWTEST_F(AmsAbilityRunningRecordTest, UpdateAbilityRunningRecord_001, TestSize.Level1)
{
    HILOG_DEBUG("UpdateAbilityRunningRecord_001 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();
    HapModuleInfo hapModuleInfo;
    hapModuleInfo.moduleName = "module789";

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();

    appRunningRecord->AddModule(appInfo, abilityInfo, token, hapModuleInfo, nullptr);
    auto moduleRecord = appRunningRecord->GetModuleRecordByModuleName(appInfo->bundleName, hapModuleInfo.moduleName);
    EXPECT_TRUE(moduleRecord);
    auto abilityRunningRecord = moduleRecord->GetAbilityRunningRecordByToken(token);

    EXPECT_TRUE(abilityRunningRecord != nullptr);
    abilityRunningRecord->SetState(AbilityState::ABILITY_STATE_READY);
    appRunningRecord->SetState(ApplicationState::APP_STATE_READY);

    EXPECT_CALL(*mockedAppClient_, ScheduleForegroundApplication()).Times(1);
    appRunningRecord->UpdateAbilityState(token, AbilityState::ABILITY_STATE_FOREGROUND);
    appRunningRecord->PopForegroundingAbilityTokens();
    EXPECT_EQ(abilityRunningRecord->GetState(), AbilityState::ABILITY_STATE_FOREGROUND) << "execute fail!";

    appRunningRecord->SetState(ApplicationState::APP_STATE_FOREGROUND);
    EXPECT_CALL(*mockedAppClient_, ScheduleBackgroundApplication()).Times(1);
    appRunningRecord->UpdateAbilityState(token, AbilityState::ABILITY_STATE_BACKGROUND);
    EXPECT_EQ(abilityRunningRecord->GetState(), AbilityState::ABILITY_STATE_BACKGROUND) << "execute fail!";
    HILOG_DEBUG("UpdateAbilityRunningRecord_001 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Update the state of AbilityRunningRecord using incorrect args.
 * EnvConditions: NA
 * CaseDescription: Verify the function UpdateAbilityState works but does not take effect using incorrect value.
 */
HWTEST_F(AmsAbilityRunningRecordTest, UpdateAbilityRunningRecord_002, TestSize.Level1)
{
    HILOG_DEBUG("UpdateAbilityRunningRecord_002 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();

    HapModuleInfo hapModuleInfo;
    hapModuleInfo.moduleName = "module789";

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();

    appRunningRecord->AddModule(appInfo, abilityInfo, token, hapModuleInfo, nullptr);
    auto moduleRecord = appRunningRecord->GetModuleRecordByModuleName(appInfo->bundleName, hapModuleInfo.moduleName);
    EXPECT_TRUE(moduleRecord);
    auto abilityRunningRecord = moduleRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_TRUE(abilityRunningRecord != nullptr);

    AbilityState state = abilityRunningRecord->GetState();
    appRunningRecord->UpdateAbilityState(token, AbilityState::ABILITY_STATE_END);
    EXPECT_EQ(abilityRunningRecord->GetState(), state);
    EXPECT_NE(abilityRunningRecord->GetState(), AbilityState::ABILITY_STATE_END);
    HILOG_DEBUG("UpdateAbilityRunningRecord_002 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Update the state of AbilityRunningRecord using nullptr.
 * EnvConditions: NA
 * CaseDescription: Verify the function UpdateAbilityState works but does not take effect using nullptr parameter.
 */
HWTEST_F(AmsAbilityRunningRecordTest, UpdateAbilityRunningRecord_003, TestSize.Level1)
{
    HILOG_DEBUG("UpdateAbilityRunningRecord_003 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();
    HapModuleInfo hapModuleInfo;
    hapModuleInfo.moduleName = "module789";

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();

    appRunningRecord->AddModule(appInfo, abilityInfo, token, hapModuleInfo, nullptr);
    auto moduleRecord = appRunningRecord->GetModuleRecordByModuleName(appInfo->bundleName, hapModuleInfo.moduleName);
    EXPECT_TRUE(moduleRecord);
    auto abilityRunningRecord = moduleRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_TRUE(abilityRunningRecord != nullptr);

    AbilityState state = abilityRunningRecord->GetState();

    appRunningRecord->UpdateAbilityState(nullptr, AbilityState::ABILITY_STATE_FOREGROUND);
    EXPECT_EQ(abilityRunningRecord->GetState(), state);
    HILOG_DEBUG("UpdateAbilityRunningRecord_003 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Update the state of AbilityRunningRecord that does not exist.
 * EnvConditions: NA
 * CaseDescription: Verify the function UpdateAbilityState cannot change the state of AbilityRunningRecord
 *                  that does not exist.
 */
HWTEST_F(AmsAbilityRunningRecordTest, UpdateAbilityRunningRecord_004, TestSize.Level1)
{
    HILOG_DEBUG("UpdateAbilityRunningRecord_004 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();

    auto anotherAbilityInfo = std::make_shared<AbilityInfo>();
    anotherAbilityInfo->name = GetAnotherTestAbilityName();
    sptr<IRemoteObject> token2 = new MockAbilityToken();

    HapModuleInfo hapModuleInfo;
    hapModuleInfo.moduleName = "module789";

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();

    appRunningRecord->AddModule(appInfo, abilityInfo, token, hapModuleInfo, nullptr);
    auto moduleRecord = appRunningRecord->GetModuleRecordByModuleName(appInfo->bundleName, hapModuleInfo.moduleName);
    EXPECT_TRUE(moduleRecord);
    auto abilityRunningRecord = moduleRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_TRUE(abilityRunningRecord != nullptr);

    AbilityState state = abilityRunningRecord->GetState();
    EXPECT_TRUE(appRunningRecord->GetAbilityRunningRecordByToken(token2) == nullptr);
    appRunningRecord->UpdateAbilityState(token2, AbilityState::ABILITY_STATE_FOREGROUND);
    EXPECT_EQ(abilityRunningRecord->GetState(), state);
    HILOG_DEBUG("UpdateAbilityRunningRecord_004 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Update one state of AbilityRunningRecords as foreground.
 * EnvConditions: NA
 * CaseDescription: Verify if there is at least one state of AbilityRunningRecords is foreground,
 *                  the state of application should be changed to foreground.
 */
HWTEST_F(AmsAbilityRunningRecordTest, UpdateAbilityRunningRecord_005, TestSize.Level1)
{
    HILOG_DEBUG("UpdateAbilityRunningRecord_005 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    auto anotherAbilityInfo = std::make_shared<AbilityInfo>();
    anotherAbilityInfo->name = GetAnotherTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();
    sptr<IRemoteObject> anotherToken = new MockAbilityToken();

    HapModuleInfo hapModuleInfo;
    hapModuleInfo.moduleName = "module789";

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();

    appRunningRecord->AddModule(appInfo, abilityInfo, token, hapModuleInfo, nullptr);
    auto moduleRecord = appRunningRecord->GetModuleRecordByModuleName(appInfo->bundleName, hapModuleInfo.moduleName);
    EXPECT_TRUE(moduleRecord);
    auto abilityRunningRecord = moduleRecord->GetAbilityRunningRecordByToken(token);

    appRunningRecord->AddModule(appInfo, anotherAbilityInfo, anotherToken, hapModuleInfo, nullptr);
    moduleRecord = appRunningRecord->GetModuleRecordByModuleName(appInfo->bundleName, hapModuleInfo.moduleName);
    EXPECT_TRUE(moduleRecord);
    auto anotherAbilityRunningRecord = moduleRecord->GetAbilityRunningRecordByToken(anotherToken);

    EXPECT_TRUE(abilityRunningRecord != nullptr);
    EXPECT_TRUE(anotherAbilityRunningRecord != nullptr);
    anotherAbilityRunningRecord->SetState(AbilityState::ABILITY_STATE_BACKGROUND);
    appRunningRecord->SetState(ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*mockedAppClient_, ScheduleForegroundApplication()).Times(1);
    appRunningRecord->UpdateAbilityState(anotherToken, AbilityState::ABILITY_STATE_FOREGROUND);
    HILOG_DEBUG("UpdateAbilityRunningRecord_005 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Update all states of AbilityRunningRecords as background.
 * EnvConditions: NA
 * CaseDescription: Verify if all states of AbilityRunningRecords are background, the state of application should be
 *                  changed to background.
 */
HWTEST_F(AmsAbilityRunningRecordTest, UpdateAbilityRunningRecord_006, TestSize.Level1)
{
    HILOG_DEBUG("UpdateAbilityRunningRecord_006 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    auto anotherAbilityInfo = std::make_shared<AbilityInfo>();
    anotherAbilityInfo->name = GetAnotherTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();
    sptr<IRemoteObject> anotherToken = new MockAbilityToken();
    HapModuleInfo hapModuleInfo;
    hapModuleInfo.moduleName = "module789";

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();

    appRunningRecord->AddModule(appInfo, abilityInfo, token, hapModuleInfo, nullptr);
    auto moduleRecord = appRunningRecord->GetModuleRecordByModuleName(appInfo->bundleName, hapModuleInfo.moduleName);
    EXPECT_TRUE(moduleRecord);
    auto abilityRunningRecord = moduleRecord->GetAbilityRunningRecordByToken(token);

    appRunningRecord->AddModule(appInfo, anotherAbilityInfo, anotherToken, hapModuleInfo, nullptr);
    moduleRecord = appRunningRecord->GetModuleRecordByModuleName(appInfo->bundleName, hapModuleInfo.moduleName);
    EXPECT_TRUE(moduleRecord);
    auto anotherAbilityRunningRecord = moduleRecord->GetAbilityRunningRecordByToken(anotherToken);
    EXPECT_TRUE(abilityRunningRecord != nullptr);
    EXPECT_TRUE(anotherAbilityRunningRecord != nullptr);
    anotherAbilityRunningRecord->SetState(AbilityState::ABILITY_STATE_FOREGROUND);
    appRunningRecord->SetState(ApplicationState::APP_STATE_FOREGROUND);
    abilityRunningRecord->SetState(AbilityState::ABILITY_STATE_FOREGROUND);

    EXPECT_CALL(*mockedAppClient_, ScheduleBackgroundApplication()).Times(2);
    appRunningRecord->UpdateAbilityState(anotherToken, AbilityState::ABILITY_STATE_FOREGROUND);

    auto abilities = appRunningRecord->GetAbilities();
    for (auto iter = abilities.begin(); iter != abilities.end(); iter++) {
        appRunningRecord->UpdateAbilityState(iter->second->GetToken(), AbilityState::ABILITY_STATE_BACKGROUND);
    }
    HILOG_DEBUG("UpdateAbilityRunningRecord_006 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Update all states of AbilityRunningRecords as terminate.
 * EnvConditions: NA
 * CaseDescription: Verify if all states of AbilityRunningRecords are terminate, the state of application should be
 *                  changed to terminate.
 */
HWTEST_F(AmsAbilityRunningRecordTest, UpdateAbilityRunningRecord_007, TestSize.Level1)
{
    HILOG_DEBUG("UpdateAbilityRunningRecord_007 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    auto anotherAbilityInfo = std::make_shared<AbilityInfo>();
    anotherAbilityInfo->name = GetAnotherTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();
    sptr<IRemoteObject> anotherToken = new MockAbilityToken();
    HapModuleInfo hapModuleInfo;
    hapModuleInfo.moduleName = "module789";

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();

    appRunningRecord->AddModule(appInfo, abilityInfo, token, hapModuleInfo, nullptr);
    auto moduleRecord = appRunningRecord->GetModuleRecordByModuleName(appInfo->bundleName, hapModuleInfo.moduleName);
    EXPECT_TRUE(moduleRecord);
    auto abilityRunningRecord = moduleRecord->GetAbilityRunningRecordByToken(token);

    appRunningRecord->AddModule(appInfo, anotherAbilityInfo, anotherToken, hapModuleInfo, nullptr);
    moduleRecord = appRunningRecord->GetModuleRecordByModuleName(appInfo->bundleName, hapModuleInfo.moduleName);
    EXPECT_TRUE(moduleRecord);
    auto anotherAbilityRunningRecord = moduleRecord->GetAbilityRunningRecordByToken(anotherToken);
    EXPECT_TRUE(abilityRunningRecord != nullptr);
    EXPECT_TRUE(anotherAbilityRunningRecord != nullptr);
    anotherAbilityRunningRecord->SetState(AbilityState::ABILITY_STATE_BACKGROUND);
    appRunningRecord->SetState(ApplicationState::APP_STATE_BACKGROUND);
    abilityRunningRecord->SetState(AbilityState::ABILITY_STATE_BACKGROUND);

    EXPECT_CALL(*mockedAppClient_, ScheduleTerminateApplication()).Times(1);
    EXPECT_CALL(*mockedAppClient_, ScheduleCleanAbility(_)).Times(2);
    auto abilities = appRunningRecord->GetAbilities();
    for (auto iter = abilities.begin(); iter != abilities.end(); iter++) {
        appRunningRecord->TerminateAbility(iter->second->GetToken(), false);
        appRunningRecord->AbilityTerminated(iter->second->GetToken());
    }
    HILOG_DEBUG("UpdateAbilityRunningRecord_007 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: IsSameState
 * FunctionPoints: Check state is same or different.
 * EnvConditions: NA
 * CaseDescription: Verify the function IsSameState judge the exact state value.
 */
HWTEST_F(AmsAbilityRunningRecordTest, IsSameState_001, TestSize.Level1)
{
    HILOG_DEBUG("IsSameState_001 start.");

    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();
    std::shared_ptr<AbilityRunningRecord> abilityRunningRecord =
        std::make_shared<AbilityRunningRecord>(abilityInfo, token);

    abilityRunningRecord->SetState(AbilityState::ABILITY_STATE_FOREGROUND);
    EXPECT_EQ(false, abilityRunningRecord->IsSameState(AbilityState::ABILITY_STATE_BACKGROUND));
    EXPECT_EQ(true, abilityRunningRecord->IsSameState(AbilityState::ABILITY_STATE_FOREGROUND));

    HILOG_DEBUG("IsSameState_001 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Create AbilityRunningRecord using correct args.
 * EnvConditions: NA
 * CaseDescription: Verify the function AddAbility can create AbilityRunningRecord add add to AppRunningRecord.
 */
HWTEST_F(AmsAbilityRunningRecordTest, SetGetAbilityRecord_001, TestSize.Level1)
{
    HILOG_DEBUG("SetGetAbilityRecord_001 start.");

    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    auto appRunningRecord = GetTestAppRunningRecord();
    sptr<IRemoteObject> token = new MockAbilityToken();
    HapModuleInfo hapModuleInfo;
    hapModuleInfo.moduleName = "module789";

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();

    appRunningRecord->AddModule(appInfo, abilityInfo, token, hapModuleInfo, nullptr);
    auto moduleRecord = appRunningRecord->GetModuleRecordByModuleName(appInfo->bundleName, hapModuleInfo.moduleName);
    EXPECT_TRUE(moduleRecord);
    auto abilityRunningRecord = moduleRecord->GetAbilityRunningRecordByToken(token);

    EXPECT_TRUE(abilityRunningRecord != nullptr);
    abilityRunningRecord->SetVisibility(1);
    abilityRunningRecord->SetPerceptibility(1);
    abilityRunningRecord->SetConnectionState(1);

    EXPECT_EQ(abilityRunningRecord, appRunningRecord->GetAbilityRunningRecordByToken(token));

    auto testRecord = appRunningRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_EQ(1, testRecord->GetVisibility());
    EXPECT_EQ(1, testRecord->GetPerceptibility());
    EXPECT_EQ(1, testRecord->GetConnectionState());

    HILOG_DEBUG("SetGetAbilityRecord_001 end.");
}
}  // namespace AppExecFwk
}  // namespace OHOS
