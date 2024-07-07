
/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "form_share_connection.h"
#include "form_constants.h"
#include "form_mgr_errors.h"
#include "form_supply_callback.h"
#include "form_task_mgr.h"
#include "hilog_wrapper.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
FormShareConnection::FormShareConnection(int64_t formId, const std::string &bundleName,
    const std::string &abilityName, const std::string &deviceId, int64_t formShareRequestCode)
    :formId_(formId), remoteDeviceId_(deviceId), formShareRequestCode_(formShareRequestCode)
{
    SetProviderKey(bundleName, abilityName);
}

void FormShareConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    HILOG_DEBUG("%{public}s called.", __func__);
    if (resultCode != ERR_OK) {
        HILOG_ERROR("%{public}s, abilityName:%{public}s, resultCode:%{public}d",
            __func__, element.GetAbilityName().c_str(), resultCode);
        FormTaskMgr::GetInstance().PostFormShareSendResponse(formShareRequestCode_,
            ERR_APPEXECFWK_FORM_BIND_PROVIDER_FAILED);
        return;
    }

    FormSupplyCallback::GetInstance()->AddConnection(this);
    Want want;
    want.SetParam(Constants::FORM_CONNECT_ID, this->GetConnectId());
    want.SetParam(Constants::FORM_SHARE_REQUEST_CODE, formShareRequestCode_);

    FormTaskMgr::GetInstance().PostShareAcquireTask(formId_, remoteDeviceId_, want, remoteObject);
}
} // namespace AppExecFwk
} // namespace OHOS
