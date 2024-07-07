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
#include "remote_client_manager.h"
#include "app_mgr_service_inner.h"
#undef private

#include <vector>
#include <gtest/gtest.h>
#include "app_launch_data.h"
#include "app_mgr_interface.h"
#include "iremote_object.h"
#include "app_state_callback_proxy.h"
#include "hilog_wrapper.h"
#include "refbase.h"
#include "mock_bundle_manager.h"
#include "mock_ability_token.h"
#include "mock_app_scheduler.h"
#include "mock_app_spawn_client.h"
#include "mock_app_spawn_socket.h"
#include "mock_iapp_state_callback.h"
#include "mock_native_token.h"
#include "system_ability_definition.h"
#include "sys_mgr_client.h"

using namespace testing::ext;
using testing::_;
using testing::Return;
using testing::SetArgReferee;
using ::testing::DoAll;

namespace {
const int32_t ABILITY_NUM = 100;
const int32_t APPLICATION_NUM = 100;
const int32_t INDEX_NUM_100 = 100;
const int32_t INDEX_NUM_MAX = 100;
const std::string TEST_APP_NAME = "com.ohos.test.helloworld";
const std::string TEST_ABILITY_NAME = "test_ability_";
#define CHECK_POINTER_IS_NULLPTR(object) \
    do {                                 \
        if (object == nullptr) {         \
            return;                      \
        }                                \
    } while (0)
}  // namespace

namespace OHOS {
namespace AppExecFwk {
struct TestProcessInfo {
    pid_t pid = 0;
    bool isStart = false;
};
// specify process condition
class AmsAppLifeCycleModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::shared_ptr<ApplicationInfo> GetApplicationInfo(const std::string &appName) const;
    std::shared_ptr<AbilityInfo> GetAbilityInfo(const std::string &abilityIndex, const std::string &name,
        const std::string &process, const std::string &applicationName) const;
    void StartAppProcess(const pid_t &pid) const;
    std::shared_ptr<AppRunningRecord> StartProcessAndLoadAbility(const sptr<MockAppScheduler> &mockAppScheduler,
        const sptr<IRemoteObject> &token, const std::shared_ptr<AbilityInfo> &abilityInfo,
        const std::shared_ptr<ApplicationInfo> &appInfo, const TestProcessInfo &testProcessInfo) const;
    void ChangeAbilityStateAfterAppStart(const sptr<MockAppScheduler> &mockAppScheduler, const pid_t &pid) const;
    void ChangeAbilityStateToForegroud(const sptr<MockAppScheduler> &mockAppScheduler,
        const std::shared_ptr<AppRunningRecord> &appRunningRecord, const sptr<IRemoteObject> &token,
        const bool isChange = false) const;
    void ChangeAbilityStateToBackGroud(const sptr<MockAppScheduler> &mockAppScheduler,
        const std::shared_ptr<AppRunningRecord> &appRunningRecord, const sptr<IRemoteObject> &token,
        const bool isChange = false) const;
    void ChangeAbilityStateToTerminate(
        const sptr<MockAppScheduler> &mockAppScheduler, const sptr<IRemoteObject> &token) const;
    void ChangeAppToTerminate(const sptr<MockAppScheduler> &mockAppScheduler,
        const std::shared_ptr<AppRunningRecord> &appRunningRecord, const sptr<IRemoteObject> &token,
        const bool isStop = false) const;
    void CheckState(const std::shared_ptr<AppRunningRecord> &appRunningRecord, const sptr<IRemoteObject> &token,
        const AbilityState abilityState, const ApplicationState appState) const;
    void CheckStateAfterClearAbility(const std::shared_ptr<AppRunningRecord> &appRunningRecord,
        const std::vector<sptr<IRemoteObject>> &tokens, const int32_t &recordId,
        const sptr<MockAppScheduler> &mockAppScheduler) const;
    void CheckStateAfterChangeAbility(const std::shared_ptr<AppRunningRecord> &appRunningRecord,
        const std::vector<sptr<IRemoteObject>> &tokens, const sptr<MockAppScheduler> &mockAppScheduler);
    void CreateAppRecentList(const int32_t appNum);

    sptr<MockAbilityToken> GetAbilityToken();

protected:
    std::shared_ptr<AppMgrServiceInner> serviceInner_ = nullptr;
    sptr<MockAbilityToken> mockToken_ = nullptr;
    sptr<MockAppStateCallback> mockAppStateCallbackStub_ = nullptr;
    std::shared_ptr<AppMgrServiceInner> inner_ = nullptr;
    sptr<BundleMgrService> mockBundleMgr;
    std::shared_ptr<AMSEventHandler> handler_ = nullptr;
};

void AmsAppLifeCycleModuleTest::SetUpTestCase()
{
    MockNativeToken::SetNativeToken();
}

void AmsAppLifeCycleModuleTest::TearDownTestCase()
{}

void AmsAppLifeCycleModuleTest::SetUp()
{
    serviceInner_.reset(new (std::nothrow) AppMgrServiceInner());
    serviceInner_->Init();
    mockAppStateCallbackStub_ = new (std::nothrow) MockAppStateCallback();

    inner_ = std::make_shared<AppMgrServiceInner>();

    if (serviceInner_ && mockAppStateCallbackStub_) {
        auto mockAppStateCallbackProxy = iface_cast<IAppStateCallback>(mockAppStateCallbackStub_);
        if (mockAppStateCallbackProxy) {
            serviceInner_->RegisterAppStateCallback(mockAppStateCallbackProxy);
            inner_->RegisterAppStateCallback(mockAppStateCallbackProxy);
        }
    }

    mockBundleMgr = new (std::nothrow) BundleMgrService();
    serviceInner_->SetBundleManager(mockBundleMgr);

    auto runner = EventRunner::Create("AmsAppLifeCycleModuleTest");
    handler_ = std::make_shared<AMSEventHandler>(runner, serviceInner_);
    serviceInner_->SetEventHandler(handler_);
}

void AmsAppLifeCycleModuleTest::TearDown()
{
    serviceInner_.reset();
    mockAppStateCallbackStub_.clear();
}

std::shared_ptr<ApplicationInfo> AmsAppLifeCycleModuleTest::GetApplicationInfo(const std::string &appName) const
{
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = appName;
    appInfo->bundleName = appName;
    return appInfo;
}

std::shared_ptr<AbilityInfo> AmsAppLifeCycleModuleTest::GetAbilityInfo(const std::string &abilityIndex,
    const std::string &name, const std::string &process, const std::string &applicationName) const
{
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = name + abilityIndex;
    if (!process.empty()) {
        abilityInfo->process = process;
    }
    abilityInfo->applicationName = applicationName;
    abilityInfo->applicationInfo.bundleName = applicationName;
    return abilityInfo;
}

std::shared_ptr<AppRunningRecord> AmsAppLifeCycleModuleTest::StartProcessAndLoadAbility(
    const sptr<MockAppScheduler> &mockAppScheduler, const sptr<IRemoteObject> &token,
    const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<ApplicationInfo> &appInfo,
    const TestProcessInfo &testProcessInfo) const
{
    if (!testProcessInfo.isStart) {
        StartAppProcess(testProcessInfo.pid);
    } else {
        EXPECT_CALL(*mockAppScheduler, ScheduleLaunchAbility(_, _, _)).Times(1);
    }

    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo, nullptr);
    BundleInfo bundleInfo;
    HapModuleInfo hapModuleInfo;
    EXPECT_TRUE(serviceInner_->GetBundleAndHapInfo(*abilityInfo, appInfo, bundleInfo, hapModuleInfo));

    std::shared_ptr<AppRunningRecord> record = serviceInner_->appRunningManager_->CheckAppRunningRecordIsExist(
        appInfo->name, abilityInfo->process, abilityInfo->applicationInfo.uid, bundleInfo);
    if (record == nullptr) {
        EXPECT_TRUE(false);
    } else {
        pid_t newPid = record->GetPriorityObject()->GetPid();
        EXPECT_EQ(newPid, testProcessInfo.pid);
    }
    return record;
}

void AmsAppLifeCycleModuleTest::StartAppProcess(const pid_t &pid) const
{
    MockAppSpawnClient *mockClientPtr = new (std::nothrow) MockAppSpawnClient();
    EXPECT_TRUE(mockClientPtr);

    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).WillOnce(DoAll(SetArgReferee<1>(pid), Return(ERR_OK)));
    EXPECT_CALL(*mockAppStateCallbackStub_, OnAppStateChanged(_));

    serviceInner_->SetAppSpawnClient(std::unique_ptr<MockAppSpawnClient>(mockClientPtr));
}

void AmsAppLifeCycleModuleTest::ChangeAbilityStateAfterAppStart(
    const sptr<MockAppScheduler> &mockAppScheduler, const pid_t &pid) const
{
    EXPECT_CALL(*mockAppScheduler, ScheduleLaunchApplication(_, _)).Times(1);
    EXPECT_CALL(*mockAppScheduler, ScheduleLaunchAbility(_, _, _)).Times(1);

    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    serviceInner_->AttachApplication(pid, client);
}

void AmsAppLifeCycleModuleTest::ChangeAbilityStateToForegroud(const sptr<MockAppScheduler> &mockAppScheduler,
    const std::shared_ptr<AppRunningRecord> &appRunningRecord, const sptr<IRemoteObject> &token,
    const bool isChange) const
{
    if (!isChange) {
        EXPECT_CALL(*mockAppScheduler, ScheduleForegroundApplication()).Times(1);
        EXPECT_CALL(*mockAppStateCallbackStub_, OnAppStateChanged(_)).Times(testing::AtLeast(1));
        EXPECT_CALL(*mockAppStateCallbackStub_, OnAbilityRequestDone(_, _)).Times(testing::AtLeast(1));
    }

    serviceInner_->UpdateAbilityState(token, AbilityState::ABILITY_STATE_FOREGROUND);

    if (!isChange) {
        EXPECT_NE(appRunningRecord, nullptr);
        CHECK_POINTER_IS_NULLPTR(appRunningRecord);
        int32_t recordId = appRunningRecord->GetRecordId();
        serviceInner_->ApplicationForegrounded(recordId);
    }
}

