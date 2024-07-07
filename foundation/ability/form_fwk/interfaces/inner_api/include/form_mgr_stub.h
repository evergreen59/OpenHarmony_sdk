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

#ifndef OHOS_FORM_FWK_FORM_MGR_STUB_H
#define OHOS_FORM_FWK_FORM_MGR_STUB_H

#include <map>

#include "form_mgr_interface.h"
#include "iremote_object.h"
#include "iremote_stub.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * @class FormMgrStub
 * Form manager service stub.
 */
class FormMgrStub : public IRemoteStub<IFormMgr> {
public:
    FormMgrStub();
    virtual ~FormMgrStub();
    /**
     * @brief Handle remote request.
     * @param data input param.
     * @param reply output param.
     * @param option message option.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);

private:
    /**
     * @brief Handle AddForm message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleAddForm(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle DeleteForm message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleDeleteForm(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle StopRenderingForm message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleStopRenderingForm(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle ReleaseForm message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleReleaseForm(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle UpdateForm message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleUpdateForm(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief handle LifecycleUpdate message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleLifecycleUpdate(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief handle SetNextRefreshTime message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleSetNextRefreshTime(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief handle RequestPublishForm message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode HandleRequestPublishForm(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle RequestForm message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleRequestForm(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle NotifyWhetherVisibleForms message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleNotifyWhetherVisibleForms(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle CastTempForm message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleCastTempForm(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle DumpStorageFormInfos message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleDumpStorageFormInfos(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle DumpFormInfoByBundleName message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleDumpFormInfoByBundleName(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle DumpFormInfoByFormId message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleDumpFormInfoByFormId(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle DumpFormTimerByFormId message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleDumpFormTimerByFormId(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle DumpFormInfoByFormId message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleMessageEvent(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle HandleRouterEvent message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleRouterEvent(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle Background message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleBackgroundEvent(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle DeleteInvalidForms message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleDeleteInvalidForms(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle AcquireFormState message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleAcquireFormState(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle NotifyFormsVisible message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleNotifyFormsVisible(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle NotifyFormsPrivacyProtected message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleNotifyFormsPrivacyProtected(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle NotifyFormsEnableUpdate message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleNotifyFormsEnableUpdate(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle GetAllFormsInfo message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleGetAllFormsInfo(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle GetFormsInfoByApp message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleGetFormsInfoByApp(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle GetFormsInfoByModule message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleGetFormsInfoByModule(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handle GetFormsInfo message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleGetFormsInfo(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle share form message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleShareForm(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Receive form sharing information from remote.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleRecvFormShareInfoFromRemote(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle IsRequestPublishFormSupported message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleIsRequestPublishFormSupported(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle StartAbility message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleStartAbility(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Check form manager service ready
     * @param data input param
     * @param reply output param
     * @return Return ERR_OK on success, others on failure
    */
    int32_t HandleCheckFMSReady(MessageParcel &data, MessageParcel &reply);
private:
    using FormMgrFunc = int32_t (FormMgrStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, FormMgrFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(FormMgrStub);

    /**
     * @brief Write a parcelabe vector objects to the proxy node.
     * @param parcelableVector Indicates the objects to be write.
     * @param reply Indicates the reply to be sent;
     * @return Returns true if objects send successfully; returns false otherwise.
     */
    template<typename T>
    bool WriteParcelableVector(std::vector<T> &parcelableVector, Parcel &reply);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_FORM_FWK_FORM_MGR_STUB_H
