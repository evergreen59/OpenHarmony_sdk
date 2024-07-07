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

#ifndef OHOS_DISTRIBUTED_HARDWARE_DHUTILS_TOOL_H
#define OHOS_DISTRIBUTED_HARDWARE_DHUTILS_TOOL_H

#include <cstdint>

#include "nlohmann/json.hpp"

#include "device_type.h"

namespace OHOS {
namespace DistributedHardware {
/**
 * return current time in millisecond.
 */
int64_t GetCurrentTime();

/**
 * return a random string id.
 */
std::string GetRandomID();

std::string GetUUIDBySoftBus(const std::string &networkId);

DeviceInfo GetLocalDeviceInfo();

/* Convert uuid to deviceId by sha256 encode */
std::string GetDeviceIdByUUID(const std::string &uuid);

std::string Sha256(const std::string& string);

bool IsUInt16(const nlohmann::json& jsonObj, const std::string& key);

bool IsInt32(const nlohmann::json& jsonObj, const std::string& key);

bool IsUInt32(const nlohmann::json& jsonObj, const std::string& key);

bool IsBool(const nlohmann::json& jsonObj, const std::string& key);

bool IsString(const nlohmann::json& jsonObj, const std::string& key);
} // namespace DistributedHardware
} // namespace OHOS
#endif
