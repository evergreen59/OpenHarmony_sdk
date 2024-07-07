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

#ifndef OHOS_FORM_FWK_FORM_HOST_INTERFACE_H
#define OHOS_FORM_FWK_FORM_HOST_INTERFACE_H

#include <vector>

#include "form_js_info.h"
#include "form_state_info.h"
#include "ipc_types.h"
#include "iremote_broker.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * @class IFormHost
 * IFormHost interface is used to access form host service.
 */
class IFormHost : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.appexecfwk.FormHost");

    /**
     * @brief Request to give back a Form.
     * @param formInfo Form info.
     * @param token Provider client token.
     */
    virtual void OnAcquired(const FormJsInfo &formInfo, const sptr<IRemoteObject> &token) = 0;

     /**
     * @brief Form is updated.
     * @param formInfo Form info.
     */
    virtual void OnUpdate(const FormJsInfo &formInfo) = 0;

    /**
     * @brief Form provider is uninstalled.
     * @param formIds The Id list of the forms.
     */
    virtual void OnUninstall(const std::vector<int64_t> &formIds) = 0;

    /**
     * @brief Form provider is acquire state
     * @param state The form state.
     * @param want The form want.
     */
    virtual void OnAcquireState(AppExecFwk::FormState state, const AAFwk::Want &want) = 0;

    /**
     * @brief Responsive form sharing.
     * @param requestCode The request code of this share form.
     * @param result Share form result.
     */
    virtual void OnShareFormResponse(int64_t requestCode, int32_t result) = 0;

    /**
     * @brief Return error to host.
     *
     * @param errorCode Indicates error-code of the form.
     * @param errorMsg Indicates error-message of the form.
     */
    virtual void OnError(int32_t errorCode, const std::string &errorMsg) = 0;

    enum class Message {
        // ipc id 1-1000 for kit
        // ipc id 1001-2000 for DMS
        // ipc id 2001-3000 for tools
        // ipc id for create (3001)
        FORM_HOST_ON_ACQUIRED = 3681,

        // ipc id for update (3682)
        FORM_HOST_ON_UPDATE,

        // ipc id for uninstall (3683)
        FORM_HOST_ON_UNINSTALL,

        // ipc id for uninstall (3684)
        FORM_HOST_ON_ACQUIRE_FORM_STATE,

        // ipc id for share form response(3685)
        FORM_HOST_ON_SHARE_FORM_RESPONSE,

        // ipc id for return form error to host(3686)
        FORM_HOST_ON_ERROR,
    };
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif // OHOS_FORM_FWK_FORM_HOST_INTERFACE_H
