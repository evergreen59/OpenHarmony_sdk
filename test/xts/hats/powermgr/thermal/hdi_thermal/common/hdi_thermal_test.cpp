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
#include <gtest/gtest.h>
#include <mutex>
#include <fcntl.h>
#include <functional>
#include <securec.h>
#include <unistd.h>

#include "hdf_base.h"
#include "osal_time.h"
#include "v1_0/ithermal_interface.h"
#include "v1_0/ithermal_callback.h"
#include "v1_0/thermal_types.h"

using namespace OHOS::HDI;
using namespace OHOS::HDI::Thermal::V1_0;
using namespace testing::ext;

namespace {
class ThermalCallbackMock : public IThermalCallback {
public:
    virtual ~ThermalCallbackMock() {}
    using ThermalEventCallback = std::function<int32_t(const HdfThermalCallbackInfo &event)>;
    static int32_t RegisterThermalEvent(const ThermalEventCallback &eventCb)
    {
        (void)eventCb;
        return 0;
    }
    int32_t OnThermalDataEvent(const HdfThermalCallbackInfo &event) override
    {
        (void)event;
        return 0;
    }
};

sptr<IThermalInterface> g_thermalInterface = nullptr;
sptr<IThermalCallback> g_callback = new ThermalCallbackMock();
std::mutex g_mutex;
const uint32_t MAX_PATH = 256;
const uint32_t WAIT_TIME = 1;
const std::string CPU_FREQ_PATH = "/data/service/el0/thermal/cooling/cpu/freq";
const std::string GPU_FREQ_PATH = "/data/service/el0/thermal/cooling/gpu/freq";
const std::string BATTERY_CHARGER_CURRENT_PATH = "/data/service/el0/thermal/cooling/battery/current";

class HdfThermalHdiTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static int32_t ReadFile(const char *path, char *buf, size_t size);
    static int32_t ConvertInt(const std::string &value);
};

void HdfThermalHdiTest::SetUpTestCase()
{
    g_thermalInterface = IThermalInterface::Get(true);
}

void HdfThermalHdiTest::TearDownTestCase()
{
}

void HdfThermalHdiTest::SetUp()
{
}

void HdfThermalHdiTest::TearDown()
{
}

int32_t HdfThermalHdiTest::ReadFile(const char *path, char *buf, size_t size)
{
    std::lock_guard<std::mutex> lck(g_mutex);
    int32_t ret;

    int32_t fd = open(path, O_RDONLY);
    if (fd < HDF_SUCCESS) {
        printf("WriteFile: failed to open file %d\n\r", fd);
        return HDF_FAILURE;
    }

    ret = read(fd, buf, size);
    if (ret < HDF_SUCCESS) {
        printf("WriteFile: failed to read file %d\n\r", ret);
        close(fd);
        return HDF_FAILURE;
    }

    close(fd);
    buf[size - 1] = '\0';
    return HDF_SUCCESS;
}

int32_t HdfThermalHdiTest::ConvertInt(const std::string &value)
{
    return std::stoi(value);
}
}

