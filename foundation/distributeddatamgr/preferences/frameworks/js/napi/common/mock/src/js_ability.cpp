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

#include "js_ability.h"

#include <cstdlib>

#include "js_logger.h"

namespace OHOS {
namespace PreferencesJsKit {
Context::Context()
{
    std::string baseDir = "";
#ifdef WINDOWS_PLATFORM
    baseDir = getenv("TEMP");
    if (!baseDir.empty()) {
        preferencesDir_ = baseDir + "\\HuaweiDevEcoStudioPreferences";
    }
#endif

#ifdef MAC_PLATFORM
    baseDir = getenv("LOGNAME");
    baseDir = "/Users/" + baseDir + "/Library/Caches";
    if (!baseDir.empty()) {
        preferencesDir_ = baseDir + "/HuaweiDevEcoStudioPreferences";
    }
#endif
}

std::string Context::GetPreferencesDir()
{
    return preferencesDir_;
}

bool JSAbility::CheckContext(napi_env env, napi_callback_info info)
{
    return true;
}

std::shared_ptr<Context> JSAbility::GetContext(napi_env env, napi_value value)
{
    return std::make_shared<Context>();
}
} // namespace PreferencesJsKit
} // namespace OHOS
