/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ABILITYRUNTIME_CONNECTION_OBSERVER_STUB_H
#define OHOS_ABILITYRUNTIME_CONNECTION_OBSERVER_STUB_H

#include <vector>

#include "iconnection_observer.h"
#include "iremote_stub.h"
#include "nocopyable.h"

namespace OHOS {
namespace AbilityRuntime {
/**
 * interface for connection observer.
 */
class ConnectionObserverStub : public IRemoteStub<IConnectionObserver> {
public:
    ConnectionObserverStub();
    virtual ~ConnectionObserverStub() = default;

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    DISALLOW_COPY_AND_MOVE(ConnectionObserverStub);

    int OnExtensionConnectedInner(MessageParcel &data, MessageParcel &reply);
    int OnExtensionDisconnectedInner(MessageParcel &data, MessageParcel &reply);
    int OnDlpAbilityOpenedInner(MessageParcel &data, MessageParcel &reply);
    int OnDlpAbilityClosedInner(MessageParcel &data, MessageParcel &reply);

    using ConnectionObserverFunc = int (ConnectionObserverStub::*)(MessageParcel &data, MessageParcel &reply);
    std::vector<ConnectionObserverFunc> vecMemberFunc_;
};
}  // namespace AbilityRuntime
}  // namespace OHOS
#endif  // OHOS_ABILITYRUNTIME_CONNECTION_OBSERVER_STUB_H
