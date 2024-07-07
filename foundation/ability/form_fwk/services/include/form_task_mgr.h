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

#ifndef OHOS_FORM_FWK_FORM_TASK_MGR_H
#define OHOS_FORM_FWK_FORM_TASK_MGR_H

#include <singleton.h>
#include <vector>

#include "form_event_handler.h"
#include "form_js_info.h"
#include "form_record.h"
#include "form_state_info.h"
#include "iremote_object.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
using Want = OHOS::AAFwk::Want;
using WantParams = OHOS::AAFwk::WantParams;
/**
 * @class FormTaskMgr
 * form task manager.
 */
class FormTaskMgr final : public DelayedRefSingleton<FormTaskMgr> {
    DECLARE_DELAYED_REF_SINGLETON(FormTaskMgr)

public:
    DISALLOW_COPY_AND_MOVE(FormTaskMgr);

    /**
     * @brief SetEventHandler.
     * @param handler event handler
     */
    inline void SetEventHandler(const std::shared_ptr<FormEventHandler> &handler)
    {
        eventHandler_ = handler;
    }

    /**
     * @brief Acquire form data from form provider(task).
     * @param formId The Id of the form.
     * @param want The want of the request.
     * @param remoteObject Form provider proxy object.
     */
    void PostAcquireTask(const int64_t formId, const Want &want, const sptr<IRemoteObject> &remoteObject);

    /**
     * @brief Acquire share form data from form provider(task).
     * @param formId The Id of the form.
     * @param remoteDeviceId The device ID to share.
     * @param want The want of the request.
     * @param remoteObject Form provider proxy object.
     */
    void PostShareAcquireTask(int64_t formId, const std::string &remoteDeviceId, const Want &want,
        const sptr<IRemoteObject> &remoteObject);

    /**
     * @brief Delete form data from form provider(task).
     * @param formId The Id of the form.
     * @param want The want of the request.
     * @param remoteObject Form provider proxy object.
     */
    void PostDeleteTask(const int64_t formId, const Want &want, const sptr<IRemoteObject> &remoteObject);
   /**
     * @brief Notify provider batch delete.
     * @param formIds The Id list.
     * @param want The want of the request.
     * @param remoteObject Form provider proxy object.
     */
    void PostProviderBatchDeleteTask(std::set<int64_t> &formIds, const Want &want,
        const sptr<IRemoteObject> &remoteObject);
    /**
     * @brief Refresh form data from form provider(task).
     *
     * @param formId The Id of the form.
     * @param want The want of the form.
     * @param remoteObject Form provider proxy object.
     * @return none.
     */
    void PostRefreshTask(const int64_t formId, const Want &want, const sptr<IRemoteObject> &remoteObject);

    /**
     * @brief Cast temp form data from form provider(task).
     *
     * @param formId The Id of the form.
     * @param want The want of the request.
     * @param remoteObject Form provider proxy object.
     * @return none.
     */
    void PostCastTempTask(const int64_t formId, const Want &want, const sptr<IRemoteObject> &remoteObject);

    /**
     * @brief Post form data to form host(task) when acquire form.
     * @param formId The Id of the form.
     * @param record form record.
     * @param remoteObject Form provider proxy object.
     */
    void PostAcquireTaskToHost(const int64_t formId, const FormRecord &record, const sptr<IRemoteObject> &remoteObject);

    /**
     * @brief Post form data to form host(task) when update form.
     * @param formId The Id of the form.
     * @param record form record.
     * @param remoteObject Form provider proxy object.
     */
    void PostUpdateTaskToHost(const int64_t formId, const FormRecord &record, const sptr<IRemoteObject> &remoteObject);

    /**
     * @brief Handel form host died(task).
     * @param remoteHost Form host proxy object.
     */
    void PostHostDiedTask(const sptr<IRemoteObject> &remoteHost);

