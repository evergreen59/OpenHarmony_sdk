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

#define private public
#include "app_mgr_service_inner.h"
#undef private

#include <unistd.h>
#include <gtest/gtest.h>

#include "refbase.h"
#include "hilog_wrapper.h"
#include "iremote_object.h"
#include "mock_bundle_manager.h"
#include "mock_ability_token.h"
#include "mock_app_scheduler.h"
#include "mock_app_spawn_client.h"

using namespace testing::ext;
using testing::_;
using testing::Return;
using testing::SetArgReferee;
using ::testing::DoAll;

namespace OHOS {
namespace AppExecFwk {
namespace {
const int32_t INDEX_NUM_1 = 1;
const int32_t INDEX_NUM_2 = 2;
const int32_t INDEX_NUM_3 = 3;
const int32_t INDEX_NUM_10 = 10;
const std::string TEST_APP_NAME = "com.ohos.test.helloworld";
const std::string TEST_ABILITY_NAME = "test_ability_";
}  // namespace

class AmsAppRecentListModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    const std::shared_ptr<ApplicationInfo> GetApplicationByIndex(const int32_t index) const;
    const std::shared_ptr<AppRunningRecord> CreateAppRunningRecordByIndex(const int32_t index) const;
    void CreateAppRecentList(const int32_t appNum);

    std::shared_ptr<AppMgrServiceInner> serviceInner_ {nullptr};
    sptr<MockAbilityToken> mockToken_ {nullptr};
    sptr<BundleMgrService> mockBundleMgr {nullptr};
};

void AmsAppRecentListModuleTest::SetUpTestCase()
{}

void AmsAppRecentListModuleTest::TearDownTestCase()
{}

void AmsAppRecentListModuleTest::SetUp()
{
    serviceInner_.reset(new (std::nothrow) AppMgrServiceInner());
    serviceInner_->Init();
    mockBundleMgr = new (std::nothrow) BundleMgrService();
    serviceInner_->SetBundleManager(mockBundleMgr);
}

void AmsAppRecentListModuleTest::TearDown()
{}

const std::shared_ptr<ApplicationInfo> AmsAppRecentListModuleTest::GetApplicationByIndex(const int32_t index) const
{
    std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = TEST_APP_NAME + std::to_string(index);
    return appInfo;
}

const std::shared_ptr<AppRunningRecord> AmsAppRecentListModuleTest::CreateAppRunningRecordByIndex(
    const int32_t index) const
{
    std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    appInfo->name = TEST_APP_NAME + std::to_string(index);
    abilityInfo->name = TEST_ABILITY_NAME + std::to_string(index);
    abilityInfo->applicationName = appInfo->name;
    abilityInfo->applicationInfo.bundleName = appInfo->name;
    abilityInfo->process =  appInfo->name;
    BundleInfo bundleInfo;
    HapModuleInfo hapModuleInfo;
    EXPECT_TRUE(serviceInner_->GetBundleAndHapInfo(*abilityInfo, appInfo, bundleInfo, hapModuleInfo));
    auto appRunningRecord = serviceInner_->appRunningManager_->CheckAppRunningRecordIsExist(
        appInfo->name, abilityInfo->process, abilityInfo->applicationInfo.uid, bundleInfo);
    EXPECT_NE(nullptr, appRunningRecord);
    return appRunningRecord;
}

void AmsAppRecentListModuleTest::CreateAppRecentList(const int32_t appNum)
{
    for (int32_t i = 0; i < appNum; i++) {
        std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        appInfo->name = TEST_APP_NAME + std::to_string(i);
        appInfo->bundleName = appInfo->name;
        appInfo->process = appInfo->name;
        abilityInfo->name = TEST_ABILITY_NAME + std::to_string(i);
        abilityInfo->applicationName = appInfo->name;
        abilityInfo->applicationInfo.bundleName = appInfo->name;
        abilityInfo->process =  appInfo->name;
        pid_t pid = i;
        sptr<IRemoteObject> token = new (std::nothrow) MockAbilityToken();
        MockAppSpawnClient *mockedSpawnClient = new MockAppSpawnClient();

        EXPECT_CALL(*mockedSpawnClient, StartProcess(_, _))
            .Times(1)
            .WillOnce(DoAll(SetArgReferee<1>(pid), Return(ERR_OK)));

        serviceInner_->SetAppSpawnClient(std::unique_ptr<MockAppSpawnClient>(mockedSpawnClient));
        serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo, nullptr);
    }
    return;
}

/*
 * Feature: Ams
 * Function: AppRecentList
 * SubFunction: create
 * FunctionPoints: Add app to RecentAppList when start a same process failed.
 * EnvConditions: AppRecentList is empty.
 * CaseDescription: Verity ams add app to AppRecentList failed when start a same process.
 */
