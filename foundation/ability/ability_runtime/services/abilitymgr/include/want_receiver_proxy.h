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

#ifndef OHOS_ABILITY_RUNTIME_WANT_RECEIVER_PROXY_H
#define OHOS_ABILITY_RUNTIME_WANT_RECEIVER_PROXY_H

#include <iremote_proxy.h>
#include "want_receiver_interface.h"

namespace OHOS {
namespace AAFwk {
class WantReceiverProxy : public IRemoteProxy<IWantReceiver> {
public:
    explicit WantReceiverProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IWantReceiver>(impl)
    {}

    virtual ~WantReceiverProxy()
    {}

    virtual void Send(const int32_t resultCode) override;
    virtual void PerformReceive(const Want &want, int resultCode, const std::string &data, const WantParams &extras,
        bool serialized, bool sticky, int sendingUser) override;

private:
    bool WriteInterfaceToken(MessageParcel &data);
    static inline BrokerDelegator<WantReceiverProxy> delegator_;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_WANT_RECEIVER_PROXY_H
