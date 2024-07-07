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

#ifndef OHOS_FORM_FWK_FORM_MGR_H
#define OHOS_FORM_FWK_FORM_MGR_H

#include <mutex>
#include <singleton.h>
#include <thread>

#include "form_callback_interface.h"
#include "form_constants.h"
#include "form_errors.h"
#include "form_death_callback.h"
#include "form_info.h"
#include "form_js_info.h"
#include "form_mgr_interface.h"
#include "form_provider_data.h"
#include "form_state_info.h"
#include "iremote_object.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
using OHOS::AAFwk::Want;

static volatile int recoverStatus_ = Constants::NOT_IN_RECOVERY;

/**
 * @class FormMgr
 * FormMgr is used to access form manager services.
 */
class FormMgr final : public DelayedRefSingleton<FormMgr> {
    DECLARE_DELAYED_REF_SINGLETON(FormMgr)
public:
    DISALLOW_COPY_AND_MOVE(FormMgr);

    friend class FormMgrDeathRecipient;

    /**
     * @brief Get the error message by error code.
     * @param errorCode the error code return form fms.
     * @return Returns the error message detail.
     */
    std::string GetErrorMsg(int errorCode);

    /**
     * @brief Add form with want, send want to form manager service.
     * @param formId The Id of the forms to add.
     * @param want The want of the form to add.
     * @param callerToken Caller ability token.
     * @param formInfo Form info.
     * @return Returns ERR_OK on success, others on failure.
     */
    int AddForm(const int64_t formId, const Want &want, const sptr<IRemoteObject> &callerToken,
        FormJsInfo &formInfo);

    /**
     * @brief Delete forms with formIds, send formIds to form manager service.
     * @param formId The Id of the forms to delete.
     * @param callerToken Caller ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    int DeleteForm(const int64_t formId, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Stop rendering form.
     * @param formId The Id of the forms to delete.
     * @param compId The compId of the forms to delete.
     * @return Returns ERR_OK on success, others on failure.
     */
    int StopRenderingForm(const int64_t formId, const std::string &compId);

    /**
     * @brief Release forms with formIds, send formIds to form manager service.
     * @param formId The Id of the forms to release.
     * @param callerToken Caller ability token.
     * @param delCache Delete Cache or not.
     * @return Returns ERR_OK on success, others on failure.
     */
    int ReleaseForm(const int64_t formId, const sptr<IRemoteObject> &callerToken, const bool delCache);

    /**
     * @brief Update form with formId, send formId to form manager service.
     * @param formId The Id of the form to update.
     * @param formBindingData Form binding data.
     * @return Returns ERR_OK on success, others on failure.
     */
    int UpdateForm(const int64_t formId, const FormProviderData &formBindingData);

    /**
     * @brief Notify the form service that the form user's lifecycle is updated.
     *
     * This should be called when form user request form.
     *
     * @param formId Indicates the unique id of form.
     * @param callerToken Indicates the callback remote object of specified form user.
     * @param want information passed to supplier.
     * @return Returns ERR_OK on success, others on failure.
     */
    int RequestForm(const int64_t formId, const sptr<IRemoteObject> &callerToken, const Want &want);

    /**
     * @brief Form visible/invisible notify, send formIds to form manager service.
     * @param formIds The Id list of the forms to notify.
     * @param callerToken Caller ability token.
     * @param formVisibleType The form visible type, including FORM_VISIBLE and FORM_INVISIBLE.
     * @return Returns ERR_OK on success, others on failure.
     */
    int NotifyWhetherVisibleForms(const std::vector<int64_t> &formIds, const sptr<IRemoteObject> &callerToken,
    const int32_t formVisibleType);