    /**
     * @brief Post event notify to form provider.
     *
     * @param formEvent The vector of form ids.
     * @param formVisibleType The form visible type, including FORM_VISIBLE and FORM_INVISIBLE.
     * @param want The want of the form.
     * @param remoteObject The form provider proxy object.
     */
    void PostEventNotifyTask(const std::vector<int64_t> &formEvent, const int32_t formVisibleType, const Want &want,
        const sptr<IRemoteObject> &remoteObject);

    /**
     * @brief Post message event to form provider.
     * @param formId The Id of the from.
     * @param message Event message.
     * @param want The want of the request.
     * @param remoteObject Form provider proxy object.
     */
    void PostFormEventTask(const int64_t formId, const std::string &message, const Want &want,
        const sptr<IRemoteObject> &remoteObject);

    /**
    * @brief Post acquire state to form provider.
    * @param wantArg The want of onAcquireFormState.
    * @param provider The provider info.
    * @param want The want of the request.
    * @param remoteObject Form provider proxy object.
    */
    void PostAcquireStateTask(const Want &wantArg, const std::string &provider, const Want &want,
                              const sptr <IRemoteObject> &remoteObject);

     /**
     * @brief Post uninstall message to form host(task).
     * @param formIds The Id list of the forms.
     * @param remoteObject Form provider proxy object.
     */
    void PostUninstallTaskToHost(const std::vector<int64_t> &formIds, const sptr<IRemoteObject> &remoteObject);

    /**
    * @brief Post acquire form state message to form host(task).
    * @param state The form state.
    * @param want The want of onAcquireFormState.
    * @param remoteObject Form provider proxy object.
    */
    void PostAcquireStateTaskToHost(AppExecFwk::FormState state, const AAFwk::Want &want,
                                    const sptr<IRemoteObject> &remoteObject);

    /**
    * @brief Post form share error code to form host(task).
    * @param formShareRequestCode The request code for this share.
    * @param result The error code of this share.
    */
    void PostFormShareSendResponse(int64_t formShareRequestCode, int32_t result);

    void PostRenderForm(const FormRecord &formRecord, const Want &want, const sptr<IRemoteObject> &remoteObject);

    void PostStopRenderingForm(const FormRecord &formRecord, const Want &want, const sptr<IRemoteObject> &remoteObject);

    void PostReloadForm(const std::vector<int64_t> &&formIds, const Want &want,
        const sptr<IRemoteObject> &remoteObject);
private:
    /**
     * @brief Acquire form data from form provider.
     * @param formId The Id of the from.
     * @param want The want of the request.
     * @param remoteObject Form provider proxy object.
     */
    void AcquireProviderFormInfo(const int64_t formId, const Want &want, const sptr<IRemoteObject> &remoteObject);

    /**
     * @brief Acquire share form data from form provider(task).
     * @param formId The Id of the form.
     * @param remoteDeviceId The device ID to share.
     * @param want The want of the request.
     * @param remoteObject Form provider proxy object.
     */
    void AcquireShareFormData(int64_t formId, const std::string &remoteDeviceId, const Want &want,
        const sptr<IRemoteObject> &remoteObject);

    /**
     * @brief Notify form provider for delete form.
     * @param formId The Id of the from.
     * @param want The want of the form.
     * @param remoteObject Form provider proxy object.
     */
    void NotifyFormDelete(const int64_t formId, const Want &want, const sptr<IRemoteObject> &remoteObject);

    /**
     * @brief Notify form provider for updating form.
     * @param formId The Id of the from.
     * @param want The want of the form.
     * @param remoteObject Form provider proxy object.
     */
    void NotifyFormUpdate(const int64_t formId, const Want &want, const sptr<IRemoteObject> &remoteObject);

    /**
     * @brief Event notify to form provider.
     *
     * @param formEvents The vector of form ids.
     * @param formVisibleType The form visible type, including FORM_VISIBLE and FORM_INVISIBLE.
     * @param want The want of the form.
     * @param remoteObject The form provider proxy object.
     */
    void EventNotify(const std::vector<int64_t> &formEvents, const int32_t formVisibleType, const Want &want,
        const sptr<IRemoteObject> &remoteObject);

