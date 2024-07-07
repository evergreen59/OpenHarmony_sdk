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

#ifndef NETWORK_SEARCH_CALLBACK_PROXY_H
#define NETWORK_SEARCH_CALLBACK_PROXY_H

#include "i_network_search_callback.h"
#include "telephony_errors.h"

namespace OHOS {
namespace Telephony {
class NetworkSearchCallBackProxy : public IRemoteProxy<INetworkSearchCallback> {
public:
    explicit NetworkSearchCallBackProxy(const sptr<IRemoteObject> &impl);
    virtual ~NetworkSearchCallBackProxy() = default;
    int32_t OnNetworkSearchCallback(
        NetworkSearchCallback requestId, MessageParcel &callBackParcel) override;

private:
    static inline BrokerDelegator<NetworkSearchCallBackProxy> delegator_;
};
} // namespace Telephony
} // namespace OHOS
#endif // NETWORK_SEARCH_CALLBACK_PROXY_H
