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

#ifndef DISPLAYMGR_IDISPLAY_MGR_H
#define DISPLAYMGR_IDISPLAY_MGR_H

#include <iremote_broker.h>
#include <vector>

#include "display_mgr_errors.h"
#include "display_power_info.h"
#include "idisplay_power_callback.h"

namespace OHOS {
namespace DisplayPowerMgr {
class IDisplayPowerMgr : public IRemoteBroker {
public:
    enum {
        SET_DISPLAY_STATE = 0,
        GET_DISPLAY_STATE,
        GET_DISPLAY_IDS,
        GET_MAIN_DISPLAY_ID,
        SET_BRIGHTNESS,
        DISCOUNT_BRIGHTNESS,
        OVERRIDE_BRIGHTNESS,
        RESTORE_BRIGHTNESS,
        GET_BRIGHTNESS,
        GET_DEFAULT_BRIGHTNESS,
        GET_MAX_BRIGHTNESS,
        GET_MIN_BRIGHTNESS,
        ADJUST_BRIGHTNESS,
        AUTO_ADJUST_BRIGHTNESS,
        IS_AUTO_ADJUST_BRIGHTNESS,
        REGISTER_CALLBACK,
        BOOST_BRIGHTNESS,
        CANCEL_BOOST_BRIGHTNESS,
        GET_DEVICE_BRIGHTNESS
    };

    virtual bool SetDisplayState(uint32_t id, DisplayState state, uint32_t reason) = 0;
    virtual DisplayState GetDisplayState(uint32_t id) = 0;
    virtual std::vector<uint32_t> GetDisplayIds() = 0;
    virtual uint32_t GetMainDisplayId() = 0;
    virtual bool SetBrightness(uint32_t value, uint32_t displayId) = 0;
    virtual bool DiscountBrightness(double discount, uint32_t displayId) = 0;
    virtual bool OverrideBrightness(uint32_t value, uint32_t displayId) = 0;
    virtual bool RestoreBrightness(uint32_t displayId) = 0;
    virtual uint32_t GetBrightness(uint32_t displayId) = 0;
    virtual uint32_t GetDefaultBrightness() = 0;
    virtual uint32_t GetMaxBrightness() = 0;
    virtual uint32_t GetMinBrightness() = 0;
    virtual bool AdjustBrightness(uint32_t id, int32_t value, uint32_t duration) = 0;
    virtual bool AutoAdjustBrightness(bool enable) = 0;
    virtual bool IsAutoAdjustBrightness() = 0;
    virtual bool RegisterCallback(sptr<IDisplayPowerCallback> callback) = 0;
    virtual bool BoostBrightness(int32_t timeoutMs, uint32_t displayId) = 0;
    virtual bool CancelBoostBrightness(uint32_t displayId) = 0;
    virtual uint32_t GetDeviceBrightness(uint32_t displayId) = 0;
    virtual DisplayErrors GetError() = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.displaypowermgr.IDisplayPowerMgr");
};
} // namespace DisplayPowerMgr
} // namespace OHOS
#endif // DISPLAYMGR_IDISPLAY_MGR_H
