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

#include <climits>
#include <gtest/gtest.h>
#include "ability_thread.h"
#include "ability_local_record.h"
#include "ability_loader.h"
#include "ability_impl_factory.h"
#include "data_ability_helper.h"
#include "context_deal.h"
#include "ohos_application.h"
#include "sys_mgr_client.h"
#include "ability_manager_interface.h"
#include "ability_manager_client.h"
#include "system_ability_definition.h"
#include "demo_ability_test.h"
#include "mock_bundle_manager.h"
#include "mock_ability_manager_service.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace AAFwk;
using OHOS::AppExecFwk::ElementName;
using namespace OHOS::AppExecFwk;
/*
 * Parameters:
 * Action
 * Entity
 * Flag
 * ElementName
 */
const std::string ABILITY_NAME("DemoAbility");
class AbilityBaseTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    OHOS::sptr<OHOS::IRemoteObject> abilityObject_;
    static constexpr int TEST_WAIT_TIME = 500 * 1000;  // 500 ms
    static const int RESULT_CODE = 1992;
};

void AbilityBaseTest::SetUpTestCase(void)
{}

void AbilityBaseTest::TearDownTestCase(void)
{}

void AbilityBaseTest::SetUp(void)
{
    abilityObject_ = new MockAbilityManagerService();
    auto sysMgr = OHOS::DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance();
    EXPECT_TRUE(sysMgr != nullptr);
    sysMgr->RegisterSystemAbility(OHOS::ABILITY_MGR_SERVICE_ID, abilityObject_);
    sysMgr->RegisterSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, new BundleMgrService());
}

void AbilityBaseTest::TearDown(void)
{
    abilityObject_ = nullptr;
}

