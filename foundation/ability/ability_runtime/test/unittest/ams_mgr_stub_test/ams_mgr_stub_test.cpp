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

#include "mock_ams_mgr_scheduler.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace AppExecFwk {
namespace {}  // namespace

class AmsMgrStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<MockAmsMgrScheduler> mockAmsMgrScheduler_;

    void WriteInterfaceToken(MessageParcel &data);
};

void AmsMgrStubTest::SetUpTestCase(void)
{}

void AmsMgrStubTest::TearDownTestCase(void)
{}

void AmsMgrStubTest::SetUp()
{
    GTEST_LOG_(INFO) << "AmsMgrStubTest::SetUp()";

    mockAmsMgrScheduler_ = new MockAmsMgrScheduler();
}

void AmsMgrStubTest::TearDown()
{}

void AmsMgrStubTest::WriteInterfaceToken(MessageParcel &data)
{
    GTEST_LOG_(INFO) << "AmsMgrStubTest::WriteInterfaceToken()";

    data.WriteInterfaceToken(AmsMgrStub::GetDescriptor());
}
}  // namespace AppExecFwk
}  // namespace OHOS
