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

#ifndef OHOS_FORM_FWK_FORM_ACQUIRE_CONNECTION_H
#define OHOS_FORM_FWK_FORM_ACQUIRE_CONNECTION_H

#include "event_handler.h"
#include "form_ability_connection.h"
#include "form_item_info.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
using WantParams = OHOS::AAFwk::WantParams;

/**
 * @class FormAcquireConnection
 * Form Acquire Connection Stub.
 */
class FormAcquireConnection : public FormAbilityConnection {
public:
    FormAcquireConnection(const int64_t formId, const FormItemInfo &info, const WantParams &wantParams,
        const sptr<IRemoteObject> hostToken);
    FormAcquireConnection() = delete; // disable default constructor.
    virtual ~FormAcquireConnection() = default;

    /**
     * @brief OnAbilityConnectDone, AbilityMs notify caller ability the result of connect.
     * @param element service ability's ElementName.
     * @param remoteObject the session proxy of service ability.
     * @param resultCode ERR_OK on success, others on failure.
     */
    void OnAbilityConnectDone(
        const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override;

private:
    FormItemInfo info_;
    WantParams wantParams_;

    DISALLOW_COPY_AND_MOVE(FormAcquireConnection);
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif // OHOS_FORM_FWK_FORM_ACQUIRE_CONNECTION_H