/**
 * @tc.number: AaFwk_Ability_AbilityFwk_Start_Test_0100
 * @tc.name: AbilityFwk Start
 * @tc.desc: The first step of startability is the attach AMS.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_AbilityFwk_Start_Test_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();

    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
    }
}

/**
 * @tc.number: AaFwk_Ability_AbilityFwk_Start_Test_0200
 * @tc.name: AbilityFwk Start
 * @tc.desc: When connecting AMS,the instance of application is empty.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_AbilityFwk_Start_Test_0200, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = nullptr;

    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
    }
}

/**
 * @tc.number: AaFwk_Ability_AbilityFwk_Start_Test_0300
 * @tc.name: AbilityFwk Start
 * @tc.desc: When connecting AMS,the instance of abilityRecord is empty
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_AbilityFwk_Start_Test_0300, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();

    std::shared_ptr<AbilityLocalRecord> abilityRecord = nullptr;

    AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);
    usleep(AbilityBaseTest::TEST_WAIT_TIME);
}

/**
 * @tc.number: AaFwk_Ability_AbilityFwk_Start_Test_0400
 * @tc.name: AbilityFwk Start
 * @tc.desc: The ability name is empty, so the ability instance cannot be created.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_AbilityFwk_Start_Test_0400, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();

    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
    }
}

/**
 * @tc.number: AaFwk_Ability_AbilityFwk_Start_Test_0500
 * @tc.name: AbilityFwk Start
 * @tc.desc: The ability type is unknown, so the AbilityImpl instance cannot be created.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_AbilityFwk_Start_Test_0500, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();

    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
    }
}

/**
 * @tc.number: AaFwk_Ability_AbilityFwk_Start_Test_0600
 * @tc.name: AbilityFwk Start
 * @tc.desc: The interface OnSaveAbilityState()/OnRestoreAbilityState() of demoability was called.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_AbilityFwk_Start_Test_0600, Function | MediumTest | Level1)
{
    std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
    std::shared_ptr<ProcessInfo> processInfo = std::make_shared<ProcessInfo>();

    appInfo->codePath = "codePath";
    appInfo->dataBaseDir = "dataBaseDir";
    appInfo->dataDir = "dataDir";
    appInfo->cacheDir = "cacheDir";
    appInfo->bundleName = "bundleName";

    contextDeal->SetProcessInfo(processInfo);
    contextDeal->SetApplicationInfo(appInfo);
    contextDeal->SetApplicationContext(application);
    application->AttachBaseContext(contextDeal);

    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->codePath = "codePath";
        abilityInfo->resourcePath = "resourcePath";

        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        sptr<IRemoteObject> remoteObject_ =
            OHOS::DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
        sptr<AAFwk::IAbilityManager> abms = iface_cast<AAFwk::IAbilityManager>(remoteObject_);
        Want want;
        sptr<IAbilityConnection> connect = nullptr;

        // Just to test two interfaces OnRestoreAbilityState/OnSaveAbilityState
        abms->ConnectAbility(want, connect, abilityToken);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
    }
}

/**
 * @tc.number: AaFwk_Ability_Lifecycle_Test_0100
 * @tc.name: Ability Lifecycle
 * @tc.desc: The ability life cycle will change from initial state initial to inactive by calling the interface
 * OnnStart(), and then to active by calling OnActive().
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_Lifecycle_Test_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();

    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        sptr<IRemoteObject> remoteObject_ =
            OHOS::DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
        sptr<AAFwk::IAbilityManager> abms = iface_cast<AAFwk::IAbilityManager>(remoteObject_);
        Want want;
        abms->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_ACTIVE);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
    }
}

/**
 * @tc.number: AaFwk_Ability_AbilityFwk_Lifecycle_Test_0200
 * @tc.name: Ability Lifecycle
 * @tc.desc: The ability life cycle will change from initial state initial to inactive by calling the interface
 * OnnStart(), and then to active by calling OnNewWant()+OnActive().
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_AbilityFwk_Lifecycle_Test_0200, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();

    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        sptr<IRemoteObject> remoteObject_ =
            OHOS::DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
        sptr<AAFwk::IAbilityManager> abms = iface_cast<AAFwk::IAbilityManager>(remoteObject_);

        abms->DisconnectAbility(nullptr);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
    }
}

/**
 * @tc.number: AaFwk_Ability_AbilityFwk_Lifecycle_Test_0300
 * @tc.name: Ability Lifecycle
 * @tc.desc: Ability life cycle changes: initial - > active - > background, the interface of demoability will be
 called:
 * OnStart()->OnActive()->OnInactive()->OnBackground().
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_AbilityFwk_Lifecycle_Test_0300, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();

    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        sptr<IRemoteObject> remoteObject_ =
            OHOS::DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
        sptr<AAFwk::IAbilityManager> abms = iface_cast<AAFwk::IAbilityManager>(remoteObject_);
        Want want;

        abms->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_ACTIVE);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
        abms->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_BACKGROUND);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
    }
}

/**
 * @tc.number: AaFwk_Ability_AbilityFwk_Lifecycle_Test_0400
 * @tc.name: Ability Lifecycle
 * @tc.desc: Ability life cycle changes: initial - > active - > inactive, the interface of demoability will be
 called:
 * OnStart()->OnActive()->OnInactive().
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_AbilityFwk_Lifecycle_Test_0400, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();

    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        sptr<IRemoteObject> remoteObject_ =
            OHOS::DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
        sptr<AAFwk::IAbilityManager> abms = iface_cast<AAFwk::IAbilityManager>(remoteObject_);
        Want want;

        abms->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_ACTIVE);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
        abms->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_INACTIVE);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
    }
}

/**
 * @tc.number: AaFwk_Ability_AbilityFwk_Lifecycle_Test_0500
 * @tc.name: Ability Lifecycle
 * @tc.desc: Ability life cycle changes: initial - > active , the interface of demoability will be called:
 * OnStart()->OnActive()->OnInactive()->OnBackground()->OnForeground().
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_AbilityFwk_Lifecycle_Test_0500, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();

    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        sptr<IRemoteObject> remoteObject_ =
            OHOS::DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
        sptr<AAFwk::IAbilityManager> abms = iface_cast<AAFwk::IAbilityManager>(remoteObject_);
        Want want;

        abms->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_ACTIVE);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
        abms->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_BACKGROUND);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
        abms->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_INACTIVE);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
    }
}

/**
 * @tc.number: AaFwk_Ability_AbilityFwk_Lifecycle_Test_0600
 * @tc.name: Ability Lifecycle
 * @tc.desc: Ability life cycle changes: initial - > active , the interface of demoability will be called:
 * OnStart()->OnActive()->OnInactive()->OnBackground()->OnStop().
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_AbilityFwk_Lifecycle_Test_0600, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();

    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        sptr<IRemoteObject> remoteObject_ =
            OHOS::DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
        sptr<AAFwk::IAbilityManager> abms = iface_cast<AAFwk::IAbilityManager>(remoteObject_);
        Want want;

        abms->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_ACTIVE);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
        abms->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_BACKGROUND);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
        abms->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_INITIAL);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
    }
}

/**
 * @tc.number: AaFwk_Ability_TerminateAbility_ForResult_Test_0100
 * @tc.name: TerminateAbility_ForResult
 * @tc.desc: 1. TerminateAbility with parameters
 *           2. AMS returns parameters through sendresult
 *           3. Compare the returned parameters with the passed in parameters.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_TerminateAbility_ForResult_Test_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();

    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        sptr<IRemoteObject> remoteObject_ =
            OHOS::DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
        sptr<AAFwk::IAbilityManager> abms = iface_cast<AAFwk::IAbilityManager>(remoteObject_);
        Want want;
        sptr<IAbilityConnection> connect = nullptr;

        abms->TerminateAbility(nullptr, RESULT_CODE, &want);
        usleep(AbilityBaseTest::TEST_WAIT_TIME);
    }
}

/*
 * Parameters:
 * Action
 * Entity
 * Flag
 * ElementName
 */
class AbilityTerminateTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

public:
    OHOS::sptr<OHOS::IRemoteObject> abilityObject_;
    static constexpr int TEST_WAIT_TIME = 500 * 1000;  // 500 ms
    static const int RESULT_CODE = 1992;
};

void AbilityTerminateTest::SetUpTestCase(void)
{}

void AbilityTerminateTest::TearDownTestCase(void)
{}

void AbilityTerminateTest::SetUp(void)
{
    abilityObject_ = new MockAbilityManagerService();
    auto sysMgr = OHOS::DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance();
    EXPECT_TRUE(sysMgr != nullptr);
    sysMgr->RegisterSystemAbility(OHOS::ABILITY_MGR_SERVICE_ID, abilityObject_);
}

void AbilityTerminateTest::TearDown(void)
{
    abilityObject_ = nullptr;
}

/**
 * @tc.number: AaFwk_Ability_Terminate_test_0100
 * @tc.name: TerminateAbility
 * @tc.desc: When the ability state is inactive, the call to terminateability terminates.
 */
HWTEST_F(AbilityTerminateTest, AaFwk_Ability_Terminate_test_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        GTEST_LOG_(INFO) << "AaFwk_Ability_Terminate_test_001 AbilityThreadMain";
        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        Want want;
        MockAbilityManagerService *mockAMS = iface_cast<MockAbilityManagerService>(abilityObject_);
        mockAMS->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_INACTIVE);
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);

        auto ability = AbilityLoader::GetInstance().GetAbilityByName(abilityInfo->name);
        EXPECT_NE(ability, nullptr);
        if (ability != nullptr) {
            ability->SetResult(RESULT_CODE, want);
            ability->TerminateAbility();
        }
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);
    }
    GTEST_LOG_(INFO) << "AaFwk_Ability_Terminate_test_001 TerminateAbility";
}

/**
 * @tc.number: AaFwk_Ability_Terminate_test_0200
 * @tc.name: TerminateAbility
 * @tc.desc: When the ability state is active, the call to terminateability terminates.
 */
HWTEST_F(AbilityTerminateTest, AaFwk_Ability_Terminate_test_0200, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        GTEST_LOG_(INFO) << "AaFwk_Ability_Terminate_test_002 AbilityThreadMain";
        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        Want want;
        MockAbilityManagerService *mockAMS = iface_cast<MockAbilityManagerService>(abilityObject_);
        mockAMS->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_ACTIVE);
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);

        auto ability = AbilityLoader::GetInstance().GetAbilityByName(abilityInfo->name);
        EXPECT_NE(ability, nullptr);
        if (ability != nullptr) {
            ability->SetResult(RESULT_CODE, want);
            ability->TerminateAbility();
        }
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);
    }
    GTEST_LOG_(INFO) << "AaFwk_Ability_Terminate_test_002 TerminateAbility";
}

