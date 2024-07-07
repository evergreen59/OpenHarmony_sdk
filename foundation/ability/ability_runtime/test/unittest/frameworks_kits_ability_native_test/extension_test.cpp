/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#define private public
#define protected public
#include "ability_handler.h"
#include "ability_info.h"
#include "ability_local_record.h"
#include "ability_thread.h"
#include "extension.h"
#undef private
#undef protected
#include "event_runner.h"
#include "mock_ability_token.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

class ExtensionTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<AbilityRuntime::Extension> extension_;
};

void ExtensionTest::SetUpTestCase(void)
{
}

void ExtensionTest::TearDownTestCase(void)
{
}

void ExtensionTest::SetUp(void)
{
    std::shared_ptr<AbilityInfo> info = std::make_shared<AbilityInfo>();
    sptr<IRemoteObject> token = new AppExecFwk::MockAbilityToken();
    auto record = std::make_shared<AbilityLocalRecord>(info, token);
    auto application = std::make_shared<AppExecFwk::OHOSApplication>();
    std::shared_ptr<EventRunner> runner;
    auto handler = std::make_shared<AppExecFwk::AbilityHandler>(runner);
    extension_ = std::make_shared<AbilityRuntime::Extension>();
    extension_->Init(record, application, handler, token);
}

void ExtensionTest::TearDown(void)
{
}

/**
 * @tc.number: AaFwk_Extension_0100
 * @tc.name: Init
 * @tc.desc: record is null, application is null, handler is null, token is not null, Init failed.
 */
HWTEST_F(ExtensionTest, AaFwk_Extension_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Extension_0100 start";
    std::shared_ptr<AppExecFwk::AbilityLocalRecord> record;
    std::shared_ptr<AppExecFwk::OHOSApplication> application;
    std::shared_ptr<AppExecFwk::AbilityHandler> handler;
    sptr<IRemoteObject> token = nullptr;
    auto extension = std::make_shared<AbilityRuntime::Extension>();
    extension->Init(record, application, handler, token);
    EXPECT_TRUE(extension->application_ == nullptr);
    EXPECT_TRUE(extension->abilityInfo_ == nullptr);
    EXPECT_TRUE(extension->handler_ == nullptr);
    GTEST_LOG_(INFO) << "AaFwk_Extension_0100 end";
}

/**
 * @tc.number: AaFwk_Extension_0200
 * @tc.name: Init
 * @tc.desc: record is not null, application is null, handler is null, token is not null, Init failed.
 */
HWTEST_F(ExtensionTest, AaFwk_Extension_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Extension_0200 start";
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    std::shared_ptr<AppExecFwk::AbilityInfo> info = std::make_shared<AppExecFwk::AbilityInfo>();
    sptr<IRemoteObject> token = nullptr;
    auto record = std::make_shared<AppExecFwk::AbilityLocalRecord>(info, token);
    std::shared_ptr<AppExecFwk::OHOSApplication> application;
    std::shared_ptr<AppExecFwk::AbilityHandler> handler;

    auto extension = std::make_shared<AbilityRuntime::Extension>();
    extension->Init(record, application, handler, token);
    EXPECT_TRUE(extension->application_ == nullptr);
    EXPECT_TRUE(extension->abilityInfo_ == nullptr);
    EXPECT_TRUE(extension->handler_ == nullptr);
    GTEST_LOG_(INFO) << "AaFwk_Extension_0200 end";
}

/**
 * @tc.number: AaFwk_Extension_0300
 * @tc.name: Init
 * @tc.desc: record is null, application is not null, handler is null, token is not null, Init failed.
 */
HWTEST_F(ExtensionTest, AaFwk_Extension_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Extension_0300 start";
    std::shared_ptr<AppExecFwk::AbilityLocalRecord> record;
    auto application = std::make_shared<AppExecFwk::OHOSApplication>();
    std::shared_ptr<AppExecFwk::AbilityHandler> handler;
    sptr<IRemoteObject> token = nullptr;

    auto extension = std::make_shared<AbilityRuntime::Extension>();
    extension->Init(record, application, handler, token);
    EXPECT_TRUE(extension->application_ == nullptr);
    EXPECT_TRUE(extension->abilityInfo_ == nullptr);
    EXPECT_TRUE(extension->handler_ == nullptr);
    GTEST_LOG_(INFO) << "AaFwk_Extension_0300 end";
}

/**
 * @tc.number: AaFwk_Extension_0300
 * @tc.name: Init
 * @tc.desc: record is null, application is null, handler is not null, token is not null, Init failed.
 */
