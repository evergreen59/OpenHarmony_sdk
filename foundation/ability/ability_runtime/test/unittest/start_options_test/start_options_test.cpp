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

#include <gtest/gtest.h>
#include <gtest/gtest.h>

#define private public
#include "start_options.h"
#undef private

#include "ability_window_configuration.h"
#include "parcel.h"

using namespace testing::ext;
namespace OHOS {
namespace AAFwk {
namespace {
const int32_t TEST_DISPLAY_ID = 1;
} // namespace

class StartOptionsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void StartOptionsTest::SetUpTestCase()
{}

void StartOptionsTest::TearDownTestCase()
{}

void StartOptionsTest::SetUp()
{}

void StartOptionsTest::TearDown()
{}

/**
 * @tc.name: start_options_test_001
 * @tc.desc: test class StartOptions number function
 * @tc.type: FUNC
 */
HWTEST_F(StartOptionsTest, start_options_test_001, TestSize.Level1)
{
    StartOptions startOptions;
    startOptions.SetWindowMode(AbilityWindowConfiguration::MULTI_WINDOW_DISPLAY_FULLSCREEN);
    startOptions.SetDisplayID(TEST_DISPLAY_ID);
    EXPECT_EQ(startOptions.GetWindowMode(), AbilityWindowConfiguration::MULTI_WINDOW_DISPLAY_FULLSCREEN);
    EXPECT_EQ(startOptions.GetDisplayID(), TEST_DISPLAY_ID);

    StartOptions secondStartOptions(startOptions);
    EXPECT_EQ(startOptions.GetWindowMode(), AbilityWindowConfiguration::MULTI_WINDOW_DISPLAY_FULLSCREEN);
    EXPECT_EQ(startOptions.GetDisplayID(), TEST_DISPLAY_ID);

    StartOptions thirdStartOptions = secondStartOptions;
    EXPECT_EQ(startOptions.GetWindowMode(), AbilityWindowConfiguration::MULTI_WINDOW_DISPLAY_FULLSCREEN);
    EXPECT_EQ(startOptions.GetDisplayID(), TEST_DISPLAY_ID);

    Parcel parcel;
    int ret1 = startOptions.Marshalling(parcel);
    EXPECT_EQ(ret1, true);
    auto retOptions = startOptions.Unmarshalling(parcel);
    EXPECT_EQ(retOptions->GetWindowMode(), AbilityWindowConfiguration::MULTI_WINDOW_DISPLAY_FULLSCREEN);
    EXPECT_EQ(retOptions->GetDisplayID(), TEST_DISPLAY_ID);

    StartOptions fourthStartOptions;
    EXPECT_TRUE(startOptions.Marshalling(parcel));
    auto ret2 = fourthStartOptions.ReadFromParcel(parcel);
    EXPECT_EQ(ret2, true);
    EXPECT_EQ(fourthStartOptions.GetWindowMode(), AbilityWindowConfiguration::MULTI_WINDOW_DISPLAY_FULLSCREEN);
    EXPECT_EQ(fourthStartOptions.GetDisplayID(), TEST_DISPLAY_ID);
}
}  // namespace AppExecFwk
}  // namespace OHOS
