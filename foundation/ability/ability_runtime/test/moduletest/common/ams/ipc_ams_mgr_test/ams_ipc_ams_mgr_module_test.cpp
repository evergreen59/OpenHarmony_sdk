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
#include <unistd.h>

#define private public
#include "app_mgr_client.h"
#undef private
#include "ams_mgr_scheduler.h"
#include "app_mgr_proxy.h"
#include "app_mgr_service_event_handler.h"
#include "app_record_id.h"
#include "app_scheduler_proxy.h"
#include "mock_ability_token.h"
#include "mock_application.h"
#include "mock_iapp_state_callback.h"
#include "mock_app_mgr_service.h"
#include "mock_app_mgr_service_inner.h"
#include "mock_app_service_mgr.h"
#include "mock_native_token.h"
#include "iremote_object.h"

using namespace testing::ext;
using OHOS::iface_cast;
using OHOS::sptr;
using testing::_;
using testing::Invoke;
using testing::InvokeWithoutArgs;

namespace OHOS {
namespace AppExecFwk {
namespace {
const int32_t COUNT = 1000;
}  // namespace
class AmsIpcAmsmgrModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::shared_ptr<ApplicationInfo> GetApplicationInfo(const std::string &appName) const;
    std::shared_ptr<AbilityInfo> GetAbilityInfo(
        const std::string &abilityIndex, const std::string &name, const std::string &applicationName) const;

    sptr<MockAbilityToken> GetAbilityToken()
    {
        mockToken_ = new (std::nothrow) MockAbilityToken();
        return mockToken_;
    }

    std::shared_ptr<MockAppMgrServiceInner> GetMockAppMgrServiceInner();
    std::shared_ptr<AMSEventHandler> GetAmsEventHandler();

protected:
    sptr<MockAbilityToken> mockToken_ = nullptr;
    sptr<MockAbilityToken> token_ {nullptr};
    std::unique_ptr<AppMgrClient> client_ {nullptr};

