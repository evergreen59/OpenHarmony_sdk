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
#include "app_scheduler_proxy.h"
#include "app_scheduler_host.h"
#include "hilog_wrapper.h"
#include "mock_ability_token.h"
#include "mock_application.h"

using namespace testing::ext;
using OHOS::iface_cast;
using OHOS::sptr;
using testing::_;
using testing::Invoke;
using testing::InvokeWithoutArgs;
namespace OHOS {
namespace AppExecFwk {
class AmsIpcAppSchedulerInterfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    sptr<MockAbilityToken> GetMockToken() const
    {
        return mock_token_;
    }

private:
    sptr<MockAbilityToken> mock_token_;
};

void AmsIpcAppSchedulerInterfaceTest::SetUpTestCase()
{}

void AmsIpcAppSchedulerInterfaceTest::TearDownTestCase()
{}

void AmsIpcAppSchedulerInterfaceTest::SetUp()
{
    mock_token_ = new (std::nothrow) MockAbilityToken();
}

void AmsIpcAppSchedulerInterfaceTest::TearDown()
{}

/*
 * Feature: AppScheduler ZIDL interface
 * Function: ScheduleForegroundApplication
 * SubFunction: NA
 * FunctionPoints: ScheduleForegroundApplication interface
 * EnvConditions: Application already running
 * CaseDescription: Test the interface ScheduleForegroundApplication of AppScheduler
 */
HWTEST_F(AmsIpcAppSchedulerInterfaceTest, Interface_001, TestSize.Level1)
{
    HILOG_DEBUG("AppSchedulerInterfaceTest_001 start");
    sptr<MockApplication> mockApplication(new MockApplication());
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);

    EXPECT_CALL(*mockApplication, ScheduleForegroundApplication())
        .Times(1)
        .WillOnce(InvokeWithoutArgs(mockApplication.GetRefPtr(), &MockApplication::Post));
    client->ScheduleForegroundApplication();
    mockApplication->Wait();
    HILOG_DEBUG("AppSchedulerInterfaceTest_001 end");
}

/*
 * Feature: AppScheduler ZIDL interface
 * Function: ScheduleBackgroundApplication
 * SubFunction: NA
 * FunctionPoints: scheduleBackgroundApplication interface
 * EnvConditions: Application already running
 * CaseDescription: Test the interface ScheduleBackgroundApplication of AppScheduler
 */
HWTEST_F(AmsIpcAppSchedulerInterfaceTest, Interface_002, TestSize.Level1)
{
    HILOG_DEBUG("AppSchedulerInterfaceTest_002 start");
    sptr<MockApplication> mockApplication(new MockApplication());
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);

    EXPECT_CALL(*mockApplication, ScheduleBackgroundApplication())
        .Times(1)
        .WillOnce(InvokeWithoutArgs(mockApplication.GetRefPtr(), &MockApplication::Post));
    client->ScheduleBackgroundApplication();
    mockApplication->Wait();
    HILOG_DEBUG("AppSchedulerInterfaceTest_002 end");
}

/*
 * Feature: AppScheduler ZIDL interface
 * Function: ScheduleTerminateApplication
 * SubFunction: NA
 * FunctionPoints: scheduleTerminateApplication interface
 * EnvConditions: Application already running
 * CaseDescription: Test the interface ScheduleTerminateApplication of AppScheduler
 */
HWTEST_F(AmsIpcAppSchedulerInterfaceTest, Interface_003, TestSize.Level1)
{
    HILOG_DEBUG("AppSchedulerInterfaceTest_003 start");
    sptr<MockApplication> mockApplication(new MockApplication());
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);

    EXPECT_CALL(*mockApplication, ScheduleTerminateApplication())
        .Times(1)
        .WillOnce(InvokeWithoutArgs(mockApplication.GetRefPtr(), &MockApplication::Post));
    client->ScheduleTerminateApplication();
    mockApplication->Wait();
    HILOG_DEBUG("AppSchedulerInterfaceTest_003 end");
}

/*
 * Feature: AppScheduler ZIDL interface
 * Function: ScheduleShrinkMemory
 * SubFunction: NA
 * FunctionPoints: scheduleShrinkMemory interface
 * EnvConditions: Application already running
 * CaseDescription: Test the interface ScheduleShrinkMemory of AppScheduler
 */
HWTEST_F(AmsIpcAppSchedulerInterfaceTest, Interface_004, TestSize.Level1)
{
    HILOG_DEBUG("AppSchedulerInterfaceTest_004 start");
    sptr<MockApplication> mockApplication(new MockApplication());
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);
    int level = 1;

    EXPECT_CALL(*mockApplication, ScheduleShrinkMemory(_))
        .Times(1)
        .WillOnce(Invoke(mockApplication.GetRefPtr(), &MockApplication::ShrinkMemory));
    client->ScheduleShrinkMemory(level);
    mockApplication->Wait();

    int shrinkLevel = mockApplication->GetShrinkLevel();
    EXPECT_EQ(level, shrinkLevel);
    HILOG_DEBUG("AppSchedulerInterfaceTest_004 end");
}

