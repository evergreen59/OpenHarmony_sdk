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
#include "ability_context.h"
#include "ability_runtime_error_util.h"
#include <algorithm>
#include "completed_callback.h"
#include "context_container.h"
#include "context_impl.h"
#include "context/application_context.h"
#include "element_name.h"
#include "event_handler.h"
#include "base_types.h"
#define private public
#define protected public
#include "pending_want.h"
#include "remote_native_token.h"
#undef private
#undef protected
#include "want.h"
#include "want_agent.h"
#include "want_agent_constant.h"
#define private public
#define protected public
#include "want_agent_helper.h"
#include "want_agent_info.h"
#undef private
#undef protected
#include "want_params.h"
#include "want_receiver_stub.h"
#include "want_sender_stub.h"
#include "bool_wrapper.h"

using namespace testing::ext;
using namespace OHOS::AAFwk;
using namespace OHOS;
using OHOS::AppExecFwk::ElementName;
using namespace OHOS::AbilityRuntime;
using namespace OHOS::AppExecFwk;
using vector_str = std::vector<std::string>;

namespace OHOS::AbilityRuntime::WantAgent {
class WantAgentHelperTest : public testing::Test {
public:
    WantAgentHelperTest()
    {}
    ~WantAgentHelperTest()
    {}
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static int callBackCancelListenerConnt;
    void SetUp();
    void TearDown();
    class CancelListenerSon : public CancelListener {
    public:
        void OnCancelled(int resultCode) override;
    };
};

int WantAgentHelperTest::callBackCancelListenerConnt = 0;

void WantAgentHelperTest::CancelListenerSon::OnCancelled(int resultCode)
{
    callBackCancelListenerConnt++;
}

void WantAgentHelperTest::SetUpTestCase(void)
{
    RemoteNativeToken::SetNativeToken();
}

void WantAgentHelperTest::TearDownTestCase(void)
{}

void WantAgentHelperTest::SetUp(void)
{}

void WantAgentHelperTest::TearDown(void)
{}

/*
 * @tc.number    : WantAgentHelper_0100
 * @tc.name      : WantAgentHelper Constructors
 * @tc.desc      : 1.WantAgentConstant::Flags::ONE_TIME_FLAG
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::vector<WantAgentConstant::Flags> flags;
    flags.emplace_back(WantAgentConstant::Flags::ONE_TIME_FLAG);
    EXPECT_EQ(wantAgentHelper->FlagsTransformer(flags), static_cast<unsigned int>(0x40000000));
}

/*
 * @tc.number    : WantAgentHelper_0200
 * @tc.name      : WantAgentHelper Constructors
 * @tc.desc      : 1.WantAgentConstant::Flags::NO_BUILD_FLAG
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_0200, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::vector<WantAgentConstant::Flags> flags;
    flags.emplace_back(WantAgentConstant::Flags::NO_BUILD_FLAG);
    EXPECT_EQ(wantAgentHelper->FlagsTransformer(flags), static_cast<unsigned int>(0x20000000));
}

/*
 * @tc.number    : WantAgentHelper_0300
 * @tc.name      : WantAgentHelper Constructors
 * @tc.desc      : 1.WantAgentConstant::Flags::CANCEL_PRESENT_FLAG
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_0300, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::vector<WantAgentConstant::Flags> flags;
    flags.emplace_back(WantAgentConstant::Flags::CANCEL_PRESENT_FLAG);
    EXPECT_EQ(wantAgentHelper->FlagsTransformer(flags), static_cast<unsigned int>(0x10000000));
}

/*
 * @tc.number    : WantAgentHelper_0400
 * @tc.name      : WantAgentHelper Constructors
 * @tc.desc      : 1.WantAgentConstant::Flags::UPDATE_PRESENT_FLAG
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_0400, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::vector<WantAgentConstant::Flags> flags;
    flags.emplace_back(WantAgentConstant::Flags::UPDATE_PRESENT_FLAG);
    EXPECT_EQ(wantAgentHelper->FlagsTransformer(flags), static_cast<unsigned int>(0x8000000));
}

/*
 * @tc.number    : WantAgentHelper_0500
 * @tc.name      : WantAgentHelper Constructors
 * @tc.desc      : 1.WantAgentConstant::Flags::CONSTANT_FLAG
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_0500, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::vector<WantAgentConstant::Flags> flags;
    flags.emplace_back(WantAgentConstant::Flags::CONSTANT_FLAG);
    EXPECT_EQ(wantAgentHelper->FlagsTransformer(flags), static_cast<unsigned int>(0x4000000));
}

/*
 * @tc.number    : WantAgentHelper_0600
 * @tc.name      : WantAgentHelper Constructors
 * @tc.desc      : 1.WantAgentConstant::Flags::REPLACE_BUNDLE
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_0600, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::vector<WantAgentConstant::Flags> flags;
    flags.emplace_back(WantAgentConstant::Flags::REPLACE_BUNDLE);
    EXPECT_EQ(
        wantAgentHelper->FlagsTransformer(flags), static_cast<unsigned int>(WantAgentConstant::Flags::ONE_TIME_FLAG));
}

/*
 * @tc.number    : WantAgentHelper_0700
 * @tc.name      : WantAgentHelper Constructors
 * @tc.desc      : 1.WantAgentConstant::Flags::CONSTANT_FLAG | UPDATE_PRESENT_FLAG | REPLACE_BUNDLE
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_0700, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::vector<WantAgentConstant::Flags> flags;
    flags.emplace_back(WantAgentConstant::Flags::CONSTANT_FLAG);
    flags.emplace_back(WantAgentConstant::Flags::UPDATE_PRESENT_FLAG);
    flags.emplace_back(WantAgentConstant::Flags::REPLACE_BUNDLE);
    EXPECT_EQ(wantAgentHelper->FlagsTransformer(flags), static_cast<unsigned int>(0xc000000));
}

/*
 * @tc.number    : WantAgentHelper_0800
 * @tc.name      : WantAgentHelper GetWantAgent
 * @tc.desc      : 1.GetWantAgent context is nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_0800, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    WantAgentInfo wantAgentInfo;
    std::shared_ptr<WantAgent> wantAgent = nullptr;
    wantAgentHelper->GetWantAgent(nullptr, wantAgentInfo, wantAgent);
    EXPECT_EQ(wantAgent, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_0900
 * @tc.name      : WantAgentHelper GetWantAgent
 * @tc.desc      : 1.GetWantAgent context is not nullptr
 *          `      2.wantAgentInfo.wants_ empty
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_0900, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    WantAgentInfo wantAgentInfo;
    wantAgentInfo.wants_.clear();
    std::shared_ptr<WantAgent> wantAgent = nullptr;
    wantAgentHelper->GetWantAgent(nullptr, wantAgentInfo, wantAgent);
    EXPECT_EQ(wantAgent, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_1000
 * @tc.name      : WantAgentHelper GetWantAgent
 * @tc.desc      : 1.GetWantAgent context is not nullptr
 *                 2.wantAgentInfo.wants_.size() != wantAgentInfo.flags_.size()
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_1000, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        OHOS::AbilityRuntime::Context::GetApplicationContext();
    WantAgentInfo wantAgentInfo;
    std::shared_ptr<Want> want = std::make_shared<Want>();
    ElementName element("device", "bundleName", "abilityName");
    want->SetElement(element);
    wantAgentInfo.wants_.emplace_back(want);
    wantAgentInfo.flags_.clear();
    std::shared_ptr<WantAgent> wantAgent = nullptr;
    wantAgentHelper->GetWantAgent(nullptr, wantAgentInfo, wantAgent);
    EXPECT_EQ(wantAgent, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_1100
 * @tc.name      : WantAgentHelper GetWantAgent
 * @tc.desc      : 1.GetWantAgent context is not nullptr
 *                 2.wantAgentInfo.wants_.size() == wantAgentInfo.flags_.size()
 *                 3.FlagsTransformer return 0
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_1100, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        OHOS::AbilityRuntime::Context::GetApplicationContext();
    WantAgentInfo wantAgentInfo;
    std::shared_ptr<Want> want = std::make_shared<Want>();
    ElementName element("device", "bundleName", "abilityName");
    want->SetElement(element);
    wantAgentInfo.wants_.emplace_back(want);
    wantAgentInfo.flags_.emplace_back(WantAgentConstant::Flags::REPLACE_BUNDLE);
    std::shared_ptr<WantAgent> wantAgent = nullptr;
    wantAgentHelper->GetWantAgent(nullptr, wantAgentInfo, wantAgent);
    EXPECT_EQ(wantAgent, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_1200
 * @tc.name      : WantAgentHelper GetWantAgent
 * @tc.desc      : 1.GetWantAgent context is not nullptr
 *                 2.wantAgentInfo.wants_.size() == wantAgentInfo.flags_.size()
 *                 3.FlagsTransformer return ok
 *                 4.Type is WantAgentConstant::OperationType::START_ABILITY
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_1200, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetInstance();
    std::shared_ptr<Want> want = std::make_shared<Want>();
    ElementName element("device", "bundleName", "abilityName");
    want->SetElement(element);
    WantAgentInfo wantAgentInfo;
    wantAgentInfo.wants_.emplace_back(want);
    wantAgentInfo.flags_.emplace_back(WantAgentConstant::Flags::CONSTANT_FLAG);
    wantAgentInfo.operationType_ = WantAgentConstant::OperationType::START_ABILITY;
    wantAgentInfo.requestCode_ = 10;
    std::shared_ptr<WantAgent> wantAgent = nullptr;
    wantAgentHelper->GetWantAgent(nullptr, wantAgentInfo, wantAgent);
    EXPECT_EQ(wantAgent, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_1300
 * @tc.name      : WantAgentHelper GetWantAgent
 * @tc.desc      : 1.GetWantAgent context is not nullptr
 *                 2.wantAgentInfo.wants_.size() == wantAgentInfo.flags_.size()
 *                 3.FlagsTransformer return ok
 *                 4.Type is WantAgentConstant::OperationType::START_ABILITIES
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_1300, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetInstance();
    std::shared_ptr<Want> want = std::make_shared<Want>();
    ElementName element("device", "bundleName", "abilityName");
    want->SetElement(element);
    WantAgentInfo wantAgentInfo;
    wantAgentInfo.wants_.emplace_back(want);
    wantAgentInfo.flags_.emplace_back(WantAgentConstant::Flags::CONSTANT_FLAG);
    wantAgentInfo.operationType_ = WantAgentConstant::OperationType::START_ABILITIES;
    wantAgentInfo.requestCode_ = 10;
    std::shared_ptr<WantAgent> wantAgent = nullptr;
    wantAgentHelper->GetWantAgent(nullptr, wantAgentInfo, wantAgent);
    EXPECT_EQ(wantAgent, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_1400
 * @tc.name      : WantAgentHelper GetWantAgent
 * @tc.desc      : 1.GetWantAgent context is not nullptr
 *                 2.wantAgentInfo.wants_.size() == wantAgentInfo.flags_.size()
 *                 3.FlagsTransformer return ok
 *                 4.Type is WantAgentConstant::OperationType::START_SERVICE
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_1400, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetInstance();
    std::shared_ptr<Want> want = std::make_shared<Want>();
    ElementName element("device", "bundleName", "abilityName");
    want->SetElement(element);
    WantAgentInfo wantAgentInfo;
    wantAgentInfo.wants_.emplace_back(want);
    wantAgentInfo.flags_.emplace_back(WantAgentConstant::Flags::CONSTANT_FLAG);
    wantAgentInfo.operationType_ = WantAgentConstant::OperationType::START_SERVICE;
    wantAgentInfo.requestCode_ = 10;
    std::shared_ptr<WantAgent> wantAgent = nullptr;
    wantAgentHelper->GetWantAgent(nullptr, wantAgentInfo, wantAgent);
    EXPECT_EQ(wantAgent, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_1500
 * @tc.name      : WantAgentHelper GetWantAgent
 * @tc.desc      : 1.GetWantAgent context is not nullptr
 *                 2.wantAgentInfo.wants_.size() == wantAgentInfo.flags_.size()
 *                 3.FlagsTransformer return ok
 *                 4.Type is WantAgentConstant::OperationType::START_FOREGROUND_SERVICE
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_1500, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetInstance();
    std::shared_ptr<Want> want = std::make_shared<Want>();
    ElementName element("device", "bundleName", "abilityName");
    want->SetElement(element);
    WantAgentInfo wantAgentInfo;
    wantAgentInfo.wants_.emplace_back(want);
    wantAgentInfo.flags_.emplace_back(WantAgentConstant::Flags::CONSTANT_FLAG);
    wantAgentInfo.operationType_ = WantAgentConstant::OperationType::START_FOREGROUND_SERVICE;
    wantAgentInfo.requestCode_ = 10;
    std::shared_ptr<WantAgent> wantAgent = nullptr;
    wantAgentHelper->GetWantAgent(nullptr, wantAgentInfo, wantAgent);
    EXPECT_EQ(wantAgent, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_1600
 * @tc.name      : WantAgentHelper GetWantAgent
 * @tc.desc      : 1.GetWantAgent context is not nullptr
 *                 2.wantAgentInfo.wants_.size() == wantAgentInfo.flags_.size()
 *                 3.FlagsTransformer return ok
 *                 4.Type is WantAgentConstant::OperationType::START_FOREGROUND_SERVICE
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_1600, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetInstance();
    std::shared_ptr<Want> want = std::make_shared<Want>();
    ElementName element("device", "bundleName", "abilityName");
    want->SetElement(element);
    WantAgentInfo wantAgentInfo;
    wantAgentInfo.wants_.emplace_back(want);
    wantAgentInfo.flags_.emplace_back(WantAgentConstant::Flags::CONSTANT_FLAG);
    wantAgentInfo.operationType_ = WantAgentConstant::OperationType::SEND_COMMON_EVENT;
    wantAgentInfo.requestCode_ = 10;
    std::shared_ptr<WantAgent> wantAgent = nullptr;
    wantAgentHelper->GetWantAgent(nullptr, wantAgentInfo, wantAgent);
    EXPECT_EQ(wantAgent, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_1700
 * @tc.name      : WantAgentHelper GetWantAgent
 * @tc.desc      : 1.GetWantAgent context is not nullptr
 *                 2.wantAgentInfo.wants_.size() == wantAgentInfo.flags_.size()
 *                 3.FlagsTransformer return ok
 *                 4.Type is 100
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_1700, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetInstance();
    std::shared_ptr<Want> want = std::make_shared<Want>();
    ElementName element("device", "bundleName", "abilityName");
    want->SetElement(element);
    WantAgentInfo wantAgentInfo;
    wantAgentInfo.wants_.emplace_back(want);
    wantAgentInfo.flags_.emplace_back(WantAgentConstant::Flags::CONSTANT_FLAG);
    wantAgentInfo.operationType_ = static_cast<WantAgentConstant::OperationType>(100);
    wantAgentInfo.requestCode_ = 10;
    std::shared_ptr<WantAgent> wantAgent = nullptr;
    wantAgentHelper->GetWantAgent(nullptr, wantAgentInfo, wantAgent);
    EXPECT_EQ(wantAgent, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_1800
 * @tc.name      : WantAgentHelper GetWantAgent(const WantAgentInfo &paramsInfo)
 * @tc.desc      : 1.wantAgentInfo.wants_.size() == 0
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_1800, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    WantAgentInfo wantAgentInfo;
    wantAgentInfo.wants_.clear();
    wantAgentInfo.flags_.emplace_back(WantAgentConstant::Flags::CONSTANT_FLAG);
    wantAgentInfo.operationType_ = static_cast<WantAgentConstant::OperationType>(100);
    wantAgentInfo.requestCode_ = 10;
    auto wantAgent = wantAgentHelper->GetWantAgent(wantAgentInfo);
    EXPECT_EQ(wantAgent, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_1900
 * @tc.name      : WantAgentHelper GetWantAgent(const WantAgentInfo &paramsInfo)
 * @tc.desc      : 1.wantAgentInfo.wants_ not empty
 *                 2.wantAgentInfo.wants_.size() != wantAgentInfo.flags_.size()
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_1900, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<Want> want = std::make_shared<Want>();
    ElementName element("device", "bundleName", "abilityName");
    want->SetElement(element);
    WantAgentInfo wantAgentInfo;
    wantAgentInfo.wants_.emplace_back(want);
    wantAgentInfo.flags_.clear();
    wantAgentInfo.operationType_ = static_cast<WantAgentConstant::OperationType>(100);
    wantAgentInfo.requestCode_ = 10;
    auto wantAgent = wantAgentHelper->GetWantAgent(wantAgentInfo);
    EXPECT_NE(wantAgent, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_2000
 * @tc.name      : WantAgentHelper GetWantAgent(const WantAgentInfo &paramsInfo)
 * @tc.desc      : 1.wantAgentInfo.wants_ not empty
 *                 2.wantAgentInfo.wants_.size() == wantAgentInfo.flags_.size()
 *                 3.wantAgentInfo.wants_[0] is nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_2000, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<Want> want = std::make_shared<Want>();
    ElementName element("device", "bundleName", "abilityName");
    want->SetElement(element);
    WantAgentInfo wantAgentInfo;
    wantAgentInfo.wants_.emplace_back(nullptr);
    wantAgentInfo.wants_.emplace_back(want);
    wantAgentInfo.flags_.emplace_back(WantAgentConstant::Flags::CONSTANT_FLAG);
    wantAgentInfo.flags_.emplace_back(WantAgentConstant::Flags::CONSTANT_FLAG);
    wantAgentInfo.operationType_ = static_cast<WantAgentConstant::OperationType>(100);
    wantAgentInfo.requestCode_ = 10;
    auto wantAgent = wantAgentHelper->GetWantAgent(wantAgentInfo);
    EXPECT_EQ(wantAgent, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_2100
 * @tc.name      : WantAgentHelper GetWantAgent(const WantAgentInfo &paramsInfo)
 * @tc.desc      : 1.wantAgentInfo.wants_ not empty
 *                 2.wantAgentInfo.wants_.size() == wantAgentInfo.flags_.size()
 *                 3.wantAgentInfo.wants_[0] is not nullptr
 *                 4.wantAgentInfo.extraInfo_ is not nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_2100, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<Want> want = std::make_shared<Want>();
    ElementName element("device", "bundleName", "abilityName");
    want->SetElement(element);
    WantAgentInfo wantAgentInfo;
    wantAgentInfo.wants_.emplace_back(want);
    wantAgentInfo.flags_.emplace_back(WantAgentConstant::Flags::CONSTANT_FLAG);
    wantAgentInfo.operationType_ = static_cast<WantAgentConstant::OperationType>(100);
    wantAgentInfo.requestCode_ = 10;
    bool value = true;
    std::string key = "key";
    std::shared_ptr<WantParams> wParams = std::make_shared<WantParams>();
    wParams->SetParam(key, Boolean::Box(value));
    wantAgentInfo.extraInfo_ = wParams;
    auto wantAgent = wantAgentHelper->GetWantAgent(wantAgentInfo);
    EXPECT_NE(wantAgent, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_2200
 * @tc.name      : WantAgentHelper GetWantAgent(const WantAgentInfo &paramsInfo)
 * @tc.desc      : 1.wantAgentInfo.wants_ not empty
 *                 2.wantAgentInfo.wants_.size() == wantAgentInfo.flags_.size()
 *                 3.wantAgentInfo.wants_[0] is not nullptr
 *                 4.wantAgentInfo.extraInfo_ is nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_2200, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<Want> want = std::make_shared<Want>();
    ElementName element("device", "bundleName", "abilityName");
    want->SetElement(element);
    WantAgentInfo wantAgentInfo;
    wantAgentInfo.wants_.emplace_back(want);
    wantAgentInfo.flags_.emplace_back(WantAgentConstant::Flags::CONSTANT_FLAG);
    wantAgentInfo.operationType_ = static_cast<WantAgentConstant::OperationType>(100);
    wantAgentInfo.requestCode_ = 10;
    wantAgentInfo.extraInfo_ = nullptr;
    auto wantAgent = wantAgentHelper->GetWantAgent(wantAgentInfo);
    EXPECT_NE(wantAgent, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_2300
 * @tc.name      : WantAgentHelper GetType
 * @tc.desc      : 1.agent is not nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_2300, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    WantAgentInfo wantAgentInfo;
    std::shared_ptr<WantAgent> wantAgent = nullptr;
    wantAgentHelper->GetWantAgent(nullptr, wantAgentInfo, wantAgent);
    auto type = wantAgentHelper->GetType(wantAgent);
    EXPECT_EQ(type, WantAgentConstant::OperationType::UNKNOWN_TYPE);
}

/*
 * @tc.number    : WantAgentHelper_2400
 * @tc.name      : WantAgentHelper GetType
 * @tc.desc      : 1.agent is not nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_2400, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<WantAgent> wantAgent(nullptr);
    auto type = wantAgentHelper->GetType(wantAgent);
    EXPECT_EQ(type, WantAgentConstant::OperationType::UNKNOWN_TYPE);
}

/*
 * @tc.number    : WantAgentHelper_2500
 * @tc.name      : WantAgentHelper GetType
 * @tc.desc      : 1.agent is not nullptr,PendingWant is nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_2500, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<PendingWant> pendingWant(nullptr);
    std::shared_ptr<WantAgent> wantAgent = std::make_shared<WantAgent>(pendingWant);
    auto type = wantAgentHelper->GetType(wantAgent);
    EXPECT_EQ(type, WantAgentConstant::OperationType::UNKNOWN_TYPE);
}

/*
 * @tc.number    : WantAgentHelper_2600
 * @tc.name      : WantAgentHelper JudgeEquality
 * @tc.desc      : 1.JudgeEquality
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_2600, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<WantAgent> wantAgent(nullptr);
    std::shared_ptr<WantAgent> wantAgent2(nullptr);
    ErrCode isEqual = wantAgentHelper->IsEquals(wantAgent, wantAgent2);
    EXPECT_EQ(isEqual, ERR_OK);
}

/*
 * @tc.number    : WantAgentHelper_2700
 * @tc.name      : WantAgentHelper JudgeEquality
 * @tc.desc      : 1.JudgeEquality,Judge whether it is equal or not
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_2700, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();

    // pendingwant
    int requestCode = 10;
    std::shared_ptr<Want> want = std::make_shared<Want>();
    ElementName element("device", "bundleName", "abilityName");
    want->SetElement(element);
    unsigned int flags = 1;
    flags |= FLAG_ONE_SHOT;
    WantAgentConstant::OperationType type = WantAgentConstant::OperationType::START_FOREGROUND_SERVICE;
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetInstance();
    std::shared_ptr<PendingWant> pendingWant = nullptr;
    PendingWant::BuildServicePendingWant(context, requestCode, want, flags, type, pendingWant);

    // pendingwant2
    int requestCode2 = 11;
    std::shared_ptr<Want> want2 = std::make_shared<Want>();
    ElementName element2("device", "bundle", "ability");
    want2->SetElement(element2);
    std::shared_ptr<PendingWant> pendingWant2 = nullptr;
    PendingWant::BuildServicePendingWant(context, requestCode2, want2, flags, type, pendingWant2);

    std::shared_ptr<WantAgent> wantAgent = std::make_shared<WantAgent>(pendingWant);
    std::shared_ptr<WantAgent> wantAgent2 = std::make_shared<WantAgent>(pendingWant2);
    ErrCode isEqual = wantAgentHelper->IsEquals(wantAgent, wantAgent2);
    EXPECT_NE(isEqual, ERR_OK);
}

/*
 * @tc.number    : WantAgentHelper_2800
 * @tc.name      : WantAgentHelper JudgeEquality
 * @tc.desc      : 1.JudgeEquality,Judge whether it is equal or not
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_2800, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();

    int requestCode = 10;
    std::shared_ptr<Want> want = std::make_shared<Want>();
    ElementName element("device", "bundleName", "abilityName");
    want->SetElement(element);
    unsigned int flags = 1;
    flags |= FLAG_ONE_SHOT;
    WantAgentConstant::OperationType type = WantAgentConstant::OperationType::START_FOREGROUND_SERVICE;
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetInstance();
    std::shared_ptr<PendingWant> pendingWant = nullptr;
    PendingWant::BuildServicePendingWant(context, requestCode, want, flags, type, pendingWant);

    std::shared_ptr<WantAgent> wantAgent = std::make_shared<WantAgent>(pendingWant);
    std::shared_ptr<WantAgent> wantAgent2 = std::make_shared<WantAgent>(pendingWant);
    ErrCode isEqual = wantAgentHelper->IsEquals(wantAgent, wantAgent2);
    EXPECT_EQ(isEqual, ERR_OK);
}

/*
 * @tc.number    : WantAgentHelper_2900
 * @tc.name      : WantAgentHelper JudgeEquality
 * @tc.desc      : 1.JudgeEquality,Judge whether it is equal or not
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_2900, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();

    int requestCode = 10;
    std::shared_ptr<Want> want = std::make_shared<Want>();
    ElementName element("device", "bundleName", "abilityName");
    want->SetElement(element);
    unsigned int flags = 1;
    flags |= FLAG_ONE_SHOT;
    WantAgentConstant::OperationType type = WantAgentConstant::OperationType::START_FOREGROUND_SERVICE;
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetInstance();
    std::shared_ptr<PendingWant> pendingWant = nullptr;
    PendingWant::BuildServicePendingWant(context, requestCode, want, flags, type, pendingWant);

    std::shared_ptr<PendingWant> pendingWant2(nullptr);
    std::shared_ptr<WantAgent> wantAgent = std::make_shared<WantAgent>(pendingWant);
    std::shared_ptr<WantAgent> wantAgent2 = std::make_shared<WantAgent>(pendingWant2);
    ErrCode isEqual = wantAgentHelper->IsEquals(wantAgent, wantAgent2);
    EXPECT_NE(isEqual, ERR_OK);
}

/*
 * @tc.number    : WantAgentHelper_3000
 * @tc.name      : WantAgentHelper GetBundleName
 * @tc.desc      : 1.GetBundleName WantAgent is nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_3000, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<WantAgent> wantAgent(nullptr);
    std::string bundleName = "";
    wantAgentHelper->GetBundleName(wantAgent, bundleName);
    EXPECT_EQ(bundleName, "");
}

/*
 * @tc.number    : WantAgentHelper_3100
 * @tc.name      : WantAgentHelper GetBundleName
 * @tc.desc      : 1.GetBundleName WantAgent.PendingWant.target is nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_3100, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<PendingWant> pendingWant(nullptr);
    std::shared_ptr<WantAgent> wantAgent = std::make_shared<WantAgent>(pendingWant);
    std::string bundleName = "";
    wantAgentHelper->GetBundleName(wantAgent, bundleName);
    EXPECT_EQ(bundleName, "");
}

/*
 * @tc.number    : WantAgentHelper_3200
 * @tc.name      : WantAgentHelper GetUid
 * @tc.desc      : 1.GetUid WantAgent is nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_3200, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<WantAgent> wantAgent(nullptr);
    int32_t uid = -1;
    wantAgentHelper->GetUid(wantAgent, uid);
    EXPECT_EQ(uid, -1);
}

/*
 * @tc.number    : WantAgentHelper_3300
 * @tc.name      : WantAgentHelper GetUid
 * @tc.desc      : 1.GetUid WantAgent.PendingWant.target is nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_3300, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<PendingWant> pendingWant(nullptr);
    std::shared_ptr<WantAgent> wantAgent = std::make_shared<WantAgent>(pendingWant);
    int32_t uid = -1;
    wantAgentHelper->GetUid(wantAgent, uid);
    EXPECT_EQ(uid, -1);
}

/*
 * @tc.number    : WantAgentHelper_3400
 * @tc.name      : WantAgentHelper GetWant
 * @tc.desc      : 1.GetWant WantAgent is nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_3400, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<WantAgent> wantAgent(nullptr);
    auto want = wantAgentHelper->GetWant(wantAgent);
    EXPECT_EQ(want, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_3500
 * @tc.name      : WantAgentHelper GetWant
 * @tc.desc      : 1.GetWant WantAgent.PendingWant.target is nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_3500, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<PendingWant> pendingWant(nullptr);
    std::shared_ptr<WantAgent> wantAgent = std::make_shared<WantAgent>(pendingWant);
    auto want = wantAgentHelper->GetWant(wantAgent);
    EXPECT_EQ(want, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_3600
 * @tc.name      : WantAgentHelper ParseFlags
 * @tc.desc      : 1.ParseFlags Check ParseFlags
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_3600, Function | MediumTest | Level1)
{
    std::vector<WantAgentConstant::Flags> flagsVec;
    nlohmann::json jsonObject;

    jsonObject["flags"] = -1;
    flagsVec = WantAgentHelper::ParseFlags(jsonObject);
    EXPECT_EQ(flagsVec.size(), 0);

    jsonObject.clear();
    jsonObject["flags"] = 1111000000;
    flagsVec = WantAgentHelper::ParseFlags(jsonObject);
    std::vector<WantAgentConstant::Flags>::iterator oneTimeIt = std::find(flagsVec.begin(), flagsVec.end(),
        WantAgentConstant::Flags::ONE_TIME_FLAG);
    EXPECT_EQ(oneTimeIt != flagsVec.end(), true);

    jsonObject.clear();
    jsonObject["flags"] = 111100000000000;
    flagsVec = WantAgentHelper::ParseFlags(jsonObject);
    std::vector<WantAgentConstant::Flags>::iterator cancelPresentIt = std::find(flagsVec.begin(), flagsVec.end(),
        WantAgentConstant::Flags::CANCEL_PRESENT_FLAG);
    EXPECT_EQ(cancelPresentIt != flagsVec.end(), true);

    jsonObject.clear();
    jsonObject["flags"] = 111100000000000;
    flagsVec = WantAgentHelper::ParseFlags(jsonObject);
    std::vector<WantAgentConstant::Flags>::iterator updateResentIt = std::find(flagsVec.begin(), flagsVec.end(),
        WantAgentConstant::Flags::UPDATE_PRESENT_FLAG);
    EXPECT_EQ(updateResentIt != flagsVec.end(), true);

    jsonObject.clear();
    jsonObject["flags"] = 111100000000000;
    flagsVec = WantAgentHelper::ParseFlags(jsonObject);
    std::vector<WantAgentConstant::Flags>::iterator constantIt = std::find(flagsVec.begin(), flagsVec.end(),
        WantAgentConstant::Flags::CONSTANT_FLAG);
    EXPECT_EQ(constantIt != flagsVec.end(), true);

    jsonObject.clear();
    jsonObject["flags"] = 1000000000;
    flagsVec = WantAgentHelper::ParseFlags(jsonObject);
    std::vector<WantAgentConstant::Flags>::iterator noBuildIt = std::find(flagsVec.begin(), flagsVec.end(),
        WantAgentConstant::Flags::NO_BUILD_FLAG);
    EXPECT_EQ(noBuildIt != flagsVec.end(), true);
}

/*
 * @tc.number    : WantAgentHelper_3700
 * @tc.name      : WantAgentHelper GetType
 * @tc.desc      : 1.agent is nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_3700, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<WantAgent> wantAgent(nullptr);
    int32_t type = static_cast<int32_t>(WantAgentConstant::OperationType::UNKNOWN_TYPE);
    EXPECT_EQ(wantAgentHelper->GetType(wantAgent, type), ERR_ABILITY_RUNTIME_EXTERNAL_INVALID_WANTAGENT);
    EXPECT_EQ(type, static_cast<int32_t>(WantAgentConstant::OperationType::UNKNOWN_TYPE));
}

/*
 * @tc.number    : WantAgentHelper_3800
 * @tc.name      : WantAgentHelper GetType
 * @tc.desc      : 1.agent is not nullptr,PendingWant is nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_3800, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<PendingWant> pendingWant(nullptr);
    std::shared_ptr<WantAgent> wantAgent = std::make_shared<WantAgent>(pendingWant);
    auto type = static_cast<int32_t>(WantAgentConstant::OperationType::UNKNOWN_TYPE);
    EXPECT_EQ(wantAgentHelper->GetType(wantAgent, type), ERR_ABILITY_RUNTIME_EXTERNAL_INVALID_WANTAGENT);
    EXPECT_EQ(type, static_cast<int32_t>(WantAgentConstant::OperationType::UNKNOWN_TYPE));
}

/*
 * @tc.number    : WantAgentHelper_3900
 * @tc.name      : WantAgentHelper GetType
 * @tc.desc      : 1.agent is not nullptr, PendingWant is not nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_3900, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    sptr<AAFwk::IWantSender> target(nullptr);
    std::shared_ptr<PendingWant> pendingWant = std::make_shared<PendingWant>(target);
    std::shared_ptr<WantAgent> wantAgent = std::make_shared<WantAgent>(pendingWant);
    auto type = static_cast<int32_t>(WantAgentConstant::OperationType::UNKNOWN_TYPE);
    EXPECT_EQ(wantAgentHelper->GetType(wantAgent, type), ERR_ABILITY_RUNTIME_EXTERNAL_INVALID_WANTAGENT);
    EXPECT_EQ(type, static_cast<int32_t>(WantAgentConstant::OperationType::UNKNOWN_TYPE));
}

/*
 * @tc.number    : WantAgentHelper_4000
 * @tc.name      : WantAgentHelper GetWant
 * @tc.desc      : 1.GetWant WantAgent is nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_4000, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<WantAgent> wantAgent(nullptr);
    std::shared_ptr<AAFwk::Want> want(nullptr);
    EXPECT_EQ(wantAgentHelper->GetWant(wantAgent, want), ERR_ABILITY_RUNTIME_EXTERNAL_INVALID_WANTAGENT);
    EXPECT_EQ(want, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_4100
 * @tc.name      : WantAgentHelper GetWant
 * @tc.desc      : 1.GetWant WantAgent.PendingWant is nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_4100, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    std::shared_ptr<PendingWant> pendingWant(nullptr);
    std::shared_ptr<WantAgent> wantAgent = std::make_shared<WantAgent>(pendingWant);
    std::shared_ptr<AAFwk::Want> want(nullptr);
    EXPECT_EQ(wantAgentHelper->GetWant(wantAgent, want), ERR_ABILITY_RUNTIME_EXTERNAL_INVALID_WANTAGENT);
    EXPECT_EQ(want, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_4200
 * @tc.name      : WantAgentHelper GetWant
 * @tc.desc      : 1.GetWant WantAgent and WantAgent.PendingWant is not nullptr
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_4200, Function | MediumTest | Level1)
{
    std::shared_ptr<WantAgentHelper> wantAgentHelper = std::make_shared<WantAgentHelper>();
    sptr<AAFwk::IWantSender> target(nullptr);
    std::shared_ptr<PendingWant> pendingWant = std::make_shared<PendingWant>(target);
    std::shared_ptr<WantAgent> wantAgent = std::make_shared<WantAgent>(pendingWant);
    std::shared_ptr<AAFwk::Want> want(nullptr);
    EXPECT_EQ(wantAgentHelper->GetWant(wantAgent, want), ERR_ABILITY_RUNTIME_EXTERNAL_INVALID_WANTAGENT);
    EXPECT_EQ(want, nullptr);
}

/*
 * @tc.number    : WantAgentHelper_4500
 * @tc.name      : WantAgentHelper RegisterCancelListener
 * @tc.desc      : 1.RegisterCancelListener when agent is null.
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_4500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "WantAgentHelper::RegisterCancelListener start";

    std::shared_ptr<CancelListener> cancelListener1 = std::make_shared<CancelListenerSon>();
    std::shared_ptr<CancelListener> cancelListener2 = std::make_shared<CancelListenerSon>();

    WantAgentHelper::RegisterCancelListener(cancelListener1, nullptr);
    WantAgentHelper::RegisterCancelListener(cancelListener2, nullptr);

    GTEST_LOG_(INFO) << "WantAgentHelper::RegisterCancelListener end";
}

/*
 * @tc.number    : WantAgentHelper_4600
 * @tc.name      : WantAgentHelper RegisterCancelListener
 * @tc.desc      : 2.RegisterCancelListener normal test
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_4600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "WantAgentHelper::RegisterCancelListener start";

    int requestCode = 10;
    std::shared_ptr<Want> want = std::make_shared<Want>();
    ElementName element("device", "bundleName", "abilityName");
    want->SetElement(element);
    unsigned int flags = 1;
    flags |= FLAG_ONE_SHOT;
    WantAgentConstant::OperationType type = WantAgentConstant::OperationType::START_FOREGROUND_SERVICE;
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetInstance();
    std::shared_ptr<PendingWant> pendingWant = nullptr;
    PendingWant::BuildServicePendingWant(context, requestCode, want, flags, type, pendingWant);
    std::shared_ptr<WantAgent> wantAgent = std::make_shared<WantAgent>(pendingWant);

    std::shared_ptr<CancelListener> cancelListener = std::make_shared<CancelListenerSon>();

    WantAgentHelper::RegisterCancelListener(cancelListener, wantAgent);

    EXPECT_EQ(static_cast<int>(wantAgent->GetPendingWant()->cancelListeners_.size()), 1);

    GTEST_LOG_(INFO) << "WantAgentHelper::RegisterCancelListener end";
}

/*
 * @tc.number    : WantAgentHelper_4700
 * @tc.name      : WantAgentHelper UnregisterCancelListener
 * @tc.desc      : 1.UnregisterCancelListener when agent is nullptr.
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_4700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "WantAgentHelper::UnregisterCancelListener start";

    std::shared_ptr<CancelListener> cancelListener1 = std::make_shared<CancelListenerSon>();
    std::shared_ptr<CancelListener> cancelListener2 = std::make_shared<CancelListenerSon>();
    WantAgentHelper::UnregisterCancelListener(cancelListener1, nullptr);
    WantAgentHelper::UnregisterCancelListener(cancelListener2, nullptr);

    GTEST_LOG_(INFO) << "WantAgentHelper::UnregisterCancelListener end";
}

/*
 * @tc.number    : WantAgentHelper_4800
 * @tc.name      : WantAgentHelper UnregisterCancelListener
 * @tc.desc      : 2.UnregisterCancelListener normal test.
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_4800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "WantAgentHelper::UnregisterCancelListener start";

    int requestCode = 10;
    std::shared_ptr<Want> want = std::make_shared<Want>();
    ElementName element("device", "bundleName", "abilityName");
    want->SetElement(element);
    unsigned int flags = 1;
    flags |= FLAG_ONE_SHOT;
    WantAgentConstant::OperationType type = WantAgentConstant::OperationType::START_FOREGROUND_SERVICE;
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetInstance();
    std::shared_ptr<PendingWant> pendingWant = nullptr;
    PendingWant::BuildServicePendingWant(context, requestCode, want, flags, type, pendingWant);
    std::shared_ptr<WantAgent> wantAgent = std::make_shared<WantAgent>(pendingWant);

    std::shared_ptr<CancelListener> cancelListener = std::make_shared<CancelListenerSon>();

    WantAgentHelper::RegisterCancelListener(cancelListener, wantAgent);
    EXPECT_EQ(static_cast<int>(wantAgent->GetPendingWant()->cancelListeners_.size()), 1);

    WantAgentHelper::UnregisterCancelListener(cancelListener, wantAgent);
    EXPECT_EQ(static_cast<int>(wantAgent->GetPendingWant()->cancelListeners_.size()), 0);

    GTEST_LOG_(INFO) << "WantAgentHelper::UnregisterCancelListener end";
}

/*
 * @tc.number    : WantAgentHelper_4900
 * @tc.name      : WantAgentHelper TriggerWantAgent
 * @tc.desc      : Test TriggerWantAgent when agent is nullptr.
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_4900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "WantAgentHelper::TriggerWantAgent start";

    std::shared_ptr<CompletedCallback> callback;
    TriggerInfo paramsInfo;
    WantAgentHelper::TriggerWantAgent(nullptr, callback, paramsInfo);

    GTEST_LOG_(INFO) << "WantAgentHelper::TriggerWantAgent end";
}

/*
 * @tc.number    : WantAgentHelper_5000
 * @tc.name      : WantAgentHelper TriggerWantAgent
 * @tc.desc      : Test TriggerWantAgent when callback is nullptr.
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_5000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "WantAgentHelper::TriggerWantAgent start";

    std::shared_ptr<WantAgent> wantAgent(nullptr);
    TriggerInfo paramsInfo;
    WantAgentHelper::TriggerWantAgent(wantAgent, nullptr, paramsInfo);

    GTEST_LOG_(INFO) << "WantAgentHelper::TriggerWantAgent end";
}

/*
 * @tc.number    : WantAgentHelper_5100
 * @tc.name      : WantAgentHelper TriggerWantAgent
 * @tc.desc      : Test TriggerWantAgent.
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_5100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "WantAgentHelper::TriggerWantAgent start";

    std::shared_ptr<WantAgent> wantAgent(nullptr);
    std::shared_ptr<CompletedCallback> callback;
    TriggerInfo paramsInfo;
    WantAgentHelper::TriggerWantAgent(wantAgent, callback, paramsInfo);

    GTEST_LOG_(INFO) << "WantAgentHelper::TriggerWantAgent end";
}

/*
 * @tc.number    : WantAgentHelper_5200
 * @tc.name      : WantAgentHelper ToString
 * @tc.desc      : Test ToString.
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_5200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "WantAgentHelper::ToString start";
    std::shared_ptr<WantAgent> agent(nullptr);
    auto result = WantAgentHelper::ToString(agent);
    EXPECT_TRUE(result == "");
    GTEST_LOG_(INFO) << "WantAgentHelper::ToString end";
}

/*
 * @tc.number    : WantAgentHelper_5300
 * @tc.name      : WantAgentHelper FromString
 * @tc.desc      : Test FromString.
 */
HWTEST_F(WantAgentHelperTest, WantAgentHelper_5300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "WantAgentHelper::FromString start";
    std::string jsonString;
    auto result = WantAgentHelper::FromString(jsonString);
    EXPECT_TRUE(result == nullptr);
    GTEST_LOG_(INFO) << "WantAgentHelper::FromString end";
}
}  // namespace OHOS::AbilityRuntime::WantAgent
