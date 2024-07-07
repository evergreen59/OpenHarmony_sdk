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

#ifndef OHOS_FORM_FWK_FORM_PROVIDER_INTERFACE_H
#define OHOS_FORM_FWK_FORM_PROVIDER_INTERFACE_H

#include <vector>

#include "form_js_info.h"
#include "ipc_types.h"
#include "iremote_broker.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
using OHOS::AAFwk::Want;
/**
 * @class IFormProvider
 * IFormProvider interface is used to access form provider service.
 */
class IFormProvider : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.appexecfwk.FormProvider");

    /**
     * @brief Acquire to give back an ProviderFormInfo. This is sync API.
     * @param formJsInfo The form js info.
     * @param want Indicates the {@link Want} structure containing form info.
     * @param callerToken Caller ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int AcquireProviderFormInfo(const FormJsInfo &formJsInfo, const Want &want,
    const sptr<IRemoteObject> &callerToken) = 0;

    /**
     * @brief Notify provider when the form was deleted.
     * @param formId The Id of the form.
     * @param want Indicates the structure containing form info.
     * @param callerToken Caller ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int NotifyFormDelete(const int64_t formId, const Want &want, const sptr<IRemoteObject> &callerToken) = 0;

    /**
     * @brief Notify provider when the forms was deleted.
     * @param formIds The id list of forms.
     * @param want Indicates the structure containing form info.
     * @param callerToken Caller ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int NotifyFormsDelete(const std::vector<int64_t> &formIds, const Want &want,
    const sptr<IRemoteObject> &callerToken) = 0;
    /**
     * @brief Notify provider when the form need update.
     * @param formId The Id of the form.
     * @param want Indicates the structure containing form info.
     * @param callerToken Caller ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int NotifyFormUpdate(const int64_t formId, const Want &want, const sptr<IRemoteObject> &callerToken) = 0;

    /**
     * @brief Event notify when change the form visible.
     *
     * @param formIds The vector of form ids.
     * @param formVisibleType The form visible type, including FORM_VISIBLE and FORM_INVISIBLE.
     * @param want Indicates the structure containing form info.
     * @param callerToken Caller ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int EventNotify(const std::vector<int64_t> &formIds, const int32_t formVisibleType,
        const Want &want, const sptr<IRemoteObject> &callerToken) = 0;

    /**
     * @brief Notify provider when the temp form was cast to normal form.
     * @param formId The Id of the form to update.
     * @param want Indicates the structure containing form info.
     * @param callerToken Caller ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int NotifyFormCastTempForm(const int64_t formId, const Want &want,
    const sptr<IRemoteObject> &callerToken) = 0;

    /**
     * @brief Fire message event to form provider.
     * @param formId The Id of the from.
     * @param message Event message.
     * @param want The want of the request.
     * @param callerToken Form provider proxy object.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int FireFormEvent(const int64_t formId, const std::string &message, const Want &want,
    const sptr<IRemoteObject> &callerToken) = 0;

    /**
     * @brief Acquire form state to form provider.
     * @param wantArg The want of onAcquireFormState.
     * @param provider The provider info.
     * @param want The want of the request.
     * @param callerToken Form provider proxy object.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int AcquireState(const Want &wantArg, const std::string &provider, const Want &want,
                             const sptr<IRemoteObject> &callerToken) = 0;

    /**
     * @brief Acquire to share form information data. This is sync API.
     * @param formId The Id of the from.
     * @param remoteDeviceId Indicates the remote device ID.
     * @param formSupplyCallback Indicates lifecycle callbacks.
     * @param requestCode Indicates the request code of this share form.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int32_t AcquireShareFormData(int64_t formId, const std::string &remoteDeviceId,
        const sptr<IRemoteObject> &formSupplyCallback, int64_t requestCode) = 0;

    enum class Message {
        // ipc id 1-1000 for kit
        // ipc id 1001-2000 for DMS
        // ipc id 2001-3000 for tools
        // ipc id for add form (3001)
        FORM_ACQUIRE_PROVIDER_FORM_INFO = 3051,

        // ipc id for delete form (3052)
        FORM_PROVIDER_NOTIFY_FORM_DELETE,

        // ipc id for form done release form (3053)
        FORM_PROVIDER_NOTIFY_FORMS_DELETE,

        // ipc id for connecting update form (3054)
        FORM_PROVIDER_NOTIFY_FORM_UPDATE,

        // ipc id for form visible notify (3055)
        FORM_PROVIDER_NOTIFY_TEMP_FORM_CAST,

        // ipc id for event notify (3056)
        FORM_PROVIDER_EVENT_NOTIFY,

        // ipc id for event notify (3057)
        FORM_PROVIDER_EVENT_MESSAGE,

        // ipc id for acquiring form state (3058)
        FORM_PROVIDER_NOTIFY_STATE_ACQUIRE,

        // ipc id for Acquire provider share form info (3059)
        FORM_ACQUIRE_PROVIDER_SHARE_FOMR_INFO,
    };
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif // OHOS_FORM_FWK_FORM_PROVIDER_INTERFACE_H