/**
 * @tc.number: AaFwk_Ability_Terminate_test_0300
 * @tc.name: TerminateAbility
 * @tc.desc: When the ability state is BACKGROUND, the call to terminateability terminates.
 */
HWTEST_F(AbilityTerminateTest, AaFwk_Ability_Terminate_test_0300, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        GTEST_LOG_(INFO) << "AaFwk_Ability_Terminate_test_0300 AbilityThreadMain";
        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        Want want;
        MockAbilityManagerService *mockAMS = iface_cast<MockAbilityManagerService>(abilityObject_);
        mockAMS->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_BACKGROUND);
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);

        auto ability = AbilityLoader::GetInstance().GetAbilityByName(abilityInfo->name);
        EXPECT_NE(ability, nullptr);
        if (ability != nullptr) {
            ability->SetResult(RESULT_CODE, want);
            ability->TerminateAbility();
        }
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);
    }
    GTEST_LOG_(INFO) << "AaFwk_Ability_Terminate_test_0300 TerminateAbility";
}

/**
 * @tc.number: AaFwk_Ability_Terminate_test_0400
 * @tc.name: TerminateAbility
 * @tc.desc: When the ability state is ABILITY_STATE_INITIAL, the call to terminateability terminates.
 */
HWTEST_F(AbilityTerminateTest, AaFwk_Ability_Terminate_test_0400, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        GTEST_LOG_(INFO) << "AaFwk_Ability_Terminate_test_0400 AbilityThreadMain";
        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        Want want;
        MockAbilityManagerService *mockAMS = iface_cast<MockAbilityManagerService>(abilityObject_);
        mockAMS->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_INITIAL);
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);

        auto ability = AbilityLoader::GetInstance().GetAbilityByName(abilityInfo->name);
        EXPECT_NE(ability, nullptr);
        if (ability != nullptr) {
            ability->SetResult(RESULT_CODE, want);
            ability->TerminateAbility();
        }
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);
    }
    GTEST_LOG_(INFO) << "AaFwk_Ability_Terminate_test_0400 TerminateAbility";
}

/**
 * @tc.number: AaFwk_Ability_Terminate_test_0500
 * @tc.name: TerminateAbility
 * @tc.desc: When the ability state is inactive, the call to terminateability terminates.
 */
HWTEST_F(AbilityTerminateTest, AaFwk_Ability_Terminate_test_0500, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        GTEST_LOG_(INFO) << "AaFwk_Ability_Terminate_test_005 AbilityThreadMain";
        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        Want want;
        MockAbilityManagerService *mockAMS = iface_cast<MockAbilityManagerService>(abilityObject_);
        mockAMS->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_INACTIVE);
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);

        mockAMS->TerminateAbility(nullptr, RESULT_CODE, &want);
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);
    }
    GTEST_LOG_(INFO) << "AaFwk_Ability_Terminate_test_005 TerminateAbility";
}

/**
 * @tc.number: AaFwk_Ability_Terminate_test_0600
 * @tc.name: TerminateAbility
 * @tc.desc: When the ability state is active, the call to terminateability terminates.
 */
HWTEST_F(AbilityTerminateTest, AaFwk_Ability_Terminate_test_0600, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        GTEST_LOG_(INFO) << "AaFwk_Ability_Terminate_test_006 AbilityThreadMain";
        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        Want want;
        MockAbilityManagerService *mockAMS = iface_cast<MockAbilityManagerService>(abilityObject_);
        mockAMS->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_ACTIVE);
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);

        mockAMS->TerminateAbility(nullptr, RESULT_CODE, &want);
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);
    }
    GTEST_LOG_(INFO) << "AaFwk_Ability_Terminate_test_006 TerminateAbility";
}

/**
 * @tc.number: AaFwk_Ability_Terminate_test_0700
 * @tc.name: TerminateAbility
 * @tc.desc: When the ability state is background, the call to terminateability terminates.
 */
