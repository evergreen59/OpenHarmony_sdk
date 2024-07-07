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

#include "form_mgr.h"

#include "appexecfwk_errors.h"
#include "form_caller_mgr.h"
#include "form_errors.h"
#include "form_mgr_errors.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppExecFwk {
FormMgr::FormMgr() {}
FormMgr::~FormMgr()
{
    if (remoteProxy_ != nullptr) {
        auto remoteObject = remoteProxy_->AsObject();
        if (remoteObject != nullptr) {
            remoteObject->RemoveDeathRecipient(deathRecipient_);
        }
    }
}

/**
 * @brief Get the error message by error code.
 * @param errorCode the error code return form fms.
 * @return Returns the error message detail.
 */
std::string FormMgr::GetErrorMsg(int errorCode)
{
    return "unknown error";
}

/**
 * @brief Add form with want, send want to form manager service.
 * @param formId The Id of the forms to add.
 * @param want The want of the form to add.
 * @param callerToken Caller ability token.
 * @param formInfo Form info.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormMgr::AddForm(
    const int64_t formId,
    const Want &want,
    const sptr<IRemoteObject> &callerToken,
    FormJsInfo &formInfo)
{
    HILOG_INFO("%{public}s called.", __func__);
    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    return remoteProxy_->AddForm(formId, want, callerToken, formInfo);
}

/**
 * @brief Delete forms with formIds, send formIds to form manager service.
 * @param formId The Id of the forms to delete.
 * @param callerToken Caller ability token.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormMgr::DeleteForm(const int64_t formId, const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("%{public}s called.", __func__);
    // check fms recover status
    if (FormMgr::GetRecoverStatus() == Constants::IN_RECOVERING) {
        HILOG_ERROR("%{public}s error, form is in recover status, can't do action on form.", __func__);
        return ERR_APPEXECFWK_FORM_SERVER_STATUS_ERR;
    }
    // check formId
    if (formId <= 0) {
        HILOG_ERROR("%{public}s error, the passed in formId can't be negative or zero.", __func__);
        return ERR_APPEXECFWK_FORM_INVALID_FORM_ID;
    }

    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    FormCallerMgr::GetInstance().RemoveFormHostCaller(formId);
    return remoteProxy_->DeleteForm(formId, callerToken);
}

/**
 * @brief Stop rendering form.
 * @param formId The Id of the forms to delete.
 * @param compId The compId of the forms to delete.
 * @return Returns ERR_OK on success, others on failure.
*/
int FormMgr::StopRenderingForm(const int64_t formId, const std::string &compId)
{
    HILOG_INFO("%{public}s called.", __func__);
    // check fms recover status
    if (FormMgr::GetRecoverStatus() == Constants::IN_RECOVERING) {
        HILOG_ERROR("%{public}s error, form is in recover status, can't do action on form.", __func__);
        return ERR_APPEXECFWK_FORM_SERVER_STATUS_ERR;
    }
    // check formId
    if (formId <= 0 || compId.empty()) {
        HILOG_ERROR("%{public}s error, the formId is invalid or compId is empty.", __func__);
        return ERR_APPEXECFWK_FORM_INVALID_FORM_ID;
    }

    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }

    return remoteProxy_->StopRenderingForm(formId, compId);
}

