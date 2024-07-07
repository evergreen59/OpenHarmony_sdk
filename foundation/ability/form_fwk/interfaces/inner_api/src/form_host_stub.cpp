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

#include "form_host_stub.h"

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
FormHostStub::FormHostStub()
{
    memberFuncMap_[static_cast<uint32_t>(IFormHost::Message::FORM_HOST_ON_ACQUIRED)] =
        &FormHostStub::HandleAcquired;
    memberFuncMap_[static_cast<uint32_t>(IFormHost::Message::FORM_HOST_ON_UPDATE)] =
        &FormHostStub::HandleOnUpdate;
    memberFuncMap_[static_cast<uint32_t>(IFormHost::Message::FORM_HOST_ON_UNINSTALL)] =
        &FormHostStub::HandleOnUninstall;
    memberFuncMap_[static_cast<uint32_t>(IFormHost::Message::FORM_HOST_ON_ACQUIRE_FORM_STATE)] =
        &FormHostStub::HandleOnAcquireState;
    memberFuncMap_[static_cast<uint32_t>(IFormHost::Message::FORM_HOST_ON_SHARE_FORM_RESPONSE)] =
        &FormHostStub::HandleOnShareFormResponse;
    memberFuncMap_[static_cast<uint32_t>(IFormHost::Message::FORM_HOST_ON_ERROR)] =
        &FormHostStub::HandleOnError;
}

FormHostStub::~FormHostStub()
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
int FormHostStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOG_INFO("FormHostStub::OnReceived, code = %{public}u, flags= %{public}d.", code, option.GetFlags());
    std::u16string descriptor = FormHostStub::GetDescriptor();
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
 * @brief handle OnAcquired event.
 * @param data input param.
 * @param reply output param.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormHostStub::HandleAcquired(MessageParcel &data, MessageParcel &reply)
{
    std::unique_ptr<FormJsInfo> formInfo(data.ReadParcelable<FormJsInfo>());
    if (!formInfo) {
        HILOG_ERROR("%{public}s, failed to ReadParcelable<FormJsInfo>", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    sptr<IRemoteObject> token = nullptr;
    if (data.ReadBool()) {
        token = data.ReadRemoteObject();
    }

    OnAcquired(*formInfo, token);
    reply.WriteInt32(ERR_OK);
    return ERR_OK;
}
/**
 * @brief handle OnUpdate event.
 * @param data input param.
 * @param reply output param.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormHostStub::HandleOnUpdate(MessageParcel &data, MessageParcel &reply)
{
    std::unique_ptr<FormJsInfo> formInfo(data.ReadParcelable<FormJsInfo>());
    if (!formInfo) {
        HILOG_ERROR("%{public}s, failed to ReadParcelable<FormJsInfo>", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    OnUpdate(*formInfo);
    reply.WriteInt32(ERR_OK);
    return ERR_OK;
}

/**
 * @brief handle OnUnInstall event.
 * @param data input param.
 * @param reply output param.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormHostStub::HandleOnUninstall(MessageParcel &data, MessageParcel &reply)
{
    std::vector<int64_t> formIds;
    bool ret = data.ReadInt64Vector(&formIds);
    if (ret) {
        OnUninstall(formIds);
        reply.WriteInt32(ERR_OK);
        return ERR_OK;
    }
    return ERR_APPEXECFWK_PARCEL_ERROR;
}

/**
 * @brief handle OnAcquireState message.
 * @param data input param.
 * @param reply output param.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormHostStub::HandleOnAcquireState(MessageParcel &data, MessageParcel &reply)
{
    auto state = (FormState) data.ReadInt32();

    std::unique_ptr<AAFwk::Want> want(data.ReadParcelable<AAFwk::Want>());
    if (!want) {
        HILOG_ERROR("%{public}s, failed to ReadParcelable<Want>", __func__);
        reply.WriteInt32(ERR_APPEXECFWK_PARCEL_ERROR);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    OnAcquireState(state, *want);
    reply.WriteInt32(ERR_OK);
    return ERR_OK;
}

int32_t FormHostStub::HandleOnShareFormResponse(MessageParcel &data, MessageParcel &reply)
{
    auto requestCode = data.ReadInt64();
    auto result = data.ReadInt32();

    OnShareFormResponse(requestCode, result);
    reply.WriteInt32(ERR_OK);
    return ERR_OK;
}

int32_t FormHostStub::HandleOnError(MessageParcel &data, MessageParcel &reply)
{
    int32_t errorCode = data.ReadInt32();
    std::string errorMsg = Str16ToStr8(data.ReadString16());

    OnError(errorCode, errorMsg);
    reply.WriteInt32(ERR_OK);
    return ERR_OK;
}
}  // namespace AppExecFwk
}  // namespace OHOS