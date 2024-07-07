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

#ifndef SEND_SHORT_MESSAGE_CALLBACK_STUB_H
#define SEND_SHORT_MESSAGE_CALLBACK_STUB_H

#include <cstdint>

#include "iremote_stub.h"

#include "i_send_short_message_callback.h"

namespace OHOS {
namespace Telephony {
class SendShortMessageCallbackStub : public IRemoteStub<ISendShortMessageCallback> {
public:
    SendShortMessageCallbackStub() = default;
    virtual ~SendShortMessageCallbackStub() = default;
    virtual void OnSmsSendResult(const ISendShortMessageCallback::SmsSendResult result) override;
    int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override final;

private:
    static constexpr int SMS_DEFAULT_RESULT = 0;
    static constexpr int SMS_DEFAULT_ERROR = -1;
};
} // namespace Telephony
} // namespace OHOS
#endif // I_SEND_SHORT_MESSAGE_CALLBACK_H