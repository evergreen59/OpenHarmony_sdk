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

#include "form_supply_proxy.h"
#include "appexecfwk_errors.h"
#include "form_constants.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
    static constexpr int32_t MAX_ALLOW_SIZE = 8 * 1024;
}
/**
 * @brief Send form binding data from form provider to fms.
 * @param providerFormInfo Form binding data.
 * @param want input data.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormSupplyProxy::OnAcquire(const FormProviderInfo &formInfo, const Want& want)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s, failed to WriteInterfaceToken", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!data.WriteParcelable(&want)) {
        HILOG_ERROR("%{public}s, failed to write want", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    if (want.GetIntParam(Constants::PROVIDER_FLAG, ERR_OK) == ERR_OK) {
        if (!data.WriteParcelable(&formInfo)) {
            HILOG_ERROR("%{public}s, failed to write formInfo", __func__);
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }

    MessageParcel reply;
    MessageOption option;
    int error = Remote()->SendRequest(
        static_cast<uint32_t>(IFormSupply::Message::TRANSACTION_FORM_ACQUIRED),
        data,
        reply,
        option);
    if (error != ERR_OK) {
        HILOG_ERROR("%{public}s, failed to SendRequest: %{public}d", __func__, error);
    }
    return error;
}


/**
 * @brief Send other event  to fms.
 * @param want input data.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormSupplyProxy::OnEventHandle(const Want& want)
{
    MessageParcel data;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s, failed to write interface token", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    if (!data.WriteParcelable(&want)) {
        HILOG_ERROR("%{public}s, failed to write want", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int error = Remote()->SendRequest(
        static_cast<uint32_t>(IFormSupply::Message::TRANSACTION_EVENT_HANDLE),
        data,
        reply,
        option);
    if (error != ERR_OK) {
        HILOG_ERROR("%{public}s, failed to SendRequest: %{public}d", __func__, error);
    }
    return error;
}

/**
 * @brief Accept form state from form provider.
 * @param state Form state.
 * @param provider provider info.
 * @param wantArg The want of onAcquireFormState.
 * @param want input data.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormSupplyProxy::OnAcquireStateResult(FormState state, const std::string &provider, const Want &wantArg,
                                          const Want &want)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s, failed to WriteInterfaceToken", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!data.WriteInt32((int32_t) state)) {
        HILOG_ERROR("%{public}s, failed to write form state", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!data.WriteString(provider)) {
        HILOG_ERROR("%{public}s, failed to provider", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!data.WriteParcelable(&wantArg)) {
        HILOG_ERROR("%{public}s, failed to write want", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!data.WriteParcelable(&want)) {
        HILOG_ERROR("%{public}s, failed to write want", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int error = Remote()->SendRequest(
        static_cast<uint32_t>(IFormSupply::Message::TRANSACTION_FORM_STATE_ACQUIRED),
        data,
        reply,
        option);
    if (error != ERR_OK) {
        HILOG_ERROR("%{public}s, failed to SendRequest: %{public}d", __func__, error);
    }
    return error;
}

template<typename T>
int  FormSupplyProxy::GetParcelableInfos(MessageParcel &reply, std::vector<T> &parcelableInfos)
{
    int32_t infoSize = reply.ReadInt32();
    if (infoSize < 0 || infoSize > MAX_ALLOW_SIZE) {
        HILOG_ERROR("%{public}s invalid size: %{public}d", __func__, infoSize);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    for (int32_t i = 0; i < infoSize; i++) {
        std::unique_ptr<T> info(reply.ReadParcelable<T>());
        if (!info) {
            HILOG_ERROR("%{public}s, failed to ReadParcelable<T>", __func__);
            return ERR_INVALID_VALUE;
        }
        parcelableInfos.emplace_back(*info);
    }
    HILOG_INFO("get parcelable infos success");
    return ERR_OK;
}

bool  FormSupplyProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(FormSupplyProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s, failed to write interface token failed", __func__);
        return false;
    }
    return true;
}

void FormSupplyProxy::OnShareAcquire(int64_t formId, const std::string &remoteDeviceId,
    const AAFwk::WantParams &wantParams, int64_t requestCode, const bool &result)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("failed to WriteInterfaceToken.");
        return;
    }
    if (!data.WriteInt64(formId)) {
        HILOG_ERROR("failed to write form formId.");
        return;
    }
    if (!data.WriteString(remoteDeviceId)) {
        HILOG_ERROR("failed to write form remoteDeviceId.");
        return;
    }
    if (!data.WriteParcelable(&wantParams)) {
        HILOG_ERROR("failed to write form wantParams.");
        return;
    }
    if (!data.WriteInt64(requestCode)) {
        HILOG_ERROR("failed to write form requestCode.");
        return;
    }
    if (!data.WriteBool(result)) {
        HILOG_ERROR("failed to write form result.");
        return;
    }
    auto error = Remote()->SendRequest(
        static_cast<uint32_t>(IFormSupply::Message::TRANSACTION_FORM_SHARE_ACQUIRED),
        data,
        reply,
        option);
    if (error != ERR_OK) {
        HILOG_ERROR("failed to SendRequest: %{public}d", error);
    }
}

int32_t FormSupplyProxy::OnRenderTaskDone(int64_t formId, const Want &want)
{
    MessageParcel data;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s, failed to write interface token", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    if (!data.WriteInt64(formId)) {
        HILOG_ERROR("%{public}s, failed to write formId", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    if (!data.WriteParcelable(&want)) {
        HILOG_ERROR("%{public}s, failed to write want", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int error = Remote()->SendRequest(
        static_cast<uint32_t>(IFormSupply::Message::TRANSACTION_FORM_RENDER_TASK_DONE),
        data,
        reply,
        option);
    if (error != ERR_OK) {
        HILOG_ERROR("%{public}s, failed to SendRequest: %{public}d", __func__, error);
    }
    return error;
}

int32_t FormSupplyProxy::OnStopRenderingTaskDone(int64_t formId, const Want &want)
{
    MessageParcel data;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s, failed to write interface token", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    if (!data.WriteInt64(formId)) {
        HILOG_ERROR("%{public}s, failed to write formId", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    if (!data.WriteParcelable(&want)) {
        HILOG_ERROR("%{public}s, failed to write want", __func__);
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int error = Remote()->SendRequest(
        static_cast<uint32_t>(IFormSupply::Message::TRANSACTION_FORM_STOP_RENDERING_TASK_DONE),
        data,
        reply,
        option);
    if (error != ERR_OK) {
        HILOG_ERROR("%{public}s, failed to SendRequest: %{public}d", __func__, error);
    }
    return error;
}
}  // namespace AppExecFwk
}  // namespace OHOS