    /**
     * @brief temp form to normal form.
     * @param formId The Id of the form.
     * @param callerToken Caller ability token.
     * @return None.
     */
    int CastTempForm(const int64_t formId, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Dump all of form storage infos.
     * @param formInfos All of form storage infos.
     * @return Returns ERR_OK on success, others on failure.
     */
    int DumpStorageFormInfos(std::string &formInfos);
    /**
     * @brief Dump form info by a bundle name.
     * @param bundleName The bundle name of form provider.
     * @param formInfos Form infos.
     * @return Returns ERR_OK on success, others on failure.
     */
    int DumpFormInfoByBundleName(const std::string bundleName, std::string &formInfos);
    /**
     * @brief Dump form info by a bundle name.
     * @param formId The id of the form.
     * @param formInfo Form info.
     * @return Returns ERR_OK on success, others on failure.
     */
    int DumpFormInfoByFormId(const std::int64_t formId, std::string &formInfo);
    /**
     * @brief Dump form timer by form id.
     * @param formId The id of the form.
     * @param formInfo Form timer.
     * @return Returns ERR_OK on success, others on failure.
     */
    int DumpFormTimerByFormId(const std::int64_t formId, std::string &isTimingService);
    /**
     * @brief Process js message event.
     * @param formId Indicates the unique id of form.
     * @param want information passed to supplier.
     * @param callerToken Caller ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    int MessageEvent(const int64_t formId, const Want &want, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Process js router event.
     * @param formId Indicates the unique id of form.
     * @param want the want of the ability to start.
     * @param callerToken Caller ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    int RouterEvent(const int64_t formId, Want &want, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Process Background event.
     * @param formId Indicates the unique id of form.
     * @param want the want of the ability to start.
     * @param callerToken Caller ability token.
     * @return Returns true if execute success, false otherwise.
     */
    int BackgroundEvent(const int64_t formId, Want &want, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Get fms recoverStatus.
     *
     * @return The current recover status.
     */
    static int GetRecoverStatus();

    /**
     * @brief Register death callback.
     *
     * @param formDeathCallback The death callback.
     */
    void RegisterDeathCallback(const std::shared_ptr<FormCallbackInterface> &formDeathCallback);

    /**
     * @brief UnRegister death callback.
     *
     * @param formDeathCallback The death callback.
     */
    void UnRegisterDeathCallback(const std::shared_ptr<FormCallbackInterface> &formDeathCallback);

    /**
     * @brief Set the next refresh time
     *
     * @param formId The id of the form.
     * @param nextTime Next refresh time
     * @return Returns ERR_OK on success, others on failure.
     */
    int SetNextRefreshTime(const int64_t formId, const int64_t nextTime);

    /**
     * @brief Request to publish a form to the form host.
     *
     * @param want The want of the form to publish.
     * @param withFormBindingData Indicates whether the formBindingData is carried with.
     * @param formBindingData Indicates the form data.
     * @param formId Return the form id to be published.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode RequestPublishForm(Want &want, bool withFormBindingData,
                               std::unique_ptr<FormProviderData> &formBindingData, int64_t &formId);

    /**
     * @brief Lifecycle Update.
     * @param formIds The id of the forms.
     * @param callerToken Host client.
     * @param updateType update type, enable if true and disable if false.
     * @return Returns ERR_OK on success, others on failure.
     */
    int LifecycleUpdate(const std::vector<int64_t> &formIds, const sptr<IRemoteObject> &callerToken,
                        bool updateType);

    /**
     * @brief Set fms recoverStatus.
     *
     * @param recoverStatus The recover status.
     */
    static void SetRecoverStatus(int recoverStatus);

    /**
     * @brief Set form mgr service for test.
     */
    void SetFormMgrService(sptr<IFormMgr> formMgrService);

    /**
     * @brief Get death recipient.
     * @return deathRecipient_.
     */
    sptr<IRemoteObject::DeathRecipient> GetDeathRecipient() const;

    /**
     * @brief Check whether the specified death callback is registered in form mgr.
     * @param formDeathCallback The specified death callback for checking.
     * @return Return true on success, false on failure.
     */
    bool CheckIsDeathCallbackRegistered(const std::shared_ptr<FormCallbackInterface> &formDeathCallback);

    /**
     * @brief Get the error message content.
     *
     * @param errCode Error code.
     * @return Message content.
     */
    std::string GetErrorMessage(int errCode);

    /**
     * @brief Delete the invalid forms.
     * @param formIds Indicates the ID of the valid forms.
     * @param callerToken Host client.
     * @param numFormsDeleted Returns the number of the deleted forms.
     * @return Returns ERR_OK on success, others on failure.
     */
    int DeleteInvalidForms(const std::vector<int64_t> &formIds, const sptr<IRemoteObject> &callerToken,
                           int32_t &numFormsDeleted);

    /**
     * @brief Acquire form state info by passing a set of parameters (using Want) to the form provider.
     * @param want Indicates a set of parameters to be transparently passed to the form provider.
     * @param callerToken Host client.
     * @param stateInfo Returns the form's state info of the specify.
     * @return Returns ERR_OK on success, others on failure.
     */
    int AcquireFormState(const Want &want, const sptr<IRemoteObject> &callerToken, FormStateInfo &stateInfo);

    /**
     * @brief Notify the form is visible or not.
     * @param formIds Indicates the ID of the forms.
     * @param isVisible Visible or not.
     * @param callerToken Host client.
     * @return Returns ERR_OK on success, others on failure.
     */
    int NotifyFormsVisible(const std::vector<int64_t> &formIds, bool isVisible, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Notify the form is privacy protected or not.
     * @param formIds Indicates the ID of the forms.
     * @param isProtected isProtected or not.
     * @param callerToken Host client.
     * @return Returns ERR_OK on success, others on failure.
     */
    int NotifyFormsPrivacyProtected(const std::vector<int64_t> &formIds, bool isProtected,
                                    const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Notify the form is enable to be updated or not.
     * @param formIds Indicates the ID of the forms.
     * @param isEnableUpdate enable update or not.
     * @param callerToken Host client.
     * @return Returns ERR_OK on success, others on failure.
     */
    int NotifyFormsEnableUpdate(const std::vector<int64_t> &formIds, bool isEnableUpdate,
                                const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Get All FormsInfo.
     * @param formInfos Return the forms' information of all forms provided.
     * @return Returns ERR_OK on success, others on failure.
     */
    int GetAllFormsInfo(std::vector<FormInfo> &formInfos);

    /**
     * @brief Get forms info by bundle name .
     * @param bundleName Application name.
     * @param formInfos Return the forms' information of the specify application name.
     * @return Returns ERR_OK on success, others on failure.
     */
    int GetFormsInfoByApp(std::string &bundleName, std::vector<FormInfo> &formInfos);

    /**
     * @brief Get forms info by bundle name and module name.
     * @param bundleName bundle name.
     * @param moduleName Module name of hap.
     * @param formInfos Return the forms' information of the specify bundle name and module name.
     * @return Returns ERR_OK on success, others on failure.
     */
    int GetFormsInfoByModule(std::string &bundleName, std::string &moduleName, std::vector<FormInfo> &formInfos);

    /**
    * @brief This function is called by formProvider and gets forms info by the bundle name of the calling ability.
    *        The bundle name will be retrieved by form service manager.
    * @param filter filter that contains attributes that the formInfos have to have.
    * @param formInfos Return the forms' information of the calling bundle name
    * @return Returns ERR_OK on success, others on failure.
    */
    int32_t GetFormsInfo(const FormInfoFilter &filter, std::vector<FormInfo> &formInfos);

    /**
     * @brief Check if the request of publishing a form is supported by the host.
     * @return Returns true if the request is supported and false otherwise.
     */
    bool IsRequestPublishFormSupported();

    /**
     * @brief Start an ability. This function can only be called by a form extension of a system app.
     * @param want includes ability name, parameters and relative info sending to an ability.
     * @param callerToken token of the ability that initially calls this function.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t StartAbility(const Want &want, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Share form with formId and remote device id.
     * @param formId The Id of the forms to share.
     * @param remoteDeviceId The Id of the remote revice to share.
     * @param callerToken Indicates the host client.
     * @param requestCode The request code of this share form.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t ShareForm(int64_t formId, const std::string &remoteDeviceId, const sptr<IRemoteObject> &callerToken,
        int64_t requestCode);

    /**
     * @brief Check form manager service ready.
     *
     * @return Return true if form manager service ready; returns false otherwise.
     */
    bool CheckFMSReady();

    /**
     * @brief Get external error from innerErrorCode.
     * @param innerErrorCode innerErrorCode, get from FMS.
     * @param externalErrorCode output externalErrorCode.
     * @param errorMsg output errorMsg.
     */
    void GetExternalError(int32_t innerErrorCode, int32_t &externalErrorCode, std::string &errorMsg);

    /**
     * @brief Get external error message by external error code.
     *
     * @param externalErrorCode External error code.
     * @return External error message.
     */
    std::string GetErrorMsgByExternalErrorCode(int32_t externalErrorCode);

private:
    /**
     * @brief Connect form manager service.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode Connect();

    /**
    * @brief Reconnect form manager service once per 1000 milliseconds,
    *        until the connection succeeds or reaching the max retry times.
    * @return Returns true if execute success, false otherwise.
    */
    bool Reconnect();
    /**
     * @brief Reset proxy.
     * @param remote remote object.
     */
    void ResetProxy(const wptr<IRemoteObject> &remote);

    /**
     * @class FormMgrDeathRecipient
     * FormMgrDeathRecipient notices IRemoteBroker died.
     */
    class FormMgrDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        FormMgrDeathRecipient() = default;
        ~FormMgrDeathRecipient() = default;

        /**
         * @brief Notices IRemoteBroker died.
         * @param remote remote object.
         */
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override;
    private:
        DISALLOW_COPY_AND_MOVE(FormMgrDeathRecipient);
    };

    std::mutex connectMutex_;
    sptr<IFormMgr> remoteProxy_;

    sptr<IRemoteObject::DeathRecipient> deathRecipient_ {nullptr};

    // True: need to get a new fms remote object,
    // False: no need to get a new fms remote object.
    volatile bool resetFlag_ = false;

    std::vector<std::shared_ptr<FormCallbackInterface>> formDeathCallbacks_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_FORM_FWK_FORM_MGR_H
