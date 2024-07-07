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

#ifndef REMOTE_PROTOCOL_H
#define REMOTE_PROTOCOL_H

namespace OHOS {
namespace Security {
namespace AccessToken {
struct RemoteProtocol {
    std::string commandName;
    std::string uniqueId;
    int32_t requestVersion;
    std::string srcDeviceId;
    std::string srcDeviceLevel;
    std::string dstDeviceId;
    std::string dstDeviceLevel;
    int32_t statusCode;
    std::string message;
    int32_t responseVersion;
    std::string responseDeviceId;
};
}  // namespace AccessToken
}  // namespace Security
}  // namespace OHOS
#endif