void AmsAppLifeCycleModuleTest::ChangeAbilityStateToBackGroud(const sptr<MockAppScheduler> &mockAppScheduler,
    const std::shared_ptr<AppRunningRecord> &appRunningRecord, const sptr<IRemoteObject> &token,
    const bool isChange) const
{
    if (!isChange) {
        EXPECT_CALL(*mockAppScheduler, ScheduleBackgroundApplication()).Times(1);
        EXPECT_CALL(*mockAppStateCallbackStub_, OnAppStateChanged(_)).Times(testing::AtLeast(1));
        EXPECT_CALL(*mockAppStateCallbackStub_, OnAbilityRequestDone(_, _)).Times(testing::AtLeast(1));
    }

    serviceInner_->UpdateAbilityState(token, AbilityState::ABILITY_STATE_BACKGROUND);

    if (!isChange) {
        EXPECT_NE(appRunningRecord, nullptr);
        CHECK_POINTER_IS_NULLPTR(appRunningRecord);
        int32_t recordId = appRunningRecord->GetRecordId();
        serviceInner_->ApplicationBackgrounded(recordId);
    }
}

void AmsAppLifeCycleModuleTest::ChangeAppToTerminate(const sptr<MockAppScheduler> &mockAppScheduler,
    const std::shared_ptr<AppRunningRecord> &appRunningRecord, const sptr<IRemoteObject> &token,
    const bool isStop) const
{
    ChangeAbilityStateToTerminate(mockAppScheduler, token);

    if (isStop) {
        EXPECT_CALL(*mockAppScheduler, ScheduleTerminateApplication()).Times(1);
        EXPECT_CALL(*mockAppStateCallbackStub_, OnAppStateChanged(_)).Times(testing::AtLeast(1));
        serviceInner_->AbilityTerminated(token);
        EXPECT_NE(appRunningRecord, nullptr);
        CHECK_POINTER_IS_NULLPTR(appRunningRecord);
        int32_t recordId = appRunningRecord->GetRecordId();
        serviceInner_->ApplicationTerminated(recordId);
    } else {
        serviceInner_->AbilityTerminated(token);
    }
}

void AmsAppLifeCycleModuleTest::ChangeAbilityStateToTerminate(
    const sptr<MockAppScheduler> &mockAppScheduler, const sptr<IRemoteObject> &token) const
{
    EXPECT_CALL(*mockAppScheduler, ScheduleCleanAbility(_)).Times(1);
    serviceInner_->TerminateAbility(token, false);
}

void AmsAppLifeCycleModuleTest::CheckState(const std::shared_ptr<AppRunningRecord> &appRunningRecord,
    const sptr<IRemoteObject> &token, const AbilityState abilityState, const ApplicationState appState) const
{
    EXPECT_NE(appRunningRecord, nullptr);
    CHECK_POINTER_IS_NULLPTR(appRunningRecord);
    auto abilityRunningRecord = appRunningRecord->GetAbilityRunningRecordByToken(token);
    ApplicationState getAppState = appRunningRecord->GetState();
    EXPECT_EQ(appState, getAppState);
    EXPECT_NE(abilityRunningRecord, nullptr);
    CHECK_POINTER_IS_NULLPTR(abilityRunningRecord);
    AbilityState getAbilityState = abilityRunningRecord->GetState();
    EXPECT_EQ(abilityState, getAbilityState);
}

void AmsAppLifeCycleModuleTest::CheckStateAfterClearAbility(const std::shared_ptr<AppRunningRecord> &appRunningRecord,
    const std::vector<sptr<IRemoteObject>> &tokens, const int32_t &recordId,
    const sptr<MockAppScheduler> &mockAppScheduler) const
{
    unsigned long size = tokens.size();
    for (unsigned long i = 0; i < size; i++) {
        if (i != size - 1) {
            ChangeAppToTerminate(mockAppScheduler, appRunningRecord, tokens[i], false);
            ApplicationState getAppState = appRunningRecord->GetState();
            EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, getAppState);
        } else {
            ChangeAppToTerminate(mockAppScheduler, appRunningRecord, tokens[i], true);
            auto record = serviceInner_->GetAppRunningRecordByAppRecordId(recordId);
            EXPECT_EQ(nullptr, record);
        }
    }
}

void AmsAppLifeCycleModuleTest::CheckStateAfterChangeAbility(const std::shared_ptr<AppRunningRecord> &appRunningRecord,
    const std::vector<sptr<IRemoteObject>> &tokens, const sptr<MockAppScheduler> &mockAppScheduler)
{
    unsigned long size = tokens.size();
    for (unsigned long i = 0; i < size; i++) {
        if (i != size - 1) {
            ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, tokens[i], true);
            CheckState(appRunningRecord,
                tokens[i],
                AbilityState::ABILITY_STATE_BACKGROUND,
                ApplicationState::APP_STATE_FOREGROUND);
        } else {
            ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, tokens[i], false);
            CheckState(appRunningRecord,
                tokens[i],
                AbilityState::ABILITY_STATE_BACKGROUND,
                ApplicationState::APP_STATE_BACKGROUND);
        }
    }
}

void AmsAppLifeCycleModuleTest::CreateAppRecentList(const int32_t appNum)
{
    for (int32_t i = INDEX_NUM_MAX - appNum + 1; i <= INDEX_NUM_MAX; i++) {
        std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        appInfo->name = TEST_APP_NAME + std::to_string(i);
        appInfo->bundleName = appInfo->name;
        abilityInfo->name = TEST_ABILITY_NAME + std::to_string(i);
        abilityInfo->applicationName = TEST_APP_NAME + std::to_string(i);
        abilityInfo->applicationInfo.bundleName = appInfo->name;
        pid_t pid = i;
        sptr<IRemoteObject> token = new (std::nothrow) MockAbilityToken();
        MockAppSpawnClient *mockedSpawnClient = new (std::nothrow) MockAppSpawnClient();
        EXPECT_TRUE(mockedSpawnClient);
        EXPECT_CALL(*mockedSpawnClient, StartProcess(_, _))
            .Times(1)
            .WillOnce(DoAll(SetArgReferee<1>(pid), Return(ERR_OK)));
        EXPECT_CALL(*mockAppStateCallbackStub_, OnAppStateChanged(_)).Times(1);

        serviceInner_->SetAppSpawnClient(std::unique_ptr<MockAppSpawnClient>(mockedSpawnClient));
        serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo, nullptr);
    }
    return;
}

