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

#include "publisher_fuzzer.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unistd.h>

#include "distributed_hardware_errno.h"
#include "publisher.h"

namespace OHOS {
namespace DistributedHardware {
namespace {
    const uint32_t TOPIC_SIZE = 4;
    const DHTopic topicFuzz[TOPIC_SIZE] = {
        DHTopic::TOPIC_START_DSCREEN, DHTopic::TOPIC_SINK_PROJECT_WINDOW_INFO,
        DHTopic::TOPIC_STOP_DSCREEN, DHTopic::TOPIC_DEV_OFFLINE
    };
}

void MockPublisherListener::OnMessage(const DHTopic topic, const std::string &message)
{
    (void)topic;
    (void)message;
}

int32_t MockPublisherListener::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    (void)code;
    (void)data;
    (void)reply;
    (void)option;
    return DH_FWK_SUCCESS;
}

void PublisherListenerFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    DHTopic topic = topicFuzz[data[0] % TOPIC_SIZE];
    sptr<IPublisherListener> listener = new MockPublisherListener();
    std::string message(reinterpret_cast<const char*>(data), size);

    Publisher::GetInstance().RegisterListener(topic, listener);
    Publisher::GetInstance().PublishMessage(topic, message);
    Publisher::GetInstance().UnregisterListener(topic, listener);
}
} // namespace DistributedHardware
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::PublisherListenerFuzzTest(data, size);
    return 0;
}

