/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_CAMERA_HSTREAM_REPEAT_PROXY_H
#define OHOS_CAMERA_HSTREAM_REPEAT_PROXY_H

#include "iremote_proxy.h"
#include "istream_repeat.h"

namespace OHOS {
namespace CameraStandard {
class HStreamRepeatProxy : public IRemoteProxy<IStreamRepeat> {
public:
    explicit HStreamRepeatProxy(const sptr<IRemoteObject> &impl);
    virtual ~HStreamRepeatProxy() = default;

    int32_t Start() override;

    int32_t Stop() override;

    int32_t SetCallback(sptr<IStreamRepeatCallback> &callback) override;

    int32_t Release() override;

    int32_t AddDeferredSurface(const sptr<OHOS::IBufferProducer> &producer) override;

private:
    static inline BrokerDelegator<HStreamRepeatProxy> delegator_;
};
} // namespace CameraStandard
} // namespace OHOS
#endif // OHOS_CAMERA_HSTREAM_REPEAT_PROXY_H