sptr<MockAbilityToken> AmsAppLifeCycleModuleTest::GetAbilityToken()
{
    if (mockToken_ != nullptr) {
        return mockToken_;
    }
    mockToken_ = new (std::nothrow) MockAbilityToken();
    return mockToken_;
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of ability
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start app
 *                  2.switch ability to foreground and call ScheduleForegroundApplication API to enable Application
 *                      foreground
 *                  3.switch ability to background and call ScheduleBackgroundApplication API to enable Application
 *                      background
 *                  4.terminate ability
 *                  5.call ScheduleTerminateApplication API to make app terminated
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_001, TestSize.Level2)
{
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);
    pid_t pid = 1024;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld");
    auto appInfo = GetApplicationInfo("com.ohos.test.helloworld");
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    EXPECT_TRUE(mockAppScheduler);

    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;
    testProcessInfo.isStart = false;
    auto appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);
    int32_t recordId = appRunningRecord->GetRecordId();

    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, token);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    ChangeAppToTerminate(mockAppScheduler, appRunningRecord, token, true);
    auto record = serviceInner_->GetAppRunningRecordByAppRecordId(recordId);
    EXPECT_EQ(nullptr, record);
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of abilities
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start app
 *                  2.switch ability to foreground and call ScheduleForegroundApplication API to enable Application
 *                    foreground
 *                  3.load other ability, and switch last ability to background and call ScheduleBackgroundApplication
 *                    API to enable Application background
 *                  4.terminate every ability
 *                  5.call ScheduleTerminateApplication API to make app terminated
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_002, TestSize.Level3)
{
    pid_t pid = 1023;
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);
    std::shared_ptr<AppRunningRecord> appRunningRecord = nullptr;
    std::vector<sptr<IRemoteObject>> tokens;
    auto abilityInfo = std::make_shared<AbilityInfo>();
    auto appInfo = std::make_shared<ApplicationInfo>();
    sptr<IRemoteObject> token;
    int32_t recordId;
    bool flag = false;
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();

    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;

    for (int i = 0; i < ABILITY_NUM; i++) {
        abilityInfo = GetAbilityInfo(std::to_string(i), "MainAbility", "p1", "com.ohos.test.helloworld");
        appInfo = GetApplicationInfo("com.ohos.test.helloworld");
        token = new (std::nothrow) MockAbilityToken();
        tokens.push_back(token);
        testProcessInfo.isStart = flag;
        appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);
        if (!flag) {
            ChangeAbilityStateAfterAppStart(mockAppScheduler, pid);
            recordId = appRunningRecord->GetRecordId();
            flag = true;
            CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);
        } else {
            CheckState(
                appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_BACKGROUND);
        }
        ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token);
        CheckState(
            appRunningRecord, token, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
        ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, token);
        CheckState(
            appRunningRecord, token, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    }
    auto abilities = appRunningRecord->GetAbilities();
    int size = abilities.size();
    EXPECT_EQ(size, ABILITY_NUM);
    CheckStateAfterClearAbility(appRunningRecord, tokens, recordId, mockAppScheduler);
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of abilities at the same time
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start 1000 abilities
 *                  2.switch every ability to foreground
 *                  3.switch every ability to background
 *                  4.terminate every ability
 *                  5.call ScheduleTerminateApplication API to make app terminated
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_003, TestSize.Level3)
{
    pid_t pid = 1025;
    EXPECT_TRUE(serviceInner_);
    std::shared_ptr<AppRunningRecord> appRunningRecord = nullptr;
    std::vector<sptr<IRemoteObject>> tokens;
    auto abilityInfo = std::make_shared<AbilityInfo>();
    auto appInfo = std::make_shared<ApplicationInfo>();
    sptr<IRemoteObject> token;
    int32_t recordId;
    bool flag = false;
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();

    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;

    for (int i = 0; i < ABILITY_NUM; i++) {
        abilityInfo = GetAbilityInfo(std::to_string(i), "MainAbility", "p1", "com.ohos.test.helloworld1");
        appInfo = GetApplicationInfo("com.ohos.test.helloworld1");
        token = new (std::nothrow) MockAbilityToken();
        tokens.push_back(token);

        testProcessInfo.isStart = flag;
        appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);
        if (!flag) {
            ChangeAbilityStateAfterAppStart(mockAppScheduler, testProcessInfo.pid);
            recordId = appRunningRecord->GetRecordId();
            flag = true;
            CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);
            ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token);
        } else {
            CheckState(
                appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_FOREGROUND);
            ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token, true);
        }
    }
    CheckStateAfterChangeAbility(appRunningRecord, tokens, mockAppScheduler);
    CheckStateAfterClearAbility(appRunningRecord, tokens, recordId, mockAppScheduler);
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of abilities
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start app
 *                  2.switch ability to foreground and call ScheduleForegroundApplication API to enable Application
 *                      foreground
 *                  3.switch ability to background and call ScheduleBackgroundApplication API to enable Application
 *                      background
 *                  4.repeat step 2~3 1000 times
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_004, TestSize.Level3)
{
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);
    pid_t pid = 1024;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "p3", "com.ohos.test.helloworld");
    auto appInfo = GetApplicationInfo("com.ohos.test.helloworld");

    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();

    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;
    testProcessInfo.isStart = false;

    auto appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);

    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    int count = 1000;
    while (count > 0) {
        ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token);
        CheckState(
            appRunningRecord, token, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

        ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, token);
        CheckState(
            appRunningRecord, token, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
        count--;
    }
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of ability
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start app
 *                  2.switch ability to foreground and call ScheduleForegroundApplication API to enable Application
 *                      foreground
 *                  3.switch ability to background
 *                  4.start new ability to foreground
 *                  5.switch ability to background and call ScheduleBackgroundApplication API to enable Application
 *                      background
 *                  6.call ScheduleTerminateApplication API to make app terminated
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_005, TestSize.Level2)
{
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);
    pid_t pid = 1024;

    sptr<IRemoteObject> token0 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo0 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld");
    auto appInfo = GetApplicationInfo("com.ohos.test.helloworld");

    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();

    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;
    testProcessInfo.isStart = false;

    auto appRunningRecord =
        StartProcessAndLoadAbility(mockAppScheduler, token0, abilityInfo0, appInfo, testProcessInfo);

    int32_t recordId = appRunningRecord->GetRecordId();

    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid);
    CheckState(appRunningRecord, token0, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token0);
    CheckState(
        appRunningRecord, token0, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, token0);
    CheckState(
        appRunningRecord, token0, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    sptr<IRemoteObject> token1 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo1 = GetAbilityInfo("1", "SubAbility", "p1", "com.ohos.test.helloworld");
    testProcessInfo.isStart = true;
    appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token1, abilityInfo1, appInfo, testProcessInfo);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token1);
    CheckState(
        appRunningRecord, token1, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, token1);
    CheckState(
        appRunningRecord, token1, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    ChangeAppToTerminate(mockAppScheduler, appRunningRecord, token0);
    ChangeAppToTerminate(mockAppScheduler, appRunningRecord, token1, true);
    auto record = serviceInner_->GetAppRunningRecordByAppRecordId(recordId);
    EXPECT_EQ(nullptr, record);
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of ability
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start app
 *                  2.switch ability to foreground and call ScheduleForegroundApplication API to enable Application
 *                      foreground
 *                  3.switch ability to background and call ScheduleBackgroundApplication API to enable Application
 *                      background
 *                  4.through appName kill application
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_006, TestSize.Level2)
{
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);

    pid_t pid = fork();
    if (pid == 0) {
        pause();
        exit(0);
    }

    EXPECT_TRUE(pid > 0);
    usleep(50000);

    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld");
    auto appInfo = GetApplicationInfo("com.ohos.test.helloworld");
    appInfo->bundleName = "com.ohos.test.helloworld";
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();

    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;
    testProcessInfo.isStart = false;

    auto appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);
    int32_t recordId = appRunningRecord->GetRecordId();

    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, token);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*mockAppScheduler, ScheduleProcessSecurityExit()).Times(1);

    int32_t ret = serviceInner_->KillApplication(appInfo->bundleName);
    EXPECT_EQ(ret, 0);
    serviceInner_->OnRemoteDied(mockAppScheduler);  // A faked death recipient.
    auto record = serviceInner_->GetAppRunningRecordByAppRecordId(recordId);
    EXPECT_EQ(nullptr, record);
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of ability
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start app
 *                  2.switch ability to foreground and call ScheduleForegroundApplication API to enable Application
 *                      foreground
 *                  3.switch ability to background and call ScheduleBackgroundApplication API to enable Application
 *                      background
 *                  4.terminate ability
 *                  5.call ScheduleTerminateApplication API to make app terminated
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_007, TestSize.Level2)
{
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);
    pid_t pid = 1024;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld");
    auto appInfo = GetApplicationInfo("com.ohos.test.helloworld");
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();

    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;
    testProcessInfo.isStart = false;

    auto appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);
    int32_t recordId = appRunningRecord->GetRecordId();

    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, token);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    ChangeAppToTerminate(mockAppScheduler, appRunningRecord, token, true);
    auto record = serviceInner_->GetAppRunningRecordByAppRecordId(recordId);
    EXPECT_EQ(nullptr, record);
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of ability
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start app
 *                  2.switch ability to foreground and call ScheduleForegroundApplication API to enable Application
 *                      foreground
 *                  3.switch ability to background and call ScheduleBackgroundApplication API to enable Application
 *                      background
 *                  4.call loadAbility API to start new app
 *                  5.switch ability to foreground and call ScheduleForegroundApplication API to enable new Application
 *                      foreground
 *                  6.switch ability to background and call ScheduleBackgroundApplication API to enable new Application
 *                      background
 *                  7.terminate ability
 *                  8.call ScheduleTerminateApplication API to make app terminated
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_008, TestSize.Level2)
{
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);
    pid_t pid_0 = 1024;
    pid_t pid_1 = 2048;
    sptr<IRemoteObject> token_0 = new (std::nothrow) MockAbilityToken();
    sptr<IRemoteObject> token_1 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo_0 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld0");
    auto appInfo_0 = GetApplicationInfo("com.ohos.test.helloworld0");
    auto abilityInfo_1 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld1");
    auto appInfo_1 = GetApplicationInfo("com.ohos.test.helloworld1");
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    TestProcessInfo testProcessInfo;

    testProcessInfo.pid = pid_0;
    testProcessInfo.isStart = false;
    auto appRunningRecord_0 =
        StartProcessAndLoadAbility(mockAppScheduler, token_0, abilityInfo_0, appInfo_0, testProcessInfo);

    testProcessInfo.pid = pid_1;
    testProcessInfo.isStart = false;
    auto appRunningRecord_1 =
        StartProcessAndLoadAbility(mockAppScheduler, token_1, abilityInfo_1, appInfo_1, testProcessInfo);
    int32_t recordId_0 = appRunningRecord_0->GetRecordId();
    int32_t recordId_1 = appRunningRecord_1->GetRecordId();

    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid_0);
    CheckState(appRunningRecord_0, token_0, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord_0, token_0);
    CheckState(
        appRunningRecord_0, token_0, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord_0, token_0);
    CheckState(
        appRunningRecord_0, token_0, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid_1);
    CheckState(appRunningRecord_1, token_1, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord_1, token_1);
    CheckState(
        appRunningRecord_1, token_1, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord_1, token_1);
    CheckState(
        appRunningRecord_1, token_1, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    ChangeAppToTerminate(mockAppScheduler, appRunningRecord_0, token_0, true);
    auto record = serviceInner_->GetAppRunningRecordByAppRecordId(recordId_0);
    EXPECT_EQ(nullptr, record);

    ChangeAppToTerminate(mockAppScheduler, appRunningRecord_1, token_1, true);
    record = serviceInner_->GetAppRunningRecordByAppRecordId(recordId_1);
    EXPECT_EQ(nullptr, record);
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of abilities at the same time
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start 100 app
 *                  2.switch every ability to foreground
 *                  3.switch every ability to background
 *                  4.terminate every ability
 *                  5.call ScheduleTerminateApplication API to make app terminated
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_009, TestSize.Level3)
{
    pid_t pid = 1025;
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);
    std::shared_ptr<AppRunningRecord> appRunningRecord = nullptr;
    auto abilityInfo = std::make_shared<AbilityInfo>();
    auto appInfo = std::make_shared<ApplicationInfo>();
    sptr<IRemoteObject> token;
    int32_t recordId;
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();

    TestProcessInfo testProcessInfo;

    for (int i = 0; i < APPLICATION_NUM; i++) {
        abilityInfo = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld1");
        appInfo = GetApplicationInfo("com.ohos.test.helloworld1");
        token = new (std::nothrow) MockAbilityToken();
        pid += i;

        testProcessInfo.pid = pid;
        testProcessInfo.isStart = false;
        appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);
        ChangeAbilityStateAfterAppStart(mockAppScheduler, testProcessInfo.pid);
        recordId = appRunningRecord->GetRecordId();
        CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);
        ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token);

        ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, token);
        CheckState(
            appRunningRecord, token, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

        ChangeAppToTerminate(mockAppScheduler, appRunningRecord, token, true);
        auto record = serviceInner_->GetAppRunningRecordByAppRecordId(recordId);
        EXPECT_EQ(nullptr, record);
    }
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test getrecentapplist and removeappfromrecentlist all process.
 * EnvConditions: system running normally
 * CaseDescription: 1.call getrecentapplist API to get current app list
 *                  2.call removeappfromrecentlist API to remove current app list
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_011, TestSize.Level1)
{
    EXPECT_TRUE(serviceInner_->GetRecentAppList().empty());
    CreateAppRecentList(INDEX_NUM_100);
    EXPECT_EQ(INDEX_NUM_100, static_cast<int32_t>(serviceInner_->GetRecentAppList().size()));
    for (int32_t i = INDEX_NUM_MAX; i > 0; i--) {
        std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
        appInfo->name = TEST_APP_NAME + std::to_string(i);
        appInfo->bundleName = appInfo->name;  // specify process condition
        auto appTaskInfo =
            serviceInner_->appProcessManager_->GetAppTaskInfoByProcessName(appInfo->name, appInfo->bundleName);
        serviceInner_->appProcessManager_->RemoveAppFromRecentList(appTaskInfo);
    }
    EXPECT_TRUE(serviceInner_->GetRecentAppList().empty());
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test getrecentapplist and clearrecentappList all process.
 * EnvConditions: system running normally
 * CaseDescription: 1.call getrecentapplist API to get current app list
 *                  2.call clearrecentapplist API to clear current app list
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_012, TestSize.Level1)
{
    EXPECT_TRUE(serviceInner_->GetRecentAppList().empty());
    CreateAppRecentList(INDEX_NUM_100);
    EXPECT_EQ(INDEX_NUM_100, static_cast<int32_t>(serviceInner_->GetRecentAppList().size()));

    serviceInner_->appProcessManager_->ClearRecentAppList();
    EXPECT_TRUE(serviceInner_->GetRecentAppList().empty());
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test get and stop all process.
 * EnvConditions: system running normally
 * CaseDescription: OnStop
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_013, TestSize.Level3)
{
    EXPECT_TRUE(serviceInner_);
    EXPECT_TRUE(serviceInner_->remoteClientManager_);
    EXPECT_TRUE(serviceInner_->remoteClientManager_->GetSpawnClient());

    auto mockAppSpawnSocket = std::make_shared<MockAppSpawnSocket>();
    EXPECT_TRUE(mockAppSpawnSocket);
    serviceInner_->remoteClientManager_->GetSpawnClient()->SetSocket(mockAppSpawnSocket);

    EXPECT_EQ(serviceInner_->QueryAppSpawnConnectionState(), SpawnConnectionState::STATE_NOT_CONNECT);

    EXPECT_CALL(*mockAppSpawnSocket, OpenAppSpawnConnection()).Times(1).WillOnce(Return(0));

    int ret = serviceInner_->OpenAppSpawnConnection();
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(serviceInner_->QueryAppSpawnConnectionState(), SpawnConnectionState::STATE_CONNECTED);

    EXPECT_CALL(*mockAppSpawnSocket, CloseAppSpawnConnection()).Times(1);

    serviceInner_->OnStop();
    usleep(50000);
    EXPECT_EQ(serviceInner_->QueryAppSpawnConnectionState(), SpawnConnectionState::STATE_NOT_CONNECT);
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test get app Running Record,When the app is added for the first time
 * EnvConditions: system running normally
 * CaseDescription: GetOrCreateAppRunningRecord
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_014, TestSize.Level1)
{
    pid_t pid = 1000;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.special");
    auto appInfo = GetApplicationInfo("com.ohos.test.special");
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;
    testProcessInfo.isStart = false;
    auto appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);

    // Because GetOrCreateAppRunningRecord was called in LoadAbility.
    // When the app is added for the first time
    EXPECT_EQ(appRunningRecord->GetName(), "com.ohos.test.special");
    EXPECT_EQ(appRunningRecord->GetProcessName(), "p1");
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: Ability Status change received
 * EnvConditions: system running normally
 * CaseDescription: OnAbilityRequestDone
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_015, TestSize.Level1)
{
    pid_t pid = 1000;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.special");
    auto appInfo = GetApplicationInfo("com.ohos.test.special");
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;
    testProcessInfo.isStart = false;

    auto appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);

    EXPECT_CALL(*mockAppScheduler, ScheduleBackgroundApplication()).Times(1);
    ChangeAbilityStateAfterAppStart(mockAppScheduler, testProcessInfo.pid);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    std::weak_ptr<AppMgrServiceInner> inner = inner_;
    appRunningRecord->SetAppMgrServiceInner(inner);

    appRunningRecord->UpdateAbilityState(token, AbilityState::ABILITY_STATE_BACKGROUND);
}

