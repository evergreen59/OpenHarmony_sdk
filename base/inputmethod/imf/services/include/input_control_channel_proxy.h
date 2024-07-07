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

#ifndef SERVICES_INCLUDE_INPUT_CONTROL_CHANNEL_PROXY_H
#define SERVICES_INCLUDE_INPUT_CONTROL_CHANNEL_PROXY_H

#include "parcel.h"
#include "iremote_broker.h"
#include "message_parcel.h"
#include "message_handler.h"

#include "global.h"
#include "iremote_proxy.h"
#include "i_input_control_channel.h"
#include "i_input_method_agent.h"

namespace OHOS {
namespace MiscServices {
    class InputControlChannelProxy : public IRemoteProxy<IInputControlChannel> {
    public:
        InputControlChannelProxy(const sptr<IRemoteObject> &impl);
        ~InputControlChannelProxy();

        int32_t HideKeyboardSelf(int flags) override;
    };
} // namespace MiscServices
} // namespace OHOS
#endif // SERVICES_INCLUDE_INPUT_CONTROL_CHANNEL_PROXY_H
