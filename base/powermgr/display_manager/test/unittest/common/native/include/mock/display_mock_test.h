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

#ifndef DISPLAY_MOCK_TEST_H
#define DISPLAY_MOCK_TEST_H

#include <gtest/gtest.h>

#include "display_power_callback_stub.h"

namespace OHOS {
namespace PowerMgr {
class DisplayMockTest : public testing::Test {
public:
    class DisplayPowerMgrTestCallback : public OHOS::DisplayPowerMgr::DisplayPowerCallbackStub {
    public:
        DisplayPowerMgrTestCallback() {};
        virtual ~DisplayPowerMgrTestCallback() {};
        virtual void OnDisplayStateChanged(uint32_t displayId, OHOS::DisplayPowerMgr::DisplayState state) override;
    };
};
} // namespace PowerMgr
} // namespace OHOS
#endif // DISPLAY_MOCK_PARCEL_TEST_H
