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

#include "nweb_adapter_helper.h"
#include "nweb_cookie_manager.h"
#include "nweb_helper.h"
#include "nweb.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS::NWeb {
namespace {
const std::string MOCK_INSTALLATION_DIR = "/data/app/el1/bundle/public/com.ohos.nweb";
} // namespace

class NapiWebCookieManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NapiWebCookieManagerTest::SetUpTestCase(void)
{}

void NapiWebCookieManagerTest::TearDownTestCase(void)
{}

void NapiWebCookieManagerTest::SetUp(void)
{}

void NapiWebCookieManagerTest::TearDown(void)
{}

/**
 * @tc.name: NapiWebCookieManagerTest_001
 * @tc.desc: GetCookieManager.
 * @tc.type: FUNC
 * @tc.require: I5P91V
 */
HWTEST_F(NapiWebCookieManagerTest, NapiWebCookieManagerTest_001, TestSize.Level1)
{
    NWebHelper::Instance().SetBundlePath(MOCK_INSTALLATION_DIR);
    NWebAdapterHelper::Instance().Init(false);
    NWebCookieManager* cookieManager = NWebHelper::Instance().GetCookieManager();
    EXPECT_NE(cookieManager, nullptr);
}
} // namespace OHOS::NWeb