    std::shared_ptr<MockAppMgrServiceInner> mockAppMgrServiceInner_ {nullptr};
    std::shared_ptr<AMSEventHandler> amsEventHandler_ {nullptr};
};

class MockMockAppMgrService : public MockAppMgrService {
public:
    MOCK_METHOD0(GetAmsMgr, sptr<IAmsMgr>());
    MOCK_METHOD1(ClearUpApplicationData, int32_t(const std::string &));
    MOCK_METHOD1(IsBackgroundRunningRestricted, int(const std::string &appName));
    MOCK_METHOD1(GetAllRunningProcesses, int(std::vector<RunningProcessInfo> &));
};

void AmsIpcAmsmgrModuleTest::SetUpTestCase()
{
    MockNativeToken::SetNativeToken();
}

void AmsIpcAmsmgrModuleTest::TearDownTestCase()
{}

void AmsIpcAmsmgrModuleTest::SetUp()
{
    token_ = new (std::nothrow) MockAbilityToken();
}

void AmsIpcAmsmgrModuleTest::TearDown()
{
    amsEventHandler_.reset();
    mockAppMgrServiceInner_.reset();
}

std::shared_ptr<ApplicationInfo> AmsIpcAmsmgrModuleTest::GetApplicationInfo(const std::string &appName) const
{
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = appName;
    return appInfo;
}

std::shared_ptr<AbilityInfo> AmsIpcAmsmgrModuleTest::GetAbilityInfo(
    const std::string &abilityIndex, const std::string &name, const std::string &applicationName) const
{
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = name + abilityIndex;
    abilityInfo->applicationName = applicationName;
    return abilityInfo;
}

std::shared_ptr<MockAppMgrServiceInner> AmsIpcAmsmgrModuleTest::GetMockAppMgrServiceInner()
{
    if (!mockAppMgrServiceInner_) {
        mockAppMgrServiceInner_ = std::make_shared<MockAppMgrServiceInner>();
    }
    return mockAppMgrServiceInner_;
}

std::shared_ptr<AMSEventHandler> AmsIpcAmsmgrModuleTest::GetAmsEventHandler()
{
    if (!amsEventHandler_) {
        auto mockAppMgrServiceInner = GetMockAppMgrServiceInner();
        amsEventHandler_ =
            std::make_shared<AMSEventHandler>(EventRunner::Create("AmsMgrSchedulerTest"), mockAppMgrServiceInner);
    }
    return amsEventHandler_;
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: AmsmgrIPCInterface
 * FunctionPoints: test terminateAbility API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: execute terminateAbility API 1000 times
 */
HWTEST_F(AmsIpcAmsmgrModuleTest, ExcuteAmsmgrIPCInterface_002, TestSize.Level3)
{
    auto mockAppMgrServiceInner = GetMockAppMgrServiceInner();
    auto amsEventHandler = GetAmsEventHandler();
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler =
        std::make_unique<AmsMgrScheduler>(mockAppMgrServiceInner, amsEventHandler);

    sptr<MockMockAppMgrService> mockMockAppMgr(new MockMockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockMockAppMgr);

    auto mockHandler = [&]() -> sptr<IAmsMgr> {
        mockMockAppMgr->Post();
        return sptr<IAmsMgr>(amsMgrScheduler.get());
    };

    EXPECT_CALL(*mockMockAppMgr, GetAmsMgr()).Times(1).WillOnce(Invoke(mockHandler));

    auto amsMgrScheduler_ = appMgrClient->GetAmsMgr();
    mockMockAppMgr->Wait();
    sptr<OHOS::IRemoteObject> token = new MockAbilityToken();

    for (int i = 0; i < COUNT; i++) {
        EXPECT_CALL(*mockAppMgrServiceInner, TerminateAbility(_, _))
            .WillOnce(InvokeWithoutArgs(mockAppMgrServiceInner.get(), &MockAppMgrServiceInner::Post));
        amsMgrScheduler_->TerminateAbility(token_, false);
        mockAppMgrServiceInner->Wait();
    }

    mockAppMgrServiceInner.reset();
    amsMgrScheduler.release();
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: AmsmgrIPCInterface
 * FunctionPoints: test updateAbilityState API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: execute updateAbilityState API 1000 times
 */
HWTEST_F(AmsIpcAmsmgrModuleTest, ExcuteAmsmgrIPCInterface_003, TestSize.Level3)
{
    auto mockAppMgrServiceInner = GetMockAppMgrServiceInner();
    auto amsEventHandler = GetAmsEventHandler();
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler =
        std::make_unique<AmsMgrScheduler>(mockAppMgrServiceInner, amsEventHandler);

    sptr<MockMockAppMgrService> mockMockAppMgr(new MockMockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockMockAppMgr);

    auto mockHandler = [&]() -> sptr<IAmsMgr> {
        mockMockAppMgr->Post();
        return sptr<IAmsMgr>(amsMgrScheduler.get());
    };

    EXPECT_CALL(*mockMockAppMgr, GetAmsMgr()).Times(1).WillOnce(Invoke(mockHandler));

    auto amsMgrScheduler_ = appMgrClient->GetAmsMgr();
    mockMockAppMgr->Wait();

    sptr<OHOS::IRemoteObject> token = new MockAbilityToken();
    AbilityState abilityState = AbilityState::ABILITY_STATE_BEGIN;

    for (int i = 0; i < COUNT; i++) {
        EXPECT_CALL(*mockAppMgrServiceInner, UpdateAbilityState(_, _))
            .WillOnce(InvokeWithoutArgs(mockAppMgrServiceInner.get(), &MockAppMgrServiceInner::Post));
        amsMgrScheduler_->UpdateAbilityState(token, abilityState);
        mockAppMgrServiceInner->Wait();
    }

    mockAppMgrServiceInner.reset();
    amsMgrScheduler.release();
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: AmsmgrIPCInterface
 * FunctionPoints: test registerappstatecallback API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: execute registerappstatecallback API 1000 times
 */
HWTEST_F(AmsIpcAmsmgrModuleTest, ExcuteAmsmgrIPCInterface_004, TestSize.Level3)
{
    auto mockAppMgrServiceInner = GetMockAppMgrServiceInner();
    auto amsEventHandler = GetAmsEventHandler();
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler =
        std::make_unique<AmsMgrScheduler>(mockAppMgrServiceInner, amsEventHandler);
    sptr<MockAppStateCallback> callback = new MockAppStateCallback();

    sptr<MockMockAppMgrService> mockMockAppMgr(new MockMockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockMockAppMgr);

    auto mockHandler = [&]() -> sptr<IAmsMgr> {
        mockMockAppMgr->Post();
        return sptr<IAmsMgr>(amsMgrScheduler.get());
    };

    EXPECT_CALL(*mockMockAppMgr, GetAmsMgr()).Times(1).WillOnce(Invoke(mockHandler));

    auto amsMgrScheduler_ = appMgrClient->GetAmsMgr();
    mockMockAppMgr->Wait();

    for (int i = 0; i < COUNT; i++) {
        EXPECT_CALL(*mockAppMgrServiceInner, RegisterAppStateCallback(_))
            .WillOnce(InvokeWithoutArgs(mockAppMgrServiceInner.get(), &MockAppMgrServiceInner::Post));
        amsMgrScheduler_->RegisterAppStateCallback(callback);
        mockAppMgrServiceInner->Wait();
    }

    mockAppMgrServiceInner.reset();
    amsMgrScheduler.release();
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: AmsmgrIPCInterface
 * FunctionPoints: test AbilityBehaviorAnalysis API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: execute AbilityBehaviorAnalysis API 1000 times
 */
HWTEST_F(AmsIpcAmsmgrModuleTest, ExcuteAmsmgrIPCInterface_005, TestSize.Level3)
{
    const int32_t visibility = 1;
    const int32_t perceptibility = 1;
    const int32_t connectionState = 1;

    auto mockAppMgrServiceInner = GetMockAppMgrServiceInner();
    auto amsEventHandler = GetAmsEventHandler();
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler =
        std::make_unique<AmsMgrScheduler>(mockAppMgrServiceInner, amsEventHandler);

    sptr<MockMockAppMgrService> mockMockAppMgr(new MockMockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockMockAppMgr);

    auto mockHandler = [&]() -> sptr<IAmsMgr> {
        mockMockAppMgr->Post();
        return sptr<IAmsMgr>(amsMgrScheduler.get());
    };

    EXPECT_CALL(*mockMockAppMgr, GetAmsMgr()).Times(1).WillOnce(Invoke(mockHandler));

    auto amsMgrScheduler_ = appMgrClient->GetAmsMgr();
    mockMockAppMgr->Wait();
    sptr<OHOS::IRemoteObject> token = new MockAbilityToken();

    for (int i = 0; i < COUNT; i++) {
        EXPECT_CALL(*mockAppMgrServiceInner, AbilityBehaviorAnalysis(_, _, _, _, _))
            .WillOnce(InvokeWithoutArgs(mockAppMgrServiceInner.get(), &MockAppMgrServiceInner::Post));
        amsMgrScheduler_->AbilityBehaviorAnalysis(token, nullptr, visibility, perceptibility, connectionState);
        mockAppMgrServiceInner->Wait();
    }

    mockAppMgrServiceInner.reset();
    amsMgrScheduler.release();
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: AmsmgrIPCInterface
 * FunctionPoints: test KillApplication API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: execute KillApplication API 1000 times
 */
HWTEST_F(AmsIpcAmsmgrModuleTest, ExcuteAmsmgrIPCInterface_006, TestSize.Level3)
{
    const std::string bundleName = "p1";
    auto mockAppMgrServiceInner = GetMockAppMgrServiceInner();
    auto amsEventHandler = GetAmsEventHandler();
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler =
        std::make_unique<AmsMgrScheduler>(mockAppMgrServiceInner, amsEventHandler);

    sptr<MockMockAppMgrService> mockMockAppMgr(new MockMockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockMockAppMgr);

    auto mockHandler = [&]() -> sptr<IAmsMgr> {
        mockMockAppMgr->Post();
        return sptr<IAmsMgr>(amsMgrScheduler.get());
    };

    EXPECT_CALL(*mockMockAppMgr, GetAmsMgr()).Times(1).WillOnce(Invoke(mockHandler));

    auto amsMgrScheduler_ = appMgrClient->GetAmsMgr();
    mockMockAppMgr->Wait();

    for (int i = 0; i < COUNT; i++) {
        EXPECT_CALL(*mockAppMgrServiceInner, KillApplication(_))
            .WillOnce(InvokeWithoutArgs(mockAppMgrServiceInner.get(), &MockAppMgrServiceInner::Post4Int));
        amsMgrScheduler_->KillApplication(bundleName);
        mockAppMgrServiceInner->Wait();
    }

    mockAppMgrServiceInner.reset();
    amsMgrScheduler.release();
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: AmsmgrIPCInterface
 * FunctionPoints: test KillProcessByAbilityToken API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: execute KillProcessByAbilityToken API 1000 times
 */
HWTEST_F(AmsIpcAmsmgrModuleTest, ExcuteAmsmgrIPCInterface_007, TestSize.Level3)
{
    auto mockAppMgrServiceInner = GetMockAppMgrServiceInner();
    auto amsEventHandler = GetAmsEventHandler();
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler =
        std::make_unique<AmsMgrScheduler>(mockAppMgrServiceInner, amsEventHandler);

    sptr<MockMockAppMgrService> mockMockAppMgr(new MockMockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockMockAppMgr);

    auto mockHandler = [&]() -> sptr<IAmsMgr> {
        mockMockAppMgr->Post();
        return sptr<IAmsMgr>(amsMgrScheduler.get());
    };

    EXPECT_CALL(*mockMockAppMgr, GetAmsMgr()).Times(1).WillOnce(Invoke(mockHandler));

    auto amsMgrScheduler_ = appMgrClient->GetAmsMgr();
    mockMockAppMgr->Wait();
    sptr<OHOS::IRemoteObject> token = new MockAbilityToken();

    for (int i = 0; i < COUNT; i++) {
        EXPECT_CALL(*mockAppMgrServiceInner, KillProcessByAbilityToken(_))
            .WillOnce(InvokeWithoutArgs(mockAppMgrServiceInner.get(), &MockAppMgrServiceInner::Post));
        amsMgrScheduler_->KillProcessByAbilityToken(token);
        mockAppMgrServiceInner->Wait();
    }

    mockAppMgrServiceInner.reset();
    amsMgrScheduler.release();
}
}  // namespace AppExecFwk
}  // namespace OHOS