/*
 * Feature: Ams
 * Function: AbilityBehaviorAnalysis
 * SubFunction: NA
 * FunctionPoints: Ability Status change received
 * EnvConditions: system running normally
 * CaseDescription: Ability record update(specify process mode)
 */
HWTEST_F(AmsAppLifeCycleModuleTest, AbilityBehaviorAnalysis_01, TestSize.Level1)
{
    const int32_t formateVaule = 0;
    const int32_t visibility = 1;
    const int32_t perceptibility = 1;
    const int32_t connectionState = 1;

    pid_t pid = 123;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "pNew", "com.ohos.test.special");
    auto appInfo = GetApplicationInfo("com.ohos.test.special");
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();

    sptr<BundleMgrService> bundleMgr = new BundleMgrService();
    serviceInner_->SetBundleManager(bundleMgr.GetRefPtr());

    StartAppProcess(pid);
    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo, nullptr);
    BundleInfo bundleInfo;
    HapModuleInfo hapModuleInfo;
    EXPECT_TRUE(serviceInner_->GetBundleAndHapInfo(*abilityInfo, appInfo, bundleInfo, hapModuleInfo));
    std::shared_ptr<AppRunningRecord> record = serviceInner_->appRunningManager_->CheckAppRunningRecordIsExist(
        appInfo->name, abilityInfo->process, abilityInfo->applicationInfo.uid, bundleInfo);
    if (record == nullptr) {
        EXPECT_TRUE(false);
    } else {
        pid_t newPid = record->GetPriorityObject()->GetPid();
        EXPECT_EQ(newPid, pid);
    }

    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord->GetVisibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetPerceptibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetConnectionState(), formateVaule);

    serviceInner_->AbilityBehaviorAnalysis(token, nullptr, visibility, perceptibility, connectionState);

    auto appRecord = serviceInner_->GetAppRunningRecordByAbilityToken(token);
    auto abilityRecord_1 = appRecord->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord_1->GetToken(), token);
    EXPECT_EQ(abilityRecord_1->GetPreToken(), nullptr);
    EXPECT_EQ(abilityRecord_1->GetVisibility(), visibility);
    EXPECT_EQ(abilityRecord_1->GetPerceptibility(), perceptibility);
    EXPECT_EQ(abilityRecord_1->GetConnectionState(), connectionState);
}

/*
 * Feature: Ams
 * Function: AbilityBehaviorAnalysis
 * SubFunction: NA
 * FunctionPoints: Ability Status change received
 * EnvConditions: system running normally
 * CaseDescription: Ability record update(specify process mode and assign visibility)
 */
HWTEST_F(AmsAppLifeCycleModuleTest, AbilityBehaviorAnalysis_02, TestSize.Level1)
{
    const int32_t formateVaule = 0;
    const int32_t visibility = 1;
    const int32_t perceptibility = 0;
    const int32_t connectionState = 0;

    pid_t pid = 123;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "pNew", "com.ohos.test.special");
    auto appInfo = GetApplicationInfo("com.ohos.test.special");
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();

    sptr<BundleMgrService> bundleMgr = new BundleMgrService();
    serviceInner_->SetBundleManager(bundleMgr.GetRefPtr());

    StartAppProcess(pid);
    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo, nullptr);
    BundleInfo bundleInfo;
    HapModuleInfo hapModuleInfo;
    EXPECT_TRUE(serviceInner_->GetBundleAndHapInfo(*abilityInfo, appInfo, bundleInfo, hapModuleInfo));
    std::shared_ptr<AppRunningRecord> record = serviceInner_->appRunningManager_->CheckAppRunningRecordIsExist(
        appInfo->name, abilityInfo->process, abilityInfo->applicationInfo.uid, bundleInfo);
    if (record == nullptr) {
        EXPECT_TRUE(false);
    } else {
        pid_t newPid = record->GetPriorityObject()->GetPid();
        EXPECT_EQ(newPid, pid);
    }

    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord->GetVisibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetPerceptibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetConnectionState(), formateVaule);

    serviceInner_->AbilityBehaviorAnalysis(token, nullptr, visibility, perceptibility, connectionState);

    auto appRecord = serviceInner_->GetAppRunningRecordByAbilityToken(token);
    auto abilityRecord_1 = appRecord->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord_1->GetToken(), token);
    EXPECT_EQ(abilityRecord_1->GetPreToken(), nullptr);
    EXPECT_EQ(abilityRecord_1->GetVisibility(), visibility);
    EXPECT_EQ(abilityRecord_1->GetPerceptibility(), perceptibility);
    EXPECT_EQ(abilityRecord_1->GetConnectionState(), connectionState);
}

/*
 * Feature: Ams
 * Function: AbilityBehaviorAnalysis
 * SubFunction: NA
 * FunctionPoints: Ability Status change received
 * EnvConditions: system running normally
 * CaseDescription: Ability record update(specify process mode and assign perceptibility)
 */
HWTEST_F(AmsAppLifeCycleModuleTest, AbilityBehaviorAnalysis_03, TestSize.Level1)
{
    const int32_t formateVaule = 0;
    const int32_t visibility = 0;
    const int32_t perceptibility = 1;
    const int32_t connectionState = 0;

    pid_t pid = 123;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "pNew", "com.ohos.test.special");
    auto appInfo = GetApplicationInfo("com.ohos.test.special");
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();

    sptr<BundleMgrService> bundleMgr = new BundleMgrService();
    serviceInner_->SetBundleManager(bundleMgr.GetRefPtr());

    StartAppProcess(pid);
    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo, nullptr);
    BundleInfo bundleInfo;
    HapModuleInfo hapModuleInfo;
    EXPECT_TRUE(serviceInner_->GetBundleAndHapInfo(*abilityInfo, appInfo, bundleInfo, hapModuleInfo));
    std::shared_ptr<AppRunningRecord> record = serviceInner_->appRunningManager_->CheckAppRunningRecordIsExist(
        appInfo->name, abilityInfo->process, abilityInfo->applicationInfo.uid, bundleInfo);
    if (record == nullptr) {
        EXPECT_TRUE(false);
    } else {
        pid_t newPid = record->GetPriorityObject()->GetPid();
        EXPECT_EQ(newPid, pid);
    }

    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord->GetVisibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetPerceptibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetConnectionState(), formateVaule);

    serviceInner_->AbilityBehaviorAnalysis(token, nullptr, visibility, perceptibility, connectionState);

    auto appRecord = serviceInner_->GetAppRunningRecordByAbilityToken(token);
    auto abilityRecord_1 = appRecord->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord_1->GetToken(), token);
    EXPECT_EQ(abilityRecord_1->GetPreToken(), nullptr);
    EXPECT_EQ(abilityRecord_1->GetVisibility(), visibility);
    EXPECT_EQ(abilityRecord_1->GetPerceptibility(), perceptibility);
    EXPECT_EQ(abilityRecord_1->GetConnectionState(), connectionState);
}

/*
 * Feature: Ams
 * Function: AbilityBehaviorAnalysis
 * SubFunction: NA
 * FunctionPoints: Ability Status change received
 * EnvConditions: system running normally
 * CaseDescription: Ability record update(specify process mode and assign connectionState)
 */
