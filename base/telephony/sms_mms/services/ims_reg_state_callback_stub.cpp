/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "ims_reg_state_callback_stub.h"

#include "sms_network_policy_manager.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
ImsRegStateCallbackStub::ImsRegStateCallbackStub(const std::shared_ptr<AppExecFwk::EventHandler> handle)
    : handle_(handle)
{}

int32_t ImsRegStateCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        TELEPHONY_LOGE("descriptor checked fail");
        return TELEPHONY_ERR_DESCRIPTOR_MISMATCH;
    }
    TELEPHONY_LOGI("Code is %{public}d", code);
    int32_t slotId = data.ReadInt32();
    int32_t imsRegState = data.ReadInt32();
    int32_t imsRegTech = data.ReadInt32();
    const ImsRegInfo info = { static_cast<ImsRegState>(imsRegState), static_cast<ImsRegTech>(imsRegTech) };
    return OnImsRegInfoChanged(slotId, static_cast<ImsServiceType>(code), info);
}

int32_t ImsRegStateCallbackStub::OnImsRegInfoChanged(int32_t slotId, ImsServiceType imsSrvType, const ImsRegInfo &info)
{
    TELEPHONY_LOGI("slotId is %{public}d, imsServiceType is %{public}d, imsRegisterState is %{public}d,"
                   "imsRegisterTech is %{public}d",
        slotId, imsSrvType, info.imsRegState, info.imsRegTech);
    if (handle_ != nullptr) {
        uint32_t item = NotificationType::NOTIFICATION_TYPE_IMS;
        handle_->SendEvent(item);
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS