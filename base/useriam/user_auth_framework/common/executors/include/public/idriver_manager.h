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

#ifndef IDRIVER_MANAGER_H
#define IDRIVER_MANAGER_H

#include <cstdint>
#include <map>

#include "iauth_driver_hdi.h"

namespace OHOS {
namespace UserIam {
namespace UserAuth {
struct HdiConfig {
    uint16_t id; // non-zero
    std::shared_ptr<IAuthDriverHdi> driver;
};

class IDriverManager {
public:
    IDriverManager() = default;
    virtual ~IDriverManager() = default;

    static int32_t Start(const std::map<std::string, HdiConfig> &hdiName2Config);
};
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS

#endif // IDRIVER_MANAGER_H