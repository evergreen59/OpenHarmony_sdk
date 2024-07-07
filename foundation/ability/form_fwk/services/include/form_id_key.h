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

#ifndef OHOS_FORM_FWK_FORM_ID_KEY_H
#define OHOS_FORM_FWK_FORM_ID_KEY_H

#include <string>

namespace OHOS {
namespace AppExecFwk {
struct FormIdKey {
public:
    FormIdKey(std::string &bundleName, std::string &abilityName)
        : bundleName(bundleName), moduleName(), abilityName(abilityName), formName(), specificationId(0), orientation(0)
    {
    }

    std::string bundleName;
    std::string moduleName;
    std::string abilityName;
    std::string formName;
    int specificationId;
    int orientation;

    bool operator== (const FormIdKey &formIdKey) const
    {
        return specificationId == formIdKey.specificationId
            && orientation == formIdKey.orientation
            && bundleName == formIdKey.bundleName
            && moduleName == formIdKey.moduleName
            && abilityName == formIdKey.abilityName
            && formName == formIdKey.formName;
    }
    /**
     * @brief overloaded == for Indicates the formDBInfo by formId
     * @return Returns true if the data equal; returns false otherwise.
     */
    bool operator< (const FormIdKey &formIdKey) const
    {
        return specificationId != formIdKey.specificationId
            || orientation != formIdKey.orientation
            || bundleName != formIdKey.bundleName
            || moduleName != formIdKey.moduleName
            || abilityName != formIdKey.abilityName
            || formName != formIdKey.formName;
    }
    int hashCode()
    {
        return std::hash<std::string>()(bundleName)
            + std::hash<std::string>()(moduleName)
            + std::hash<std::string>()(abilityName)
            + std::hash<std::string>()(formName)
            + std::hash<int>()(specificationId)
            + std::hash<int>()(orientation);
    }
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif // OHOS_FORM_FWK_FORM_ID_KEY_H
