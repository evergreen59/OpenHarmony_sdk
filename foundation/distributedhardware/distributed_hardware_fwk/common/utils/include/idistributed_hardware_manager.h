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

#ifndef OHOS_DISTRIBUTED_HARDWARE_IDISTRIBUTED_HARDWARE_MANAGER_H
#define OHOS_DISTRIBUTED_HARDWARE_IDISTRIBUTED_HARDWARE_MANAGER_H

#include <string>
#include <unordered_map>

#include "device_type.h"

namespace OHOS {
namespace DistributedHardware {
class IDistributedHardwareManager {
public:
    virtual ~IDistributedHardwareManager() {}
    virtual int32_t Initialize() = 0;
    virtual int32_t Release() = 0;
    virtual int32_t SendOnLineEvent(const std::string &networkId, const std::string &uuid,
        uint16_t deviceType) = 0;
    virtual int32_t SendOffLineEvent(const std::string &networkId, const std::string &uuid,
        uint16_t deviceType) = 0;
    virtual size_t GetOnLineCount() = 0;
    virtual int32_t GetComponentVersion(std::unordered_map<DHType, std::string> &versionMap) = 0;
    virtual int32_t Dump(const std::vector<std::string> &argsStr, std::string &result) = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_HARDWARE_IDISTRIBUTED_HARDWARE_MANAGER_H