HWTEST_F(AbilityTerminateTest, AaFwk_Ability_Terminate_test_0700, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        GTEST_LOG_(INFO) << "AaFwk_Ability_Terminate_test_007 AbilityThreadMain";
        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        Want want;
        MockAbilityManagerService *mockAMS = iface_cast<MockAbilityManagerService>(abilityObject_);
        mockAMS->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_BACKGROUND);
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);

        mockAMS->TerminateAbility(nullptr, RESULT_CODE, &want);
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);
    }
    GTEST_LOG_(INFO) << "AaFwk_Ability_Terminate_test_007 TerminateAbility";
}

/**
 * @tc.number: AaFwk_Ability_Terminate_test_0800
 * @tc.name: TerminateAbility
 * @tc.desc: When the ability state is initial, the call to terminateability terminates.
 */
HWTEST_F(AbilityTerminateTest, AaFwk_Ability_Terminate_test_0800, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        GTEST_LOG_(INFO) << "AaFwk_Ability_Terminate_test_008 AbilityThreadMain";
        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        Want want;
        MockAbilityManagerService *mockAMS = iface_cast<MockAbilityManagerService>(abilityObject_);
        mockAMS->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_INITIAL);
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);

        mockAMS->TerminateAbility(nullptr, RESULT_CODE, &want);
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);
    }
    GTEST_LOG_(INFO) << "AaFwk_Ability_Terminate_test_008 TerminateAbility";
}

/**
 * @tc.number: AaFwk_WMS_window_test_0100
 * @tc.name: WMS Link
 * @tc.desc: Start pageability and call GetWindow to get the window handle.
 */
HWTEST_F(AbilityTerminateTest, AaFwk_WMS_window_test_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        GTEST_LOG_(INFO) << "AaFwk_WMS_window_test_001 AbilityThreadMain";
        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        Want want;
        MockAbilityManagerService *mockAMS = iface_cast<MockAbilityManagerService>(abilityObject_);
        GTEST_LOG_(INFO) << "AaFwk_WMS_window_test_001 StartAbility";
        mockAMS->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_ACTIVE);
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);
        mockAMS->TerminateAbility(nullptr, RESULT_CODE, &want);
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);
    }
    GTEST_LOG_(INFO) << "AaFwk_WMS_window_test_001 TerminateAbility";
}

/**
 * @tc.number: AaFwk_WMS_window_test_0200
 * @tc.name: WMS Link
 * @tc.desc: Pageability switches to the foreground and calls Window.show.
 *           Pageability switches to the background and calls Window.hide.
 */
HWTEST_F(AbilityTerminateTest, AaFwk_WMS_window_test_0200, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::PAGE;
        abilityInfo->name = ABILITY_NAME;
        abilityInfo->isNativeAbility = true;
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        GTEST_LOG_(INFO) << "AaFwk_WMS_window_test_002 AbilityThreadMain";
        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        Want want;
        MockAbilityManagerService *mockAMS = iface_cast<MockAbilityManagerService>(abilityObject_);
        mockAMS->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_BACKGROUND);
        GTEST_LOG_(INFO) << "AaFwk_WMS_window_test_002 BackGround";
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);
        mockAMS->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_ACTIVE);
        GTEST_LOG_(INFO) << "AaFwk_WMS_window_test_002 Active";
        usleep(AbilityTerminateTest::TEST_WAIT_TIME);

        mockAMS->TerminateAbility(nullptr, RESULT_CODE, &want);
    }
    GTEST_LOG_(INFO) << "AaFwk_WMS_window_test_002 TerminateAbility";
}

/**
 * @tc.number: AaFwk_DataAbility_Launch_0100
 * @tc.name: DataAbilityHelper
 * @tc.desc: The AbilityManager could receive the shelder of abilitythread for dataability when the dataability
 * launched.
 */
HWTEST_F(AbilityTerminateTest, AaFwk_DataAbility_Launch_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbility_Launch_0100";

    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::DATA;
        abilityInfo->name = "DemoAbility";
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        sptr<IRemoteObject> remoteObject_ =
            OHOS::DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
        sptr<AAFwk::IAbilityManager> abms = iface_cast<AAFwk::IAbilityManager>(remoteObject_);

        Uri uri("testuri");
        EXPECT_TRUE(abms->AcquireDataAbility(uri, false, nullptr) == nullptr);
    }
    GTEST_LOG_(INFO) << "AaFwk_DataAbility_Launch_0100";
}