    /**
     * @brief Notify form provider for cast temp form.
     *
     * @param formId The Id of the from.
     * @param want The want of the form.
     * @param remoteObject Form provider proxy object.
     * @return none.
     */
    void NotifyCastTemp(const int64_t formId, const Want &want, const sptr<IRemoteObject> &remoteObject);
    /**
     * @brief Post form data to form host when acquire form..
     * @param formId The Id of the form.
     * @param record form record.
     * @param remoteObject Form provider proxy object.
     */
    void AcquireTaskToHost(const int64_t formId, const FormRecord &record, const sptr<IRemoteObject> &remoteObject);

    /**
     * @brief Post form data to form host when update form.
     * @param formId The Id of the form.
     * @param record form record.
     * @param remoteObject Form provider proxy object.
     */
    void UpdateTaskToHost(const int64_t formId, const FormRecord &record, const sptr<IRemoteObject> &remoteObject);

    /**
     * @brief Handle form host died.
     * @param remoteHost Form host proxy object.
     */
    void HostDied(const sptr<IRemoteObject> &remoteHost);

    /**
     * @brief Post provider batch delete.
     * @param formIds The Id list.
     * @param want The want of the request.
     * @param remoteObject Form provider proxy object.
     */
    void ProviderBatchDelete(std::set<int64_t> &formIds, const Want &want, const sptr<IRemoteObject> &remoteObject);
    /**
     * @brief Fire message event to form provider.
     * @param formId The Id of the from.
     * @param message Event message.
     * @param want The want of the request.
     * @param remoteObject Form provider proxy object.
     */
    void FireFormEvent(const int64_t formId, const std::string &message, const Want &want,
        const sptr<IRemoteObject> &remoteObject);

    /**
     * @brief Acquire form state to form provider.
     * @param wantArg The want of onAcquireFormState.
     * @param provider The provider info.
     * @param want The want of the request.
     * @param remoteObject Form provider proxy object.
     */
    void AcquireState(const Want &wantArg, const std::string &provider, const Want &want,
                      const sptr <IRemoteObject> &remoteObject);

    /**
     * @brief Handle uninstall message.
     * @param formIds The Id list of the forms.
     * @param remoteObject Form provider proxy object.
     */
    void FormUninstall(const std::vector<int64_t> &formIds, const sptr<IRemoteObject> &remoteObject);

    /**
     * @brief Handle acquire state.
     * @param state the form state.
     * @param want The want of onAcquireFormState.
     * @param remoteObject Form provider proxy object.
     */
    void AcquireStateBack(AppExecFwk::FormState state, const AAFwk::Want &want,
                          const sptr <IRemoteObject> &remoteObject);

    /**
     * @brief Create form data for form host.
     * @param formId The Id of the form.
     * @param record Form record.
     * @return Form data.
     */
    FormJsInfo CreateFormJsInfo(const int64_t formId, const FormRecord &record);

    /**
    * @brief Post form share error code to form host(task).
    * @param formShareRequestCode The request code for this share.
    * @param result The error code of this share.
    */
    void FormShareSendResponse(int64_t formShareRequestCode, int32_t result);

    /**
    * @brief Post form share error code to form host(task).
    * @param formShareRequestCode The request code for this share.
    * @param result The error code of this share.
    */
    void RenderForm(const FormRecord &formRecord, const Want &want, const sptr<IRemoteObject> &remoteObject);

    void StopRenderingForm(const FormRecord &formRecord, const Want &want, const sptr<IRemoteObject> &remoteObject);

    void ReloadForm(const std::vector<int64_t> &&formIds, const Want &want, const sptr<IRemoteObject> &remoteObject);
private:
    std::shared_ptr<FormEventHandler> eventHandler_ = nullptr;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif // OHOS_FORM_FWK_FORM_TASK_MGR_H
