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

#ifndef OHOS_IDLTESTSERVICESTUB_H
#define OHOS_IDLTESTSERVICESTUB_H

#include <iremote_stub.h>

#include "iidl_test_service.h"

namespace OHOS {
class IdlTestServiceStub : public IRemoteStub<IIdlTestService> {
public:
    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;

private:
    static constexpr int COMMAND_TEST_INT_TRANSACTION = MIN_TRANSACTION_ID + 0;
    static constexpr int COMMAND_TEST_STRING_TRANSACTION = MIN_TRANSACTION_ID + 1;
};
} // namespace OHOS
#endif // OHOS_IDLTESTSERVICESTUB_H

