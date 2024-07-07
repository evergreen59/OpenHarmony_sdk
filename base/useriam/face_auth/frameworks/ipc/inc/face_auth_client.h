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

#ifndef FACE_AUTH_CLIENT_H
#define FACE_AUTH_CLIENT_H

#include <list>

#include "singleton.h"

#include "iface_auth.h"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
class FaceAuthClient : public IRemoteObject::DeathRecipient, public Singleton<FaceAuthClient> {
public:
    void ResetFaceAuthProxy();
    int32_t SetBufferProducer(sptr<IBufferProducer> &producer);
    void OnRemoteDied(const wptr<IRemoteObject> &remote) override;

private:
    sptr<IFaceAuth> GetFaceAuthProxy();
    std::mutex mutex_;
    sptr<IFaceAuth> faceAuthProxy_;
};
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

#endif // FACE_AUTH_CLIENT_H