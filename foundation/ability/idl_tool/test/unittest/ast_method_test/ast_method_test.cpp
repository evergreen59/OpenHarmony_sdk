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
#include "ast/ast_method.h"
#include "ast/ast_void_type.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS {
namespace idl {
class AstMethodUnitTest : public testing::Test {
public:
    AstMethodUnitTest() {}

    virtual ~AstMethodUnitTest() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void AstMethodUnitTest::SetUpTestCase() {}

void AstMethodUnitTest::TearDownTestCase() {}

void AstMethodUnitTest::SetUp() {}

void AstMethodUnitTest::TearDown() {}

/*
 * @tc.name: GetParameterTest_0100
 * @tc.desc: test GetParameter in AstMethodUnitTest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstMethodUnitTest, GetParameterTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstMethodUnitTest, GetParameterTest_0100, TestSize.Level1";
    std::shared_ptr<ASTMethod> imageASTMethod = std::make_shared<ASTMethod>();
    size_t index = 3;
    AutoPtr<ASTParameter> result = imageASTMethod->GetParameter(index);

    EXPECT_EQ(result, nullptr);
}
} // namespace idl
} // namespace OHOS
