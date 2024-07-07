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

#include "net_policy_event_handler.h"
#include "net_policy_core.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr const char *NET_POLICY_WORK_TEST_THREAD = "NET_POLICY_WORK_TEST_THREAD";
std::shared_ptr<AppExecFwk::EventRunner> g_runner = nullptr;
std::shared_ptr<NetPolicyCore> g_netPolicyCore = nullptr;
} // namespace

class UtNetPolicyEventHandlerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<NetPolicyEventHandler> instance_ = nullptr;
};

void UtNetPolicyEventHandlerTest::SetUpTestCase()
{
    g_runner = AppExecFwk::EventRunner::Create(NET_POLICY_WORK_TEST_THREAD);
    g_netPolicyCore = DelayedSingleton<NetPolicyCore>::GetInstance();
    instance_ = std::make_shared<NetPolicyEventHandler>(g_runner, g_netPolicyCore);
}

void UtNetPolicyEventHandlerTest::TearDownTestCase() {}

void UtNetPolicyEventHandlerTest::SetUp() {}

void UtNetPolicyEventHandlerTest::TearDown() {}

HWTEST_F(UtNetPolicyEventHandlerTest, ProcessEventTest001, TestSize.Level1)
{
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get();
    event.reset();
    instance_->ProcessEvent(event);
}

HWTEST_F(UtNetPolicyEventHandlerTest, ProcessEventTest002, TestSize.Level1)
{
    std::shared_ptr<NetPolicyCore> netPolicyCore = nullptr;
    auto instance = std::make_shared<NetPolicyEventHandler>(g_runner, netPolicyCore);
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get();
    instance->ProcessEvent(event);
}

HWTEST_F(UtNetPolicyEventHandlerTest, ProcessEventTest003, TestSize.Level1)
{
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get();
    instance_->ProcessEvent(event);
}

} // namespace NetManagerStandard
} // namespace OHOS