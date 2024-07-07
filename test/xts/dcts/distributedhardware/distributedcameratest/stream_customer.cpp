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

#include "stream_customer.h"
#include "distributed_hardware_log.h"
#include "video_key_info.h"

namespace OHOS {
namespace DistributedHardware {
StreamCustomer::StreamCustomer() {}
StreamCustomer::~StreamCustomer() {}

sptr<OHOS::IBufferProducer> StreamCustomer::CreateProducer(CaptureMode mode,
    const std::function<void(void*, uint32_t)> callback)
{
    consumer_ = OHOS::Surface::CreateSurfaceAsConsumer();
    if (consumer_ == nullptr) {
        return nullptr;
    }
    sptr<IBufferConsumerListener> listener = nullptr;
    if (mode == CAPTURE_PREVIEW) {
    } else if (mode == CAPTURE_SNAPSHOT) {
        listener = new TestBuffersConsumerListener(consumer_, callback);
    } else if (mode == CAPTURE_VIDEO) {
        listener = new TestBuffersConsumerListener(consumer_, callback);
    }
    consumer_->RegisterConsumerListener(listener);

    auto producer = consumer_->GetProducer();
    if (producer == nullptr) {
        return nullptr;
    }

    DHLOGI("demo test, create a buffer queue producer %p", producer.GetRefPtr());
    return producer;
}
}
}