HWTEST_F(ExtensionTest, AaFwk_Extension_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Extension_0400 start";
    std::shared_ptr<AppExecFwk::AbilityLocalRecord> record;
    std::shared_ptr<AppExecFwk::OHOSApplication> application;
    std::shared_ptr<EventRunner> runner;
    auto handler = std::make_shared<AppExecFwk::AbilityHandler>(runner);
    sptr<IRemoteObject> token = nullptr;

    auto extension = std::make_shared<AbilityRuntime::Extension>();
    extension->Init(record, application, handler, token);
    EXPECT_TRUE(extension->application_ == nullptr);
    EXPECT_TRUE(extension->abilityInfo_ == nullptr);
    EXPECT_TRUE(extension->handler_ == nullptr);
    GTEST_LOG_(INFO) << "AaFwk_Extension_0400 end";
}

/**
 * @tc.number: AaFwk_Extension_0500
 * @tc.name: Init
 * @tc.desc: record is not null, application is not null, handler is not null, token is not null, Init succeeded.
 */
HWTEST_F(ExtensionTest, AaFwk_Extension_0500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Extension_0500 start";
    std::shared_ptr<AbilityInfo> info = std::make_shared<AbilityInfo>();
    sptr<IRemoteObject> token = new AppExecFwk::MockAbilityToken();
    auto record = std::make_shared<AbilityLocalRecord>(info, token);

    auto application = std::make_shared<AppExecFwk::OHOSApplication>();

    std::shared_ptr<EventRunner> runner;
    auto handler = std::make_shared<AppExecFwk::AbilityHandler>(runner);
    auto extension = std::make_shared<AbilityRuntime::Extension>();
    extension->Init(record, application, handler, token);
    EXPECT_TRUE(extension->application_ != nullptr);
    EXPECT_TRUE(extension->abilityInfo_ != nullptr);
    EXPECT_TRUE(extension->handler_ != nullptr);
    GTEST_LOG_(INFO) << "AaFwk_Extension_0500 end";
}

/**
 * @tc.number: AaFwk_Extension_0600
 * @tc.name: OnStart
 * @tc.desc: Incoming want verified OnStart successfully.
 */
HWTEST_F(ExtensionTest, AaFwk_Extension_0600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Extension_0600 start";
    Want want;
    want.SetElementName("DemoDeviceId", "DemoBundleName", "DemoAbilityName");
    extension_->OnStart(want);
    EXPECT_TRUE(extension_->launchWant_ != nullptr);
    auto deviceId = extension_->launchWant_->GetElement().GetDeviceID();
    auto bundleName = extension_->launchWant_->GetElement().GetBundleName();
    auto abilityName = extension_->launchWant_->GetElement().GetAbilityName();
    EXPECT_STREQ(deviceId.c_str(), "DemoDeviceId");
    EXPECT_STREQ(bundleName.c_str(), "DemoBundleName");
    EXPECT_STREQ(abilityName.c_str(), "DemoAbilityName");
    GTEST_LOG_(INFO) << "AaFwk_Extension_0600 end";
}

/**
 * @tc.number: AaFwk_Extension_0700
 * @tc.name: OnStart
 * @tc.desc: Incoming want verified OnStart successfully.
 */
HWTEST_F(ExtensionTest, AaFwk_Extension_0700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Extension_0700 start";
    Want want;
    want.SetElementName("DemoDeviceId", "DemoBundleName", "DemoAbilityName");
    extension_->OnStart(want);
    EXPECT_TRUE(extension_->lastRequestWant_ != nullptr);
    auto deviceId = extension_->lastRequestWant_->GetElement().GetDeviceID();
    auto bundleName = extension_->lastRequestWant_->GetElement().GetBundleName();
    auto abilityName = extension_->lastRequestWant_->GetElement().GetAbilityName();
    EXPECT_STREQ(deviceId.c_str(), "DemoDeviceId");
    EXPECT_STREQ(bundleName.c_str(), "DemoBundleName");
    EXPECT_STREQ(abilityName.c_str(), "DemoAbilityName");
    GTEST_LOG_(INFO) << "AaFwk_Extension_0700 end";
}

/**
 * @tc.number: AaFwk_Extension_0800
 * @tc.name: OnStop
 * @tc.desc: Successfully verified OnStop.
 */
HWTEST_F(ExtensionTest, AaFwk_Extension_0800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Extension_0800 start";
    extension_->OnStop();
    GTEST_LOG_(INFO) << "AaFwk_Extension_0800 end";
}

/**
 * @tc.number: AaFwk_Extension_0900
 * @tc.name: OnConnect
 * @tc.desc: Incoming want verified OnConnect successfully.
 */
HWTEST_F(ExtensionTest, AaFwk_Extension_0900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Extension_0900 start";
    Want want;
    want.SetElementName("DemoDeviceId", "DemoBundleName", "DemoAbilityName");
    auto remoteObject = extension_->OnConnect(want);
    EXPECT_TRUE(remoteObject == nullptr);
    GTEST_LOG_(INFO) << "AaFwk_Extension_0900 end";
}