/**
 * @brief Release forms with formIds, send formIds to form manager service.
 * @param formId The Id of the forms to release.
 * @param callerToken Caller ability token.
 * @param delCache Delete Cache or not.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormMgr::ReleaseForm(const int64_t formId, const sptr<IRemoteObject> &callerToken, const bool delCache)
{
    HILOG_INFO("%{public}s called.", __func__);
    // check fms recover status
    if (FormMgr::GetRecoverStatus() == Constants::IN_RECOVERING) {
        HILOG_ERROR("%{public}s error, form is in recover status, can't do action on form.", __func__);
        return ERR_APPEXECFWK_FORM_SERVER_STATUS_ERR;
    }
    // check formId
    if (formId <= 0) {
        HILOG_ERROR("%{public}s error, the passed in formId can't be negative or zero.", __func__);
        return ERR_APPEXECFWK_FORM_INVALID_FORM_ID;
    }

    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    FormCallerMgr::GetInstance().RemoveFormHostCaller(formId);
    return remoteProxy_->ReleaseForm(formId, callerToken, delCache);
}

/**
 * @brief Update form with formId, send formId to form manager service.
 * @param formId The Id of the form to update.
 * @param formBindingData Form binding data.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormMgr::UpdateForm(const int64_t formId, const FormProviderData &formBindingData)
{
    HILOG_INFO("%{public}s called.", __func__);

    if (FormMgr::GetRecoverStatus() == Constants::IN_RECOVERING) {
        HILOG_ERROR("%{public}s error, form is in recover status, can't do action on form.", __func__);
        return ERR_APPEXECFWK_FORM_SERVER_STATUS_ERR;
    }

    if (formId <= 0) {
        HILOG_ERROR("%{public}s error, the passed in formId can't be negative or zero.", __func__);
        return ERR_APPEXECFWK_FORM_INVALID_FORM_ID;
    }

    // check formBindingData
    if (formBindingData.GetDataString().empty()) {
        HILOG_ERROR("%{public}s error, the formProviderData is null.", __func__);
        return ERR_APPEXECFWK_FORM_PROVIDER_DATA_EMPTY;
    }

    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    auto hostCaller = FormCallerMgr::GetInstance().GetFormHostCaller(formId);
    if (hostCaller != nullptr) {
        hostCaller->UpdateForm(formId, formBindingData);
    } else {
        std::vector<std::shared_ptr<FormProviderCaller>> formProviderCallers;
        FormCallerMgr::GetInstance().GetFormProviderCaller(formId, formProviderCallers);
        for (const auto &formProviderCaller : formProviderCallers) {
            formProviderCaller->UpdateForm(formId, formBindingData);
        }
    }
    return remoteProxy_->UpdateForm(formId, formBindingData);
}

/**
 * @brief Notify the form service that the form user's lifecycle is updated.
 *
 * This should be called when form user request form.
 *
 * @param formId Indicates the unique id of form.
 * @param callerToken Indicates the callback remote object of specified form user.
 * @param want information passed to supplier.
 * @return Returns true if execute success, false otherwise.
 */
int FormMgr::RequestForm(const int64_t formId, const sptr<IRemoteObject> &callerToken, const Want &want)
{
    HILOG_INFO("%{public}s called.", __func__);
    if (formId <= 0) {
        HILOG_ERROR("%{public}s error, The passed formid is invalid. Its value must be larger than 0.", __func__);
        return ERR_APPEXECFWK_FORM_INVALID_FORM_ID;
    }
    if (GetRecoverStatus() == Constants::IN_RECOVERING) {
        HILOG_ERROR("%{public}s error, form is in recover status, can't do action on form.", __func__);
        return ERR_APPEXECFWK_FORM_SERVER_STATUS_ERR;
    }
    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    auto hostCaller = FormCallerMgr::GetInstance().GetFormHostCaller(formId);
    if (hostCaller != nullptr) {
        HILOG_DEBUG("request form by host caller");
        return hostCaller->RequestForm(formId, callerToken, want);
    }
    ErrCode resultCode = remoteProxy_->RequestForm(formId, callerToken, want);
    if (resultCode != ERR_OK) {
        HILOG_ERROR(
            "%{public}s error, failed to notify the form service that the form user's lifecycle is updated, error "
            "code is %{public}d.",
            __func__,
            resultCode);
    }
    return resultCode;
}

/**
 * @brief Form visible/invisible notify, send formIds to form manager service.
 * @param formIds The Id list of the forms to notify.
 * @param callerToken Caller ability token.
 * @param formVisibleType The form visible type, including FORM_VISIBLE and FORM_INVISIBLE.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormMgr::NotifyWhetherVisibleForms(
    const std::vector<int64_t> &formIds,
    const sptr<IRemoteObject> &callerToken,
    const int32_t formVisibleType)
{
    HILOG_INFO("%{public}s called.", __func__);

    if (formIds.empty() || formIds.size() > Constants::MAX_VISIBLE_NOTIFY_LIST) {
        HILOG_ERROR("%{public}s, formIds is empty or exceed 32.", __func__);
        return ERR_APPEXECFWK_FORM_INVALID_FORM_ID;
    }

    if (GetRecoverStatus() == Constants::IN_RECOVERING) {
        HILOG_ERROR("%{public}s error, form is in recover status, can't do action on form.", __func__);
        return ERR_APPEXECFWK_FORM_SERVER_STATUS_ERR;
    }

    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    // IPC entry
    ErrCode resultCode = remoteProxy_->NotifyWhetherVisibleForms(formIds, callerToken, formVisibleType);
    if (resultCode != ERR_OK) {
        HILOG_ERROR("%{public}s error, internal error occurs, error code:%{public}d.", __func__, resultCode);
    }
    return resultCode;
}

/**
 * @brief temp form to normal form.
 * @param formId The Id of the form.
 * @param callerToken Caller ability token.
 * @return None.
 */
int FormMgr::CastTempForm(const int64_t formId, const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("%{public}s called.", __func__);
    if (formId <= 0) {
        HILOG_ERROR("%{public}s error, passing in form id can't be negative.", __func__);
        return ERR_APPEXECFWK_FORM_INVALID_FORM_ID;
    }

    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    return remoteProxy_->CastTempForm(formId, callerToken);
}

/**
 * @brief Dump all of form storage infos.
 * @param formInfos All of form storage infos.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormMgr::DumpStorageFormInfos(std::string &formInfos)
{
    HILOG_INFO("%{public}s called.", __func__);
    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    return remoteProxy_->DumpStorageFormInfos(formInfos);
}
/**
 * @brief Dump form info by a bundle name.
 * @param bundleName The bundle name of form provider.
 * @param formInfos Form infos.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormMgr::DumpFormInfoByBundleName(const std::string bundleName, std::string &formInfos)
{
    HILOG_INFO("%{public}s called.", __func__);
    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    return remoteProxy_->DumpFormInfoByBundleName(bundleName, formInfos);
}
/**
 * @brief Dump form info by a bundle name.
 * @param formId The id of the form.
 * @param formInfo Form info.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormMgr::DumpFormInfoByFormId(const std::int64_t formId, std::string &formInfo)
{
    HILOG_INFO("%{public}s called.", __func__);
    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    return remoteProxy_->DumpFormInfoByFormId(formId, formInfo);
}
/**
 * @brief Dump form timer by form id.
 * @param formId The id of the form.
 * @param formInfo Form timer.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormMgr::DumpFormTimerByFormId(const std::int64_t formId, std::string &isTimingService)
{
    HILOG_INFO("%{public}s called.", __func__);
    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    return remoteProxy_->DumpFormTimerByFormId(formId, isTimingService);
}
/**
 * @brief Process js message event.
 * @param formId Indicates the unique id of form.
 * @param want information passed to supplier.
 * @param callerToken Caller ability token.
 * @return Returns true if execute success, false otherwise.
 */
int FormMgr::MessageEvent(const int64_t formId, const Want &want, const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("%{public}s called.", __func__);
    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    auto hostCaller = FormCallerMgr::GetInstance().GetFormHostCaller(formId);
    if (hostCaller != nullptr) {
        HILOG_DEBUG("send message by host caller");
        return hostCaller->MessageEvent(formId, want, callerToken);
    }
    return remoteProxy_->MessageEvent(formId, want, callerToken);
}

/**
 * @brief Process js router event.
 * @param formId Indicates the unique id of form.
 * @param want the want of the ability to start.
 * @param callerToken Caller ability token.
 * @return Returns true if execute success, false otherwise.
 */
int FormMgr::RouterEvent(const int64_t formId, Want &want, const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("%{public}s called.", __func__);
    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    return remoteProxy_->RouterEvent(formId, want, callerToken);
}

/**
 * @brief Process Background event.
 * @param formId Indicates the unique id of form.
 * @param want the want of the ability to start.
 * @param callerToken Caller ability token.
 * @return Returns true if execute success, false otherwise.
 */
int FormMgr::BackgroundEvent(const int64_t formId, Want &want, const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("%{public}s called.", __func__);
    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    return remoteProxy_->BackgroundEvent(formId, want, callerToken);
}