/*
 * Feature: AppScheduler ZIDL interface
 * Function: ScheduleLowMemory
 * SubFunction: NA
 * FunctionPoints: scheduleLowMemory interface
 * EnvConditions: Application already running
 * CaseDescription: Test the interface ScheduleLowMemory of AppScheduler
 */
HWTEST_F(AmsIpcAppSchedulerInterfaceTest, Interface_005, TestSize.Level1)
{
    HILOG_DEBUG("AppSchedulerInterfaceTest_005 start");
    sptr<MockApplication> mockApplication(new MockApplication());
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);

    EXPECT_CALL(*mockApplication, ScheduleLowMemory())
        .Times(1)
        .WillOnce(InvokeWithoutArgs(mockApplication.GetRefPtr(), &MockApplication::Post));
    client->ScheduleLowMemory();
    mockApplication->Wait();
    HILOG_DEBUG("AppSchedulerInterfaceTest_005 end");
}

/*
 * Feature: AppScheduler ZIDL interface
 * Function: ScheduleLaunchApplication
 * SubFunction: NA
 * FunctionPoints: scheduleLaunchApplication interface
 * EnvConditions: Application already running
 * CaseDescription: Test the interface ScheduleLaunchApplication of AppScheduler
 */
HWTEST_F(AmsIpcAppSchedulerInterfaceTest, Interface_006, TestSize.Level1)
{
    HILOG_DEBUG("AppSchedulerInterfaceTest_006 start");
    sptr<MockApplication> mockApplication(new MockApplication());
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);

    std::string applicationName("mockApplicationInfo");
    ApplicationInfo applicationInfo;
    applicationInfo.name = applicationName;
    std::string profileName("mockProfile");
    Profile profile(profileName);
    std::string processName("mockProcessInfo");
    ProcessInfo processInfo(processName, 1);

    AppLaunchData launchData;
    launchData.SetApplicationInfo(applicationInfo);
    launchData.SetProfile(profile);
    launchData.SetProcessInfo(processInfo);

    Configuration config;
    EXPECT_CALL(*mockApplication, ScheduleLaunchApplication(_, _))
        .Times(1)
        .WillOnce(Invoke(mockApplication.GetRefPtr(), &MockApplication::LaunchApplication));
    client->ScheduleLaunchApplication(launchData, config);
    mockApplication->Wait();

    bool isEqual = mockApplication->CompareAppLaunchData(launchData);
    EXPECT_EQ(true, isEqual);
    HILOG_DEBUG("AppSchedulerInterfaceTest_006 end");
}

/*
 * Feature: AppScheduler ZIDL interface
 * Function: ScheduleCleanAbility
 * SubFunction: NA
 * FunctionPoints: scheduleCleanAbility interface
 * EnvConditions: Application already running
 * CaseDescription: Test the interface ScheduleCleanAbility of AppScheduler
 */
HWTEST_F(AmsIpcAppSchedulerInterfaceTest, Interface_008, TestSize.Level1)
{
    HILOG_DEBUG("AppSchedulerInterfaceTest_008 start");
    sptr<MockApplication> mockApplication(new MockApplication());
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);

    EXPECT_CALL(*mockApplication, ScheduleCleanAbility(_))
        .Times(1)
        .WillOnce(InvokeWithoutArgs(mockApplication.GetRefPtr(), &MockApplication::Post));
    client->ScheduleCleanAbility(GetMockToken());
    mockApplication->Wait();
    HILOG_DEBUG("AppSchedulerInterfaceTest_008 end");
}

/*
 * Feature: AppScheduler ZIDL interface
 * Function: ScheduleCleanAbility
 * SubFunction: NA
 * FunctionPoints: scheduleProfileChanged interface
 * EnvConditions: Application already running
 * CaseDescription: Test the interface ScheduleProfileChanged of AppScheduler
 */
HWTEST_F(AmsIpcAppSchedulerInterfaceTest, Interface_009, TestSize.Level1)
{
    HILOG_DEBUG("AppSchedulerInterfaceTest_009 start");
    sptr<MockApplication> mockApplication(new MockApplication());
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);
    std::string profileName("mockProfile");
    Profile profile(profileName);

    EXPECT_CALL(*mockApplication, ScheduleProfileChanged(_))
        .Times(1)
        .WillOnce(Invoke(mockApplication.GetRefPtr(), &MockApplication::ProfileChanged));
    client->ScheduleProfileChanged(profile);
    mockApplication->Wait();

    bool isEqual = mockApplication->CompareProfile(profile);
    EXPECT_EQ(true, isEqual);
    HILOG_DEBUG("AppSchedulerInterfaceTest_009 end");
}
}  // namespace AppExecFwk
}  // namespace OHOS
