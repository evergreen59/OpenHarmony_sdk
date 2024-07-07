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

#ifndef OHOS_FORM_FWK_FORM_MGR_ADAPTER_H
#define OHOS_FORM_FWK_FORM_MGR_ADAPTER_H

#include <singleton.h>

#include "bundle_info.h"
#include "bundle_mgr_interface.h"
#include "form_info.h"
#include "form_item_info.h"
#include "form_js_info.h"
#include "form_provider_data.h"
#include "form_state_info.h"
#include "iremote_object.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
using Want = OHOS::AAFwk::Want;
using WantParams = OHOS::AAFwk::WantParams;
/**
 * @class FormMgrAdapter
 * Form request handler from form host.
 */
class FormMgrAdapter  final : public DelayedRefSingleton<FormMgrAdapter> {
DECLARE_DELAYED_REF_SINGLETON(FormMgrAdapter)
public:
    DISALLOW_COPY_AND_MOVE(FormMgrAdapter);

    /**
     * @brief Add form with want, send want to form manager service.
     * @param formId The Id of the forms to add.
     * @param want The want of the form to add.
     * @param callerToken Caller ability token.
     * @param formInfo Form info.
     * @return Returns ERR_OK on success, others on failure.
     */
    int AddForm(const int64_t formId, const Want &want, const sptr<IRemoteObject> &callerToken, FormJsInfo &formInfo);

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
     * @brief Release forms with formIds, send formIds to form Mgr service.
     * @param formId The Id of the forms to release.
     * @param callerToken Caller ability token.
     * @param delCache Delete Cache or not.
     * @return Returns ERR_OK on success, others on failure.
     */
    int ReleaseForm(const int64_t formId, const sptr<IRemoteObject> &callerToken, const bool delCache);

    /**
     * @brief Update form with formId, send formId to form manager service.
     * @param formId The Id of the form to update.
     * @param bundleName Provider ability bundleName.
     * @param formProviderData form provider data.
     * @return Returns ERR_OK on success, others on failure.
     */
    int UpdateForm(const int64_t formId, const std::string &bundleName, const FormProviderData &formProviderData);

    /**
     * @brief Request form with formId and want, send formId and want to form manager service.
     *
     * @param formId The Id of the form to update.
     * @param callerToken Caller ability token.
     * @param want The want of the form to request.
     * @return Returns ERR_OK on success, others on failure.
     */
    int RequestForm(const int64_t formId, const sptr<IRemoteObject> &callerToken, const Want &want);

    /**
     * @brief Form visible/invisible notify, send formIds to form manager service.
     *
     * @param formIds The vector of form Ids.
     * @param callerToken Caller ability token.
     * @param formVisibleType The form visible type, including FORM_VISIBLE and FORM_INVISIBLE.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode NotifyWhetherVisibleForms(const std::vector<int64_t> &formIds, const sptr<IRemoteObject> &callerToken,
        const int32_t formVisibleType);

    /**
     * @brief temp form to normal form.
     * @param formId The Id of the form.
     * @param callerToken Caller ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    int CastTempForm(const int64_t formId, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Dump all of form storage infos.
     * @param formInfos All of form storage infos.
     * @return Returns ERR_OK on success, others on failure.
     */
    int DumpStorageFormInfos(std::string &formInfos) const;
    /**
     * @brief Dump form info by a bundle name.
     * @param bundleName The bundle name of form provider.
     * @param formInfos Form infos.
     * @return Returns ERR_OK on success, others on failure.
     */
    int DumpFormInfoByBundleName(const std::string &bundleName, std::string &formInfos) const;
    /**
     * @brief Dump form info by a bundle name.
     * @param formId The id of the form.
     * @param formInfo Form info.
     * @return Returns ERR_OK on success, others on failure.
     */
    int DumpFormInfoByFormId(const std::int64_t formId, std::string &formInfo) const;
    /**
     * @brief Dump form timer by form id.
     * @param formId The id of the form.
     * @param isTimingService "true" or "false".
     * @return Returns ERR_OK on success, others on failure.
     */
    int DumpFormTimerByFormId(const std::int64_t formId, std::string &isTimingService) const;

    /**
     * @brief set next refresh time.
     * @param formId The id of the form.
     * @param nextTime next refresh time.
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
     * @brief Check if the request of publishing a form is supported by the host.
     * @return Returns true if the request is supported and false otherwise.
     */
    bool IsRequestPublishFormSupported();

