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

#ifndef OHOS_IDISTRIBUTED_HARDWARE_H
#define OHOS_IDISTRIBUTED_HARDWARE_H

#include <cstdint>
#include <string>
#include <unordered_map>

#include "iremote_broker.h"
#include "device_type.h"

#include "ipublisher_listener.h"

namespace OHOS {
namespace DistributedHardware {
class IDistributedHardware : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.distributedhardware.distributedhardwarefwk");
    IDistributedHardware() = default;
    virtual ~IDistributedHardware() = default;
    virtual int32_t RegisterPublisherListener(const DHTopic topic, const sptr<IPublisherListener> &listener) = 0;
    virtual int32_t UnregisterPublisherListener(const DHTopic topic, const sptr<IPublisherListener> &listener) = 0;
    virtual int32_t PublishMessage(const DHTopic topic, const std::string &msg) = 0;
public:
    enum class Message : uint32_t {
        REG_PUBLISHER_LISTNER = 1,
        UNREG_PUBLISHER_LISTENER = 2,
        PUBLISH_MESSAGE = 3
    };
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_IDISTRIBUTED_HARDWARE_H
