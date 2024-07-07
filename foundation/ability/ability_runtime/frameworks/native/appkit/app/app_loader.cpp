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

#include "app_loader.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * @description: Gets the ApplicationLoader object to application register
 * @param None
 * @return ApplicationLoader
 */
ApplicationLoader &ApplicationLoader::GetInstance()
{
    static ApplicationLoader applicationLoader;
    return applicationLoader;
}

/**
 * @description: Gets the ApplicationLoader object to register application
 * @param bundleName the bundle name of the application.
 * @param createFunc constructor function of application class.
 * @return None
 */
void ApplicationLoader::RegisterApplication(const std::string &bundleName, const CreateApplication &createFunc)
{
    applications_.emplace(bundleName, createFunc);
    HILOG_DEBUG("ApplicationLoader::RegisterApplication:%{public}s", bundleName.c_str());
}

/**
 * @description: Gets the {@link OHOSApplication} object
 * @return Return {@link OHOSApplication} object which is registered by developer.
 */
OHOSApplication *ApplicationLoader::GetApplicationByName()
{
    auto it = applications_.find("OHOSApplication");
    if (it == applications_.end()) {
        HILOG_ERROR("ApplicationLoader::GetApplicationByName failed:OHOSApplication");
    } else {
        return it->second();
    }
    return nullptr;
}
}  // namespace AppExecFwk
}  // namespace OHOS