/**
 * @brief Set next refresh time.
 * @param formId The id of the form.
 * @param nextTime Next refresh time.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormMgr::SetNextRefreshTime(const int64_t formId, const int64_t nextTime)
{
    HILOG_INFO("%{public}s called.", __func__);

    if (nextTime < (Constants::MIN_NEXT_TIME / Constants::SEC_PER_MIN)) {
        HILOG_ERROR("next time less than 5 mins");
        return ERR_APPEXECFWK_FORM_INVALID_REFRESH_TIME;
    }

    if (GetInstance().GetRecoverStatus() == Constants::IN_RECOVERING) {
        HILOG_ERROR("%{public}s, formManager is in recovering", __func__);
        return ERR_APPEXECFWK_FORM_SERVER_STATUS_ERR;
    }

    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    return remoteProxy_->SetNextRefreshTime(formId, nextTime);
}

ErrCode FormMgr::RequestPublishForm(Want &want, bool withFormBindingData,
                                    std::unique_ptr<FormProviderData> &formBindingData, int64_t &formId)
{
    HILOG_INFO("%{public}s called.", __func__);
    ErrCode errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    return remoteProxy_->RequestPublishForm(want, withFormBindingData, formBindingData, formId);
}

int FormMgr::LifecycleUpdate(
    const std::vector<int64_t> &formIds,
    const sptr<IRemoteObject> &callerToken,
    bool updateType)
{
    HILOG_INFO("%{public}s called.", __func__);

    if (GetRecoverStatus() == Constants::IN_RECOVERING) {
        HILOG_ERROR("%{public}s error, form is in recover status, can't do action on form.", __func__);
        return ERR_APPEXECFWK_FORM_SERVER_STATUS_ERR;
    }

    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    return remoteProxy_->LifecycleUpdate(formIds, callerToken, updateType);
}
/**
 * @brief Get fms recoverStatus.
 *
 * @return The current recover status.
 */
int FormMgr::GetRecoverStatus()
{
    HILOG_INFO("%{public}s called.", __func__);
    return recoverStatus_;
}

/**
 * @brief Set fms recoverStatus.
 *
 * @param recoverStatus The recover status.
 */
void FormMgr::SetRecoverStatus(int recoverStatus)
{
    HILOG_INFO("%{public}s called.", __func__);
    recoverStatus_ = recoverStatus;
}

/**
 * @brief Get the error message content.
 *
 * @param errCode Error code.
 * @return Message content.
 */
std::string FormMgr::GetErrorMessage(int errCode)
{
    HILOG_INFO("%{public}s called.", __func__);
    return FormErrors::GetInstance().GetErrorMessage(errCode);
}

/**
 * @brief Register death callback.
 *
 * @param deathCallback Death callback.
 */
void FormMgr::RegisterDeathCallback(const std::shared_ptr<FormCallbackInterface> &formDeathCallback)
{
    HILOG_INFO("%{public}s called.", __func__);
    if (formDeathCallback == nullptr) {
        HILOG_ERROR("%{public}s error, form death callback is nullptr.", __func__);
        return;
    }
    formDeathCallbacks_.emplace_back(formDeathCallback);
}

/**
 * @brief UnRegister death callback.
 *
 * @param deathCallback Death callback.
 */
void FormMgr::UnRegisterDeathCallback(const std::shared_ptr<FormCallbackInterface> &formDeathCallback)
{
    HILOG_INFO("%{public}s called.", __func__);
    if (formDeathCallback == nullptr) {
        HILOG_ERROR("%{public}s error, form death callback is nullptr.", __func__);
        return;
    }

    // Remove the specified death callback in the vector of death callback
    auto iter = std::find(formDeathCallbacks_.begin(), formDeathCallbacks_.end(), formDeathCallback);
    if (iter != formDeathCallbacks_.end()) {
        formDeathCallbacks_.erase(iter);
    }
    HILOG_INFO("%{public}s end.", __func__);
}

/**
 * @brief Get death recipient.
 * @return deathRecipient_.
 */
sptr<IRemoteObject::DeathRecipient> FormMgr::GetDeathRecipient() const
{
    return deathRecipient_;
}

