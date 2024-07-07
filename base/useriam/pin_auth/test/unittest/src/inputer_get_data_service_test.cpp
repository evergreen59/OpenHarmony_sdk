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

#include "inputer_get_data_service_test.h"

#include "iam_ptr.h"
#include "inputer_get_data_service.h"
#include "mock_inputer.h"
#include "mock_inputer_set_data.h"

namespace OHOS {
namespace UserIam {
namespace PinAuth {
using namespace testing;
using namespace testing::ext;

void InputerGetDataServiceTest::SetUpTestCase()
{
}

void InputerGetDataServiceTest::TearDownTestCase()
{
}

void InputerGetDataServiceTest::SetUp()
{
}

void InputerGetDataServiceTest::TearDown()
{
}

HWTEST_F(InputerGetDataServiceTest, InputerGetDataServiceTest001, TestSize.Level0)
{
    int32_t testAuthSubType = 10000;
    std::vector<uint8_t> testSalt = {1, 2, 3, 4, 5};
    sptr<InputerSetData> testInputerSetData = new MockInputerSetData();
    EXPECT_NE(testInputerSetData, nullptr);

    auto testInputer = Common::MakeShared<MockInputer>();
    EXPECT_NE(testInputer, nullptr);

    EXPECT_CALL(*testInputer, OnGetData(_, _))
        .Times(Exactly(1))
        .WillOnce([&testAuthSubType](int32_t authSubType, std::shared_ptr<IInputerData> inputerData) {
            EXPECT_EQ(authSubType, testAuthSubType);
            return;
        });

    auto service = Common::MakeShared<InputerGetDataService>(testInputer);
    EXPECT_NE(service, nullptr);

    service->OnGetData(testAuthSubType, testSalt, testInputerSetData);
}
} // namespace PinAuth
} // namespace UserIam
} // namespace OHOS
