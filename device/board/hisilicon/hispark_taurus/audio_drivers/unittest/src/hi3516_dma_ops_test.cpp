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
#include "hdf_uhdf_test.h"
#include "hi3516_audio_driver_test.h"

using namespace testing::ext;

namespace {
class Hi3516DmaOpsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void Hi3516DmaOpsTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void Hi3516DmaOpsTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void Hi3516DmaOpsTest::SetUp()
{
}

void Hi3516DmaOpsTest::TearDown()
{
}

// dma test
HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_AudioDmaDeviceInit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAUDIODMADEVICEINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaBufAlloc, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMABUFALLOC, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaBufFree, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMABUFFREE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaRequestChannel, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMAREQUESTCHANNEL, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaConfigChannel, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMACONFIGCHANNEL, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaPrep, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMAPREP, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}


HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaSubmit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMASUBMIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaPending, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMAPENDING, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaPause, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMAPAUSE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}


HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaResume, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMARESUME, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaPointer, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMAPOINTER, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