/**
 * @brief Check whether the specified death callback is registered in form mgr.
 * @param formDeathCallback The specified death callback for checking.
 * @return Return true on success, false on failure.
 */
bool FormMgr::CheckIsDeathCallbackRegistered(const std::shared_ptr<FormCallbackInterface> &formDeathCallback)
{
    HILOG_INFO("%{public}s called.", __func__);
    auto iter = std::find(formDeathCallbacks_.begin(), formDeathCallbacks_.end(), formDeathCallback);
    if (iter != formDeathCallbacks_.end()) {
        return true;
    }
    return false;
}

/**
 * @brief Notices IRemoteBroker died.
 * @param remote remote object.
 */
void FormMgr::FormMgrDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    HILOG_INFO("%{public}s called.", __func__);
    if (remote == nullptr) {
        HILOG_ERROR("%{public}s failed, remote is nullptr.", __func__);
        return;
    }

    if (FormMgr::GetInstance().GetRecoverStatus() == Constants::IN_RECOVERING) {
        HILOG_WARN("%{public}s, fms in recovering.", __func__);
        return;
    }
    // Reset proxy
    FormMgr::GetInstance().ResetProxy(remote);

    if (!FormMgr::GetInstance().Reconnect()) {
        HILOG_ERROR("%{public}s, form mgr service died, try to reconnect to fms failed.", __func__);
        FormMgr::GetInstance().SetRecoverStatus(Constants::RECOVER_FAIL);
        return;
    }

    // refresh form host.
    for (auto &deathCallback : FormMgr::GetInstance().formDeathCallbacks_) {
        deathCallback->OnDeathReceived();
    }
    FormMgr::GetInstance().SetRecoverStatus(Constants::NOT_IN_RECOVERY);
}

/**
 * @brief Reconnect form manager service once per 1000 milliseconds,
 *        until the connection succeeds or reaching the max retry times.
 * @return Returns true if execute success, false otherwise.
 */
bool FormMgr::Reconnect()
{
    HILOG_INFO("%{public}s called.", __func__);
    for (int i = 0; i < Constants::MAX_RETRY_TIME; i++) {
        // Sleep 1000 milliseconds before reconnect.
        std::this_thread::sleep_for(std::chrono::milliseconds(Constants::SLEEP_TIME));

        // try to connect fms
        if (Connect() != ERR_OK) {
            HILOG_ERROR("%{public}s, get fms proxy fail, try again.", __func__);
            continue;
        }

        HILOG_INFO("%{public}s, get fms proxy success.", __func__);
        return true;
    }

    return false;
}

/**
 * @brief Connect form manager service.
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormMgr::Connect()
{
    std::lock_guard<std::mutex> lock(connectMutex_);
    if (remoteProxy_ != nullptr && !resetFlag_) {
        return ERR_OK;
    }

    sptr<ISystemAbilityManager> systemManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemManager == nullptr) {
        HILOG_ERROR("%{private}s fail to get registry", __func__);
        return ERR_APPEXECFWK_FORM_GET_SYSMGR_FAILED;
    }
    sptr<IRemoteObject> remoteObject = systemManager->GetSystemAbility(FORM_MGR_SERVICE_ID);
    if (remoteObject == nullptr) {
        HILOG_ERROR("%{private}s fail to connect FormMgrService", __func__);
        return ERR_APPEXECFWK_FORM_GET_FMS_FAILED;
    }
    deathRecipient_ = sptr<IRemoteObject::DeathRecipient>(new (std::nothrow) FormMgrDeathRecipient());
    if (deathRecipient_ == nullptr) {
        HILOG_ERROR("%{public}s Failed to create FormMgrDeathRecipient!", __func__);
        return ERR_APPEXECFWK_FORM_COMMON_CODE;
    }
    if ((remoteObject->IsProxyObject()) && (!remoteObject->AddDeathRecipient(deathRecipient_))) {
        HILOG_ERROR("%{public}s Add death recipient to FormMgrService failed.", __func__);
        return ERR_APPEXECFWK_FORM_COMMON_CODE;
    }

    remoteProxy_ = iface_cast<IFormMgr>(remoteObject);
    HILOG_DEBUG("%{public}s Connecting FormMgrService success.", __func__);
    return ERR_OK;
}

/**
 * @brief Reset proxy.
 * @param remote remote object.
 */
