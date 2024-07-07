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

#ifndef OHOS_DISTRIBUTED_HARDWARE_EVENT_REGISTRATION_H
#define OHOS_DISTRIBUTED_HARDWARE_EVENT_REGISTRATION_H

#include <set>
#include <memory>

#include "event_sender.h"

namespace OHOS {
namespace DistributedHardware {
class EventRegistration {
public:
    using Registrations = std::set<std::shared_ptr<EventRegistration>>;

    EventRegistration(void * const handler, EventSender * const sender) : handler(handler), sender(sender) {}

    virtual ~EventRegistration() {}

    const void *GetHandler()
    {
        return handler;
    }

    const EventSender *GetSender()
    {
        return sender;
    }

private:
    void * const handler;
    EventSender * const sender;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif
