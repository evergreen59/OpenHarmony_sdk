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

#include "thermal_hisysevent.h"

#include "hisysevent.h"
#include "thermal_log.h"

namespace OHOS {
namespace PowerMgr {
template<typename... Types>
static void WriteEvent(const std::string& eventType, Types... args)
{
    int ret = HiviewDFX::HiSysEvent::Write("THERMAL", eventType, HiviewDFX::HiSysEvent::EventType::STATISTIC, args...);
    if (ret != 0) {
        THERMAL_HILOGE(COMP_SVC, "Write event fail: %{public}s", eventType.c_str());
    }
}

void WriteLevelChangedHiSysEvent(bool enableEvent, int32_t level)
{
    if (enableEvent) {
        WriteEvent("THERMAL_LEVEL_CHANGED", "LEVEL", level);
    }
}

void WriteActionTriggeredHiSysEvent(bool enableEvent, const std::string& actionName, int32_t value)
{
    if (enableEvent) {
        WriteEvent("THERMAL_ACTION_TRIGGERED", "ACTION", actionName, "VALUE", value);
    }
}

void WriteActionTriggeredHiSysEventWithRatio(bool enableEvent, const std::string& actionName, float value)
{
    if (enableEvent) {
        WriteEvent("THERMAL_ACTION_TRIGGERED", "ACTION", actionName, "RATIO", value);
    }
}
} // namespace PowerMgr
} // namespace OHOS