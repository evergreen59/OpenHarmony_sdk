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

#include "form_supply_stub.h"

#include "appexecfwk_errors.h"
#include "form_constants.h"
#include "form_mgr_errors.h"
#include "form_supply_stub.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iremote_object.h"

namespace OHOS {
namespace AppExecFwk {
FormSupplyStub::FormSupplyStub()
{
    memberFuncMap_[static_cast<uint32_t>(IFormSupply::Message::TRANSACTION_FORM_ACQUIRED)] =
        &FormSupplyStub::HandleOnAcquire;
    memberFuncMap_[static_cast<uint32_t>(IFormSupply::Message::TRANSACTION_EVENT_HANDLE)] =
        &FormSupplyStub::HandleOnEventHandle;
    memberFuncMap_[static_cast<uint32_t>(IFormSupply::Message::TRANSACTION_FORM_STATE_ACQUIRED)] =
        &FormSupplyStub::HandleOnAcquireStateResult;
    memberFuncMap_[static_cast<uint32_t>(IFormSupply::Message::TRANSACTION_FORM_SHARE_ACQUIRED)] =
        &FormSupplyStub::HandleOnShareAcquire;
    memberFuncMap_[static_cast<uint32_t>(IFormSupply::Message::TRANSACTION_FORM_RENDER_TASK_DONE)] =
        &FormSupplyStub::HandleOnRenderTaskDone;
    memberFuncMap_[static_cast<uint32_t>(IFormSupply::Message::TRANSACTION_FORM_STOP_RENDERING_TASK_DONE)] =
        &FormSupplyStub::HandleOnStopRenderingTaskDone;
}

FormSupplyStub::~FormSupplyStub()
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
int FormSupplyStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOG_INFO("FormSupplyStub::OnReceived, code = %{public}u, flags= %{public}d.", code, option.GetFlags());
    std::u16string descriptor = FormSupplyStub::GetDescriptor();
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
/**
 * @brief handle OnAcquire message.
 * @param data input param.
 * @param reply output param.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormSupplyStub::HandleOnAcquire(MessageParcel &data, MessageParcel &reply)
{
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        HILOG_ERROR("%{public}s, failed to ReadParcelable<Want>", __func__);
        reply.WriteInt32(ERR_APPEXECFWK_PARCEL_ERROR);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    int errCode = ERR_OK;
    do {
        errCode = want->GetIntParam(Constants::PROVIDER_FLAG, ERR_OK);
        if (errCode != ERR_OK) {
            HILOG_ERROR("%{public}s, provider error", __func__);
            break;
        }
        std::unique_ptr<FormProviderInfo> formInfo(data.ReadParcelable<FormProviderInfo>());
        if (formInfo == nullptr) {
            HILOG_ERROR("%{public}s, failed to ReadParcelable<FormProviderInfo>", __func__);
            errCode = ERR_APPEXECFWK_PARCEL_ERROR;
            break;
        }
        int32_t result = OnAcquire(*formInfo, *want);
        reply.WriteInt32(result);
        return result;
    } while (false);

    FormProviderInfo formProviderInfo;
    want->SetParam(Constants::PROVIDER_FLAG, errCode);
    OnAcquire(formProviderInfo, *want);
    reply.WriteInt32(errCode);
    return errCode;
}
/**
 * @brief handle OnEventHandle message.
 * @param data input param.
 * @param reply output param.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormSupplyStub::HandleOnEventHandle(MessageParcel &data, MessageParcel &reply)
{
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        HILOG_ERROR("%{public}s, failed to ReadParcelable<Want>", __func__);
        reply.WriteInt32(ERR_APPEXECFWK_PARCEL_ERROR);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    int32_t result = OnEventHandle(*want);
    reply.WriteInt32(result);
    return result;
}

/**
 * @brief handle OnAcquireStateResult message.
 * @param data input param.
 * @param reply output param.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormSupplyStub::HandleOnAcquireStateResult(MessageParcel &data, MessageParcel &reply)
{
    auto state = (FormState) data.ReadInt32();
    std::string provider = data.ReadString();
    std::unique_ptr<Want> wantArg(data.ReadParcelable<Want>());
    if (!wantArg) {
        HILOG_ERROR("%{public}s, failed to ReadParcelable<Want>", __func__);
        reply.WriteInt32(ERR_APPEXECFWK_PARCEL_ERROR);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        HILOG_ERROR("%{public}s, failed to ReadParcelable<Want>", __func__);
        reply.WriteInt32(ERR_APPEXECFWK_PARCEL_ERROR);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    int32_t result = OnAcquireStateResult(state, provider, *wantArg, *want);
    reply.WriteInt32(result);
    return result;
}

int32_t FormSupplyStub::HandleOnShareAcquire(MessageParcel &data, MessageParcel &reply)
{
    auto formId = data.ReadInt64();
    if (formId <= 0) {
        HILOG_ERROR("failed to ReadInt64<formId>");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    auto remoteDeviceId = data.ReadString();
    if (remoteDeviceId.empty()) {
        HILOG_ERROR("failed to ReadString<DeviceId>");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    std::shared_ptr<AAFwk::WantParams> wantParams(data.ReadParcelable<AAFwk::WantParams>());
    if (wantParams == nullptr) {
        HILOG_ERROR("failed to ReadParcelable<wantParams>");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    auto requestCode = data.ReadInt64();
    if (requestCode <= 0) {
        HILOG_ERROR("failed to ReadInt64<requestCode>");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    auto result = data.ReadBool();
    OnShareAcquire(formId, remoteDeviceId, *wantParams, requestCode, result);
    return ERR_OK;
}

int32_t FormSupplyStub::HandleOnRenderTaskDone(MessageParcel &data, MessageParcel &reply)
{
    auto formId = data.ReadInt64();
    if (formId <= 0) {
        HILOG_ERROR("failed to ReadInt64<formId>");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        HILOG_ERROR("%{public}s, failed to ReadParcelable<Want>", __func__);
        reply.WriteInt32(ERR_APPEXECFWK_PARCEL_ERROR);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    int32_t result = OnRenderTaskDone(formId, *want);
    reply.WriteInt32(result);
    return result;
}

int32_t FormSupplyStub::HandleOnStopRenderingTaskDone(MessageParcel &data, MessageParcel &reply)
{
    auto formId = data.ReadInt64();
    if (formId <= 0) {
        HILOG_ERROR("failed to ReadInt64<formId>");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        HILOG_ERROR("%{public}s, failed to ReadParcelable<Want>", __func__);
        reply.WriteInt32(ERR_APPEXECFWK_PARCEL_ERROR);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    int32_t result = OnStopRenderingTaskDone(formId, *want);
    reply.WriteInt32(result);
    return result;
}
}  // namespace AppExecFwk
}  // namespace OHOS