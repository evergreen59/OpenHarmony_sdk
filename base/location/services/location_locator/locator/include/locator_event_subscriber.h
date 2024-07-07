/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef LOCATOR_EVENT_SUBSCRIBER_H
#define LOCATOR_EVENT_SUBSCRIBER_H

#include "common_event_data.h"
#include "common_event_subscribe_info.h"
#include "common_event_subscriber.h"

namespace OHOS {
namespace Location {
const std::string MODE_CHANGED_EVENT = "usual.event.location.MODE_STATE_CHANGED";
class LocatorEventSubscriber : public OHOS::EventFwk::CommonEventSubscriber {
public:
    explicit LocatorEventSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo &info);
    ~LocatorEventSubscriber() override;
    void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &event) override;
};
} // namespace Location
} // namespace OHOS
#endif // LOCATOR_EVENT_SUBSCRIBER_H