namespace {
/**
  * @tc.name: HdfThermalHdiTest001
  * @tc.desc: Get a client and check whether the client is empty.
  * @tc.type: FUNC
  */
HWTEST_F(HdfThermalHdiTest, HdfThermalHdiTest001, TestSize.Level1)
{
    ASSERT_NE(nullptr, g_thermalInterface);
}

/**
  * @tc.name: HdfThermalHdiTest002
  * @tc.desc: SetCpuFreq
  * @tc.type: FUNC
  */
HWTEST_F(HdfThermalHdiTest, HdfThermalHdiTest002, TestSize.Level1)
{
    printf("HdfThermalHdiTest002: start.");
    int32_t cpuFreq = 1994100;
    int32_t ret = g_thermalInterface->SetCpuFreq(cpuFreq);
    EXPECT_EQ(0, ret);

    char cpuBuf[MAX_PATH] = {0};
    char freqValue[MAX_PATH] = {0};

    if (snprintf_s(cpuBuf, MAX_PATH, sizeof(cpuBuf) - 1, CPU_FREQ_PATH.c_str()) < EOK) {
        return;
    }

    sleep(WAIT_TIME);
    ret = HdfThermalHdiTest::ReadFile(cpuBuf, freqValue, sizeof(freqValue));
    if (ret != HDF_SUCCESS) {
        printf("HdfThermalHdiTest002: Failed to read file ");
        return;
    }

    std::string freq = freqValue;
    int32_t value = HdfThermalHdiTest::ConvertInt(freq);
    printf("freq is %d\n\r", value);
    EXPECT_EQ(value, cpuFreq) << "HdfThermalHdiTest002 failed";
    printf("HdfThermalHdiTest002: return.");
}

/**
  * @tc.name: HdfThermalHdiTest003
  * @tc.desc: SetGpuFreq
  * @tc.type: FUNC
  */
HWTEST_F(HdfThermalHdiTest, HdfThermalHdiTest003, TestSize.Level1)
{
    printf("HdfThermalHdiTest003: start.");
    int32_t gpuFreq = 40000;
    int32_t ret = g_thermalInterface->SetGpuFreq(gpuFreq);
    EXPECT_EQ(0, ret);

    char cpuBuf[MAX_PATH] = {0};
    char freqValue[MAX_PATH] = {0};

    if (snprintf_s(cpuBuf, MAX_PATH, sizeof(cpuBuf) - 1, GPU_FREQ_PATH.c_str()) < EOK) {
        return;
    }

    sleep(WAIT_TIME);
    ret = HdfThermalHdiTest::ReadFile(cpuBuf, freqValue, sizeof(freqValue));
    if (ret != HDF_SUCCESS) {
        printf("HdfThermalHdiTest003: Failed to read file ");
        return;
    }

    std::string freq = freqValue;
    int32_t value = HdfThermalHdiTest::ConvertInt(freq);
    printf("freq is %d\n\r", value);
    EXPECT_EQ(value, gpuFreq) << "HdfThermalHdiTest003 failed";
    printf("HdfThermalHdiTest003: return.");
}

/**
  * @tc.name: HdfThermalHdiTest004
  * @tc.desc: SetBatteryCurrent
  * @tc.type: FUNC
  */
HWTEST_F(HdfThermalHdiTest, HdfThermalHdiTest004, TestSize.Level1)
{
    printf("HdfThermalHdiTest004: start.");
    int32_t batteryCurrent = 1000;
    int32_t ret = g_thermalInterface->SetBatteryCurrent(batteryCurrent);
    EXPECT_EQ(0, ret);

    char cpuBuf[MAX_PATH] = {0};
    char currentValue[MAX_PATH] = {0};

    if (snprintf_s(cpuBuf, MAX_PATH, sizeof(cpuBuf) - 1, BATTERY_CHARGER_CURRENT_PATH.c_str()) < EOK) {
        return;
    }

    sleep(WAIT_TIME);
    ret = HdfThermalHdiTest::ReadFile(cpuBuf, currentValue, sizeof(currentValue));
    if (ret != HDF_SUCCESS) {
        printf("HdfThermalHdiTest004: Failed to read file ");
        return;
    }

    std::string current = currentValue;
    int32_t value = HdfThermalHdiTest::ConvertInt(current);
    printf("freq is %d\n\r", value);
    EXPECT_EQ(value, batteryCurrent) << "HdfThermalHdiTest004 failed";
    printf("HdfThermalHdiTest004: return.");
}

/**
  * @tc.name: HdfThermalHdiTest005
  * @tc.desc: GetThermalZoneInfo
  * @tc.type: FUNC
  */
HWTEST_F(HdfThermalHdiTest, HdfThermalHdiTest005, TestSize.Level1)
{
    HdfThermalCallbackInfo event;
    printf("HdfThermalHdiTest005: start.");
    int32_t ret = g_thermalInterface->GetThermalZoneInfo(event);
    EXPECT_EQ(0, ret) << "HdfThermalHdiTest005 failed";
    for (auto iter : event.info) {
        printf("type is %s\n\r", iter.type.c_str());
        printf("temp is %d\n\r", iter.temp);
    }
    printf("HdfThermalHdiTest005: return.");
}

/**
  * @tc.name: HdfThermalHdiTest006
  * @tc.desc: Register
  * @tc.type: FUNC
  */
HWTEST_F(HdfThermalHdiTest, HdfThermalHdiTest006, TestSize.Level1)
{
    printf("HdfThermalHdiTest006: start.");
    int32_t ret = g_thermalInterface->Register(g_callback);
    EXPECT_EQ(0, ret) << "HdfThermalHdiTest006 failed";
    printf("HdfThermalHdiTest006: return.");
}

/**
  * @tc.name: HdfThermalHdiTest007
  * @tc.desc: Unregister
  * @tc.type: FUNC
  */
HWTEST_F(HdfThermalHdiTest, HdfThermalHdiTest007, TestSize.Level1)
{
    printf("HdfThermalHdiTest007: start.");
    int32_t ret = g_thermalInterface->Unregister();
    EXPECT_EQ(0, ret) << "HdfThermalHdiTest007 failed";
    printf("HdfThermalHdiTest007: return.");
}
}