/**
 * @tc.number: AaFwk_DataAbility_Start_0100
 * @tc.name: DataAbilityHelper
 * @tc.desc: The AbilityManager could receive the inactive state from abilitythread for dataability when the
 dataability
 * change its lifecycle state to inactive.
 */
HWTEST_F(AbilityTerminateTest, AaFwk_DataAbility_Start_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbility_Start_0100";

    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::DATA;
        abilityInfo->name = "DemoAbility";
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        sptr<IRemoteObject> remoteObject_ =
            OHOS::DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
        sptr<AAFwk::IAbilityManager> abms = iface_cast<AAFwk::IAbilityManager>(remoteObject_);
        Want want;
        abms->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_INACTIVE);

        usleep(AbilityBaseTest::TEST_WAIT_TIME);
    }
    GTEST_LOG_(INFO) << "AaFwk_DataAbility_Start_0100";
}

/**
 * @tc.number: AaFwk_DataAbility_Start_0200
 * @tc.name: DataAbilityHelper
 * @tc.desc: The AbilityManager could not receive the initial state from abilitythread for dataability.
 *           And the OnStop coulde be called. When the dataability change its lifecycle state to initial.
 */
HWTEST_F(AbilityTerminateTest, AaFwk_DataAbility_Start_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbility_Start_0200";

    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::DATA;
        abilityInfo->name = "DemoAbility";
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        sptr<IRemoteObject> remoteObject_ =
            OHOS::DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
        sptr<AAFwk::IAbilityManager> abms = iface_cast<AAFwk::IAbilityManager>(remoteObject_);
        Want want;
        abms->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_INITIAL);

        usleep(AbilityBaseTest::TEST_WAIT_TIME);
    }
    GTEST_LOG_(INFO) << "AaFwk_DataAbility_Start_0200";
}

/**
 * @tc.number: AaFwk_DataAbility_Start_0300
 * @tc.name: DataAbilityHelper
 * @tc.desc: The AbilityManager could not receive the active state from abilitythread for dataability.
 *           And the OnActive coulde be called. When the dataability change its lifecycle state to active.
 */
HWTEST_F(AbilityTerminateTest, AaFwk_DataAbility_Start_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbility_Start_0300";

    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::DATA;
        abilityInfo->name = "DemoAbility";
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        sptr<IRemoteObject> remoteObject_ =
            OHOS::DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
        sptr<AAFwk::IAbilityManager> abms = iface_cast<AAFwk::IAbilityManager>(remoteObject_);
        Want want;
        abms->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_ACTIVE);

        usleep(AbilityBaseTest::TEST_WAIT_TIME);
    }
    GTEST_LOG_(INFO) << "AaFwk_DataAbility_Start_0300";
}

/**
 * @tc.number: AaFwk_DataAbility_Start_0400
 * @tc.name: DataAbilityHelper
 * @tc.desc: The AbilityManager could not receive the background state from abilitythread for dataability.
 *           And the OnBackground coulde be called. When the dataability change its lifecycle state to background.
 */
HWTEST_F(AbilityTerminateTest, AaFwk_DataAbility_Start_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbility_Start_0400";

    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    EXPECT_NE(abilityToken, nullptr);
    if (abilityToken != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->type = AppExecFwk::AbilityType::DATA;
        abilityInfo->name = "DemoAbility";
        std::shared_ptr<AbilityLocalRecord> abilityRecord =
            std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

        AbilityThread::AbilityThreadMain(application, abilityRecord, nullptr);

        sptr<IRemoteObject> remoteObject_ =
            OHOS::DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
        sptr<AAFwk::IAbilityManager> abms = iface_cast<AAFwk::IAbilityManager>(remoteObject_);
        Want want;
        abms->StartAbility(want, AbilityLifeCycleState::ABILITY_STATE_BACKGROUND);

        usleep(AbilityBaseTest::TEST_WAIT_TIME);
    }
    GTEST_LOG_(INFO) << "AaFwk_DataAbility_Start_0400";
}
}  // namespace AppExecFwk
}  // namespace OHOS