    /**
     * @brief enable update form.
     * @param formIDs The id of the forms.
     * @param callerToken Caller ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    int EnableUpdateForm(const std::vector<int64_t> formIDs, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief disable update form.
     * @param formIDs The id of the forms.
     * @param callerToken Caller ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    int DisableUpdateForm(const std::vector<int64_t> formIDs, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Process js message event.
     * @param formId Indicates the unique id of form.
     * @param want information passed to supplier.
     * @param callerToken Caller ability token.
     * @return Returns true if execute success, false otherwise.
     */
    int MessageEvent(const int64_t formId, const Want &want, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Process js router event.
     * @param formId Indicates the unique id of form.
     * @param want the want of the ability to start.
     * @param callerToken Caller ability token.
     * @return Returns true if execute success, false otherwise.
     */
    int RouterEvent(const int64_t formId, Want &want, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Process background router event.
     * @param formId Indicates the unique id of form.
     * @param want the want of the ability to start.
     * @param callerToken Caller ability token.
     * @return Returns true if execute success, false otherwise.
     */
    int BackgroundEvent(const int64_t formId, Want &want, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Acquire form data from form provider.
     * @param formId The Id of the from.
     * @param want The want of the request.
     * @param remoteObject Form provider proxy object.
     */
    void AcquireProviderFormInfo(const int64_t formId, const Want &want, const sptr<IRemoteObject> &remoteObject);
    /**
     * @brief Notify form provider for delete form.
     * @param formId The Id of the from.
     * @param want The want of the form.
     * @param remoteObject Form provider proxy object.
     * @return none.
     */
    void NotifyFormDelete(const int64_t formId, const Want &want, const sptr<IRemoteObject> &remoteObject);

    /**
     * @brief Delete the invalid forms.
     * @param formIds Indicates the ID of the valid forms.
     * @param callerToken Caller ability token.
     * @param numFormsDeleted Returns the number of the deleted forms.
     * @return Returns ERR_OK on success, others on failure.
     */
    int DeleteInvalidForms(const std::vector<int64_t> &formIds, const sptr<IRemoteObject> &callerToken,
                           int32_t &numFormsDeleted);

    /**
     * @brief Acquire form state info by passing a set of parameters (using Want) to the form provider.
     * @param want Indicates a set of parameters to be transparently passed to the form provider.
     * @param callerToken Caller ability token.
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
    int GetFormsInfoByApp(const std::string &bundleName, std::vector<FormInfo> &formInfos);

    /**
     * @brief Get forms info by bundle name and module name.
     * @param bundleName bundle name.
     * @param moduleName Module name of hap.
     * @param formInfos Return the forms' information of the specify bundle name and module name.
     * @return Returns ERR_OK on success, others on failure.
     */
    int GetFormsInfoByModule(const std::string &bundleName, const std::string &moduleName,
        std::vector<FormInfo> &formInfos);
private:
    /**
     * @brief Get form configure info.
     * @param want The want of the request.
     * @param formItemInfo Form configure info.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode GetFormConfigInfo(const Want& want, FormItemInfo &formItemInfo);
    /**
     * @brief Get bundle info.
     * @param want The want of the request.
     * @param bundleInfo Bundle info.
     * @param packageName Package name.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode GetBundleInfo(const AAFwk::Want &want, BundleInfo &bundleInfo, std::string &packageName);
    /**
     * @brief Get form info.
     * @param want The want of the request.
     * @param formInfo Form info.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode GetFormInfo(const AAFwk::Want &want, FormInfo &formInfo);
    /**
     * @brief Get form configure info.
     * @param want The want of the request.
     * @param bundleInfo Bundle info.
     * @param formInfo Form info.
     * @param formItemInfo Form configure info.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode GetFormItemInfo(const AAFwk::Want &want, const BundleInfo &bundleInfo, const FormInfo &formInfo,
        FormItemInfo &formItemInfo);
    /**
     * @brief Dimension valid check.
     * @param formInfo Form info.
     * @param dimensionId Dimension id.
     * @return Returns true on success, false on failure.
     */
    bool IsDimensionValid(const FormInfo &formInfo, int dimensionId) const;
    /**
     * @brief Create form configure info.
     * @param bundleInfo Bundle info.
     * @param formInfo Form info.
     * @param itemInfo Form configure info.
     * @param want The want of the request.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode CreateFormItemInfo(const BundleInfo& bundleInfo, const FormInfo& formInfo, FormItemInfo& itemInfo,
        const AAFwk::Want &want);
    /**
     * @brief Allocate form by formId.
     * @param info Form configure info.
     * @param callerToken Caller ability token.
     * @param wantParams WantParams of the request.
     * @param formInfo Form info for form host.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode AllotFormById(const FormItemInfo &info, const sptr<IRemoteObject> &callerToken,
        const WantParams &wantParams, FormJsInfo &formInfo);
    /**
     * @brief Allocate form by form configure info.
     * @param info Form configure info.
     * @param callerToken Caller ability token.
     * @param wantParams WantParams of the request.
     * @param formInfo Form info for form host.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode AllotFormByInfo(const FormItemInfo &info, const sptr<IRemoteObject> &callerToken,
        const WantParams& wantParams, FormJsInfo &formInfo);
    /**
     * @brief Acquire form data from form provider.
     * @param formId The Id of the form..
     * @param info Form configure info.
     * @param wantParams WantParams of the request.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode AcquireProviderFormInfoAsync(const int64_t formId, const FormItemInfo &info, const WantParams &wantParams);

    /**
     * @brief Handle release form.
     * @param formId The form id.
     * @param callerToken Caller ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode HandleReleaseForm(const int64_t formId, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Handle delete form.
     * @param formId The form id.
     * @param callerToken Caller ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode HandleDeleteForm(const int64_t formId, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Handle delete temp form.
     * @param formId The form id.
     * @param callerToken Caller ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode HandleDeleteTempForm(const int64_t formId, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Handle delete form storage.
     * @param dbRecord Form storage information.
     * @param uid calling user id.
     * @param formId The form id.
     * @return Function result and has other host flag.
     */
    ErrCode HandleDeleteFormCache(FormRecord &dbRecord, const int uid, const int64_t formId);

    /**
     * @brief Add existed form record.
     * @param info Form configure info.
     * @param callerToken Caller ability token.
     * @param record Form data.
     * @param formId The form id.
     * @param wantParams WantParams of the request.
     * @param formInfo Form info for form host.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode AddExistFormRecord(const FormItemInfo &info, const sptr<IRemoteObject> &callerToken,
        const FormRecord &record, const int64_t formId, const WantParams &wantParams, FormJsInfo &formInfo);

    /**
     * @brief Add new form record.
     * @param info Form configure info.
     * @param formId The form id.
     * @param callerToken Caller ability token.
     * @param wantParams WantParams of the request.
     * @param formInfo Form info for form host.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode AddNewFormRecord(const FormItemInfo &info, const int64_t formId,
        const sptr<IRemoteObject> &callerToken, const WantParams &wantParams, FormJsInfo &formInfo);

    /**
     * @brief Send event notify to form provider. The event notify type include FORM_VISIBLE and FORM_INVISIBLE.
     *
     * @param providerKey The provider key string which consists of the provider bundle name and ability name.
     * @param formIdsByProvider The map of form Ids and their event type which have the same provider.
     * @param formVisibleType The form visible type, including FORM_VISIBLE and FORM_INVISIBLE.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode HandleEventNotify(const std::string &providerKey, const std::vector<int64_t> &formIdsByProvider,
        const int32_t formVisibleType);

    /**
     * @brief Increase the timer refresh count.
     *
     * @param formId The form id.
     */
    void IncreaseTimerRefreshCount(const int64_t formId);

    /**
     * @brief handle update form flag.
     * @param formIDs The id of the forms.
     * @param callerToken Caller ability token.
     * @param flag form flag.
     * @param isOnlyEnableUpdate form enable update form flag.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode HandleUpdateFormFlag(const std::vector<int64_t> &formIds, const sptr<IRemoteObject> &callerToken,
                                 bool flag, bool isOnlyEnableUpdate);

    /**
     * @brief check form cached.
     * @param record Form information.
     * @return Returns true on cached, false on not.
     */
    bool IsFormCached(const FormRecord record);

    /**
     * @brief set next refresh time locked.
     * @param formId The form's id.
     * @param nextTime next refresh time.
     * @param userId User ID.
     * @return Returns ERR_OK on success, others on failure.
     */
    int SetNextRefreshTimeLocked(const int64_t formId, const int64_t nextTime, const int32_t userId = 0);

    /**
     * @brief check if update is valid.
     * @param formId The form's id.
     * @param bundleName Provider ability bundleName.
     * @return Returns true or false.
     */
    bool IsUpdateValid(const int64_t formId, const std::string &bundleName);
    /**
     * @brief Handle cast temp form.
     * @param formId The form id.
     * @param record Form information.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode HandleCastTempForm(const int64_t formId, const FormRecord &record);

    /**
     * @brief Add form timer.
     * @param formRecord Form information.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode AddFormTimer(const FormRecord &formRecord);

    /**
     * @brief check the publish form.
     * @param want The want of the form to publish.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode CheckPublishForm(Want &want);

    /**
     * @brief Query the request host.
     * @param want The want of the form to publish.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode QueryPublishFormToHost(Want &want);

    /**
     * @brief Post request publish form to host.
     * @param want The want of the form to publish.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode RequestPublishFormToHost(Want &want);

    /**
     * @brief check the argv of AddRequestPublishForm.
     * @param want The want of the form to add.
     * @param formProviderWant The want of the form to publish from provider.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode CheckAddRequestPublishForm(const Want &want, const Want &formProviderWant);

    /**
     * @brief add request publish form.
     * @param formItemInfo Form configure info.
     * @param want The want of the form to add.
     * @param callerToken Caller ability token.
     * @param formJsInfo Return form info to form host.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode AddRequestPublishForm(const FormItemInfo &formItemInfo, const Want &want,
        const sptr<IRemoteObject> &callerToken, FormJsInfo &formJsInfo);

    /**
     * @brief get bundleName.
     * @param bundleName for output.
     * @return Returns true on success, others on failure.
     */
    bool GetBundleName(std::string &bundleName);

    /**
     * @brief Update provider info to host
     *
     * @param matchedFormId The Id of the form
     * @param callerToken Caller ability token.
     * @param formVisibleType The form visible type, including FORM_VISIBLE and FORM_INVISIBLE.
     * @param formRecord Form storage information
     * @return Returns true on success, false on failure.
     */
    bool UpdateProviderInfoToHost(const int64_t matchedFormId, const sptr<IRemoteObject> &callerToken,
        const int32_t formVisibleType, FormRecord &formRecord);

    /**
     * @brief If the form provider is system app and the config item 'formVisibleNotify' is true,
     *        notify the form provider that the current form is visible.
     *
     * @param iBundleMgr BundleManager
     * @param bundleName BundleName
     * @return Returns true if the form provider is system app, false if not.
     */
    bool CheckIsSystemAppByBundleName(const sptr<IBundleMgr> &iBundleMgr, const std::string &bundleName);

    /**
     * @brief if the ability have permission for keeping background running is true,
     * @param iBundleMgr BundleManagerProxy
     * @param bundleName BundleName
     * @return Returns true if the ability have permission for keeping background running, false if not.
     */
    bool CheckKeepBackgroundRunningPermission(const sptr<IBundleMgr> &iBundleMgr, const std::string &bundleName);
    /**
     * @brief Create eventMaps for event notify.
     *
     * @param matchedFormId The Id of the form
     * @param formRecord Form storage information
     * @param eventMaps eventMaps for event notify
     * @return Returns true on success, false on failure.
     */
    bool CreateHandleEventMap(const int64_t matchedFormId, const FormRecord &formRecord,
        std::map<std::string, std::vector<int64_t>> &eventMaps);
    /**
     * @brief Get current user ID.
     * @param callingUid calling Uid.
     * @return Returns user ID.
     */
    int32_t GetCurrentUserId(const int callingUid);
    /**
     * @brief AcquireFormState want check.
     * @param bundleName The bundle name of the form.
     * @param abilityName The ability name of the form.
     * @param want The want of the form.
     * @param provider the provider info.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode AcquireFormStateCheck(const std::string &bundleName, const std::string &abilityName, const Want &want,
                                  std::string &provider);
    /**
     * @brief check if the form host is system app
     * @param formRecord Form storage information
     * @return Returns true if the form host is system app, false if not.
     */
    bool checkFormHostHasSaUid(const FormRecord &formRecord);
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif // OHOS_FORM_FWK_FORM_MGR_ADAPTER_H