HWTEST_F(AmsAppLifeCycleModuleTest, AbilityBehaviorAnalysis_04, TestSize.Level1)
{
    const int32_t formateVaule = 0;
    const int32_t visibility = 0;
    const int32_t perceptibility = 0;
    const int32_t connectionState = 1;

    pid_t pid = 123;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "pNew", "com.ohos.test.special");
    auto appInfo = GetApplicationInfo("com.ohos.test.special");
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();

    sptr<BundleMgrService> bundleMgr = new BundleMgrService();
    serviceInner_->SetBundleManager(bundleMgr.GetRefPtr());

    StartAppProcess(pid);
    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo, nullptr);
    BundleInfo bundleInfo;
    HapModuleInfo hapModuleInfo;
    EXPECT_TRUE(serviceInner_->GetBundleAndHapInfo(*abilityInfo, appInfo, bundleInfo, hapModuleInfo));
    std::shared_ptr<AppRunningRecord> record = serviceInner_->appRunningManager_->CheckAppRunningRecordIsExist(
        appInfo->name, abilityInfo->process, abilityInfo->applicationInfo.uid, bundleInfo);
    if (record == nullptr) {
        EXPECT_TRUE(false);
    } else {
        pid_t newPid = record->GetPriorityObject()->GetPid();
        EXPECT_EQ(newPid, pid);
    }

    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord->GetVisibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetPerceptibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetConnectionState(), formateVaule);

    serviceInner_->AbilityBehaviorAnalysis(token, nullptr, visibility, perceptibility, connectionState);

    auto appRecord = serviceInner_->GetAppRunningRecordByAbilityToken(token);
    auto abilityRecord_1 = appRecord->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord_1->GetToken(), token);
    EXPECT_EQ(abilityRecord_1->GetPreToken(), nullptr);
    EXPECT_EQ(abilityRecord_1->GetVisibility(), visibility);
    EXPECT_EQ(abilityRecord_1->GetPerceptibility(), perceptibility);
    EXPECT_EQ(abilityRecord_1->GetConnectionState(), connectionState);
}

/*
 * Feature: Ams
 * Function: AbilityBehaviorAnalysis
 * SubFunction: NA
 * FunctionPoints: Ability Status change received
 * EnvConditions: system running normally
 * CaseDescription: Ability record update(specify process mode and assign proken)
 */
HWTEST_F(AmsAppLifeCycleModuleTest, AbilityBehaviorAnalysis_05, TestSize.Level1)
{
    const int32_t formateVaule = 0;
    const int32_t visibility = 0;
    const int32_t perceptibility = 0;
    const int32_t connectionState = 0;

    pid_t pid = 123;
    sptr<IRemoteObject> token = new (std::nothrow) MockAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "pNew", "com.ohos.test.special");
    auto appInfo = GetApplicationInfo("com.ohos.test.special");
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();

    sptr<BundleMgrService> bundleMgr = new BundleMgrService();
    serviceInner_->SetBundleManager(bundleMgr.GetRefPtr());

    StartAppProcess(pid);
    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo, nullptr);
    BundleInfo bundleInfo;
    HapModuleInfo hapModuleInfo;
    EXPECT_TRUE(serviceInner_->GetBundleAndHapInfo(*abilityInfo, appInfo, bundleInfo, hapModuleInfo));
    std::shared_ptr<AppRunningRecord> record = serviceInner_->appRunningManager_->CheckAppRunningRecordIsExist(
        appInfo->name, abilityInfo->process, abilityInfo->applicationInfo.uid, bundleInfo);
    if (record == nullptr) {
        EXPECT_TRUE(false);
    } else {
        pid_t newPid = record->GetPriorityObject()->GetPid();
        EXPECT_EQ(newPid, pid);
    }

    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord->GetVisibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetPerceptibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetConnectionState(), formateVaule);

    sptr<IRemoteObject> preToken = new (std::nothrow) MockAbilityToken();
    serviceInner_->AbilityBehaviorAnalysis(token, preToken, visibility, perceptibility, connectionState);

    auto appRecord = serviceInner_->GetAppRunningRecordByAbilityToken(token);
    auto abilityRecord_1 = appRecord->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord_1->GetToken(), token);
    EXPECT_EQ(abilityRecord_1->GetPreToken(), preToken);
    EXPECT_EQ(abilityRecord_1->GetVisibility(), visibility);
    EXPECT_EQ(abilityRecord_1->GetPerceptibility(), perceptibility);
    EXPECT_EQ(abilityRecord_1->GetConnectionState(), connectionState);
}

/*
 * Feature: Ams
 * Function: AbilityBehaviorAnalysis
 * SubFunction: NA
 * FunctionPoints: Ability Status change received
 * EnvConditions: system running normally
 * CaseDescription: Ability record update(specify process mode and assign bundleName)
 */
HWTEST_F(AmsAppLifeCycleModuleTest, AbilityBehaviorAnalysis_06, TestSize.Level1)
{
    const int32_t formateVaule = 0;
    const int32_t visibility = 0;
    const int32_t perceptibility = 0;
    const int32_t connectionState = 0;

    pid_t pid = 123;
    sptr<IRemoteObject> token = new (std::nothrow) MockAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "pNew", "com.ohos.test.special");
    auto appInfo = GetApplicationInfo("com.ohos.test.special");
    appInfo->bundleName = appInfo->name;
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();

    sptr<BundleMgrService> bundleMgr = new BundleMgrService();
    serviceInner_->SetBundleManager(bundleMgr.GetRefPtr());

    StartAppProcess(pid);
    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo, nullptr);
    BundleInfo bundleInfo;
    HapModuleInfo hapModuleInfo;
    EXPECT_TRUE(serviceInner_->GetBundleAndHapInfo(*abilityInfo, appInfo, bundleInfo, hapModuleInfo));
    std::shared_ptr<AppRunningRecord> record = serviceInner_->appRunningManager_->CheckAppRunningRecordIsExist(
        appInfo->name, abilityInfo->process, abilityInfo->applicationInfo.uid, bundleInfo);
    if (record == nullptr) {
        EXPECT_TRUE(false);
    } else {
        pid_t newPid = record->GetPriorityObject()->GetPid();
        EXPECT_EQ(newPid, pid);
    }

    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord->GetVisibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetPerceptibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetConnectionState(), formateVaule);

    sptr<IRemoteObject> preToken = new (std::nothrow) MockAbilityToken();
    serviceInner_->AbilityBehaviorAnalysis(token, preToken, visibility, perceptibility, connectionState);

    auto appRecord = serviceInner_->GetAppRunningRecordByAbilityToken(token);
    auto abilityRecord_1 = appRecord->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord_1->GetToken(), token);
    EXPECT_EQ(abilityRecord_1->GetPreToken(), preToken);
    EXPECT_EQ(abilityRecord_1->GetVisibility(), visibility);
    EXPECT_EQ(abilityRecord_1->GetPerceptibility(), perceptibility);
    EXPECT_EQ(abilityRecord_1->GetConnectionState(), connectionState);
}

/*
 * Feature: AbilityMgr
 * Function: LoadAbility
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: NA
 */
HWTEST_F(AmsAppLifeCycleModuleTest, LoadAbility_001, TestSize.Level1)
{
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);
    pid_t pid_0 = 1024;
    pid_t pid_1 = 2048;
    sptr<IRemoteObject> token_0 = new (std::nothrow) MockAbilityToken();
    sptr<IRemoteObject> token_1 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo_0 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld0");
    auto appInfo_0 = GetApplicationInfo("com.ohos.test.helloworld0");
    auto abilityInfo_1 = GetAbilityInfo("0", "MainAbility1", "p2", "com.ohos.test.helloworld0");
    auto appInfo_1 = GetApplicationInfo("com.ohos.test.helloworld0");
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    TestProcessInfo testProcessInfo;

    testProcessInfo.pid = pid_0;
    testProcessInfo.isStart = false;
    auto appRunningRecord_0 =
        StartProcessAndLoadAbility(mockAppScheduler, token_0, abilityInfo_0, appInfo_0, testProcessInfo);

    testProcessInfo.pid = pid_1;
    testProcessInfo.isStart = false;
    auto appRunningRecord_1 =
        StartProcessAndLoadAbility(mockAppScheduler, token_1, abilityInfo_1, appInfo_1, testProcessInfo);
    int32_t recordId_0 = appRunningRecord_0->GetRecordId();
    int32_t recordId_1 = appRunningRecord_1->GetRecordId();
    EXPECT_NE(recordId_0, recordId_1);
    EXPECT_NE(appRunningRecord_0, appRunningRecord_1);
    auto appMap = serviceInner_->appRunningManager_->GetAppRunningRecordMap();
    EXPECT_EQ(2, static_cast<int>(appMap.size()));
}

/*
 * Feature: AbilityMgr
 * Function: LoadAbility
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: NA
 */
HWTEST_F(AmsAppLifeCycleModuleTest, LoadAbility_002, TestSize.Level1)
{
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);
    pid_t pid_0 = 1024;
    sptr<IRemoteObject> token_0 = new (std::nothrow) MockAbilityToken();
    sptr<IRemoteObject> token_1 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo_0 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld0");
    auto appInfo_0 = GetApplicationInfo("com.ohos.test.helloworld0");
    auto abilityInfo_1 = GetAbilityInfo("0", "MainAbility1", "p1", "com.ohos.test.helloworld0");
    auto appInfo_1 = GetApplicationInfo("com.ohos.test.helloworld0");
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    TestProcessInfo testProcessInfo;

    testProcessInfo.pid = pid_0;
    testProcessInfo.isStart = false;
    auto appRunningRecord_0 =
        StartProcessAndLoadAbility(mockAppScheduler, token_0, abilityInfo_0, appInfo_0, testProcessInfo);

    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid_0);
    CheckState(appRunningRecord_0, token_0, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord_0, token_0);
    CheckState(appRunningRecord_0, token_0, AbilityState::ABILITY_STATE_FOREGROUND,
        ApplicationState::APP_STATE_FOREGROUND);

    testProcessInfo.pid = pid_0;
    testProcessInfo.isStart = true;
    auto appRunningRecord_1 =
        StartProcessAndLoadAbility(mockAppScheduler, token_1, abilityInfo_1, appInfo_1, testProcessInfo);
    int32_t recordId_0 = appRunningRecord_0->GetRecordId();
    int32_t recordId_1 = appRunningRecord_1->GetRecordId();
    EXPECT_EQ(recordId_0, recordId_1);
    EXPECT_EQ(appRunningRecord_0, appRunningRecord_1);
    auto appMap = serviceInner_->appRunningManager_->GetAppRunningRecordMap();
    EXPECT_EQ(1, static_cast<int>(appMap.size()));
}

/*
 * Feature: AbilityMgr
 * Function: LoadAbility
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: NA
 */
