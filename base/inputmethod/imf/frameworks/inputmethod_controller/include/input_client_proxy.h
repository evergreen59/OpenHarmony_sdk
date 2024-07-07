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

#ifndef FRAMEWORKS_INPUTMETHOD_CONTROLLER_INCLUDE_INPUT_CLIENT_PROXY_H
#define FRAMEWORKS_INPUTMETHOD_CONTROLLER_INCLUDE_INPUT_CLIENT_PROXY_H

#include <cstdint>
#include <functional>

#include "i_input_client.h"
#include "i_input_method_agent.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "iremote_proxy.h"
#include "nocopyable.h"
#include "refbase.h"

namespace OHOS {
namespace MiscServices {
    class InputClientProxy : public IRemoteProxy<IInputClient> {
    public:
        explicit InputClientProxy(const sptr<IRemoteObject> &object);
        ~InputClientProxy() = default;
        DISALLOW_COPY_AND_MOVE(InputClientProxy);

        int32_t onInputReady(const sptr<IInputMethodAgent>& agent) override;
        int32_t OnSwitchInput(const Property &property, const SubProperty &subProperty) override;

    private:
        static inline BrokerDelegator<InputClientProxy> delegator_;
        using ParcelHandler = std::function<bool(MessageParcel &)>;
        int32_t SendRequest(int code, ParcelHandler input = nullptr, ParcelHandler output = nullptr);
    };
} // namespace MiscServices
} // namespace OHOS
#endif // FRAMEWORKS_INPUTMETHOD_CONTROLLER_INCLUDE_INPUT_CLIENT_PROXY_H
