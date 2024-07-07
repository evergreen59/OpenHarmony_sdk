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
#include <cmath>
#include <cstdio>
#include <vector>

#include "nnrt_utils.h"

using namespace testing::ext;
using namespace OHOS::NeuralNetworkRuntime::Test;

class DeviceTest : public testing::Test {};

/**
 * @tc.number : SUB_AI_NNRtt_Func_North_Device_DeviceID_0100
 * @tc.name   : 获取设备ID，*allDevicesID为nullptr
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(DeviceTest, SUB_AI_NNRt_Func_North_Device_DeviceID_0100, Function | MediumTest | Level3)
{
    uint32_t count{0};

    OH_NN_ReturnCode ret = OH_NNDevice_GetAllDevicesID(nullptr, &count);
    EXPECT_EQ(OH_NN_INVALID_PARAMETER, ret);
}

/**
 * @tc.number : SUB_AI_NNRt_Func_North_Device_DeviceID_0200
 * @tc.name   : 获取设备ID，**allDevicesID非nullptr
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(DeviceTest, SUB_AI_NNRt_Func_North_Device_DeviceID_0200, Function | MediumTest | Level3)
{
    const size_t allDeviceIds = 0;
    const size_t *pAllDeviceIds = &allDeviceIds;
    uint32_t count{0};

    OH_NN_ReturnCode ret = OH_NNDevice_GetAllDevicesID(&pAllDeviceIds, &count);
    EXPECT_EQ(OH_NN_INVALID_PARAMETER, ret);
}

/**
 * @tc.number : SUB_AI_NNRt_Func_North_Device_DeviceID_0300
 * @tc.name   : 获取设备ID，获取设备ID，deviceCount为nullptr
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(DeviceTest, SUB_AI_NNRt_Func_North_Device_DeviceID_0300, Function | MediumTest | Level3)
{
    const size_t *allDeviceIds = nullptr;

    OH_NN_ReturnCode ret = OH_NNDevice_GetAllDevicesID(&allDeviceIds, nullptr);
    EXPECT_EQ(OH_NN_INVALID_PARAMETER, ret);
}

/**
 * @tc.number : SUB_AI_NNRt_Func_North_Device_DeviceID_0400
 * @tc.name   : 获取设备ID，设备数量校验
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(DeviceTest, SUB_AI_NNRt_Func_North_Device_DeviceID_0400, Function | MediumTest | Level2)
{
    const size_t *allDeviceIds = nullptr;
    uint32_t count{0};

    OH_NN_ReturnCode ret = OH_NNDevice_GetAllDevicesID(&allDeviceIds, &count);
    EXPECT_EQ(OH_NN_SUCCESS, ret);

    uint32_t expectCount = 1;
    EXPECT_EQ(expectCount, count);
}

/**
 * @tc.number : SUB_AI_NNRt_Func_North_Device_DeviceName_0100
 * @tc.name   : 获取硬件名称，deviceID不存在
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(DeviceTest, SUB_AI_NNRt_Func_North_Device_DeviceName_0100, Function | MediumTest | Level3)
{
    const size_t deviceID{100000};
    const char *name = nullptr;

    OH_NN_ReturnCode ret = OH_NNDevice_GetName(deviceID, &name);
    EXPECT_EQ(OH_NN_FAILED, ret);
}

/**
 * @tc.number : SUB_AI_NNRt_Func_North_Device_DeviceName_0200
 * @tc.name   : 获取硬件名称，*name为nullprt
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(DeviceTest, SUB_AI_NNRt_Func_North_Device_DeviceName_0200, Function | MediumTest | Level3)
{
    const size_t *devicesID{nullptr};
    uint32_t devicesCount{0};
    ASSERT_EQ(OH_NN_SUCCESS, OH_NNDevice_GetAllDevicesID(&devicesID, &devicesCount));
    size_t targetDevice = devicesID[0];

    OH_NN_ReturnCode ret = OH_NNDevice_GetName(targetDevice, nullptr);
    EXPECT_EQ(OH_NN_INVALID_PARAMETER, ret);
}

/**
 * @tc.number : SUB_AI_NNRt_Func_North_Device_DeviceName_0300
 * @tc.name   : 获取硬件名称，**name非nullptr
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(DeviceTest, SUB_AI_NNRt_Func_North_Device_DeviceName_0300, Function | MediumTest | Level3)
{
    const size_t *devicesID{nullptr};
    uint32_t devicesCount{0};
    ASSERT_EQ(OH_NN_SUCCESS, OH_NNDevice_GetAllDevicesID(&devicesID, &devicesCount));
    size_t targetDevice = devicesID[0];

    const char *name = "name";

    OH_NN_ReturnCode ret = OH_NNDevice_GetName(targetDevice, &name);
    EXPECT_EQ(OH_NN_INVALID_PARAMETER, ret);
}

/**
 * @tc.number : SUB_AI_NNRt_Func_North_Device_DeviceName_0400
 * @tc.name   : 获取硬件名称， 结果校验
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(DeviceTest, SUB_AI_NNRt_Func_North_Device_DeviceName_0400, Function | MediumTest | Level1)
{
    const size_t *devicesID{nullptr};
    uint32_t devicesCount{0};
    ASSERT_EQ(OH_NN_SUCCESS, OH_NNDevice_GetAllDevicesID(&devicesID, &devicesCount));
    size_t targetDevice = devicesID[0];

    const char *name = nullptr;
    std::string m_deviceName{"Device-CPU_TestVendor"};

    OH_NN_ReturnCode ret = OH_NNDevice_GetName(targetDevice, &name);
    EXPECT_EQ(OH_NN_SUCCESS, ret);
    std::string sName(name);
    EXPECT_EQ(m_deviceName, sName);
}

/**
 * @tc.number : SUB_AI_NNRt_Func_North_Device_DeviceType_0100
 * @tc.name   : 获取硬件类别，deviceType为nullprt
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(DeviceTest, SUB_AI_NNRt_Func_North_Device_DeviceType_0100, Function | MediumTest | Level3)
{
    const size_t *devicesID{nullptr};
    uint32_t devicesCount{0};
    ASSERT_EQ(OH_NN_SUCCESS, OH_NNDevice_GetAllDevicesID(&devicesID, &devicesCount));
    size_t targetDevice = devicesID[0];

    OH_NN_ReturnCode ret = OH_NNDevice_GetType(targetDevice, nullptr);
    EXPECT_EQ(OH_NN_INVALID_PARAMETER, ret);
}

/**
 * @tc.number : SUB_AI_NNRt_Func_North_Device_DeviceType_0200
 * @tc.name   : 获取硬件类别，deviceID不存在
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(DeviceTest, SUB_AI_NNRt_Func_North_Device_DeviceType_0200, Function | MediumTest | Level3)
{
    const size_t deviceID{100000};
    OH_NN_DeviceType type{OH_NN_OTHERS};

    OH_NN_ReturnCode ret = OH_NNDevice_GetType(deviceID, &type);
    EXPECT_EQ(OH_NN_INVALID_PARAMETER, ret);
}

/**
 * @tc.number : SUB_AI_NNRt_Func_North_Device_DeviceType_0300
 * @tc.name   :获取硬件类别，结果校验
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(DeviceTest, SUB_AI_NNRt_Func_North_Device_DeviceType_0300, Function | MediumTest | Level1)
{
    const size_t *devicesID{nullptr};
    uint32_t devicesCount{0};
    ASSERT_EQ(OH_NN_SUCCESS, OH_NNDevice_GetAllDevicesID(&devicesID, &devicesCount));
    size_t targetDevice = devicesID[0];

    OH_NN_DeviceType type{OH_NN_OTHERS};

    OH_NN_ReturnCode ret = OH_NNDevice_GetType(targetDevice, &type);
    EXPECT_EQ(OH_NN_SUCCESS, ret);
}