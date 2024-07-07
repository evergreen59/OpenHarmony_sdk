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
#include <iservice_registry.h>
#include <native_buffer.h>
#include "surface_type.h"
#include "graphic_common_c.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class NativeBufferTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline OH_NativeBuffer_Config config = {
        .width = 0x100,
        .height = 0x100,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
    };
    static inline OH_NativeBuffer_Config checkConfig = {};
    static inline OH_NativeBuffer* buffer = nullptr;
};

void NativeBufferTest::SetUpTestCase()
{
    buffer = nullptr;
}

void NativeBufferTest::TearDownTestCase()
{
    buffer = nullptr;
}

/*
 * @tc.name  OHNativeBufferAlloc001
 * @tc.desc  test for call OH_NativeBuffer_Alloc by abnormal input and check ret
 * @tc.type  FUNC
 */
HWTEST_F(NativeBufferTest, OHNativeBufferAlloc001, Function | MediumTest | Level2)
{
    buffer = OH_NativeBuffer_Alloc(nullptr);
    ASSERT_EQ(buffer, nullptr);
}

/*
 * @tc.name  OHNativeBufferAlloc002
 * @tc.desc  test for call OH_NativeBuffer_Alloc and check ret
 * @tc.type  FUNC
 */
HWTEST_F(NativeBufferTest, OHNativeBufferAlloc002, Function | MediumTest | Level2)
{
    buffer = OH_NativeBuffer_Alloc(&config);
    ASSERT_NE(buffer, nullptr);
}

/*
 * @tc.name  OHNativeBufferGetSeqNum001
 * @tc.desc  test for call OH_NativeBuffer_GetSeqNum by abnormal input and check ret
 * @tc.type  FUNC
 */
HWTEST_F(NativeBufferTest, OHNativeBufferGetSeqNum001, Function | MediumTest | Level2)
{
    uint32_t id = OH_NativeBuffer_GetSeqNum(nullptr);
    ASSERT_EQ(id, GSERROR_INVALID_ARGUMENTS);
}

/*
 * @tc.name  OHNativeBufferGetSeqNum002
 * @tc.desc  test for call OH_NativeBuffer_GetSeqNum and check ret
 * @tc.type  FUNC
 */
HWTEST_F(NativeBufferTest, OHNativeBufferGetSeqNum002, Function | MediumTest | Level2)
{
    uint32_t id = OH_NativeBuffer_GetSeqNum(buffer);
    ASSERT_NE(id, GSERROR_INVALID_ARGUMENTS);
}

/*
 * @tc.name  OHNativeBufferGetConfig001
 * @tc.desc  test for call OH_NativeBuffer_GetConfig and check ret
 * @tc.type  FUNC
 */
HWTEST_F(NativeBufferTest, OHNativeBufferGetConfig001, Function | MediumTest | Level2)
{
    OH_NativeBuffer_GetConfig(buffer, &checkConfig);
    ASSERT_NE(&checkConfig, nullptr);
}

/*
 * @tc.name  OHNativeBufferGetConfig002
 * @tc.desc  test for call OH_NativeBuffer_GetConfig by abnormal input and check ret
 * @tc.type  FUNC
 */
HWTEST_F(NativeBufferTest, OHNativeBufferGetConfig002, Function | MediumTest | Level2)
{
    checkConfig.width = 0x0;
    checkConfig.height = 0x0;
    checkConfig.format = 0x0;
    checkConfig.usage = 0x0;
    OH_NativeBuffer_GetConfig(nullptr, &checkConfig);
    ASSERT_EQ(checkConfig.width, 0x0);
    ASSERT_EQ(checkConfig.height, 0x0);
    ASSERT_EQ(checkConfig.format, 0x0);
    ASSERT_EQ(checkConfig.usage, 0x0);
}


/*
 * @tc.name  OHNativeBufferReference001
 * @tc.desc  test for call OH_NativeBuffer_Reference by abnormal input and check ret
 * @tc.type  FUNC
 */
HWTEST_F(NativeBufferTest, OHNativeBufferReference001, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeBuffer_Reference(nullptr);
    ASSERT_NE(ret, GSERROR_OK);
}

/*
 * @tc.name  OHNativeBufferReference002
 * @tc.desc  test for call OH_NativeBuffer_Reference and check ret
 * @tc.type  FUNC
 */
HWTEST_F(NativeBufferTest, OHNativeBufferReference002, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeBuffer_Reference(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
 * @tc.name  OHNativeBufferUnreference001
 * @tc.desc  test for call OH_NativeBuffer_Unreference by abnormal input and check ret
 * @tc.type  FUNC
 */
HWTEST_F(NativeBufferTest, OHNativeBufferUnreference001, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeBuffer_Unreference(nullptr);
    ASSERT_NE(ret, GSERROR_OK);
}

/*
 * @tc.name  OHNativeBufferUnreference002
 * @tc.desc  test for call OH_NativeBuffer_Unreference and check ret
 * @tc.type  FUNC
 */
HWTEST_F(NativeBufferTest, OHNativeBufferUnreference002, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeBuffer_Unreference(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
 * @tc.name  OHNativeBufferGetSeqNum003
 * @tc.desc  test for check OH_NativeBuffer_GetSeqNum
 * @tc.type  FUNC
 */
HWTEST_F(NativeBufferTest, OHNativeBufferGetSeqNum003, Function | MediumTest | Level2)
{
    uint32_t oldSeq = OH_NativeBuffer_GetSeqNum(buffer);
    int32_t ret = OH_NativeBuffer_Unreference(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
    buffer = OH_NativeBuffer_Alloc(&config);
    ASSERT_EQ(oldSeq + 1, OH_NativeBuffer_GetSeqNum(buffer));
}

/*
 * @tc.name  OHNativeBufferMap001
 * @tc.desc  test for call OH_NativeBuffer_Map by abnormal input and check ret
 * @tc.type  FUNC
 */
HWTEST_F(NativeBufferTest, OHNativeBufferMap001, Function | MediumTest | Level2)
{
    void *virAddr = nullptr;
    int32_t ret = OH_NativeBuffer_Map(nullptr, &virAddr);
    ASSERT_NE(ret, GSERROR_OK);
}

/*
 * @tc.name  OHNativeBufferMap002
 * @tc.desc  test for call OH_NativeBuffer_Map and check ret
 * @tc.type  FUNC
 */
HWTEST_F(NativeBufferTest, OHNativeBufferMap002, Function | MediumTest | Level2)
{
    void *virAddr = nullptr;
    int32_t ret = OH_NativeBuffer_Map(buffer, &virAddr);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_NE(virAddr, nullptr);
}

/*
 * @tc.name  OHNativeBufferUnmap001
 * @tc.desc  test for call OH_NativeBuffer_Unmap by abnormal input and check ret
 * @tc.type  FUNC
 */
HWTEST_F(NativeBufferTest, OHNativeBufferUnmap001, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeBuffer_Unmap(nullptr);
    ASSERT_NE(ret, GSERROR_OK);
}

/*
 * @tc.name  OHNativeBufferUnmap001
 * @tc.desc  test for call OH_NativeBuffer_Unmap
 * @tc.type  FUNC
 */
HWTEST_F(NativeBufferTest, OHNativeBufferUnmap002, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeBuffer_Unmap(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
    ret = OH_NativeBuffer_Unreference(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
}
}