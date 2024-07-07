/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include <gtest/gtest.h>
#include <securec.h>
#include "hdf_base.h"
#include "osal_time.h"
#include "osal_mem.h"
#include "light_if.h"
#include "light_type.h"

using namespace testing::ext;

namespace {
    const struct LightInterface *g_lightDev = nullptr;
    static struct LightInfo *g_lightInfo = nullptr;
    static uint32_t g_count = 0;
    const int32_t g_onTime = 500;
    const int32_t g_offTime = 500;
    const int32_t LIGHT_WAIT_TIME = 3;
    const int32_t g_minLightId = LIGHT_ID_NONE;
    const int32_t g_maxLightId = LIGHT_ID_BUTT;
}

class HdfLightTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfLightTest::SetUpTestCase()
{
    g_lightDev = NewLightInterfaceInstance();
    if (g_lightDev == nullptr) {
        printf("test light get Module instance failed\n\r");
    }
    int32_t ret = g_lightDev->GetLightInfo(&g_lightInfo, &g_count);
    if (ret == HDF_FAILURE) {
        printf("get light information failed\n\r");
    }
}

void HdfLightTest::TearDownTestCase()
{
    if (g_lightDev != nullptr) {
        FreeLightInterfaceInstance();
        g_lightDev = nullptr;
    }
}

void HdfLightTest::SetUp()
{
}

void HdfLightTest::TearDown()
{
}

/**
  * @tc.name: CheckLightInstanceIsEmpty
  * @tc.desc: Creat a light instance. The instance is not empty.
  * @tc.type: FUNC
  */
HWTEST_F(HdfLightTest, SUB_DriverSystem_LightHdi_0010, Function | MediumTest | Level1)
{
    ASSERT_NE(nullptr, g_lightDev);
}

/**
  * @tc.name: GetLightList001
  * @tc.desc: Obtains information about all lights in the system. Validity check of input parameters.
  * @tc.type: FUNC
  */
HWTEST_F(HdfLightTest, SUB_DriverSystem_LightHdi_0100, Function | MediumTest | Level1)
{
    struct LightInfo *info = nullptr;

    if (g_lightInfo == nullptr) {
        EXPECT_NE(nullptr, g_lightInfo);
        return;
    }

    printf("get light list num[%d]\n\r", g_count);
    info = g_lightInfo;

    for (int i = 0; i < g_count; ++i) {
        printf("get lightId[%d]\n\r", info->lightId);
        EXPECT_GE(info->lightId, g_minLightId);
        EXPECT_LE(info->lightId, g_maxLightId);
        info++;
    }
}

/**
  * @tc.name: GetLightList002
  * @tc.desc: Obtains information about all lights in the system. Validity check of input parameters.
  * @tc.type: FUNC
  */
HWTEST_F(HdfLightTest, SUB_DriverSystem_LightHdi_0110 , Function | MediumTest | Level1)
{
    int32_t ret = g_lightDev->GetLightInfo(nullptr, &g_count);
    EXPECT_EQ(HDF_FAILURE, ret);
    ret = g_lightDev->GetLightInfo(&g_lightInfo, nullptr);
    EXPECT_EQ(HDF_FAILURE, ret);
    ret = g_lightDev->GetLightInfo(nullptr, nullptr);
    EXPECT_EQ(HDF_FAILURE, ret);
}

/**
  * @tc.name: EnableLight001
  * @tc.desc: Enables the light available in the light list based on the specified light id.
  * @tc.type: FUNC
  */
