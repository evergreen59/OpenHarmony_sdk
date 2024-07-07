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

#include <functional>
#include <gtest/gtest.h>

#define private public
#define protected public
#include "bundle_manager_helper.h"
#undef private
#undef protected

#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "access_token_helper.h"

using namespace testing::ext;
namespace OHOS {
namespace Notification {
class BundleManagerHelperTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number    : BundleManagerHelperTest_00100
 * @tc.name      : ANS_GetBundleNameByUid_0100
 * @tc.desc      : Test GetBundleNameByUid function
 */
HWTEST_F(BundleManagerHelperTest, BundleManagerHelperTest_00100, Function | SmallTest | Level1)
{
    pid_t callingUid = IPCSkeleton::GetCallingUid();
    std::shared_ptr<BundleManagerHelper> bundleManager = BundleManagerHelper::GetInstance();
    EXPECT_EQ(bundleManager->GetBundleNameByUid(callingUid), "bundleName");
}

/**
 * @tc.number    : BundleManagerHelperTest_00200
 * @tc.name      : ANS_IsSystemApp_0100
 * @tc.desc      : Test IsSystemApp function
 */
HWTEST_F(BundleManagerHelperTest, BundleManagerHelperTest_00200, Function | SmallTest | Level1)
{
    pid_t callingUid = IPCSkeleton::GetCallingUid();
    std::shared_ptr<BundleManagerHelper> bundleManager = BundleManagerHelper::GetInstance();
    EXPECT_TRUE(bundleManager->IsSystemApp(callingUid));
}

/**
 * @tc.number    : BundleManagerHelperTest_00300
 * @tc.name      : CheckApiCompatibility
 * @tc.desc      : Test CheckApiCompatibility function when the  bundleOption is nullptr,return is true
 * @tc.require   : issueI5S4VP
 */
HWTEST_F(BundleManagerHelperTest, BundleManagerHelperTest_00300, Level1)
{
    sptr<NotificationBundleOption> bundleOption = nullptr;
    BundleManagerHelper bundleManagerHelper;
    bool result = bundleManagerHelper.CheckApiCompatibility(bundleOption);
    EXPECT_EQ(result, true);
}

/**
 * @tc.number    : BundleManagerHelperTest_00400
 * @tc.name      : GetBundleInfoByBundleName
 * @tc.desc      : get bundleinfo by bundlename when the parameeter are normal, return is true
 * @tc.require   : issueI5S4VP
 */
HWTEST_F(BundleManagerHelperTest, BundleManagerHelperTest_00400, Level1)
{
    std::string bundle = "Bundle";
    int32_t userId = 1;
    AppExecFwk::BundleInfo bundleInfo;
    BundleManagerHelper bundleManagerHelper;
    bool result = bundleManagerHelper.GetBundleInfoByBundleName(bundle, userId, bundleInfo);
    EXPECT_EQ(result, true);
}

/**
 * @tc.number    : BundleManagerHelperTest_00500
 * @tc.name      : GetDefaultUidByBundleName
 * @tc.desc      : Test GetDefaultUidByBundleName function  when the parameeter are normal
 * @tc.require   : issueI5S4VP
 */
HWTEST_F(BundleManagerHelperTest, BundleManagerHelperTest_00500, Level1)
{
    std::string bundle = "Bundle";
    int32_t userId = 1;
    BundleManagerHelper bundleManagerHelper;
    int32_t result = bundleManagerHelper.GetDefaultUidByBundleName(bundle, userId);
    EXPECT_EQ(result, 1000);
}

/**
 * @tc.number    : GetDistributedNotificationEnabled_00100
 * @tc.name      : GetDistributedNotificationEnabled
 * @tc.desc      : Test GetDistributedNotificationEnabled function  when the parameeter are normal
 */
HWTEST_F(BundleManagerHelperTest, GetDistributedNotificationEnabled_00100, Level1)
{
    std::string bundle = "Bundle";
    int32_t userId = 1;
    BundleManagerHelper bundleManagerHelper;
    bool result = bundleManagerHelper.GetDistributedNotificationEnabled(bundle, userId);
    EXPECT_EQ(result, true);
}
}  // namespace Notification
}  // namespace OHOS