HWTEST_F(AmsAppLifeCycleModuleTest, LoadAbility_003, TestSize.Level1)
{
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);
    pid_t pid_0 = 1024;
    sptr<IRemoteObject> token_0 = new (std::nothrow) MockAbilityToken();
    sptr<IRemoteObject> token_1 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo_0 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld101");
    auto appInfo_0 = GetApplicationInfo("com.ohos.test.helloworld101");
    auto abilityInfo_1 = GetAbilityInfo("0", "MainAbility1", "p1", "com.ohos.test.helloworld102");
    auto appInfo_1 = GetApplicationInfo("com.ohos.test.helloworld102");
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    TestProcessInfo testProcessInfo;

    testProcessInfo.pid = pid_0;
    testProcessInfo.isStart = false;
    auto appRunningRecord_0 =
        StartProcessAndLoadAbility(mockAppScheduler, token_0, abilityInfo_0, appInfo_0, testProcessInfo);
    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid_0);
    CheckState(appRunningRecord_0, token_0, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord_0, token_0);
    CheckState(appRunningRecord_0, token_0, AbilityState::ABILITY_STATE_FOREGROUND,
        ApplicationState::APP_STATE_FOREGROUND);

    testProcessInfo.pid = pid_0;
    testProcessInfo.isStart = true;
    auto appRunningRecord_1 =
        StartProcessAndLoadAbility(mockAppScheduler, token_1, abilityInfo_1, appInfo_1, testProcessInfo);
    int32_t recordId_0 = appRunningRecord_0->GetRecordId();
    int32_t recordId_1 = appRunningRecord_1->GetRecordId();
    EXPECT_EQ(recordId_0, recordId_1);
    EXPECT_EQ(appRunningRecord_0, appRunningRecord_1);
    auto appMap = serviceInner_->appRunningManager_->GetAppRunningRecordMap();
    EXPECT_EQ(1, static_cast<int>(appMap.size()));
}

/*
 * Feature: AbilityMgr
 * Function: LoadAbility
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: NA
 */
HWTEST_F(AmsAppLifeCycleModuleTest, LoadAbility_004, TestSize.Level1)
{
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);
    pid_t pid_0 = 1024;
    pid_t pid_1 = 2048;
    sptr<IRemoteObject> token_0 = new (std::nothrow) MockAbilityToken();
    sptr<IRemoteObject> token_1 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo_0 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld103");
    auto appInfo_0 = GetApplicationInfo("com.ohos.test.helloworld103");
    auto abilityInfo_1 = GetAbilityInfo("0", "MainAbility1", "p1", "com.ohos.test.helloworld104");
    auto appInfo_1 = GetApplicationInfo("com.ohos.test.helloworld104");
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    TestProcessInfo testProcessInfo;

    testProcessInfo.pid = pid_0;
    testProcessInfo.isStart = false;
    auto appRunningRecord_0 =
        StartProcessAndLoadAbility(mockAppScheduler, token_0, abilityInfo_0, appInfo_0, testProcessInfo);

    testProcessInfo.pid = pid_1;
    testProcessInfo.isStart = false;
    auto appRunningRecord_1 =
        StartProcessAndLoadAbility(mockAppScheduler, token_1, abilityInfo_1, appInfo_1, testProcessInfo);
    int32_t recordId_0 = appRunningRecord_0->GetRecordId();
    int32_t recordId_1 = appRunningRecord_1->GetRecordId();
    EXPECT_NE(recordId_0, recordId_1);
    EXPECT_NE(appRunningRecord_0, appRunningRecord_1);
    auto appMap = serviceInner_->appRunningManager_->GetAppRunningRecordMap();
    EXPECT_EQ(2, static_cast<int>(appMap.size()));
}

/*
 * Feature: AbilityMgr
 * Function: KillApplication
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: NA
 */
HWTEST_F(AmsAppLifeCycleModuleTest, KillApplication_001, TestSize.Level1)
{
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);
    pid_t pid_0 = 1024;
    pid_t pid_1 = 2048;
    sptr<IRemoteObject> token_0 = new (std::nothrow) MockAbilityToken();
    sptr<IRemoteObject> token_1 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo_0 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld0");
    auto appInfo_0 = GetApplicationInfo("com.ohos.test.helloworld0");
    auto abilityInfo_1 = GetAbilityInfo("0", "MainAbility1", "p2", "com.ohos.test.helloworld0");
    auto appInfo_1 = GetApplicationInfo("com.ohos.test.helloworld0");
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    sptr<MockAppScheduler> mockAppScheduler1 = new (std::nothrow) MockAppScheduler();
    TestProcessInfo testProcessInfo;

    testProcessInfo.pid = pid_0;
    testProcessInfo.isStart = false;
    auto appRunningRecord_0 =
        StartProcessAndLoadAbility(mockAppScheduler, token_0, abilityInfo_0, appInfo_0, testProcessInfo);

    testProcessInfo.pid = pid_1;
    testProcessInfo.isStart = false;
    auto appRunningRecord_1 =
        StartProcessAndLoadAbility(mockAppScheduler1, token_1, abilityInfo_1, appInfo_1, testProcessInfo);
    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid_0);
    CheckState(appRunningRecord_0, token_0, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);
    ChangeAbilityStateAfterAppStart(mockAppScheduler1, pid_1);
    CheckState(appRunningRecord_1, token_1, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    int32_t recordId_0 = appRunningRecord_0->GetRecordId();
    int32_t recordId_1 = appRunningRecord_1->GetRecordId();
    EXPECT_NE(recordId_0, recordId_1);
    EXPECT_NE(appRunningRecord_0, appRunningRecord_1);
    auto appMap = serviceInner_->appRunningManager_->GetAppRunningRecordMap();
    EXPECT_EQ(2, static_cast<int>(appMap.size()));
    EXPECT_CALL(*mockAppScheduler, ScheduleProcessSecurityExit()).Times(1);
    EXPECT_CALL(*mockAppScheduler1, ScheduleProcessSecurityExit()).Times(1);

    int32_t ret = serviceInner_->KillApplication("com.ohos.test.helloworld0");
    EXPECT_EQ(ret, 0);
    serviceInner_->OnRemoteDied(mockAppScheduler);  // A faked death recipient.
    auto appMap1 = serviceInner_->appRunningManager_->GetAppRunningRecordMap();
    EXPECT_EQ(1, static_cast<int>(appMap1.size()));
    serviceInner_->OnRemoteDied(mockAppScheduler1);
    auto appMap2 = serviceInner_->appRunningManager_->GetAppRunningRecordMap();
    EXPECT_EQ(0, static_cast<int>(appMap2.size()));
}

/*
 * Feature: AbilityMgr
 * Function: KillApplication
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: NA
 */
HWTEST_F(AmsAppLifeCycleModuleTest, KillApplication_002, TestSize.Level1)
{
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);
    pid_t pid_0 = 1024;
    pid_t pid_1 = 2048;
    sptr<IRemoteObject> token_0 = new (std::nothrow) MockAbilityToken();
    sptr<IRemoteObject> token_1 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo_0 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld103");
    auto appInfo_0 = GetApplicationInfo("com.ohos.test.helloworld103");
    auto abilityInfo_1 = GetAbilityInfo("0", "MainAbility1", "p1", "com.ohos.test.helloworld104");
    auto appInfo_1 = GetApplicationInfo("com.ohos.test.helloworld104");
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    sptr<MockAppScheduler> mockAppScheduler1 = new (std::nothrow) MockAppScheduler();
    TestProcessInfo testProcessInfo;

    testProcessInfo.pid = pid_0;
    testProcessInfo.isStart = false;
    auto appRunningRecord_0 =
        StartProcessAndLoadAbility(mockAppScheduler, token_0, abilityInfo_0, appInfo_0, testProcessInfo);

    testProcessInfo.pid = pid_1;
    testProcessInfo.isStart = false;
    auto appRunningRecord_1 =
        StartProcessAndLoadAbility(mockAppScheduler1, token_1, abilityInfo_1, appInfo_1, testProcessInfo);
    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid_0);
    CheckState(appRunningRecord_0, token_0, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);
    ChangeAbilityStateAfterAppStart(mockAppScheduler1, pid_1);
    CheckState(appRunningRecord_1, token_1, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);
    int32_t recordId_0 = appRunningRecord_0->GetRecordId();
    int32_t recordId_1 = appRunningRecord_1->GetRecordId();
    EXPECT_NE(recordId_0, recordId_1);
    EXPECT_NE(appRunningRecord_0, appRunningRecord_1);
    auto appMap = serviceInner_->appRunningManager_->GetAppRunningRecordMap();
    EXPECT_EQ(2, static_cast<int>(appMap.size()));
    EXPECT_CALL(*mockAppScheduler, ScheduleProcessSecurityExit()).Times(1);

    int32_t ret = serviceInner_->KillApplication("com.ohos.test.helloworld103");
    EXPECT_EQ(ret, 0);
    serviceInner_->OnRemoteDied(mockAppScheduler);  // A faked death recipient.
    auto appMap1 = serviceInner_->appRunningManager_->GetAppRunningRecordMap();
    EXPECT_EQ(1, static_cast<int>(appMap1.size()));
}

/*
 * Feature: AbilityMgr
 * Function: KillApplication
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: NA
 */
HWTEST_F(AmsAppLifeCycleModuleTest, KillApplication_003, TestSize.Level1)
{
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);
    pid_t pid_0 = 1024;
    sptr<IRemoteObject> token_0 = new (std::nothrow) MockAbilityToken();
    sptr<IRemoteObject> token_1 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo_0 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld101");
    auto appInfo_0 = GetApplicationInfo("com.ohos.test.helloworld101");
    auto abilityInfo_1 = GetAbilityInfo("0", "MainAbility1", "p1", "com.ohos.test.helloworld102");
    auto appInfo_1 = GetApplicationInfo("com.ohos.test.helloworld102");
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    TestProcessInfo testProcessInfo;

    testProcessInfo.pid = pid_0;
    testProcessInfo.isStart = false;
    auto appRunningRecord_0 =
        StartProcessAndLoadAbility(mockAppScheduler, token_0, abilityInfo_0, appInfo_0, testProcessInfo);
    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid_0);
    CheckState(appRunningRecord_0, token_0, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord_0, token_0);
    CheckState(appRunningRecord_0, token_0, AbilityState::ABILITY_STATE_FOREGROUND,
        ApplicationState::APP_STATE_FOREGROUND);

    testProcessInfo.pid = pid_0;
    testProcessInfo.isStart = true;
    auto appRunningRecord_1 =
        StartProcessAndLoadAbility(mockAppScheduler, token_1, abilityInfo_1, appInfo_1, testProcessInfo);
    int32_t recordId_0 = appRunningRecord_0->GetRecordId();
    int32_t recordId_1 = appRunningRecord_1->GetRecordId();
    EXPECT_EQ(recordId_0, recordId_1);
    EXPECT_EQ(appRunningRecord_0, appRunningRecord_1);
    auto appMap = serviceInner_->appRunningManager_->GetAppRunningRecordMap();
    EXPECT_EQ(1, static_cast<int>(appMap.size()));
    EXPECT_CALL(*mockAppScheduler, ScheduleProcessSecurityExit()).Times(1);

    int32_t ret = serviceInner_->KillApplication("com.ohos.test.helloworld101");
    EXPECT_EQ(ret, 0);
    serviceInner_->OnRemoteDied(mockAppScheduler);  // A faked death recipient.
    auto appMap1 = serviceInner_->appRunningManager_->GetAppRunningRecordMap();
    EXPECT_EQ(0, static_cast<int>(appMap1.size()));
}