/**
 * @tc.number: AaFwk_Extension_1000
 * @tc.name: OnDisconnect
 * @tc.desc: Incoming want verified OnDisconnect successfully.
 */
HWTEST_F(ExtensionTest, AaFwk_Extension_1000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Extension_1000 start";
    Want want;
    want.SetElementName("DemoDeviceId", "DemoBundleName", "DemoAbilityName");
    extension_->OnDisconnect(want);
    GTEST_LOG_(INFO) << "AaFwk_Extension_1000 end";
}

/**
 * @tc.number: AaFwk_Extension_1100
 * @tc.name: OnDisconnect
 * @tc.desc: Incoming want verified OnDisconnect successfully.
 */
HWTEST_F(ExtensionTest, AaFwk_Extension_1100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Extension_1100 start";
    Want want;
    want.SetElementName("DemoDeviceId", "DemoBundleName", "DemoAbilityName");
    AppExecFwk::AbilityTransactionCallbackInfo<> *callbackInfo = nullptr;
    bool isAsyncCallback = true;
    extension_->OnDisconnect(want, callbackInfo, isAsyncCallback);
    EXPECT_FALSE(isAsyncCallback);
    GTEST_LOG_(INFO) << "AaFwk_Extension_1100 end";
}

/**
 * @tc.number: AaFwk_Extension_1200
 * @tc.name: OnCommand
 * @tc.desc: Incoming want verified OnCommand successfully.
 */
HWTEST_F(ExtensionTest, AaFwk_Extension_1200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Extension_1200 start";
    Want want;
    want.SetElementName("DemoDeviceId", "DemoBundleName", "DemoAbilityName");
    bool restart = false;
    int32_t startId = 0;
    extension_->OnCommand(want, restart, startId);
    EXPECT_TRUE(extension_->lastRequestWant_ != nullptr);
    auto deviceId = extension_->lastRequestWant_->GetElement().GetDeviceID();
    auto bundleName = extension_->lastRequestWant_->GetElement().GetBundleName();
    auto abilityName = extension_->lastRequestWant_->GetElement().GetAbilityName();
    EXPECT_STREQ(deviceId.c_str(), "DemoDeviceId");
    EXPECT_STREQ(bundleName.c_str(), "DemoBundleName");
    EXPECT_STREQ(abilityName.c_str(), "DemoAbilityName");
    GTEST_LOG_(INFO) << "AaFwk_Extension_1200 end";
}

/**
 * @tc.number: AaFwk_Extension_1300
 * @tc.name: SetCallingInfo and GetCallingInfo
 * @tc.desc: Verifying SetCallingInfo and GetCallingInfo succeeded.
 */
HWTEST_F(ExtensionTest, AaFwk_Extension_1300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Extension_1300 start";
    CallingInfo callingInfo;
    callingInfo.callingUid = 1000;
    extension_->SetCallingInfo(callingInfo);
    auto info = extension_->GetCallingInfo();
    EXPECT_TRUE(extension_->callingInfo_ != nullptr);
    EXPECT_EQ(info->callingUid, callingInfo.callingUid);
    GTEST_LOG_(INFO) << "AaFwk_Extension_1300 end";
}

/**
 * @tc.number: AaFwk_Extension_1400
 * @tc.name: OnConfigurationUpdated
 * @tc.desc: Successfully verified OnConfigurationUpdated.
 */
HWTEST_F(ExtensionTest, AaFwk_Extension_1400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Extension_1400 start";
    AppExecFwk::Configuration config;
    extension_->OnConfigurationUpdated(config);
    GTEST_LOG_(INFO) << "AaFwk_Extension_1400 end";
}

/**
 * @tc.number: AaFwk_Extension_1500
 * @tc.name: OnMemoryLevel
 * @tc.desc: Successfully verified OnMemoryLevel.
 */
HWTEST_F(ExtensionTest, AaFwk_Extension_1500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Extension_1500 start";
    int32_t level = 1;
    extension_->OnMemoryLevel(level);
    GTEST_LOG_(INFO) << "AaFwk_Extension_1500 end";
}

/**
 * @tc.number: AaFwk_Extension_1600
 * @tc.name: Dump
 * @tc.desc: Successfully verified dump.
 */
HWTEST_F(ExtensionTest, AaFwk_Extension_1600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Extension_1600 start";
    std::vector<std::string> params;
    std::vector<std::string> info;
    extension_->Dump(params, info);
    GTEST_LOG_(INFO) << "AaFwk_Extension_1600 end";
}
} // namespace AppExecFwk
} // namespace OHOS
