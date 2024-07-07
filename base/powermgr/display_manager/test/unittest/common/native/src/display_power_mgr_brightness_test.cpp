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
#include "display_power_mgr_client.h"
#include "setting_provider.h"
#include "system_ability_definition.h"
#include "display_log.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::DisplayPowerMgr;

namespace {
const std::string SETTING_BRIGHTNESS_KEY {"settings.display.screen_brightness_status"};
const double NO_DISCOUNT = 1.00;
}

class DisplayPowerMgrBrightnessTest : public Test {
public:
    void SetUp()
    {
        DisplayPowerMgrClient::GetInstance().SetDisplayState(DisplayState::DISPLAY_ON);
        DisplayPowerMgrClient::GetInstance().DiscountBrightness(NO_DISCOUNT);
    }

    void TearDown()
    {
        DisplayPowerMgrClient::GetInstance().RestoreBrightness();
        DisplayPowerMgrClient::GetInstance().CancelBoostBrightness();
    }
};

namespace {
/**
 * @tc.name: DisplayPowerMgrSetBrightness001
 * @tc.desc: Test SetBrightness
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrSetBrightness001, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrSetBrightness001: fun is start");
    const uint32_t SET_BRIGHTNESS = 90;
    bool ret = DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    EXPECT_TRUE(ret);
    uint32_t deviceBrightness = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(deviceBrightness, SET_BRIGHTNESS);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrSetBrightness001: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrSettingBrightness001
 * @tc.desc: Test setting brightness value changed when SetBrightness
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrSettingBrightness001, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrSettingBrightness001: fun is start");
    const uint32_t SET_BRIGHTNESS_VALUE = 223;
    bool ret = DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS_VALUE);
    EXPECT_TRUE(ret);
    const int SLEEP_TIME = 100000;
    usleep(SLEEP_TIME); // sleep 100ms, wait for setting update
    uint32_t brightness = DisplayPowerMgrClient::GetInstance().GetBrightness();
    EXPECT_EQ(brightness, SET_BRIGHTNESS_VALUE);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrSettingBrightness001: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrDiscountBrightnessNormal
 * @tc.desc: Test DiscountBrightness the normal test
 * @tc.type: FUNC
 * @tc.require: issueI5HWMN
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrDiscountBrightnessNormal, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightnessNormal: fun is start");
    const uint32_t SET_BRIGHTNESS = 91;
    const int32_t SLEEP_TIME = 200000;
    DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    usleep(SLEEP_TIME); // sleep 200ms, wait for setBrightness
    const double DISCOUNT_NORMAL = 0.8;
    bool ret = DisplayPowerMgrClient::GetInstance().DiscountBrightness(DISCOUNT_NORMAL);
    EXPECT_TRUE(ret);

    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, static_cast<uint32_t>(SET_BRIGHTNESS * DISCOUNT_NORMAL));
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightnessNormal: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrDiscountBrightnessNormal
 * @tc.desc: Test DiscountBrightness when screen is off
 * @tc.type: FUNC
 * @tc.require: issueI5HWMN
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrDiscountBrightnessScreenOff, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightnessScreenOff: fun is start");
    DisplayPowerMgrClient::GetInstance().SetDisplayState(DisplayState::DISPLAY_OFF);
    const double DISCOUNT_VALUE = 0.8;
    bool ret = DisplayPowerMgrClient::GetInstance().DiscountBrightness(DISCOUNT_VALUE);
    EXPECT_FALSE(ret);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightnessScreenOff: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrDiscountBrightnessBoundary001
 * @tc.desc: Test DiscountBrightness the boundary test
 * @tc.type: FUNC
 * @tc.require: issueI5HWMN
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrDiscountBrightnessBoundary001, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightnessBoundary001: fun is start");
    const uint32_t SET_BRIGHTNESS = 92;
    DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    const double DISCOUNT_LOWER_BOUNDARY = 0.01;
    bool ret = DisplayPowerMgrClient::GetInstance().DiscountBrightness(DISCOUNT_LOWER_BOUNDARY);
    EXPECT_TRUE(ret);

    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    uint32_t minBrightness = DisplayPowerMgrClient::GetInstance().GetMinBrightness();
    EXPECT_EQ(value, minBrightness);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightnessBoundary001: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrDiscountBrightnessBoundary002
 * @tc.desc: Test DiscountBrightness the boundary test
 * @tc.type: FUNC
 * @tc.require: issueI5HWMN
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrDiscountBrightnessBoundary002, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightnessBoundary002: fun is start");
    const uint32_t SET_BRIGHTNESS = 93;
    const int32_t SLEEP_TIME = 200000;
    DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    usleep(SLEEP_TIME); // sleep 200ms, wait for setBrightness
    const double DISCOUNT_UPPER_BOUNDARY = 1.0;
    bool ret = DisplayPowerMgrClient::GetInstance().DiscountBrightness(DISCOUNT_UPPER_BOUNDARY);
    EXPECT_TRUE(ret);

    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, static_cast<uint32_t>(SET_BRIGHTNESS * DISCOUNT_UPPER_BOUNDARY));
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightnessBoundary002: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrDiscountBrightnessAbnormal001
 * @tc.desc: Test DiscountBrightness the abnormal test
 * @tc.type: FUNC
 * @tc.require: issueI5HWMN
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrDiscountBrightnessAbnormal001, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightnessAbnormal001: fun is start");
    const uint32_t SET_BRIGHTNESS = 94;
    DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    const double DISCOUNT_BEYOND_LOWER_BOUNDARY = 0.0;
    bool ret = DisplayPowerMgrClient::GetInstance().DiscountBrightness(DISCOUNT_BEYOND_LOWER_BOUNDARY);
    EXPECT_TRUE(ret);

    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    uint32_t minBrightness = DisplayPowerMgrClient::GetInstance().GetMinBrightness();
    EXPECT_EQ(value, minBrightness);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightnessAbnormal001: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrDiscountBrightnessAbnormal002
 * @tc.desc: Test DiscountBrightness the abnormal test
 * @tc.type: FUNC
 * @tc.require: issueI5HWMN
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrDiscountBrightnessAbnormal002, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightnessAbnormal002: fun is start");
    const uint32_t SET_BRIGHTNESS = 95;
    const int32_t SLEEP_TIME = 200000;
    DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    usleep(SLEEP_TIME); // sleep 200ms, wait for setBrightness
    const double DISCOUNT_BEYOND_UPPER_BOUNDARY = 2.0;
    bool ret = DisplayPowerMgrClient::GetInstance().DiscountBrightness(DISCOUNT_BEYOND_UPPER_BOUNDARY);
    EXPECT_TRUE(ret);

    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    const double DISCOUNT_MAX_VALUE = 1.0;
    EXPECT_EQ(value, static_cast<uint32_t>(SET_BRIGHTNESS * DISCOUNT_MAX_VALUE));
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightnessAbnormal002: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrDiscountBrightnessAbnormal003
 * @tc.desc: Test DiscountBrightness the abnormal test
 * @tc.type: FUNC
 * @tc.require: issueI5HWMN
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrDiscountBrightnessAbnormal003, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightnessAbnormal003: fun is start");
    const uint32_t SET_BRIGHTNESS = 96;
    DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    const double DISCOUNT_NEGATIVE_VALUE = -1.0;
    bool ret = DisplayPowerMgrClient::GetInstance().DiscountBrightness(DISCOUNT_NEGATIVE_VALUE);
    EXPECT_TRUE(ret);

    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    uint32_t minBrightness = DisplayPowerMgrClient::GetInstance().GetMinBrightness();
    EXPECT_EQ(value, minBrightness);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightnessAbnormal003: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrDiscountBrightness001
 * @tc.desc: Test OverrideBrightness after DiscountBrightness, then RestoreBrightness
 * @tc.type: FUNC
 * @tc.require: issueI5HWMN
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrDiscountBrightness001, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightness001: fun is start");
    const uint32_t SET_BRIGHTNESS = 97;
    const int32_t SLEEP_TIME = 200000;
    DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    usleep(SLEEP_TIME); // sleep 200ms, wait for setBrightness
    const double DISCOUNT_VALUE = 0.8;
    bool ret = DisplayPowerMgrClient::GetInstance().DiscountBrightness(DISCOUNT_VALUE);
    EXPECT_TRUE(ret);

    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, static_cast<uint32_t>(DISCOUNT_VALUE * SET_BRIGHTNESS));

    const uint32_t SET_OVERRIDE_BRIGHTNESS = 200;
    DisplayPowerMgrClient::GetInstance().OverrideBrightness(SET_OVERRIDE_BRIGHTNESS);
    value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, static_cast<uint32_t>(DISCOUNT_VALUE * SET_OVERRIDE_BRIGHTNESS));

    DisplayPowerMgrClient::GetInstance().RestoreBrightness();
    value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, static_cast<uint32_t>(DISCOUNT_VALUE * SET_BRIGHTNESS));
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightness001: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrDiscountBrightness002
 * @tc.desc: Test DiscountBrightness after OverrideBrightness, then RestoreBrightness
 * @tc.type: FUNC
 * @tc.require: issueI5HWMN
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrDiscountBrightness002, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightness002: fun is start");
    const uint32_t SET_BRIGHTNESS = 98;
    DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    const uint32_t SET_OVERRIDE_BRIGHTNESS = 200;
    DisplayPowerMgrClient::GetInstance().OverrideBrightness(SET_OVERRIDE_BRIGHTNESS);

    const double DISCOUNT_VALUE = 0.8;
    double ret = DisplayPowerMgrClient::GetInstance().DiscountBrightness(DISCOUNT_VALUE);
    EXPECT_TRUE(ret);
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, static_cast<uint32_t>(DISCOUNT_VALUE * SET_OVERRIDE_BRIGHTNESS));

    ret = DisplayPowerMgrClient::GetInstance().RestoreBrightness();
    EXPECT_TRUE(ret);

    value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, static_cast<uint32_t>(DISCOUNT_VALUE * SET_BRIGHTNESS));
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightness002: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrDiscountBrightness003
 * @tc.desc: Test DisconutBrightness after BoostBrightness and CancelBoostBrightness
 * @tc.type: FUNC
 * @tc.require: issueI5HWMN
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrDiscountBrightness003, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightness003: fun is start");
    const uint32_t SET_BRIGHTNESS = 99;
    DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    const uint32_t SET_BOOST_BRIGHTNESS_TIME = 10000;
    DisplayPowerMgrClient::GetInstance().BoostBrightness(SET_BOOST_BRIGHTNESS_TIME);

    const double DISCOUNT_VALUE = 0.8;
    bool ret = DisplayPowerMgrClient::GetInstance().DiscountBrightness(DISCOUNT_VALUE);
    EXPECT_TRUE(ret);

    uint32_t maxBrightness = DisplayPowerMgrClient::GetInstance().GetMaxBrightness();
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, static_cast<uint32_t>(DISCOUNT_VALUE * maxBrightness));

    DisplayPowerMgrClient::GetInstance().CancelBoostBrightness();

    value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, static_cast<uint32_t>(DISCOUNT_VALUE * SET_BRIGHTNESS));
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightness003: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrDiscountBrightness004
 * @tc.desc: Test BoostBrightness after DiscountBrightness, then CancelBoostBrightness
 * @tc.type: FUNC
 * @tc.require: issueI5HWMN
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrDiscountBrightness004, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightness004: fun is start");
    const uint32_t SET_BRIGHTNESS = 100;
    DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    const double DISCOUNT_VALUE = 0.8;
    bool ret = DisplayPowerMgrClient::GetInstance().DiscountBrightness(DISCOUNT_VALUE);
    EXPECT_TRUE(ret);

    const uint32_t SET_BOOST_BRIGHTNESS_TIME = 10000;
    DisplayPowerMgrClient::GetInstance().BoostBrightness(SET_BOOST_BRIGHTNESS_TIME);
    uint32_t maxBrightness = DisplayPowerMgrClient::GetInstance().GetMaxBrightness();
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, static_cast<uint32_t>(DISCOUNT_VALUE * maxBrightness));

    DisplayPowerMgrClient::GetInstance().CancelBoostBrightness();

    value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, static_cast<uint32_t>(DISCOUNT_VALUE * SET_BRIGHTNESS));
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightness004: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrDiscountBrightness005
 * @tc.desc: Test DisconutBrightness after SetBrightness, then GetBrightness
 * @tc.type: FUNC
 * @tc.require: issueI5HWMN
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrDiscountBrightness005, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightness005: fun is start");
    const uint32_t SET_BRIGHTNESS = 103;
    DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    const double DISCOUNT_VALUE = 0.8;
    bool ret = DisplayPowerMgrClient::GetInstance().DiscountBrightness(DISCOUNT_VALUE);
    EXPECT_TRUE(ret);

    uint32_t beforeBrightness = DisplayPowerMgrClient::GetInstance().GetBrightness();
    EXPECT_EQ(SET_BRIGHTNESS, beforeBrightness);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDiscountBrightness005: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrOverrideBrightness001
 * @tc.desc: Test OverrideBrightness
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrOverrideBrightness001, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrOverrideBrightness001: fun is start");
    const uint32_t OVERRIDE_BRIGHTNESS = 255;
    bool ret = DisplayPowerMgrClient::GetInstance().OverrideBrightness(OVERRIDE_BRIGHTNESS);
    EXPECT_TRUE(ret);
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, OVERRIDE_BRIGHTNESS);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrOverrideBrightness001: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrOverrideBrightness002
 * @tc.desc: Test RestoreBrightness after OverrideBrightness
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrOverrideBrightness002, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrOverrideBrightness002: fun is start");
    const uint32_t SET_BRIGHTNESS = 104;
    const uint32_t OVERRIDE_BRIGHTNESS = 255;
    const int32_t SLEEP_TIME = 200000;
    bool ret = DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    usleep(SLEEP_TIME); // sleep 200ms, wait for setBrightness
    EXPECT_TRUE(ret);
    ret = DisplayPowerMgrClient::GetInstance().OverrideBrightness(OVERRIDE_BRIGHTNESS);
    EXPECT_TRUE(ret);
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, OVERRIDE_BRIGHTNESS);
    ret = DisplayPowerMgrClient::GetInstance().RestoreBrightness(); // restore
    EXPECT_TRUE(ret);
    value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, SET_BRIGHTNESS);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrOverrideBrightness002: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrOverrideBrightness003
 * @tc.desc: Test SetBrightness after OverrideBrightness
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrOverrideBrightness003, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrOverrideBrightness003: fun is start");
    const uint32_t SET_BRIGHTNESS = 105;
    const uint32_t OVERRIDE_BRIGHTNESS = 255;
    bool ret = DisplayPowerMgrClient::GetInstance().OverrideBrightness(OVERRIDE_BRIGHTNESS);
    EXPECT_TRUE(ret);
    ret = DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    EXPECT_FALSE(ret);
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, OVERRIDE_BRIGHTNESS);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrOverrideBrightness003: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrOverrideBrightness004
 * @tc.desc: Test RestoreBrightness after not override
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrOverrideBrightness004, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrOverrideBrightness004: fun is start");
    const uint32_t SET_BRIGHTNESS = 106;
    bool ret = DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    EXPECT_TRUE(ret);
    ret = DisplayPowerMgrClient::GetInstance().RestoreBrightness();
    EXPECT_FALSE(ret);
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, SET_BRIGHTNESS);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrOverrideBrightness004: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrOverrideBrightness005
 * @tc.desc: Test OverrideBrightness off Disable the call
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrOverrideBrightness005, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrOverrideBrightness005: fun is start");
    DisplayPowerMgrClient::GetInstance().SetDisplayState(DisplayState::DISPLAY_OFF);
    const uint32_t OVERRIDE_BRIGHTNESS = 100;
    bool isBoost = DisplayPowerMgrClient::GetInstance().OverrideBrightness(OVERRIDE_BRIGHTNESS);
    EXPECT_FALSE(isBoost);
    bool isRestore = DisplayPowerMgrClient::GetInstance().RestoreBrightness();
    EXPECT_FALSE(isRestore);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrOverrideBrightness005: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrOverrideBrightness006
 * @tc.desc: Test device brightness when override brightness is 256
 * @tc.type: FUNC
 * @tc.require: issueI5YZQR
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrOverrideBrightness006, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrOverrideBrightness006: fun is start");
    uint32_t overrideValue = 256;
    uint32_t brightnessMax = 255;
    DisplayPowerMgrClient::GetInstance().OverrideBrightness(overrideValue);
    uint32_t deviceBrightness = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(brightnessMax, deviceBrightness);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrOverrideBrightness006: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrOverrideBrightness007
 * @tc.desc: Test device brightness when override brightness is 0
 * @tc.type: FUNC
 * @tc.require: issueI5YZQR
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrOverrideBrightness007, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrOverrideBrightness007: fun is start");
    uint32_t overrideValue = 0;
    uint32_t brightnessMin = 5;
    EXPECT_TRUE(DisplayPowerMgrClient::GetInstance().OverrideBrightness(overrideValue));
    uint32_t deviceBrightness = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(brightnessMin, deviceBrightness);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrOverrideBrightness007: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrOverrideBrightness008
 * @tc.desc: Test device brightness when override brightness is -1
 * @tc.type: FUNC
 * @tc.require: issueI5YZQR
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrOverrideBrightness008, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrOverrideBrightness008: fun is start");
    uint32_t overrideValue = -1;
    uint32_t brightnessMax = 255;
    EXPECT_TRUE(DisplayPowerMgrClient::GetInstance().OverrideBrightness(overrideValue));
    uint32_t deviceBrightness = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(brightnessMax, deviceBrightness);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrOverrideBrightness008: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrMaxBrightness001
 * @tc.desc: Test GetMaxBrightness less equals 255
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrMaxBrightness001, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrMaxBrightness001: fun is start");
    uint32_t max = DisplayPowerMgrClient::GetInstance().GetMaxBrightness();
    EXPECT_LE(max, 255);
    bool ret = DisplayPowerMgrClient::GetInstance().SetBrightness(max + 100);
    EXPECT_TRUE(ret);
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, max);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrMaxBrightness001: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrMinBrightness001
 * @tc.desc: Test GetMinBrightness greater equals 0
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrMinBrightness001, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrMinBrightness001: fun is start");
    const uint32_t SET_BRIGHTNESS_VALUE = 0;
    uint32_t min = DisplayPowerMgrClient::GetInstance().GetMinBrightness();
    EXPECT_GE(min, 0);
    bool ret = DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS_VALUE);
    EXPECT_TRUE(ret);
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(value, min);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrMinBrightness001: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrDefaultBrightness001
 * @tc.desc: Test GetDefaultnBrightness greater equals 0 and less equals 255
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrDefaultBrightness001, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDefaultBrightness001: fun is start");
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDefaultBrightness();
    EXPECT_GE(value, 0);
    EXPECT_LE(value, 255);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDefaultBrightness001: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessNormal
 * @tc.desc: Test BoostBrightness the normal test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessNormal, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessNormal: fun is start");
    const uint32_t BOOST_TIME = 100; // keep BoostBrightness 100ms
    bool isSucc = DisplayPowerMgrClient::GetInstance().BoostBrightness(BOOST_TIME);
    EXPECT_TRUE(isSucc);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessNormal: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessAbnormal
 * @tc.desc: Test BoostBrightness the abnormal value
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessAbnormal, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessAbnormal: fun is start");
    const uint32_t BOOST_TIME = -1;
    bool isSucc = DisplayPowerMgrClient::GetInstance().BoostBrightness(BOOST_TIME);
    EXPECT_FALSE(isSucc);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessAbnormal: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrCancelBoostBrightnessNormal
 * @tc.desc: Test CancelBoostBrightness the normal test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrCancelBoostBrightnessNormal, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrCancelBoostBrightnessNormal: fun is start");
    const uint32_t BOOST_TIME = 50000;
    bool isSucc = DisplayPowerMgrClient::GetInstance().BoostBrightness(BOOST_TIME);
    EXPECT_TRUE(isSucc);
    bool isCancel = DisplayPowerMgrClient::GetInstance().CancelBoostBrightness();
    EXPECT_TRUE(isCancel);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrCancelBoostBrightnessNormal: fun is end");
}

/**
 * @tc.name: BoostAndOverrideMutuallyExclusive1
 * @tc.desc: Test BoostBrightness and OverrideBrightness are mutually exclusive
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, BoostAndOverrideMutuallyExclusive1, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "BoostAndOverrideMutuallyExclusive: fun is start");
    const uint32_t BOOST_TIME = 50000;
    const uint32_t OVERRIDE_BIRGHTNESS = 100;
    bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(BOOST_TIME);
    EXPECT_TRUE(isBoost);
    bool isOverride = DisplayPowerMgrClient::GetInstance().OverrideBrightness(OVERRIDE_BIRGHTNESS);
    EXPECT_FALSE(isOverride);
    bool isRestore = DisplayPowerMgrClient::GetInstance().RestoreBrightness();
    EXPECT_FALSE(isRestore);
    DISPLAY_HILOGI(LABEL_TEST, "BoostAndOverrideMutuallyExclusive: fun is end");
}

/**
 * @tc.name: BoostAndOverrideMutuallyExclusive2
 * @tc.desc: Test BoostBrightness and OverrideBrightness are mutually exclusive
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, BoostAndOverrideMutuallyExclusive2, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "BoostAndOverrideMutuallyExclusive2: fun is start");
    const uint32_t BOOST_TIME = 50000;
    const uint32_t OVERRIDE_BIRGHTNESS = 255;
    bool isOverride = DisplayPowerMgrClient::GetInstance().OverrideBrightness(OVERRIDE_BIRGHTNESS);
    EXPECT_TRUE(isOverride);
    bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(BOOST_TIME);
    EXPECT_FALSE(isBoost);
    bool isCancel = DisplayPowerMgrClient::GetInstance().CancelBoostBrightness();
    EXPECT_FALSE(isCancel);
    DISPLAY_HILOGI(LABEL_TEST, "BoostAndOverrideMutuallyExclusive2: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessTimeout1
 * @tc.desc: Test BoostBrightness timeout restore system brightness
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessTimeout1, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessTimeout1: fun is start");
    uint32_t beforeValue = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    const uint32_t BOOST_BRIGHTNESS_TIME = 100;
    bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(BOOST_BRIGHTNESS_TIME);
    EXPECT_TRUE(isBoost);
    const int SLEEP_TIME = 200000;
    usleep(SLEEP_TIME); // sleep 200ms, wait for setting update
    uint32_t currentValue = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(beforeValue, currentValue);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessTimeout1: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessTimeout2
 * @tc.desc: Test BoostBrightness timeout brightness adjustment can be called
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessTimeout2, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessTimeout2: fun is start");
    const uint32_t SET_BRIGHTNESS = 106;
    const uint32_t BOOST_BRIGHTNESS_TIME = 100;
    const int SLEEP_TIME = 100000;
    bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(BOOST_BRIGHTNESS_TIME);
    EXPECT_TRUE(isBoost);
    usleep(SLEEP_TIME); // sleep 100ms, wait for cancel BoostBrightness
    bool isSet = DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    EXPECT_TRUE(isSet);
    uint32_t currentValue = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(SET_BRIGHTNESS, currentValue);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessTimeout2: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessCancel1
 * @tc.desc: Test BoostBrightness Cancel restore system brightness
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessCancel1, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessCancel1: fun is start");
    const uint32_t SET_BRIGHTNESS = 106;
    DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    const uint32_t BOOST_BRIGHTNESS_TIME = 10000;
    const int SLEEP_TIME = 100000;
    bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(BOOST_BRIGHTNESS_TIME);
    EXPECT_TRUE(isBoost);
    bool isCancel = DisplayPowerMgrClient::GetInstance().CancelBoostBrightness();
    EXPECT_TRUE(isCancel);
    usleep(SLEEP_TIME); // sleep 100ms, wait for cancel BoostBrightness
    uint32_t currentValue = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(SET_BRIGHTNESS, currentValue) << "beforeValue: " << SET_BRIGHTNESS << " currentVal: " << currentValue;
    DISPLAY_HILOGI(LABEL_TEST, "BoostBrightnessCancel1: beforeValue: %{public}d, currentVal: %{public}d",
        SET_BRIGHTNESS, currentValue);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessCancel1: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessCancel2
 * @tc.desc: Test BoostBrightness Cancel brightness adjustment can be called
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessCancel2, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessCancel2: fun is start");
    const uint32_t BOOST_BRIGHTNESS_TIME = 10000;
    bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(BOOST_BRIGHTNESS_TIME);
    EXPECT_TRUE(isBoost);
    bool isCancel = DisplayPowerMgrClient::GetInstance().CancelBoostBrightness();
    EXPECT_TRUE(isCancel);
    const uint32_t SET_BRIGHTNESS_VALUE = 111;
    bool isSet = DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS_VALUE);
    EXPECT_TRUE(isSet);
    uint32_t currentValue = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(SET_BRIGHTNESS_VALUE, currentValue);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessCancel2: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessNotAdjust
 * @tc.desc: Test BoostBrightness do not adjust brightness
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessNotAdjust, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessNotAdjust: fun is start");
    const uint32_t SET_BRIGHTNESS = 107;
    const uint32_t BOOST_BRIGHTNESS_TIME = 10000;
    bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(BOOST_BRIGHTNESS_TIME);
    EXPECT_TRUE(isBoost);
    bool isSet = DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    EXPECT_FALSE(isSet);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessNotAdjust: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessScreenOff
 * @tc.desc: Test BoostBrightness off Disable the call
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessScreenOff, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessScreenOff: fun is start");
    DisplayPowerMgrClient::GetInstance().SetDisplayState(DisplayState::DISPLAY_OFF);
    const uint32_t BOOST_BRIGHTNESS_TIME = 10000;
    bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(BOOST_BRIGHTNESS_TIME);
    EXPECT_FALSE(isBoost);
    bool isCancel = DisplayPowerMgrClient::GetInstance().CancelBoostBrightness();
    EXPECT_FALSE(isCancel);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessScreenOff: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessMultipleCalls
 * @tc.desc: Test BoostBrightness multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessMultipleCalls, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessMultipleCalls: fun is start");
    const uint32_t SET_BRIGHTNESS = 108;
    const uint32_t COUNT = 3;
    const uint32_t TIMEOUT = 100;
    const uint32_t SLEEP_TIME = 200000;
    const uint32_t UPDATE_TIME = 20000;
    for (uint32_t i = 0; i < COUNT; ++i) {
        bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(TIMEOUT);
        usleep(UPDATE_TIME); // wait for set newest BoostBrightness time
        EXPECT_TRUE(isBoost);
    }
    // Brightness adjustment is not allowed
    bool isSet = DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS);
    EXPECT_FALSE(isSet);
    // Wait for a timeout
    usleep(SLEEP_TIME);
    bool isCancel = DisplayPowerMgrClient::GetInstance().CancelBoostBrightness();
    EXPECT_FALSE(isCancel);
    const uint32_t SET_BRIGHTNESS_VALUE = 222;
    isSet = DisplayPowerMgrClient::GetInstance().SetBrightness(SET_BRIGHTNESS_VALUE);
    EXPECT_TRUE(isSet);
    uint32_t currentVal = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(SET_BRIGHTNESS_VALUE, currentVal);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessMultipleCalls: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrAdjustBrightness001
 * @tc.desc: Test AdjustBrightness
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrAdjustBrightness001, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrAdjustBrightness001: fun is start");
    bool ret = DisplayPowerMgrClient::GetInstance().AdjustBrightness(0, 3000);
    EXPECT_TRUE(ret);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrAdjustBrightness001: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrAdjustBrightness002
 * @tc.desc: Test AutoAdjustBrightness
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrAdjustBrightness002, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrAdjustBrightness002: fun is start");
    bool ret = DisplayPowerMgrClient::GetInstance().AutoAdjustBrightness(true);
    if (ret) {
        DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrAdjustBrightness002: is supported");
        ret = DisplayPowerMgrClient::GetInstance().AutoAdjustBrightness(false);
        EXPECT_TRUE(ret);
    } else {
        DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrAdjustBrightness002: is not supported");
        EXPECT_FALSE(ret);
    }
}

/**
 * @tc.name: DisplayPowerMgrAdjustBrightness003
 * @tc.desc: Test IsAutoAdjustBrightness
 * @tc.type: FUNC
 * @tc.require: issueI5YZQR
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrAdjustBrightness003, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrAdjustBrightness003: fun is start");
    bool ret = DisplayPowerMgrClient::GetInstance().IsAutoAdjustBrightness();
    DisplayPowerMgrClient::GetInstance().AutoAdjustBrightness(false);
    EXPECT_EQ(DisplayPowerMgrClient::GetInstance().IsAutoAdjustBrightness(), false);
    DisplayPowerMgrClient::GetInstance().AutoAdjustBrightness(ret);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrAdjustBrightness003: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrScreenBrightnessEquality001
 * @tc.desc: Test whether the screen brightness is equal after the screen is on and before the screen is off
 * @tc.type: FUNC
 * @tc.require: issueI5YZQR
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrScreenBrightnessEquality001, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrScreenBrightnessEquality001: fun is start");
    uint32_t deviceBrightness1 = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    DisplayPowerMgrClient::GetInstance().SetDisplayState(DisplayState::DISPLAY_OFF);
    DisplayPowerMgrClient::GetInstance().SetDisplayState(DisplayState::DISPLAY_ON);
    uint32_t deviceBrightness2 = DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
    EXPECT_EQ(deviceBrightness1, deviceBrightness2);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrScreenBrightnessEquality001: fun is end");
}
} // namespace
