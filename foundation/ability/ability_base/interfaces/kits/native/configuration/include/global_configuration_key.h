/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ABILITY_BASE_GLOBAL_CONFIGURATION_KEY_H
#define OHOS_ABILITY_BASE_GLOBAL_CONFIGURATION_KEY_H

#include <string>

namespace OHOS {
namespace AAFwk {
namespace GlobalConfigurationKey {
    /* For the time being, there is no uniform standard */
    /* Must be synchronized with the keystore(SystemConfigurationKeyStore)in the configuration */
    constexpr const char* SYSTEM_LANGUAGE = "ohos.system.language";
    constexpr const char* SYSTEM_COLORMODE = "ohos.system.colorMode";
    constexpr const char* INPUT_POINTER_DEVICE = "input.pointer.device";
    constexpr const char* DEVICE_TYPE = "const.build.characteristics";
} // namespace GlobalConfigurationKey
} // namespace AAFwk
} // namespace OHOS
#endif // OHOS_ABILITY_BASE_GLOBAL_CONFIGURATION_KEY_H