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

#ifndef IMS_SMS_H
#define IMS_SMS_H

#include "ims_base.h"
#include "ims_sms_callback_interface.h"
#include "ims_sms_stub.h"

namespace OHOS {
namespace Telephony {
class ImsSms : public ImsSmsStub, public ImsBase, public std::enable_shared_from_this<ImsSms> {
public:
    ImsSms();

    virtual ~ImsSms();

    bool Init() override;

    /*
     * @param slotId
     * @param imsMessageInfo
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t ImsSendMessage(int32_t slotId, const ImsMessageInfo &imsMessageInfo) override;

    /**
     * IMS set ims sms config interface
     *
     * @param slotId
     * @param imsSmsConfig 1:enable ims sms, 0:disable ims sms
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t ImsSetSmsConfig(int32_t slotId, int32_t imsSmsConfig) override;

    /**
     * IMS get sms config interface
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t ImsGetSmsConfig(int32_t slotId) override;

    /**
     * Register CallBack
     *
     * @param sptr<ImsSmsCallbackInterface>
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t RegisterImsSmsCallback(const sptr<ImsSmsCallbackInterface> &callback) override;

private:
    sptr<ImsSmsCallbackInterface> imsSmsCallback_ = nullptr;
};
} // namespace Telephony
} // namespace OHOS
#endif // IMS_SMS_H
