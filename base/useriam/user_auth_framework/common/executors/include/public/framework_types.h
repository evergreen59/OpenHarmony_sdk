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

#ifndef FRAMEWORK_TYPES_H
#define FRAMEWORK_TYPES_H

#include <cstdint>
#include <vector>

namespace OHOS {
namespace UserIam {
namespace UserAuth {
enum UserAuthResult : int32_t {
    USERAUTH_SUCCESS = 0,
    USERAUTH_ERROR = 1,
};

struct TemplateInfo {
    uint32_t executorType;
    int32_t freezingTime;
    int32_t remainTimes;
    std::vector<uint8_t> extraInfo;
};
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS

#endif // FRAMEWORK_TYPES_H
