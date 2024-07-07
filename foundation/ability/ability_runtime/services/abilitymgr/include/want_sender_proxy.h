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

#ifndef OHOS_ABILITY_RUNTIME_WANT_SENDER_PROXY_H
#define OHOS_ABILITY_RUNTIME_WANT_SENDER_PROXY_H

#include <iremote_proxy.h>
#include "want_sender_interface.h"

namespace OHOS {
namespace AAFwk {
class WantSenderProxy : public IRemoteProxy<IWantSender> {
public:
    explicit WantSenderProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IWantSender>(impl)
    {}

    virtual ~WantSenderProxy()
    {}

    virtual void Send(SenderInfo &senderInfo) override;

private:
    bool WriteInterfaceToken(MessageParcel &data);

private:
    static inline BrokerDelegator<WantSenderProxy> delegator_;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_WANT_SENDER_PROXY_H
