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

#include "mock_form_provider_client.h"

#include "errors.h"
#include "form_constants.h"
#include "form_supply_interface.h"
#include "hilog_wrapper.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * Acquire to give back an ProviderFormInfo. This is sync API.
 *
 * @param want, The want of the form to create.
 * @param callerToken, Caller ability token.
 * @return none.
 */
int MockFormProviderClient::AcquireProviderFormInfo(const FormJsInfo &formJsInfo, const Want &want,
    const sptr<IRemoteObject> &callerToken)
{
    // avoid the user modify the number in onCreate

    HILOG_DEBUG("Acquire provider form info");

    sptr<IFormSupply> formSupply = iface_cast<IFormSupply>(callerToken);
    if (formSupply == nullptr) {
        HILOG_ERROR("failed to get formSupplyProxy");
        return ERR_OK;
    }

    FormProviderInfo formProviderInfo;
    Want newWant(want);
    newWant.SetParam(Constants::ACQUIRE_TYPE, want.GetIntParam(Constants::ACQUIRE_TYPE, 0));
    newWant.SetParam(Constants::FORM_CONNECT_ID, want.GetIntParam(Constants::FORM_CONNECT_ID, 0));
    newWant.SetParam(Constants::FORM_SUPPLY_INFO, want.GetStringParam(Constants::FORM_SUPPLY_INFO));
    newWant.SetParam(Constants::PROVIDER_FLAG, true);
    newWant.SetParam(Constants::PARAM_FORM_IDENTITY_KEY, std::to_string(formJsInfo.formId));
    formSupply->OnAcquire(formProviderInfo, newWant);
    return ERR_OK;
}

/**
 * Notify provider when the form was deleted.
 *
 * @param formId, The Id of the form.
 * @param callerToken, Caller ability token.
 * @return none.
 */
int MockFormProviderClient::NotifyFormDelete(const int64_t formId, const Want &want,
    const sptr<IRemoteObject> &callerToken)
{
    HILOG_DEBUG("Notify form delete");
    return ERR_OK;
}
/**
 * Notify provider when the forms was deleted.
 *
 * @param formIds, The id list of forms.
 * @param want Indicates the structure containing form info.
 * @param callerToken, Caller ability token.
 * @return none.
 */
int MockFormProviderClient::NotifyFormsDelete(const std::vector<int64_t> &formIds, const Want &want,
    const sptr<IRemoteObject> &callerToken)
{
    HILOG_DEBUG("Notify forms delete");
    return ERR_OK;
}

/**
 * @brief Notify provider when the form need update.
 * @param formId The Id of the form.
 * @param want Indicates the structure containing form info.
 * @param callerToken Caller ability token.
 */
int MockFormProviderClient::NotifyFormUpdate(const int64_t formId, const Want &want,
    const sptr<IRemoteObject> &callerToken)
{
    HILOG_DEBUG("Notify form update");
    return ERR_OK;
}

/**
 * @brief Event notify when change the form visible.
 *
 * @param formEvents The vector of form ids.
 * @param formVisibleType The form visible type, including FORM_VISIBLE and FORM_INVISIBLE.
 * @param want Indicates the structure containing form info.
 * @param callerToken Caller ability token.
 * @return Returns ERR_OK on success, others on failure.
 */
int MockFormProviderClient::EventNotify(const std::vector<int64_t> &formIds, const int32_t formVisibleType,
    const Want &want, const sptr<IRemoteObject> &callerToken)
{
    HILOG_DEBUG("Event notify");
    return ERR_OK;
}

/**
 * Notify provider when the temp form was cast to normal form.
 *
 * @param formId, The Id of the form to update.
 * @param callerToken, Caller ability token.
 * @return none.
 */
int MockFormProviderClient::NotifyFormCastTempForm(const int64_t formId, const Want &want,
    const sptr<IRemoteObject> &callerToken)
{
    HILOG_DEBUG("Notify cast temp form");
    return ERR_OK;
}
/**
 * @brief Fire message event to form provider.
 * @param formId The Id of the from.
 * @param message Event message.
 * @param want The want of the request.
 * @param callerToken Form provider proxy object.
 * @return Returns ERR_OK on success, others on failure.
 */
int MockFormProviderClient::FireFormEvent(const int64_t formId, const std::string &message, const Want &want,
    const sptr<IRemoteObject> &callerToken)
{
    HILOG_DEBUG("Fire form event");
    return ERR_OK;
}

/**
 * @brief Acquire form state to form provider.
 * @param wantArg The want of onAcquireFormState.
 * @param provider The provider info.
 * @param want The want of the request.
 * @param callerToken Form provider proxy object.
 * @return Returns ERR_OK on success, others on failure.
 */
int MockFormProviderClient::AcquireState(const Want &wantArg, const std::string &provider, const Want &want,
                                         const sptr<IRemoteObject> &callerToken)
{
    HILOG_DEBUG("Acquire state");
    return ERR_OK;
}

int32_t MockFormProviderClient::AcquireShareFormData(int64_t formId, const std::string &remoteDeviceId,
    const sptr<IRemoteObject> &formSupplyCallback, int64_t requestCode)
{
    HILOG_DEBUG("MockFormProviderClient::AcquireShareFormData");
    acquireShareFormState_ = true;
    return ERR_OK;
}
}  // namespace AppExecFwk
}  // namespace OHOS
