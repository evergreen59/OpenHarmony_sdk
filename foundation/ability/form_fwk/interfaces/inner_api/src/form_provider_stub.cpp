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
#include "form_provider_stub.h"
#include "appexecfwk_errors.h"
#include "app_scheduler_interface.h"
#include "errors.h"
#include "form_mgr_errors.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iremote_object.h"

namespace OHOS {
namespace AppExecFwk {
FormProviderStub::FormProviderStub()
{
    memberFuncMap_[static_cast<uint32_t>(IFormProvider::Message::FORM_ACQUIRE_PROVIDER_FORM_INFO)] =
        &FormProviderStub::HandleAcquireProviderFormInfo;
    memberFuncMap_[static_cast<uint32_t>(IFormProvider::Message::FORM_PROVIDER_NOTIFY_FORM_DELETE)] =
        &FormProviderStub::HandleNotifyFormDelete;
    memberFuncMap_[static_cast<uint32_t>(IFormProvider::Message::FORM_PROVIDER_NOTIFY_FORMS_DELETE)] =
        &FormProviderStub::HandleNotifyFormsDelete;
    memberFuncMap_[static_cast<uint32_t>(IFormProvider::Message::FORM_PROVIDER_NOTIFY_FORM_UPDATE)] =
        &FormProviderStub::HandleNotifyFormUpdate;
    memberFuncMap_[static_cast<uint32_t>(IFormProvider::Message::FORM_PROVIDER_EVENT_NOTIFY)] =
        &FormProviderStub::HandleEventNotify;
    memberFuncMap_[static_cast<uint32_t>(IFormProvider::Message::FORM_PROVIDER_NOTIFY_TEMP_FORM_CAST)] =
        &FormProviderStub::HandleNotifyFormCastTempForm;
    memberFuncMap_[static_cast<uint32_t>(IFormProvider::Message::FORM_PROVIDER_EVENT_MESSAGE)] =
        &FormProviderStub::HandleFireFormEvent;
    memberFuncMap_[static_cast<uint32_t>(IFormProvider::Message::FORM_PROVIDER_NOTIFY_STATE_ACQUIRE)] =
        &FormProviderStub::HandleAcquireState;
    memberFuncMap_[static_cast<uint32_t>(IFormProvider::Message::FORM_ACQUIRE_PROVIDER_SHARE_FOMR_INFO)] =
        &FormProviderStub::HandleAcquireShareFormData;
}

FormProviderStub::~FormProviderStub()
{
    memberFuncMap_.clear();
}
/**
 * @brief handle remote request.
 * @param data input param.
 * @param reply output param.
 * @param option message option.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormProviderStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOG_INFO("FormProviderStub::OnReceived, code = %{public}u, flags= %{public}d.", code, option.GetFlags());
    std::u16string descriptor = FormProviderStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_ERROR("%{public}s failed, local descriptor is not equal to remote", __func__);
        return ERR_APPEXECFWK_FORM_INVALID_PARAM;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }

    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int FormProviderStub::HandleAcquireProviderFormInfo(MessageParcel &data, MessageParcel &reply)
{
    std::unique_ptr<FormJsInfo> formJsInfo(data.ReadParcelable<FormJsInfo>());
    if (!formJsInfo) {
        HILOG_ERROR("%{public}s, failed to ReadParcelable<formJsInfo>", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        HILOG_ERROR("%{public}s, failed to ReadParcelable<Want>", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    sptr<IRemoteObject> client = data.ReadRemoteObject();
    if (client == nullptr) {
        HILOG_ERROR("%{public}s, failed to get remote object.", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    int32_t result = AcquireProviderFormInfo(*formJsInfo, *want, client);
    reply.WriteInt32(result);
    return result;
}
/**
 * @brief handle NotifyFormDelete message.
 * @param data input param.
 * @param reply output param.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormProviderStub::HandleNotifyFormDelete(MessageParcel &data, MessageParcel &reply)
{
    int64_t formId = data.ReadInt64();
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        HILOG_ERROR("%{public}s fail, ReadParcelable<FormReqInfo> failed", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    sptr<IRemoteObject> client = data.ReadRemoteObject();
    if (client == nullptr) {
        HILOG_ERROR("%{public}s, failed to get remote object.", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    int32_t result = NotifyFormDelete(formId, *want, client);
    reply.WriteInt32(result);
    return result;
}
/**
 * @brief handle NotifyFormsDelete message.
 * @param data input param.
 * @param reply output param.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormProviderStub::HandleNotifyFormsDelete(MessageParcel &data, MessageParcel &reply)
{
    std::vector<int64_t> formIds;
    bool ret = data.ReadInt64Vector(&formIds);
    if (ret) {
        std::unique_ptr<Want> want(data.ReadParcelable<Want>());
        if (!want) {
            HILOG_ERROR("%{public}s fail, ReadParcelable<FormReqInfo> failed", __func__);
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }

        sptr<IRemoteObject> client = data.ReadRemoteObject();
        if (client == nullptr) {
            HILOG_ERROR("%{public}s, failed to get remote object.", __func__);
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }

        int32_t result = NotifyFormsDelete(formIds, *want, client);
        reply.WriteInt32(result);
        return result;
    }

    return ERR_INVALID_DATA;
}
/**
 * @brief handle NotifyFormUpdate message.
 * @param data input param.
 * @param reply output param.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormProviderStub::HandleNotifyFormUpdate(MessageParcel &data, MessageParcel &reply)
{
    int64_t formId = data.ReadInt64();

    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        HILOG_ERROR("%{public}s, failed to ReadParcelable<Want>", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    sptr<IRemoteObject> client = data.ReadRemoteObject();
    if (client == nullptr) {
        HILOG_ERROR("%{public}s, failed to get remote object.", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    int32_t result = NotifyFormUpdate(formId, *want, client);
    reply.WriteInt32(result);
    return result;
}

/**
 * @brief handle EventNotify message.
 * @param data input param.
 * @param reply output param.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormProviderStub::HandleEventNotify(MessageParcel &data, MessageParcel &reply)
{
    std::vector<int64_t> formIds;
    bool ret = data.ReadInt64Vector(&formIds);
    if (ret) {
        int32_t formVisibleType = data.ReadInt32();

        std::unique_ptr<Want> want(data.ReadParcelable<Want>());
        if (!want) {
            HILOG_ERROR("%{public}s, failed to ReadParcelable<Want>", __func__);
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }

        sptr<IRemoteObject> client = data.ReadRemoteObject();
        if (client == nullptr) {
            HILOG_ERROR("%{public}s, failed to get remote object.", __func__);
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }

        int32_t result = EventNotify(formIds, formVisibleType, *want, client);
        reply.WriteInt32(result);
        return result;
    }

    return ERR_INVALID_DATA;
}

/**
 * @brief handle NotifyFormCastTempForm message.
 * @param data input param.
 * @param reply output param.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormProviderStub::HandleNotifyFormCastTempForm(MessageParcel &data, MessageParcel &reply)
{
    int64_t formId = data.ReadInt64();

    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        HILOG_ERROR("%{public}s fail, ReadParcelable<Want> failed", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    sptr<IRemoteObject> client = data.ReadRemoteObject();
    if (client == nullptr) {
        HILOG_ERROR("%{public}s, failed to get remote object.", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    int32_t result = NotifyFormCastTempForm(formId, *want, client);
    reply.WriteInt32(result);
    return result;
}
/**
 * @brief handle NotifyFormCastTempForm message.
 * @param data input param.
 * @param reply output param.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormProviderStub::HandleFireFormEvent(MessageParcel &data, MessageParcel &reply)
{
    int64_t formId = data.ReadInt64();
    std::string message = data.ReadString();
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        HILOG_ERROR("%{public}s, failed to get want.", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    sptr<IRemoteObject> client = data.ReadRemoteObject();
    if (client == nullptr) {
        HILOG_ERROR("%{public}s, failed to get remote object.", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    int32_t result = FireFormEvent(formId, message, *want, client);
    reply.WriteInt32(result);
    return result;
}
/**
 * @brief handle AcquireState message.
 * @param data input param.
 * @param reply output param.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormProviderStub::HandleAcquireState(MessageParcel &data, MessageParcel &reply)
{
    std::unique_ptr<Want> wantArg(data.ReadParcelable<Want>());
    if (!wantArg) {
        HILOG_ERROR("%{public}s fail, ReadParcelable<Want> failed", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    std::string provider = data.ReadString();
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        HILOG_ERROR("%{public}s fail, ReadParcelable<Want> failed", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    sptr<IRemoteObject> client = data.ReadRemoteObject();
    if (client == nullptr) {
        HILOG_ERROR("%{public}s, failed to get remote object.", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    int32_t result = AcquireState(*wantArg, provider, *want, client);
    reply.WriteInt32(result);
    return result;
}

int32_t FormProviderStub::HandleAcquireShareFormData(MessageParcel &data, MessageParcel &reply)
{
    auto formId = data.ReadInt64();
    auto remoteDeviceId = data.ReadString();
    auto remoteObj = data.ReadRemoteObject();
    if (remoteObj == nullptr) {
        HILOG_ERROR("failed to get remote object.");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    auto requestCode = data.ReadInt64();
    auto result = AcquireShareFormData(formId, remoteDeviceId, remoteObj, requestCode);
    if (!reply.WriteInt32(result)) {
        HILOG_ERROR("failed to Write result.");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    return ERR_OK;
}
}  // namespace AppExecFwk
}  // namespace OHOS