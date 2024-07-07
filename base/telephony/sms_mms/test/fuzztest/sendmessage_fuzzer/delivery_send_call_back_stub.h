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

#ifndef DELIVERY_SEND_CALL_BACK_STUB_H
#define DELIVERY_SEND_CALL_BACK_STUB_H

#include "delivery_short_message_callback_stub.h"
#include "napi/native_api.h"

namespace OHOS {
namespace Telephony {
class DeliverySendCallbackStub : public DeliveryShortMessageCallbackStub {
public:
    DeliverySendCallbackStub() = default;
    ~DeliverySendCallbackStub() = default;
    void OnSmsDeliveryResult(const std::u16string &pdu) override;
};
}  // namespace Telephony
}  // namespace OHOS

#endif  // DELIVERY_SEND_CALL_BACK_STUB_H