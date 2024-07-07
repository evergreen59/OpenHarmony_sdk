/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <gtest/gtest.h>
#include "hi3516_audio_driver_test.h"
#include "hdf_uhdf_test.h"

using namespace testing::ext;

namespace {
class Hi3516DaiOpsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void Hi3516DaiOpsTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void Hi3516DaiOpsTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void Hi3516DaiOpsTest::SetUp()
{
}

void Hi3516DaiOpsTest::TearDown()
{
}

// dai driver test
HWTEST_F(Hi3516DaiOpsTest, Hi3516DaiOpsTest_DaiDeviceInit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTDAIDEVICEINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DaiOpsTest, Hi3516DaiOpsTest_DaiStartup, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTDAISTARTUP, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}


HWTEST_F(Hi3516DaiOpsTest, Hi3516DaiOpsTest_DaiHwParams, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTDAIHWPARAMS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DaiOpsTest, Hi3516DaiOpsTest_Trigger, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTDAITRIGGER, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
