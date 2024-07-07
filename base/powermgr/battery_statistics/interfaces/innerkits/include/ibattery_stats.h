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

#ifndef IBATTERY_STATS_H
#define IBATTERY_STATS_H

#include "iremote_broker.h"
#include "iremote_object.h"

#include "battery_stats_info.h"

namespace OHOS {
namespace PowerMgr {
class IBatteryStats : public IRemoteBroker {
public:
    enum {
        BATTERY_STATS_GET = 0,
        BATTERY_STATS_GETAPPMAH,
        BATTERY_STATS_GETAPPPER,
        BATTERY_STATS_GETPARTMAH,
        BATTERY_STATS_GETPARTPER,
        BATTERY_STATS_GETTIME,
        BATTERY_STATS_GETDATA,
        BATTERY_STATS_RESET,
        BATTERY_STATS_SETONBATT,
        BATTERY_STATS_DUMP,
    };
    virtual BatteryStatsInfoList GetBatteryStats() = 0;
    virtual void SetOnBattery(bool isOnBattery) = 0;
    virtual double GetAppStatsMah(const int32_t& uid) = 0;
    virtual double GetAppStatsPercent(const int32_t& uid) = 0;
    virtual double GetPartStatsMah(const BatteryStatsInfo::ConsumptionType& type) = 0;
    virtual double GetPartStatsPercent(const BatteryStatsInfo::ConsumptionType& type) = 0;
    virtual uint64_t GetTotalTimeSecond(const StatsUtils::StatsType& statsType,
        const int32_t& uid = StatsUtils::INVALID_VALUE) = 0;
    virtual uint64_t GetTotalDataBytes(const StatsUtils::StatsType& statsType,
        const int32_t& uid = StatsUtils::INVALID_VALUE) = 0;
    virtual void Reset() = 0;
    virtual std::string ShellDump(const std::vector<std::string>& args, uint32_t argc) = 0;
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.powermgr.IBatteryStats");
};
} // namespace PowerMgr
} // namespace OHOS

#endif // IBATTERY_STATS_H
