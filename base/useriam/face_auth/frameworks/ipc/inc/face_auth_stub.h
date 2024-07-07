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

#ifndef FACE_AUTH_STUB_H
#define FACE_AUTH_STUB_H

#include <map>

#include "iremote_stub.h"
#include "nocopyable.h"

#include "iface_auth.h"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
class FaceAuthStub : public IRemoteStub<IFaceAuth>, public NoCopyable {
public:
    FaceAuthStub();
    ~FaceAuthStub() override = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    using Handler = int32_t (FaceAuthStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, Handler> keyToHandle_;
    void RegisterKeyToHandle();
    int32_t FaceAuthSetBufferProducer(MessageParcel &data, MessageParcel &reply);
};
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

#endif // FACE_AUTH_STUB_H