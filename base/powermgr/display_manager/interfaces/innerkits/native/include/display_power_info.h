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

#ifndef DISPLAYMGR_DISPLAY_POWER_INFO_H
#define DISPLAYMGR_DISPLAY_POWER_INFO_H

namespace OHOS {
namespace DisplayPowerMgr {
/**
 * Display State
 */
enum class DisplayState : uint32_t {
    DISPLAY_OFF = 0,
    DISPLAY_DIM = 1,
    DISPLAY_ON = 2,
    DISPLAY_SUSPEND = 3,
    DISPLAY_UNKNOWN = 4,
};
} // namespace DisplayPowerMgr
} // namespace OHOS
#endif // DISPLAYMGR_DISPLAY_POWER_INFO_H