void FormMgr::ResetProxy(const wptr<IRemoteObject> &remote)
{
    HILOG_INFO("%{public}s called.", __func__);
    std::lock_guard<std::mutex> lock(connectMutex_);
    if (remoteProxy_ == nullptr) {
        HILOG_ERROR("%{public}s failed, remote proxy is nullptr.", __func__);
        return;
    }

    // set formMgr's recover status to IN_RECOVERING.
    recoverStatus_ = Constants::IN_RECOVERING;

    // remove the death recipient
    auto serviceRemote = remoteProxy_->AsObject();
    if ((serviceRemote != nullptr) && (serviceRemote == remote.promote())) {
        serviceRemote->RemoveDeathRecipient(deathRecipient_);
    }
    // clearn the remote proxy
    remoteProxy_ = nullptr;
}

/**
 * @brief Set form mgr service for test.
 */
void FormMgr::SetFormMgrService(sptr<IFormMgr> formMgrService)
{
    HILOG_INFO("%{public}s called.", __func__);
    remoteProxy_ = formMgrService;
}

/**
 * @brief Delete the invalid forms.
 * @param formIds Indicates the ID of the valid forms.
 * @param callerToken Host client.
 * @param numFormsDeleted Returns the number of the deleted forms.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormMgr::DeleteInvalidForms(const std::vector<int64_t> &formIds, const sptr<IRemoteObject> &callerToken,
                                int32_t &numFormsDeleted)
{
    HILOG_INFO("%{public}s start.", __func__);

    if (GetRecoverStatus() == Constants::IN_RECOVERING) {
        HILOG_ERROR("%{public}s error, form is in recover status, can't do action on form.", __func__);
        return ERR_APPEXECFWK_FORM_SERVER_STATUS_ERR;
    }

    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed, errCode: %{public}d.", __func__, errCode);
        return errCode;
    }
    int resultCode = remoteProxy_->DeleteInvalidForms(formIds, callerToken, numFormsDeleted);
    if (resultCode != ERR_OK) {
        HILOG_ERROR("%{public}s error, failed to DeleteInvalidForms, error code is %{public}d.", __func__, resultCode);
    }
    return resultCode;
}

/**
 * @brief Acquire form state info by passing a set of parameters (using Want) to the form provider.
 * @param want Indicates a set of parameters to be transparently passed to the form provider.
 * @param callerToken Host client.
 * @param stateInfo Returns the form's state info of the specify.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormMgr::AcquireFormState(const Want &want, const sptr<IRemoteObject> &callerToken, FormStateInfo &stateInfo)
{
    HILOG_INFO("%{public}s start.", __func__);

    if (GetRecoverStatus() == Constants::IN_RECOVERING) {
        HILOG_ERROR("%{public}s error, form is in recover status, can't do action on form.", __func__);
        return ERR_APPEXECFWK_FORM_SERVER_STATUS_ERR;
    }

    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed, errCode: %{public}d.", __func__, errCode);
        return errCode;
    }
    int resultCode = remoteProxy_->AcquireFormState(want, callerToken, stateInfo);
    if (resultCode != ERR_OK) {
        HILOG_ERROR("%{public}s error, failed to AcquireFormState, error code is %{public}d.", __func__, resultCode);
    }
    return resultCode;
}

/**
 * @brief Notify the form is visible or not.
 * @param formIds Indicates the ID of the forms.
 * @param isVisible Visible or not.
 * @param callerToken Host client.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormMgr::NotifyFormsVisible(const std::vector<int64_t> &formIds, bool isVisible,
                                const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("%{public}s start.", __func__);

    if (FormMgr::GetRecoverStatus() == Constants::IN_RECOVERING) {
        HILOG_ERROR("%{public}s error, form is in recover status, can't do action on form.", __func__);
        return ERR_APPEXECFWK_FORM_SERVER_STATUS_ERR;
    }

    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed, errCode: %{public}d.", __func__, errCode);
        return errCode;
    }

    int resultCode = remoteProxy_->NotifyFormsVisible(formIds, isVisible, callerToken);
    if (resultCode != ERR_OK) {
        HILOG_ERROR("%{public}s error, failed to NotifyFormsVisible, error code is %{public}d.", __func__, resultCode);
    }
    return resultCode;
}

int FormMgr::NotifyFormsPrivacyProtected(const std::vector<int64_t> &formIds, bool isProtected,
                                         const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("%{public}s start.", __func__);

    if (FormMgr::GetRecoverStatus() == Constants::IN_RECOVERING) {
        HILOG_ERROR("%{public}s error, form is in recover status, can't do action on form.", __func__);
        return ERR_APPEXECFWK_FORM_SERVER_STATUS_ERR;
    }

    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed, errCode: %{public}d.", __func__, errCode);
        return errCode;
    }

    int resultCode = remoteProxy_->NotifyFormsPrivacyProtected(formIds, isProtected, callerToken);
    if (resultCode != ERR_OK) {
        HILOG_ERROR("%{public}s error, failed to NotifyFormsPrivacyProtected, error code is %{public}d.", __func__,
            resultCode);
    }
    return resultCode;
}

/**
 * @brief Notify the form is enable to be updated or not.
 * @param formIds Indicates the ID of the forms.
 * @param isEnableUpdate enable update or not.
 * @param callerToken Host client.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormMgr::NotifyFormsEnableUpdate(const std::vector<int64_t> &formIds, bool isEnableUpdate,
                                     const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("%{public}s start.", __func__);

    if (FormMgr::GetRecoverStatus() == Constants::IN_RECOVERING) {
        HILOG_ERROR("%{public}s error, form is in recover status, can't do action on form.", __func__);
        return ERR_APPEXECFWK_FORM_SERVER_STATUS_ERR;
    }

    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed, errCode: %{public}d.", __func__, errCode);
        return errCode;
    }

    int resultCode = remoteProxy_->NotifyFormsEnableUpdate(formIds, isEnableUpdate, callerToken);
    if (resultCode != ERR_OK) {
        HILOG_ERROR("%{public}s error, failed to NotifyFormsEnableUpdate, error code is %{public}d.", __func__,
            resultCode);
    }
    return resultCode;
}

/**
 * @brief Get All FormsInfo.
 * @param formInfos Return the forms' information of all forms provided.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormMgr::GetAllFormsInfo(std::vector<FormInfo> &formInfos)
{
    HILOG_INFO("%{public}s start.", __func__);

    if (FormMgr::GetRecoverStatus() == Constants::IN_RECOVERING) {
        HILOG_ERROR("%{public}s error, form is in recover status, can't do action on form.", __func__);
        return ERR_APPEXECFWK_FORM_SERVER_STATUS_ERR;
    }

    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_INFO("%{public}s failed, errCode: %{public}d.", __func__, errCode);
        return errCode;
    }
    int resultCode = remoteProxy_->GetAllFormsInfo(formInfos);
    if (resultCode != ERR_OK) {
        HILOG_ERROR("%{public}s error, failed to GetAllFormsInfo, error code is %{public}d.", __func__,
            resultCode);
    }
    return resultCode;
}

/**
 * @brief Get forms info by bundle name .
 * @param bundleName Application name.
 * @param formInfos Return the forms' information of the specify application name.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormMgr::GetFormsInfoByApp(std::string &bundleName, std::vector<FormInfo> &formInfos)
{
    HILOG_INFO("%{public}s start.", __func__);

    if (bundleName.empty()) {
        HILOG_WARN("Failed to Get forms info, because empty bundle name");
        return ERR_APPEXECFWK_FORM_INVALID_BUNDLENAME;
    }

    if (FormMgr::GetRecoverStatus() == Constants::IN_RECOVERING) {
        HILOG_ERROR("%{public}s error, form is in recover status, can't do action on form.", __func__);
        return ERR_APPEXECFWK_FORM_SERVER_STATUS_ERR;
    }

    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }

    int resultCode = remoteProxy_->GetFormsInfoByApp(bundleName, formInfos);
    if (resultCode != ERR_OK) {
        HILOG_ERROR("%{public}s error, failed to GetFormsInfoByApp, error code is %{public}d.", __func__,
            resultCode);
    }
    return resultCode;
}
/**
 * @brief Get forms info by bundle name and module name.
 * @param bundleName bundle name.
 * @param moduleName Module name of hap.
 * @param formInfos Return the forms' information of the specify bundle name and module name.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormMgr::GetFormsInfoByModule(std::string &bundleName, std::string &moduleName, std::vector<FormInfo> &formInfos)
{
    HILOG_INFO("%{public}s start.", __func__);
    if (bundleName.empty()) {
        HILOG_WARN("Failed to Get forms info, because empty bundleName");
        return ERR_APPEXECFWK_FORM_INVALID_BUNDLENAME;
    }

    if (moduleName.empty()) {
        HILOG_WARN("Failed to Get forms info, because empty moduleName");
        return ERR_APPEXECFWK_FORM_INVALID_MODULENAME;
    }

    if (FormMgr::GetRecoverStatus() == Constants::IN_RECOVERING) {
        HILOG_ERROR("%{public}s error, form is in recover status, can't do action on form.", __func__);
        return ERR_APPEXECFWK_FORM_SERVER_STATUS_ERR;
    }

    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }

    int resultCode = remoteProxy_->GetFormsInfoByModule(bundleName, moduleName, formInfos);
    if (resultCode != ERR_OK) {
        HILOG_ERROR("%{public}s error, failed to GetFormsInfoByApp, error code is %{public}d.", __func__, resultCode);
    }
    return resultCode;
}
int32_t FormMgr::GetFormsInfo(const FormInfoFilter &filter, std::vector<FormInfo> &formInfos)
{
    HILOG_INFO("%{public}s starts.", __func__);
    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    return remoteProxy_->GetFormsInfo(filter, formInfos);
}

bool FormMgr::IsRequestPublishFormSupported()
{
    HILOG_INFO("%{public}s starts.", __func__);
    int errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return false;
    }
    return remoteProxy_->IsRequestPublishFormSupported();
}

int32_t FormMgr::StartAbility(const Want &want, const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("%{public}s starts.", __func__);
    int32_t errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("%{public}s failed errCode:%{public}d.", __func__, errCode);
        return errCode;
    }
    return remoteProxy_->StartAbility(want, callerToken);
}

int32_t FormMgr::ShareForm(int64_t formId, const std::string &remoteDeviceId, const sptr<IRemoteObject> &callerToken,
    int64_t requestCode)
{
    HILOG_DEBUG("%{public}s called.", __func__);
    int32_t errCode = Connect();
    if (errCode != ERR_OK) {
        HILOG_ERROR("share form failed, errCode:%{public}d.", errCode);
        return errCode;
    }
    return remoteProxy_->ShareForm(formId, remoteDeviceId, callerToken, requestCode);
}

bool FormMgr::CheckFMSReady()
{
    HILOG_INFO("%{public}s called.", __func__);

    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = systemAbilityManager->GetSystemAbility(FORM_MGR_SERVICE_ID);
    if (remoteObject == nullptr) {
        HILOG_INFO("%{public}s, form manager service is not ready.", __func__);
        return false;
    }

    return true;
}

void FormMgr::GetExternalError(int32_t innerErrorCode, int32_t &externalErrorCode, std::string &errorMsg)
{
    externalErrorCode = FormErrors::GetInstance().QueryExternalErrorCode(innerErrorCode);
    errorMsg = FormErrors::GetInstance().QueryExternalErrorMessage(innerErrorCode, externalErrorCode);
    HILOG_DEBUG("innerErrorCode: %{public}d, externalErrorCode: %{public}d, errorMsg: %{public}s",
        innerErrorCode, externalErrorCode, errorMsg.c_str());
}

std::string FormMgr::GetErrorMsgByExternalErrorCode(int32_t externalErrorCode)
{
    return FormErrors::GetInstance().GetErrorMsgByExternalErrorCode(externalErrorCode);
}
}  // namespace AppExecFwk
}  // namespace OHOS