/*
 * Feature: AbilityMgr
 * Function: UpdateConfiguration
 * SubFunction: NA
 * FunctionPoints: Environmental Change Notification
 * EnvConditions: NA
 * CaseDescription: Start two apps and then make environment change notifications
 */
HWTEST_F(AmsAppLifeCycleModuleTest, UpdateConfiguration_001, TestSize.Level1)
{
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);
    pid_t pid_0 = 1024;
    pid_t pid_1 = 2048;
    sptr<IRemoteObject> token_0 = new (std::nothrow) MockAbilityToken();
    sptr<IRemoteObject> token_1 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo_0 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld0");
    auto appInfo_0 = GetApplicationInfo("com.ohos.test.helloworld0");
    auto abilityInfo_1 = GetAbilityInfo("0", "MainAbility1", "p2", "com.ohos.test.helloworld0");
    auto appInfo_1 = GetApplicationInfo("com.ohos.test.helloworld0");
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    sptr<MockAppScheduler> mockAppScheduler1 = new (std::nothrow) MockAppScheduler();
    TestProcessInfo testProcessInfo;

    testProcessInfo.pid = pid_0;
    testProcessInfo.isStart = false;
    auto appRunningRecord_0 =
        StartProcessAndLoadAbility(mockAppScheduler, token_0, abilityInfo_0, appInfo_0, testProcessInfo);

    testProcessInfo.pid = pid_1;
    testProcessInfo.isStart = false;
    auto appRunningRecord_1 =
        StartProcessAndLoadAbility(mockAppScheduler1, token_1, abilityInfo_1, appInfo_1, testProcessInfo);
    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid_0);
    CheckState(appRunningRecord_0, token_0, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);
    ChangeAbilityStateAfterAppStart(mockAppScheduler1, pid_1);
    CheckState(appRunningRecord_1, token_1, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    auto testLanguge = std::string("ch-zh");
    auto configUpdate = [testLanguge](const Configuration &config) {
        auto l = config.GetItem(AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE);
        EXPECT_TRUE(testLanguge == l);
    };

    Configuration config;
    config.AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE, testLanguge);

    auto appMap = serviceInner_->appRunningManager_->GetAppRunningRecordMap();
    EXPECT_EQ(2, static_cast<int>(appMap.size()));

    EXPECT_CALL(*mockAppScheduler, ScheduleConfigurationUpdated(_))
        .Times(1)
        .WillOnce(testing::Invoke(configUpdate));
    EXPECT_CALL(*mockAppScheduler1, ScheduleConfigurationUpdated(_))
        .Times(1)
        .WillOnce(testing::Invoke(configUpdate));

    serviceInner_->UpdateConfiguration(config);
}

/*
 * Feature: AbilityMgr
 * Function: UpdateConfiguration
 * SubFunction: NA
 * FunctionPoints: Environmental Change Notification
 * EnvConditions: NA
 * CaseDescription: Verify duplicate notification conditions
 */
HWTEST_F(AmsAppLifeCycleModuleTest, UpdateConfiguration_002, TestSize.Level1)
{
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);
    pid_t pid_0 = 1024;
    pid_t pid_1 = 2048;
    sptr<IRemoteObject> token_0 = new (std::nothrow) MockAbilityToken();
    sptr<IRemoteObject> token_1 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo_0 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld0");
    auto appInfo_0 = GetApplicationInfo("com.ohos.test.helloworld0");
    auto abilityInfo_1 = GetAbilityInfo("0", "MainAbility1", "p2", "com.ohos.test.helloworld0");
    auto appInfo_1 = GetApplicationInfo("com.ohos.test.helloworld0");
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    sptr<MockAppScheduler> mockAppScheduler1 = new (std::nothrow) MockAppScheduler();
    TestProcessInfo testProcessInfo;

    testProcessInfo.pid = pid_0;
    testProcessInfo.isStart = false;
    auto appRunningRecord_0 =
        StartProcessAndLoadAbility(mockAppScheduler, token_0, abilityInfo_0, appInfo_0, testProcessInfo);

    testProcessInfo.pid = pid_1;
    testProcessInfo.isStart = false;
    auto appRunningRecord_1 =
        StartProcessAndLoadAbility(mockAppScheduler1, token_1, abilityInfo_1, appInfo_1, testProcessInfo);
    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid_0);
    CheckState(appRunningRecord_0, token_0, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);
    ChangeAbilityStateAfterAppStart(mockAppScheduler1, pid_1);
    CheckState(appRunningRecord_1, token_1, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    auto testLanguge = std::string("ch-zh");
    auto again = std::string("Russian");
    auto displayId = 10;
    auto configUpdate = [testLanguge, displayId](const Configuration &config) {
        auto l = config.GetItem(displayId, AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE);
        EXPECT_TRUE(testLanguge == l);
    };

    auto configUpdateAgain = [again, displayId](const Configuration &config) {
        auto l = config.GetItem(displayId, AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE);
        EXPECT_TRUE(again == l);
    };

    Configuration config;
    config.AddItem(displayId, AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE, testLanguge);

    auto appMap = serviceInner_->appRunningManager_->GetAppRunningRecordMap();
    EXPECT_EQ(2, static_cast<int>(appMap.size()));

    EXPECT_CALL(*mockAppScheduler, ScheduleConfigurationUpdated(_))
        .Times(2)
        .WillOnce(testing::Invoke(configUpdate))
        .WillOnce(testing::Invoke(configUpdateAgain));

    EXPECT_CALL(*mockAppScheduler1, ScheduleConfigurationUpdated(_))
        .Times(2)
        .WillOnce(testing::Invoke(configUpdate))
        .WillOnce(testing::Invoke(configUpdateAgain));

    serviceInner_->UpdateConfiguration(config);

    config.AddItem(displayId, AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE, again);

    serviceInner_->UpdateConfiguration(config);
}

/*
 * Feature: AbilityMgr
 * Function: UpdateConfiguration
 * SubFunction: NA
 * FunctionPoints: Environmental Change Notification
 * EnvConditions: NA
 * CaseDescription: Two types of notifications
 */
HWTEST_F(AmsAppLifeCycleModuleTest, UpdateConfiguration_003, TestSize.Level1)
{
    EXPECT_NE(serviceInner_, nullptr);
    CHECK_POINTER_IS_NULLPTR(serviceInner_);
    pid_t pid_0 = 1024;
    pid_t pid_1 = 2048;
    sptr<IRemoteObject> token_0 = new (std::nothrow) MockAbilityToken();
    sptr<IRemoteObject> token_1 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo_0 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld0");
    auto appInfo_0 = GetApplicationInfo("com.ohos.test.helloworld0");
    auto abilityInfo_1 = GetAbilityInfo("0", "MainAbility1", "p2", "com.ohos.test.helloworld0");
    auto appInfo_1 = GetApplicationInfo("com.ohos.test.helloworld0");
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    sptr<MockAppScheduler> mockAppScheduler1 = new (std::nothrow) MockAppScheduler();
    TestProcessInfo testProcessInfo;

    testProcessInfo.pid = pid_0;
    testProcessInfo.isStart = false;
    auto appRunningRecord_0 =
        StartProcessAndLoadAbility(mockAppScheduler, token_0, abilityInfo_0, appInfo_0, testProcessInfo);

    testProcessInfo.pid = pid_1;
    testProcessInfo.isStart = false;
    auto appRunningRecord_1 =
        StartProcessAndLoadAbility(mockAppScheduler1, token_1, abilityInfo_1, appInfo_1, testProcessInfo);
    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid_0);
    CheckState(appRunningRecord_0, token_0, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);
    ChangeAbilityStateAfterAppStart(mockAppScheduler1, pid_1);
    CheckState(appRunningRecord_1, token_1, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    auto testLanguge = std::string("ch-zh");
    auto displayId = 10;
    auto configUpdate = [testLanguge, displayId](const Configuration &config) {
        auto ld = config.GetItem(displayId, AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE);
        auto l = config.GetItem(displayId, AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE);
        EXPECT_TRUE(testLanguge == ld);
        EXPECT_TRUE(testLanguge == l);
    };

    Configuration config;
    config.AddItem(displayId, AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE, testLanguge);
    config.AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE, testLanguge);

    auto appMap = serviceInner_->appRunningManager_->GetAppRunningRecordMap();
    EXPECT_EQ(2, static_cast<int>(appMap.size()));

    EXPECT_CALL(*mockAppScheduler, ScheduleConfigurationUpdated(_))
        .Times(1)
        .WillOnce(testing::Invoke(configUpdate));

    EXPECT_CALL(*mockAppScheduler1, ScheduleConfigurationUpdated(_))
        .Times(1)
        .WillOnce(testing::Invoke(configUpdate));

    serviceInner_->UpdateConfiguration(config);
}

/*
 * Feature: AbilityMgr
 * Function: LoadResidentProcess
 * SubFunction: NA
 * FunctionPoints: start resident process
 * EnvConditions: NA
 * CaseDescription: Start a resident process normally
 */
HWTEST_F(AmsAppLifeCycleModuleTest, LoadResidentProcess_001, TestSize.Level1)
{
    std::string appName = "KeepAliveApp";
    std::string proc = "KeepAliveApplication";
    int uid = 2100;
    BundleInfo info;
    info.name = proc;
    info.uid = uid;

    ApplicationInfo appInfo;
    appInfo.name = "KeepAliveApp";
    appInfo.bundleName = "KeepAliveApplication";
    appInfo.uid = 2100;

    info.applicationInfo = appInfo;
    HapModuleInfo hapModuleInfo;
    hapModuleInfo.name = "Module";
    HapModuleInfo hapModuleInfo1;
    hapModuleInfo1.name = "Module1";
    info.hapModuleInfos.push_back(hapModuleInfo);
    info.hapModuleInfos.push_back(hapModuleInfo1);

    auto appRecord = serviceInner_->appRunningManager_->CheckAppRunningRecordIsExist(appName, proc, uid, info);
    EXPECT_FALSE(appRecord);
}

