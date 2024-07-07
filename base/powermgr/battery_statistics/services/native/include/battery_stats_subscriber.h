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

#ifndef BATTERY_STATS_SUBSCRIBER_H
#define BATTERY_STATS_SUBSCRIBER_H

#include "common_event_data.h"
#include "common_event_subscriber.h"
#include "common_event_subscribe_info.h"

namespace OHOS {
namespace PowerMgr {
class BatteryStatsSubscriber : public EventFwk::CommonEventSubscriber {
public:
    explicit BatteryStatsSubscriber(const EventFwk::CommonEventSubscribeInfo& subscribeInfo)
        : EventFwk::CommonEventSubscriber(subscribeInfo) {}
    virtual ~BatteryStatsSubscriber() {}
    void OnReceiveEvent(const EventFwk::CommonEventData &data) override;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS 4_SUBSCRIBER_H