HWTEST_F(AmsAppRecentListModuleTest, Create_Recent_List_001, TestSize.Level1)
{
    HILOG_INFO("Create_Recent_List_001 start");
    EXPECT_TRUE(serviceInner_->GetRecentAppList().empty());
    CreateAppRecentList(INDEX_NUM_10);
    EXPECT_EQ(INDEX_NUM_10, static_cast<int32_t>(serviceInner_->GetRecentAppList().size()));

    // Load ability_2 again, fail to add.
    std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    appInfo->name = TEST_APP_NAME + std::to_string(INDEX_NUM_2);
    abilityInfo->name = TEST_ABILITY_NAME + std::to_string(INDEX_NUM_2);
    abilityInfo->applicationName = TEST_APP_NAME + std::to_string(INDEX_NUM_2);
    abilityInfo->applicationInfo.bundleName = appInfo->name;
    abilityInfo->process =  appInfo->name;

    sptr<IRemoteObject> token = new MockAbilityToken();
    MockAppSpawnClient *mockedSpawnClient = new MockAppSpawnClient();

    serviceInner_->SetAppSpawnClient(std::unique_ptr<MockAppSpawnClient>(mockedSpawnClient));
    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo, nullptr);
    EXPECT_EQ(INDEX_NUM_10, static_cast<int32_t>(serviceInner_->GetRecentAppList().size()));
    HILOG_INFO("Create_Recent_List_001 end");
}

/*
 * Feature: Ams
 * Function: AppRecentList
 * SubFunction: create
 * FunctionPoints: Add app to RecentAppList when start a new process success.
 * EnvConditions: AppRecentList is empty.
 * CaseDescription: Verity ams can add app to AppRecentList success when start a new process success.
 */
HWTEST_F(AmsAppRecentListModuleTest, Create_Recent_List_002, TestSize.Level1)
{
    HILOG_INFO("Create_Recent_List_002 start");
    EXPECT_TRUE(serviceInner_->GetRecentAppList().empty());
    CreateAppRecentList(INDEX_NUM_10);
    EXPECT_EQ(INDEX_NUM_10, static_cast<int32_t>(serviceInner_->GetRecentAppList().size()));

    // Load ability_11 , add successful.
    std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    appInfo->name = TEST_APP_NAME + std::to_string(INDEX_NUM_10 + INDEX_NUM_1);
    appInfo->bundleName = appInfo->name;
    abilityInfo->name = TEST_ABILITY_NAME + std::to_string(INDEX_NUM_10 + INDEX_NUM_1);
    abilityInfo->applicationName = TEST_APP_NAME + std::to_string(INDEX_NUM_10 + INDEX_NUM_1);
    abilityInfo->applicationInfo.bundleName = appInfo->name;
    abilityInfo->process =  appInfo->name;
    pid_t pid = static_cast<int32_t>(INDEX_NUM_10 + INDEX_NUM_1);
    sptr<IRemoteObject> token = new MockAbilityToken();
    MockAppSpawnClient *mockedSpawnClient = new MockAppSpawnClient();
    EXPECT_CALL(*mockedSpawnClient, StartProcess(_, _)).Times(1).WillOnce(DoAll(SetArgReferee<1>(pid), Return(ERR_OK)));

    serviceInner_->SetAppSpawnClient(std::unique_ptr<MockAppSpawnClient>(mockedSpawnClient));
    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo, nullptr);
    EXPECT_EQ(INDEX_NUM_10 + INDEX_NUM_1, static_cast<int32_t>(serviceInner_->GetRecentAppList().size()));
    HILOG_INFO("Create_Recent_List_002 end");
}

/*
 * Feature: Ams
 * Function: AppRecentList
 * SubFunction: update
 * FunctionPoints: Remove app from AppRecentList when app died.
 * EnvConditions: AppRecentList has application.
 * CaseDescription: Verity ams can remove app from AppRecentList when app died.
 */
HWTEST_F(AmsAppRecentListModuleTest, Update_Recent_List_002, TestSize.Level1)
{
    HILOG_INFO("Update_Recent_List_002 start");
    EXPECT_TRUE(serviceInner_->GetRecentAppList().empty());
    CreateAppRecentList(INDEX_NUM_10);
    EXPECT_EQ(INDEX_NUM_10, static_cast<int32_t>(serviceInner_->GetRecentAppList().size()));

    auto appRecord = CreateAppRunningRecordByIndex(INDEX_NUM_10 - INDEX_NUM_3);
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    appRecord->SetApplicationClient(client);
    sptr<IRemoteObject> object = client->AsObject();
    wptr<IRemoteObject> app = object;
    serviceInner_->OnRemoteDied(app);
    EXPECT_EQ(INDEX_NUM_10 - INDEX_NUM_1, static_cast<int32_t>(serviceInner_->GetRecentAppList().size()));
    HILOG_INFO("Update_Recent_List_002 end");
}

