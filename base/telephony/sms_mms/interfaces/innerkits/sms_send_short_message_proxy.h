/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef SMS_SEND_SHORT_MESSAGE_PROXY_H
#define SMS_SEND_SHORT_MESSAGE_PROXY_H

#include "i_send_short_message_callback.h"
#include "iremote_object.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Telephony {
class SmsSendShortMessageProxy : public IRemoteProxy<ISendShortMessageCallback> {
public:
    explicit SmsSendShortMessageProxy(const sptr<IRemoteObject> &impl);
    virtual ~SmsSendShortMessageProxy() = default;

    /**
     * @brief OnSmsDeliveryResult
     * Status of sending SMS messages to the SMS service center.
     * @param result [in]
     */
    void OnSmsSendResult(const SmsSendResult result) override;

private:
    static inline BrokerDelegator<SmsSendShortMessageProxy> delegator_;
};
} // namespace Telephony
} // namespace OHOS
#endif // SEND_SHORT_MESSAGE_CALLBACK_PROXY_H