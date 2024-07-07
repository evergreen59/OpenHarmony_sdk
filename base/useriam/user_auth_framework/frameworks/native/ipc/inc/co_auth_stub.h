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

#ifndef CO_AUTH_STUB_H
#define CO_AUTH_STUB_H

#include "co_auth_interface.h"

#include <iremote_stub.h>
#include <message_parcel.h>

namespace OHOS {
namespace UserIam {
namespace UserAuth {
class CoAuthStub : public IRemoteStub<CoAuthInterface> {
public:
    static constexpr uint64_t INVALID_EXECUTOR_INDEX = 0;
    CoAuthStub() = default;
    ~CoAuthStub() override = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int32_t ExecutorRegisterStub(MessageParcel &data, MessageParcel &reply);
    int32_t ReadExecutorRegisterInfo(ExecutorRegisterInfo &executorInfo, MessageParcel &data);
};
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS
#endif // CO_AUTH_STUB_H