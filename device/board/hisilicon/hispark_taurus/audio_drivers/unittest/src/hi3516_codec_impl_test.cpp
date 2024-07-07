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
class Hi3516CodecImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void Hi3516CodecImplTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void Hi3516CodecImplTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void Hi3516CodecImplTest::SetUp()
{
}

void Hi3516CodecImplTest::TearDown()
{
}


HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_CodecHalSysInit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTCODECHALSYSINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_AcodecRegBitsRead, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTACODECREGBITSREAD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_CodecRegBitsUpdate, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTCODECREGBITSUPDATE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_CodecRegDefaultInit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTCODECREGDEFAULTINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_CodecSetAdcTuneEnable, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTCODECSETADCTUNEENABLE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_CodecDaiParamsUpdate, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTCODECDAIPARAMSUPDATE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_AudioCodecAiaoSetCtrlOps, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAUDIOCODECAIAOSETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_AudioCodecAiaoGetCtrlOps, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAUDIOCODECAIAOGETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
