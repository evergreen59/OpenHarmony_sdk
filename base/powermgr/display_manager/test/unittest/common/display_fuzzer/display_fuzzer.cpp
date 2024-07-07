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

#include "display_fuzzer.h"

#include <iostream>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <random>
#include <ctime>
#include "securec.h"
#include "display_power_mgr_client.h"

using namespace OHOS::DisplayPowerMgr;
using namespace std;

namespace {
auto& g_displayMgrClient = OHOS::DisplayPowerMgr::DisplayPowerMgrClient::GetInstance();
constexpr size_t DATANUM = 4;
constexpr int32_t SIZE = 1;
constexpr int32_t INDEX_0 = 0;
}

static void SetDisplayState(const uint8_t* data)
{
    int32_t type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }
    g_displayMgrClient.SetDisplayState(static_cast<DisplayState>(type[0]));
}

static void GetDisplayState(const uint8_t* data)
{
    int32_t type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }

    g_displayMgrClient.GetDisplayState();
}

static void GetDisplayIds(const uint8_t* data)
{
    int32_t type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }

    g_displayMgrClient.GetDisplayIds();
}

static void GetMainDisplayId(const uint8_t* data)
{
    int32_t type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }

    g_displayMgrClient.GetMainDisplayId();
}

static void SetBrightness(const uint8_t* data)
{
    int32_t type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }

    g_displayMgrClient.SetBrightness(type[0]);
}

static void AdjustBrightness(const uint8_t* data, size_t size)
{
    int32_t type[1];
    int32_t duration[1];
    size_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }
    if (size <= (idSize + DATANUM) || (memcpy_s(duration, sizeof(duration), (data + DATANUM), idSize) != EOK)) {
        duration[INDEX_0] = type[INDEX_0];
    }

    g_displayMgrClient.AdjustBrightness(type[0], duration[0]);
}

static void AutoAdjustBrightness(const uint8_t* data)
{
    int32_t type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }

    g_displayMgrClient.AutoAdjustBrightness(type[0]);
}

static void RegisterCallback(const uint8_t* data)
{
    static OHOS::sptr<OHOS::DisplayPowerMgr::IDisplayPowerCallback> callback;
    int32_t idSize = 4;
    if ((memcpy_s(callback, sizeof(callback), data, idSize)) != EOK) {
        return;
    }

    g_displayMgrClient.RegisterCallback(callback);
}

static void OverrideBrightness(const uint8_t* data)
{
    uint32_t type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }
    g_displayMgrClient.OverrideBrightness(type[0]);
}

static void RestoreBrightness(const uint8_t* data)
{
    uint32_t type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }
    g_displayMgrClient.RestoreBrightness(type[0]);
}

static void BoostBrightness(const uint8_t* data)
{
    uint32_t type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }
    g_displayMgrClient.BoostBrightness(type[0]);
}

static void CancelBoostBrightness(const uint8_t* data)
{
    uint32_t type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }
    g_displayMgrClient.CancelBoostBrightness(type[0]);
}

static void DiscountBrightness(const uint8_t* data)
{
    double type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }

    g_displayMgrClient.DiscountBrightness(type[0]);
}

static void GetBrightness(const uint8_t* data)
{
    uint32_t type[SIZE];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }
    g_displayMgrClient.GetBrightness(type[INDEX_0]);
}

static void GetDeviceBrightness(const uint8_t* data)
{
    uint32_t type[SIZE];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }
    g_displayMgrClient.GetDeviceBrightness(type[INDEX_0]);
}

namespace OHOS {
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    int32_t idSize = 4;
    int32_t cond[1];
    if (static_cast<int32_t>(size) > idSize) {
        if ((memcpy_s(cond, sizeof(cond), data, idSize)) != EOK) {
            return false;
        }
        std::random_device rd;
        std::default_random_engine engine(rd());
        std::uniform_int_distribution<int32_t> randomNum(static_cast<int32_t>(ApiNumber::NUM_ZERO),
            static_cast<int32_t>(ApiNumber::NUM_END) - 1);
        int32_t number = randomNum(engine);

        switch (static_cast<ApiNumber>(number)) {
            case ApiNumber::NUM_ZERO:
                SetDisplayState(data);
                break;
            case ApiNumber::NUM_ONE:
                GetDisplayState(data);
                break;
            case ApiNumber::NUM_TWO:
                GetDisplayIds(data);
                break;
            case ApiNumber::NUM_THREE:
                GetMainDisplayId(data);
                break;
            case ApiNumber::NUM_FOUR:
                SetBrightness(data);
                break;
            case ApiNumber::NUM_FIVE:
                AdjustBrightness(data, size);
                break;
            case ApiNumber::NUM_SIX:
                AutoAdjustBrightness(data);
                break;
            case ApiNumber::NUM_SEVEN:
                RegisterCallback(data);
                break;
            case ApiNumber::NUM_EIGHT:
                OverrideBrightness(data);
                break;
            case ApiNumber::NUM_NINE:
                RestoreBrightness(data);
                break;
            case ApiNumber::NUM_TEN:
                BoostBrightness(data);
                break;
            case ApiNumber::NUM_ELEVEN:
                CancelBoostBrightness(data);
                break;
            case ApiNumber::NUM_TWELVE:
                g_displayMgrClient.GetDefaultBrightness();
                break;
            case ApiNumber::NUM_THIRTEEN:
                g_displayMgrClient.GetMaxBrightness();
                break;
            case ApiNumber::NUM_FOURTEEN:
                g_displayMgrClient.GetMinBrightness();
                break;
            case ApiNumber::NUM_FIFTEEN:
                DiscountBrightness(data);
                break;
            case ApiNumber::NUM_SIXTEEN:
                GetBrightness(data);
                break;
            case ApiNumber::NUM_SEVENTEEN:
                GetDeviceBrightness(data);
                break;
            case ApiNumber::NUM_EIGHTEEN:
                g_displayMgrClient.IsAutoAdjustBrightness();
                break;
            case ApiNumber::NUM_NINETEEN:
                g_displayMgrClient.GetError();
                break;
            default:
                break;
        }
    }
    return true;
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
