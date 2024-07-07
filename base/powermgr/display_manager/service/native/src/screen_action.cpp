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

#include "screen_action.h"

#include <hisysevent.h>

#include <ipc_skeleton.h>
#include "dm_common.h"
#include "display_manager.h"
#include "display_log.h"
#include "screen_manager.h"

namespace OHOS {
namespace DisplayPowerMgr {
ScreenAction::ScreenAction(uint32_t displayId) : displayId_(displayId)
{}

uint32_t ScreenAction::GetDefaultDisplayId()
{
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    uint64_t defaultId = Rosen::DisplayManager::GetInstance().GetDefaultDisplayId();
    IPCSkeleton::SetCallingIdentity(identity);
    return static_cast<uint32_t>(defaultId);
}

std::vector<uint32_t> ScreenAction::GetAllDisplayId()
{
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    std::vector<uint64_t> allIds = Rosen::DisplayManager::GetInstance().GetAllDisplayIds();
    IPCSkeleton::SetCallingIdentity(identity);
    std::vector<uint32_t> displayIds;
    if (allIds.empty()) {
        displayIds.push_back(DEFAULT_DISPLAY_ID);
        return displayIds;
    }
    std::transform(allIds.begin(), allIds.end(), back_inserter(displayIds), [](uint64_t id) {
        return static_cast<uint32_t>(id);
    });
    return displayIds;
}

uint32_t ScreenAction::GetDisplayId()
{
    return displayId_;
}

DisplayState ScreenAction::GetDisplayState()
{
    DisplayState state = DisplayState::DISPLAY_UNKNOWN;
    Rosen::ScreenPowerState powerState = Rosen::ScreenManager::GetInstance().GetScreenPower(displayId_);
    DISPLAY_HILOGI(FEAT_STATE, "ScreenPowerState=%{public}d", static_cast<uint32_t>(powerState));
    switch (powerState) {
        case Rosen::ScreenPowerState::POWER_ON:
            state = DisplayState::DISPLAY_ON;
            break;
        case Rosen::ScreenPowerState::POWER_STAND_BY:
            state = DisplayState::DISPLAY_DIM;
            break;
        case Rosen::ScreenPowerState::POWER_SUSPEND:
            state = DisplayState::DISPLAY_SUSPEND;
            break;
        case Rosen::ScreenPowerState::POWER_OFF:
            state = DisplayState::DISPLAY_OFF;
            break;
        default:
            break;
    }
    DISPLAY_HILOGI(FEAT_STATE, "state=%{public}u displayId=%{public}u", static_cast<uint32_t>(state), displayId_);
    return state;
}

bool ScreenAction::SetDisplayState(DisplayState state, const std::function<void(DisplayState)>& callback)
{
    DISPLAY_HILOGI(FEAT_STATE, "SetDisplayState: displayId=%{public}u, state=%{public}u",
                   displayId_, static_cast<uint32_t>(state));

    Rosen::DisplayState rds = Rosen::DisplayState::UNKNOWN;
    switch (state) {
        case DisplayState::DISPLAY_ON:
            rds = Rosen::DisplayState::ON;
            break;
        case DisplayState::DISPLAY_OFF:
            rds = Rosen::DisplayState::OFF;
            break;
        default:
            break;
    }
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    bool ret = Rosen::DisplayManager::GetInstance().SetDisplayState(rds,
        [callback](Rosen::DisplayState rosenState) {
            DISPLAY_HILOGI(FEAT_STATE, "SetDisplayState Callback:%{public}d",
                static_cast<uint32_t>(rosenState));
            DisplayState state;
            switch (rosenState) {
                case Rosen::DisplayState::ON:
                    state = DisplayState::DISPLAY_ON;
                    break;
                case Rosen::DisplayState::OFF:
                    state = DisplayState::DISPLAY_OFF;
                    break;
                default:
                    return;
            }
            callback(state);
    });
    IPCSkeleton::SetCallingIdentity(identity);
    // Notify screen state change event to battery statistics
    HiviewDFX::HiSysEvent::Write("DISPLAY", "SCREEN_STATE",
        HiviewDFX::HiSysEvent::EventType::STATISTIC, "STATE", static_cast<int32_t>(state));
    DISPLAY_HILOGI(FEAT_STATE, "SetDisplayState:%{public}d", ret);
    return ret;
}

bool ScreenAction::SetDisplayPower(DisplayState state, uint32_t reason)
{
    DISPLAY_HILOGI(FEAT_STATE, "SetDisplayPower: displayId=%{public}u, state=%{public}u, state=%{public}u",
                   displayId_, static_cast<uint32_t>(state), reason);
    Rosen::ScreenPowerState status = Rosen::ScreenPowerState::INVALID_STATE;
    switch (state) {
        case DisplayState::DISPLAY_ON:
            status = Rosen::ScreenPowerState::POWER_ON;
            break;
        case DisplayState::DISPLAY_DIM:
            status = Rosen::ScreenPowerState::POWER_STAND_BY;
            break;
        case DisplayState::DISPLAY_SUSPEND:
            status = Rosen::ScreenPowerState::POWER_SUSPEND;
            break;
        case DisplayState::DISPLAY_OFF:
            status = Rosen::ScreenPowerState::POWER_OFF;
            break;
        default:
            break;
    }
    bool ret = Rosen::ScreenManager::GetInstance().SetScreenPowerForAll(status,
        Rosen::PowerStateChangeReason::POWER_BUTTON);
    DISPLAY_HILOGI(FEAT_STATE, "SetScreenPowerForAll:%{public}d", ret);
    return true;
}

uint32_t ScreenAction::GetBrightness()
{
    std::lock_guard lock(mutexBrightness_);
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    brightness_ = Rosen::DisplayManager::GetInstance().GetScreenBrightness(displayId_);
    IPCSkeleton::SetCallingIdentity(identity);
    DISPLAY_HILOGD(FEAT_BRIGHTNESS, "displayId=%{public}u, brightness=%{public}u", displayId_, brightness_);
    return brightness_;
}

bool ScreenAction::SetBrightness(uint32_t value)
{
    DISPLAY_HILOGD(FEAT_BRIGHTNESS, "displayId=%{public}u, brightness=%{public}u", displayId_, value);
    // Notify screen brightness change event to battery statistics
    HiviewDFX::HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT",
        HiviewDFX::HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", value);
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    bool isSucc = Rosen::DisplayManager::GetInstance().SetScreenBrightness(displayId_, value);
    IPCSkeleton::SetCallingIdentity(identity);
    std::lock_guard lock(mutexBrightness_);
    brightness_ = isSucc ? value : brightness_;
    return isSucc;
}
} // namespace DisplayPowerMgr
} // namespace OHOS
