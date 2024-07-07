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

#include "form_ams_helper.h"

#include "ability_manager_interface.h"
#include "form_mgr_errors.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "in_process_call_wrapper.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppExecFwk {
const int FORM_DISCONNECT_DELAY_TIME = 5000; // ms
FormAmsHelper::FormAmsHelper()
{}

FormAmsHelper::~FormAmsHelper()
{}

/**
 * @brief acquire a form ability manager, if it not existed,
 * @return returns the ability manager ipc object, or nullptr for failed.
 */
sptr<AAFwk::IAbilityManager> FormAmsHelper::GetAbilityManager()
{
    if (abilityManager_ == nullptr) {
        sptr<ISystemAbilityManager> systemManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (systemManager == nullptr) {
            HILOG_ERROR("%{public}s:fail to get registry", __func__);
            return nullptr;
        }
        sptr<IRemoteObject> remoteObject = systemManager->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
        if (remoteObject == nullptr) {
            HILOG_ERROR("%{public}s:fail to connect AbilityMgrService", __func__);
            return nullptr;
        }
        HILOG_DEBUG("connect AbilityMgrService success");

        abilityManager_ = iface_cast<AAFwk::IAbilityManager>(remoteObject);
    }

    return abilityManager_;
}

/**
 * @brief ConnectAbility, connect session with service ability.
 * @param want Special want for service type's ability.
 * @param connect Callback used to notify caller the result of connecting or disconnecting.
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormAmsHelper::ConnectServiceAbility(
    const Want &want, const sptr<AAFwk::IAbilityConnection> &connect)
{
    HILOG_INFO("%{public}s called.", __func__);
    sptr<AAFwk::IAbilityManager> ams = GetAbilityManager();
    if (ams == nullptr) {
        HILOG_ERROR("%{public}s:ability service not connect", __func__);
        return ERR_APPEXECFWK_FORM_BIND_PROVIDER_FAILED;
    }
    return IN_PROCESS_CALL(ams->ConnectAbility(want, connect, nullptr));
}
/**
 * @brief Disconnect ability, disconnect session with service ability.
 * @param want Special want for service type's ability.
 * @param connect Callback used to notify caller the result of connecting or disconnecting.
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormAmsHelper::DisconnectServiceAbility(const sptr<AAFwk::IAbilityConnection> &connect)
{
    HILOG_INFO("%{public}s called.", __func__);
    sptr<AAFwk::IAbilityManager> ams = GetAbilityManager();
    if (ams == nullptr) {
        HILOG_ERROR("%{public}s:ability service not connect", __func__);
        return ERR_APPEXECFWK_FORM_BIND_PROVIDER_FAILED;
    }
    return IN_PROCESS_CALL(ams->DisconnectAbility(connect));
}
/**
 * @brief Disconnect ability delay, disconnect session with service ability.
 * @param want Special want for service type's ability.
 * @param connect Callback used to notify caller the result of connecting or disconnecting.
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormAmsHelper::DisconnectServiceAbilityDelay(const sptr<AAFwk::IAbilityConnection> &connect)
{
    if (eventHandler_ == nullptr) {
        HILOG_ERROR("%{public}s fail, eventhandler invalidate", __func__);
        return ERR_INVALID_OPERATION;
    }
    auto disConnectAbilityFunc = [connect]() {
        FormAmsHelper::GetInstance().DisconnectAbilityTask(connect);
    };
    if (!eventHandler_->PostTask(disConnectAbilityFunc, FORM_DISCONNECT_DELAY_TIME)) {
        HILOG_ERROR("%{public}s, failed to disconnect ability", __func__);
        return ERR_APPEXECFWK_FORM_BIND_PROVIDER_FAILED;
    }
    return ERR_OK;
}
/**
 * @brief Add the ability manager instance for debug.
 * @param abilityManager the ability manager ipc object.
 */
void FormAmsHelper::SetAbilityManager(const sptr<AAFwk::IAbilityManager> &abilityManager)
{
    abilityManager_ = abilityManager;
}

/**
 * @brief Disconnect ability task, disconnect session with service ability.
 * @param want Special want for service type's ability.
 * @param connect Callback used to notify caller the result of connecting or disconnecting.
 */
void FormAmsHelper::DisconnectAbilityTask(const sptr<AAFwk::IAbilityConnection> &connect)
{
    sptr<AAFwk::IAbilityManager> ams = GetAbilityManager();
    if (ams == nullptr) {
        HILOG_ERROR("%{public}s, ability service not connect", __func__);
        return;
    }
    IN_PROCESS_CALL_WITHOUT_RET(ams->DisconnectAbility(connect));
}

ErrCode FormAmsHelper::StartAbility(const Want &want, int32_t userId)
{
    sptr<AAFwk::IAbilityManager> ams = GetAbilityManager();
    if (ams == nullptr) {
        HILOG_ERROR("%{public}s, ability service not connect", __func__);
        return ERR_APPEXECFWK_FORM_BIND_PROVIDER_FAILED;
    }
    return IN_PROCESS_CALL(ams->StartAbility(want, userId));
}
}  // namespace AppExecFwk
}  // namespace OHOS
