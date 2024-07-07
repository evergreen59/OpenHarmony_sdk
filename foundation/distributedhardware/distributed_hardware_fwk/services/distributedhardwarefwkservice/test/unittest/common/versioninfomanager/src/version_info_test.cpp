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

#include "version_info_test.h"

#include <cerrno>
#include <sys/stat.h>
#include <sys/types.h>

#define private public
#include "version_info_manager.h"
#include "version_manager.h"
#include "version_info.h"
#undef private
#include "dh_context.h"
#include "distributed_hardware_errno.h"
#include "distributed_hardware_log.h"

using namespace testing::ext;
using namespace std;
namespace OHOS {
namespace DistributedHardware {
#undef DH_LOG_TAG
#define DH_LOG_TAG "VersionInfoTest"

namespace {
const string DEV_ID_1 = "bb536a637105409e904d4da83790a4a7";
const string NAME_SCREEN = "distributed_screen";
const string VERSION_1 = "1.0";
}

void VersionInfoTest::SetUpTestCase(void)
{
}

void VersionInfoTest::TearDownTestCase(void)
{
}

void VersionInfoTest::SetUp()
{
}

void VersionInfoTest::TearDown()
{
}

/**
 * @tc.name:version_info_test_001
 * @tc.desc: Verify the VersionInfoTest ToJson function.
 * @tc.type: FUNC
 * @tc.require: AR000GHSCV
 */
HWTEST_F(VersionInfoTest, version_info_test_001, TestSize.Level0)
{
    CompVersion compVersions1 = {
        .name = NAME_SCREEN,
        .dhType = DHType::SCREEN,
        .handlerVersion = VERSION_1,
        .sourceVersion = VERSION_1,
        .sinkVersion = VERSION_1
    };

    VersionInfo verInfo1;
    verInfo1.deviceId = DEV_ID_1;
    verInfo1.dhVersion = VERSION_1;
    verInfo1.compVersions.insert(std::pair<DHType, CompVersion>(compVersions1.dhType, compVersions1));

    std::string jsonStr = verInfo1.ToJsonString();
    EXPECT_NE(jsonStr.empty(), true);

    VersionInfo verInfo2;
    verInfo2.FromJsonString(jsonStr);

    EXPECT_EQ(verInfo2.ToJsonString(), jsonStr);
}
} // namespace DistributedHardware
} // namespace OHOS
