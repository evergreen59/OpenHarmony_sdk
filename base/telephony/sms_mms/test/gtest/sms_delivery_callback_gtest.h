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

#ifndef SMS_DELIVERY_CALLBACK_GTEST_H
#define SMS_DELIVERY_CALLBACK_GTEST_H

#include <cstdint>

#include "i_delivery_short_message_callback.h"
#include "iremote_stub.h"
#include "sms_mms_test_helper.h"

namespace OHOS {
namespace Telephony {
class SmsDeliveryCallbackGTest : public IRemoteStub<IDeliveryShortMessageCallback> {
public:
    explicit SmsDeliveryCallbackGTest(SmsMmsTestHelper &helper);
    ~SmsDeliveryCallbackGTest();
    void OnSmsDeliveryResult(const std::u16string &pdu) override;
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) final;

private:
    SmsMmsTestHelper *callbackHelper_;
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_DELIVERY_CALLBACK_GTEST_H