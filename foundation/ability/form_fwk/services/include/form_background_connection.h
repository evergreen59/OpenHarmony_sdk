/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_FORM_FWK_FORM_BACKGROUND_CONNECTION_H
#define OHOS_FORM_FWK_FORM_BACKGROUND_CONNECTION_H

#include "form_ability_connection.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * @class FormBackgroundConnection
 * Form Background Connection Stub.
 */
class FormBackgroundConnection : public FormAbilityConnection {
public:
    FormBackgroundConnection(const int64_t formId, const std::string &bundleName, const std::string &abilityName,
                             const std::string &funcName, const std::string &params);
    virtual ~FormBackgroundConnection() = default;

    /**
     * @brief OnAbilityConnectDone, AbilityMs notify caller ability the result of connect.
     * @param element service ability's ElementName.
     * @param remoteObject the session proxy of service ability.
     * @param resultCode ERR_OK on success, others on failure.
     */
    void OnAbilityConnectDone(
        const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override;

private:
    int64_t formId_ = -1;
    std::string funcName_ = "";
    std::string params_ = "";
    DISALLOW_COPY_AND_MOVE(FormBackgroundConnection);
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif // OHOS_FORM_FWK_FORM_BACKGROUND_CONNECTION_H
