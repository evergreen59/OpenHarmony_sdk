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

#ifndef OHOS_FORM_FWK_FORM_SUPPLY_STUB_H
#define OHOS_FORM_FWK_FORM_SUPPLY_STUB_H

#include <map>
#include "form_supply_interface.h"
#include "iremote_object.h"
#include "iremote_stub.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * @class FormSupplyStub
 * form supply service stub.
 */
class FormSupplyStub : public IRemoteStub<IFormSupply> {
public:
    FormSupplyStub();
    virtual ~FormSupplyStub();
    /**
     * @brief handle remote request.
     * @param data input param.
     * @param reply output param.
     * @param option message option.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);

private:
    /**
     * @brief handle OnAcquire message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int HandleOnAcquire(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief handle OnEventHandle message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int HandleOnEventHandle(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief handle OnAcquireStateResult message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int HandleOnAcquireStateResult(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief handle OnShareAcquire message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleOnShareAcquire(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief handle OnRenderTaskDone message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleOnRenderTaskDone(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief handle OnStopRenderingTaskDone message.
     * @param data input param.
     * @param reply output param.
     * @return Returns ERR_OK on success, others on failure.
     */
    int32_t HandleOnStopRenderingTaskDone(MessageParcel &data, MessageParcel &reply);
private:
    using FormSupplyFunc = int32_t (FormSupplyStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, FormSupplyFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(FormSupplyStub);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_FORM_FWK_FORM_SUPPLY_STUB_H