HWTEST_F(HdfLightTest, SUB_DriverSystem_LightHdi_0120, Function | MediumTest | Level1)
{
    int32_t i;
    int32_t ret;
    struct LightEffect effect;
    effect.lightColor.colorValue.rgbColor.r = 255;
    effect.lightColor.colorValue.rgbColor.g = 0;
    effect.lightColor.colorValue.rgbColor.b = 0;
    effect.flashEffect.flashMode = LIGHT_FLASH_NONE;
    effect.flashEffect.onTime = 0;
    effect.flashEffect.offTime = 0;

    for (i = 0; i < g_count; ++i) {
        ret = g_lightDev->TurnOnLight(g_lightInfo[i].lightId, &effect);
        EXPECT_EQ(0, ret);

        OsalSleep(LIGHT_WAIT_TIME);

        ret = g_lightDev->TurnOffLight(g_lightInfo[i].lightId);
        EXPECT_EQ(0, ret);
    }
}

/**
  * @tc.name: EnableLight002 
  * @tc.desc: Enables the light available in the light list based on the specified light id.
  * @tc.type: FUNC
  */
HWTEST_F(HdfLightTest, SUB_DriverSystem_LightHdi_0130, Function | MediumTest | Level1)
{
    int32_t i;
    int32_t ret;
    struct LightEffect effect;
    effect.lightColor.colorValue.rgbColor.r = 255;
    effect.lightColor.colorValue.rgbColor.g = 0;
    effect.lightColor.colorValue.rgbColor.b = 0;
    effect.flashEffect.flashMode = LIGHT_FLASH_BLINK;
    effect.flashEffect.onTime = g_onTime;
    effect.flashEffect.offTime = g_offTime;

    for (i = 0; i < g_count; ++i) {
        ret = g_lightDev->TurnOnLight(g_lightInfo[i].lightId, &effect);
        EXPECT_EQ(0, ret);

        OsalSleep(LIGHT_WAIT_TIME);

        ret = g_lightDev->TurnOffLight(g_lightInfo[i].lightId);
        EXPECT_EQ(0, ret);
    }
}

/**
  * @tc.name: EnableLight003
  * @tc.desc: Enables the light available in the light list based on the specified light id.
  * @tc.type: FUNC
  */
HWTEST_F(HdfLightTest, SUB_DriverSystem_LightHdi_0140, Function | MediumTest | Level1)
{
    int32_t i;
    int32_t ret;
    uint32_t lightId = LIGHT_ID_BUTT;
    struct LightEffect effect;

    ret = g_lightDev->TurnOnLight(lightId, &effect);
    EXPECT_EQ(LIGHT_NOT_SUPPORT, ret);

    for (i = 0; i < g_count; ++i) {
        effect.lightColor.colorValue.rgbColor.r = 255;
        effect.lightColor.colorValue.rgbColor.g = 0;
        effect.lightColor.colorValue.rgbColor.b = 0;
        effect.flashEffect.flashMode = LIGHT_FLASH_BUTT;
        effect.flashEffect.onTime = g_onTime;
        effect.flashEffect.offTime = g_offTime;

        ret = g_lightDev->TurnOnLight(g_lightInfo[i].lightId, &effect);
        EXPECT_EQ(LIGHT_NOT_FLASH, ret);

        effect.flashEffect.flashMode = LIGHT_FLASH_BLINK;
        effect.flashEffect.onTime = 0;
        ret = g_lightDev->TurnOnLight(g_lightInfo[i].lightId, &effect);
        EXPECT_EQ(LIGHT_NOT_FLASH, ret);

        effect.flashEffect.onTime = g_onTime;
        effect.flashEffect.offTime = 0;
        ret = g_lightDev->TurnOnLight(g_lightInfo[i].lightId, &effect);
        EXPECT_EQ(LIGHT_NOT_FLASH, ret);
    }
}

/**
  * @tc.name: DisableLight001
  * @tc.desc: Disable the light available in the light list based on the specified light id.
  * @tc.type: FUNC
  */
HWTEST_F(HdfLightTest, SUB_DriverSystem_LightHdi_0150, Function | MediumTest | Level1)
{
    uint32_t lightId = LIGHT_ID_BUTT;

    int32_t ret = g_lightDev->TurnOffLight(lightId);
    EXPECT_EQ(HDF_FAILURE, ret);
}