/*
 * Feature: Ams
 * Function: AppRecentList
 * SubFunction: remove
 * FunctionPoints: Remove app from AppRecentList.
 * EnvConditions: AppRecentList has application.
 * CaseDescription: Verity ams can remove app from AppRecentList when call remove app from RecentList.
 */
HWTEST_F(AmsAppRecentListModuleTest, Remove_Recent_List_001, TestSize.Level1)
{
    HILOG_INFO("Remove_Recent_List_001 start");
    EXPECT_TRUE(serviceInner_->GetRecentAppList().empty());
    CreateAppRecentList(INDEX_NUM_10);
    EXPECT_EQ(INDEX_NUM_10, static_cast<int32_t>(serviceInner_->GetRecentAppList().size()));

    std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = TEST_APP_NAME + std::to_string(INDEX_NUM_10 - INDEX_NUM_1);
    appInfo->bundleName = appInfo->name;

    auto appRecord = CreateAppRunningRecordByIndex(INDEX_NUM_10 - INDEX_NUM_1);
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    appRecord->SetApplicationClient(client);
    EXPECT_CALL(*mockAppScheduler, ScheduleProcessSecurityExit()).Times(1);

    serviceInner_->RemoveAppFromRecentList(appInfo->name, appInfo->bundleName);  // specify process condition
    EXPECT_EQ(INDEX_NUM_10 - INDEX_NUM_1, static_cast<int32_t>(serviceInner_->GetRecentAppList().size()));
    HILOG_INFO("Remove_Recent_List_001 end");
}

/*
 * Feature: Ams
 * Function: AppRecentList
 * SubFunction: remove
 * FunctionPoints: Remove app from AppRecentList.
 * EnvConditions: AppRecentList has application.
 * CaseDescription: Verity ams can remove all app from AppRecentList.
 */
HWTEST_F(AmsAppRecentListModuleTest, Remove_Recent_List_002, TestSize.Level1)
{
    HILOG_INFO("Remove_Recent_List_002 start");
    EXPECT_TRUE(serviceInner_->GetRecentAppList().empty());
    CreateAppRecentList(INDEX_NUM_10);
    EXPECT_EQ(INDEX_NUM_10, static_cast<int32_t>(serviceInner_->GetRecentAppList().size()));
    for (int32_t i = 0; i < INDEX_NUM_10; i++) {
        std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
        appInfo->name = TEST_APP_NAME + std::to_string(i);
        appInfo->bundleName = appInfo->name;

        auto appRecord = CreateAppRunningRecordByIndex(i);
        sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
        sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
        appRecord->SetApplicationClient(client);
        EXPECT_CALL(*mockAppScheduler, ScheduleProcessSecurityExit()).Times(1);
        serviceInner_->RemoveAppFromRecentList(appInfo->name, appInfo->bundleName);  // specify process condition
    }
    EXPECT_TRUE(serviceInner_->GetRecentAppList().empty());
    HILOG_INFO("Remove_Recent_List_002 end");
}

/*
 * Feature: Ams
 * Function: AppRecentList
 * SubFunction: clear
 * FunctionPoints: Clear AppRecentList.
 * EnvConditions: AppRecentList has application.
 * CaseDescription: Verity ams can clear AppRecentList after removing some apps.
 */
HWTEST_F(AmsAppRecentListModuleTest, Clear_Recent_List_001, TestSize.Level1)
{
    HILOG_INFO("Clear_Recent_List_002 start");
    EXPECT_TRUE(serviceInner_->GetRecentAppList().empty());
    CreateAppRecentList(INDEX_NUM_10);
    EXPECT_EQ(INDEX_NUM_10, static_cast<int32_t>(serviceInner_->GetRecentAppList().size()));

    for (int32_t i = 0; i < INDEX_NUM_10; i++) {
        std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
        appInfo->name = TEST_APP_NAME + std::to_string(i);
        appInfo->bundleName = appInfo->name;

        auto appRecord = CreateAppRunningRecordByIndex(i);
        sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
        sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());

        if (appRecord) {
            appRecord->SetApplicationClient(client);
        }

        EXPECT_CALL(*mockAppScheduler, ScheduleProcessSecurityExit()).Times(1);
        serviceInner_->RemoveAppFromRecentList(appInfo->name, appInfo->bundleName);  //  specify process condition
    }
    serviceInner_->ClearRecentAppList();
    EXPECT_TRUE(serviceInner_->GetRecentAppList().empty());
    HILOG_INFO("Clear_Recent_List_002 end");
}
}  // namespace AppExecFwk
}  // namespace OHOS