/*
 * Feature: AbilityMgr
 * Function: LoadResidentProcess
 * SubFunction: NA
 * FunctionPoints: start resident process
 * EnvConditions: NA
 * CaseDescription: Start multiple resident processes
 */
HWTEST_F(AmsAppLifeCycleModuleTest, LoadResidentProcess_002, TestSize.Level1)
{
    std::vector<BundleInfo> infos;
    std::string appName = "KeepAliveApp";
    std::string proc = "KeepAliveApplication";
    int uid = 2100;

    HapModuleInfo hapModuleInfo;
    hapModuleInfo.moduleName = "KeepAliveApplication";
    BundleInfo info;
    info.name = proc;
    info.uid = uid;
    info.isKeepAlive = true;
    info.appId = "com.ohos.test.helloworld_code123";
    info.hapModuleInfos.push_back(hapModuleInfo);

    ApplicationInfo appInfo;
    appInfo.name = appName;
    appInfo.bundleName = proc;
    appInfo.uid = 2100;
    info.applicationInfo = appInfo;

    int appUid1 = 2101;
    std::string appName1 = "KeepAliveApp1";
    std::string proc1 = "KeepAliveApplication1";

    HapModuleInfo hapModuleInfo1;
    hapModuleInfo1.moduleName = proc1;

    ApplicationInfo appInfo1;
    appInfo1.name = appName1;
    appInfo1.bundleName = proc1;
    appInfo1.uid = appUid1;

    BundleInfo info1;
    info1.name = proc1;
    info1.uid = appUid1;
    info1.isKeepAlive = true;
    info1.appId = "com.ohos.test.helloworld_code123";
    info1.hapModuleInfos.push_back(hapModuleInfo1);
    info1.applicationInfo = appInfo1;

    infos.push_back(info);
    infos.push_back(info1);

    MockAppSpawnClient *mockClientPtr = new (std::nothrow) MockAppSpawnClient();
    EXPECT_TRUE(mockClientPtr);
    if (mockClientPtr) {
        serviceInner_->SetAppSpawnClient(std::unique_ptr<MockAppSpawnClient>(mockClientPtr));

        // start process
        serviceInner_->StartResidentProcess(infos, -1, true);
        serviceInner_->appRunningManager_->GetAppRunningRecordMap();
    }
}

/*
 * Feature: AbilityMgr
 * Function: RestartResidentProcess
 * SubFunction: NA
 * FunctionPoints: Resident process exception recovery
 * EnvConditions: NA
 * CaseDescription: Start the resident process and let it die abnormally, then resume
 */
HWTEST_F(AmsAppLifeCycleModuleTest, RestartResidentProcess_001, TestSize.Level1)
{
    pid_t pid = 123;
    std::string appName = "KeepAliveApp";
    std::string proc = "KeepAliveApplication";
    int uid = 2100;

    HapModuleInfo hapModuleInfo;
    hapModuleInfo.moduleName = "KeepAliveApplication";
    std::vector<BundleInfo> infos;
    BundleInfo info;
    info.name = proc;
    info.uid = uid;
    info.appId = "com.ohos.test.helloworld_code123";
    info.hapModuleInfos.push_back(hapModuleInfo);

    ApplicationInfo appInfo;
    appInfo.name = "KeepAliveApp";
    appInfo.bundleName = "KeepAliveApplication";
    appInfo.uid = 2100;
    info.applicationInfo = appInfo;
    infos.push_back(info);

    MockAppSpawnClient *mockClientPtr = new (std::nothrow) MockAppSpawnClient();
    EXPECT_TRUE(mockClientPtr);
    serviceInner_->SetAppSpawnClient(std::unique_ptr<MockAppSpawnClient>(mockClientPtr));
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());

    // start process
    serviceInner_->StartResidentProcess(infos, -1, true);
    auto appRecord = serviceInner_->appRunningManager_->CheckAppRunningRecordIsExist(appName, proc, uid, info);
    if (appRecord == nullptr) {
        EXPECT_FALSE(appRecord);
    } else {
        EXPECT_TRUE(appRecord->GetUid() == uid);
        EXPECT_TRUE(appRecord->GetProcessName() == proc);
        EXPECT_TRUE(appRecord->IsKeepAliveApp());

        serviceInner_->AttachApplication(pid, client);

        serviceInner_->OnRemoteDied(mockAppScheduler, false);
        appRecord = serviceInner_->appRunningManager_->CheckAppRunningRecordIsExist(appName, proc, uid, info);
        EXPECT_FALSE(appRecord);

        sleep(1);
        appRecord = serviceInner_->appRunningManager_->CheckAppRunningRecordIsExist(appName, proc, uid, info);
        EXPECT_TRUE(appRecord);
    }
}

/*
 * Feature: AbilityMgr
 * Function: RestartResidentProcess
 * SubFunction: NA
 * FunctionPoints: Resident process exception recovery
 * EnvConditions: NA
 * CaseDescription: Start an ordinary process, it should not be pulled up after abnormal death
 */
HWTEST_F(AmsAppLifeCycleModuleTest, RestartResidentProcess_002, TestSize.Level1)
{
    pid_t pid_0 = 1024;

    sptr<IRemoteObject> token_0 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo_0 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld103");
    auto appInfo_0 = GetApplicationInfo("com.ohos.test.helloworld103");

    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    TestProcessInfo testProcessInfo;

    testProcessInfo.pid = pid_0;
    testProcessInfo.isStart = false;
    auto appRunningRecord_0 =
        StartProcessAndLoadAbility(mockAppScheduler, token_0, abilityInfo_0, appInfo_0, testProcessInfo);
    EXPECT_TRUE(appRunningRecord_0);

    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    serviceInner_->AttachApplication(pid_0, client);
    serviceInner_->OnRemoteDied(mockAppScheduler, false);

    sleep(1);
    auto recordMap = serviceInner_->appRunningManager_->GetAppRunningRecordMap();
    EXPECT_TRUE(recordMap.empty());
}

/*
 * Feature: AbilityMgr
 * Function: RestartResidentProcess
 * SubFunction: NA
 * FunctionPoints: Resident process exception recovery
 * EnvConditions: NA
 * CaseDescription:  1.start a normal process
 *                   2.start a resident process
 *                   3.make both processes die
 */
HWTEST_F(AmsAppLifeCycleModuleTest, RestartResidentProcess_003, TestSize.Level1)
{
    pid_t pid_0 = 1024;

    sptr<IRemoteObject> token_0 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo_0 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld103");
    auto appInfo_0 = GetApplicationInfo("com.ohos.test.helloworld103");

    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    TestProcessInfo testProcessInfo;

    testProcessInfo.pid = pid_0;
    testProcessInfo.isStart = false;
    auto appRunningRecord_0 =
        StartProcessAndLoadAbility(mockAppScheduler, token_0, abilityInfo_0, appInfo_0, testProcessInfo);
    EXPECT_TRUE(appRunningRecord_0);

    pid_t pid = 123;
    std::string appName = "KeepAliveApp";
    std::string proc = "KeepAliveApplication";
    int uid = 2100;

    HapModuleInfo hapModuleInfo;
    hapModuleInfo.moduleName = "KeepAliveApplication";
    std::vector<BundleInfo> infos;
    BundleInfo info;
    info.name = proc;
    info.uid = uid;
    info.isKeepAlive = true;
    info.appId = "com.ohos.test.helloworld_code123";
    info.hapModuleInfos.push_back(hapModuleInfo);

    ApplicationInfo appInfo;
    appInfo.name = "KeepAliveApp";
    appInfo.bundleName = "KeepAliveApplication";
    appInfo.uid = 2100;
    info.applicationInfo = appInfo;
    infos.push_back(info);

    MockAppSpawnClient *mockClientPtr = new (std::nothrow) MockAppSpawnClient();
    EXPECT_TRUE(mockClientPtr);

    serviceInner_->SetAppSpawnClient(std::unique_ptr<MockAppSpawnClient>(mockClientPtr));

    // start process
    serviceInner_->StartResidentProcess(infos, -1, true);
    auto residentRecord = serviceInner_->appRunningManager_->CheckAppRunningRecordIsExist(appName, proc, uid, info);
    if (residentRecord == nullptr) {
        EXPECT_FALSE(residentRecord);
        return;
    }
    EXPECT_TRUE(residentRecord);
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    serviceInner_->AttachApplication(pid_0, client);

    sptr<MockAppScheduler> mockAppSchedulerResident = new (std::nothrow) MockAppScheduler();
    EXPECT_CALL(*mockAppSchedulerResident, ScheduleLaunchApplication(_, _));
    sptr<IAppScheduler> residentClient = iface_cast<IAppScheduler>(mockAppSchedulerResident.GetRefPtr());
    serviceInner_->AttachApplication(pid, residentClient);

    serviceInner_->OnRemoteDied(mockAppScheduler, false);
    serviceInner_->OnRemoteDied(mockAppSchedulerResident, false);

    auto recordMap = serviceInner_->appRunningManager_->GetAppRunningRecordMap();
    EXPECT_TRUE(recordMap.empty());

    sleep(1);
    auto appRecord = serviceInner_->appRunningManager_->CheckAppRunningRecordIsExist(appName, proc, uid, info);
    if (appRecord == nullptr) {
        EXPECT_FALSE(appRecord);
    } else {
        EXPECT_TRUE(appRecord);
    }
}

/*
 * Feature: AbilityMgr
 * Function: KillProcessWithAccount
 * SubFunction: NA
 * FunctionPoints: Test KillProcessWithAccount
 * EnvConditions: NA
 * CaseDescription: Specify user to kill process
 */
HWTEST_F(AmsAppLifeCycleModuleTest, KillProcessWithAccount_001, TestSize.Level1)
{
    const std::string STRING_BUNDLE_NAME = "com.ix.hiworld";
    constexpr int ACCOUNT_ID = 100;
    auto instance = DelayedSingleton<SysMrgClient>::GetInstance();
    EXPECT_NE(instance, nullptr);

    auto object = instance->GetSystemAbility(APP_MGR_SERVICE_ID);
    EXPECT_NE(object, nullptr);

    auto proxy = iface_cast<IAppMgr>(object);
    EXPECT_NE(proxy, nullptr);

    ErrCode result = proxy->GetAmsMgr()->KillProcessWithAccount(STRING_BUNDLE_NAME, ACCOUNT_ID);
    EXPECT_EQ(result, ERR_OK);
}
}  // namespace AppExecFwk
}  // namespace OHOS
