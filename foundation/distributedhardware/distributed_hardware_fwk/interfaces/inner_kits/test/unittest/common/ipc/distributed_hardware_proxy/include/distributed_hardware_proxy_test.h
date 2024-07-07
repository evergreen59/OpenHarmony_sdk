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

#ifndef OHOS_DISTRIBUTED_HARDWARE_PROXY_TEST_H
#define OHOS_DISTRIBUTED_HARDWARE_PROXY_TEST_H

#include <cstdint>
#include <gtest/gtest.h>
#include <mutex>

#include "device_type.h"
#include "distributed_hardware_errno.h"
#include "idistributed_hardware.h"
#define private public
#include "distributed_hardware_proxy.h"
#undef private

namespace OHOS {
namespace DistributedHardware {
class DistributedHardwareProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp();
    virtual void TearDown();

    std::shared_ptr<DistributedHardwareProxy> hardwareProxy_ = nullptr;
};

class MockIDistributedHardware : public IDistributedHardware {
public:
    sptr<IRemoteObject> AsObject()
    {
        return nullptr;
    }

    int32_t RegisterPublisherListener(const DHTopic topic, const sptr<IPublisherListener> &listener)
    {
        (void)topic;
        (void)listener;
        return DH_FWK_SUCCESS;
    }

    int32_t UnregisterPublisherListener(const DHTopic topic, const sptr<IPublisherListener> &listener)
    {
        (void)topic;
        (void)listener;
        return DH_FWK_SUCCESS;
    }

    int32_t PublishMessage(const DHTopic topic, const std::string &msg)
    {
        (void)topic;
        (void)msg;
        return DH_FWK_SUCCESS;
    }
};

class MockIPublisherListener : public IPublisherListener {
public:
    sptr<IRemoteObject> AsObject()
    {
        return nullptr;
    }

    void OnMessage(const DHTopic topic, const std::string& message)
    {
        (void)topic;
        (void)message;
    }
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_HARDWARE_PROXY_TEST_H