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
#include "const.h"
#include "mock_idevice.h"

namespace OHOS {
namespace HDI {
namespace Nnrt {
namespace V1_0 {

sptr<INnrtDevice> INnrtDevice::Get(bool isStub)
{
    return INnrtDevice::Get("mock_device_service", isStub);
}

sptr<INnrtDevice> INnrtDevice::Get(const std::string &serviceName, bool isStub)
{
    if (isStub) {
        return nullptr;
    }
    sptr<INnrtDevice> mockIDevice = sptr<MockIDevice>(MockIDevice::GetInstance());
    return mockIDevice;
}

MockIDevice::~MockIDevice()
{
    for (auto ash : m_ashmems) {
        ash.second->UnmapAshmem();
        ash.second->CloseAshmem();
    }
}

MockIDevice *MockIDevice::GetInstance()
{
    static MockIDevice iDevice;
    return &iDevice;
}

void MockIDevice::SetFP16Supported(bool isSupported)
{
    m_fp16 = isSupported;
}

void MockIDevice::SetPerformanceSupported(bool isSupported)
{
    m_performance = isSupported;
}

void MockIDevice::SetPrioritySupported(bool isSupported)
{
    m_priority = isSupported;
}

void MockIDevice::SetModelCacheSupported(bool isSupported)
{
    m_cache = isSupported;
}

void MockIDevice::SetOperationsSupported(std::vector<bool> isSupported)
{
    m_operations = isSupported;
}

void MockIDevice::SetDynamicInputSupported(bool isSupported)
{
    m_dynamic = isSupported;
}

int32_t MockIDevice::GetDeviceName(std::string& name)
{
    name = "Device-CPU";
    return HDF_SUCCESS;
}

int32_t MockIDevice::GetVendorName(std::string& name)
{
    name = "TestVendor";
    return HDF_SUCCESS;
}

int32_t MockIDevice::GetDeviceType(DeviceType& deviceType)
{
    deviceType = DeviceType::CPU;
    return HDF_SUCCESS;
}

int32_t MockIDevice::GetDeviceStatus(DeviceStatus& status)
{
    status = DeviceStatus::AVAILABLE;
    return HDF_SUCCESS;
}

int32_t MockIDevice::GetVersion(uint32_t &majorVersion, uint32_t &minorVersion)
{
    majorVersion = 1;
    minorVersion = 0;
    return HDF_SUCCESS;
}

int32_t MockIDevice::GetSupportedOperation(const Model& model, std::vector<bool>& ops)
{
    ops = m_operations;
    return HDF_SUCCESS;
}

int32_t MockIDevice::IsFloat16PrecisionSupported(bool& isSupported)
{
    isSupported = m_fp16;
    return HDF_SUCCESS;
}

int32_t MockIDevice::IsPerformanceModeSupported(bool& isSupported)
{
    isSupported = m_performance;
    return HDF_SUCCESS;
}

int32_t MockIDevice::IsPrioritySupported(bool& isSupported)
{
    isSupported = m_priority;
    return HDF_SUCCESS;
}

int32_t MockIDevice::IsDynamicInputSupported(bool& isSupported)
{
    isSupported = m_dynamic;
    return HDF_SUCCESS;
}

int32_t MockIDevice::IsModelCacheSupported(bool& isSupported)
{
    isSupported = m_cache;
    return HDF_SUCCESS;
}

int32_t MockIDevice::AllocateBuffer(uint32_t length, SharedBuffer &buffer)
{
    std::lock_guard<std::mutex> lock(m_mtx);
    sptr<Ashmem> ashptr = Ashmem::CreateAshmem("allocateBuffer", length);
    if (ashptr == nullptr) {
        LOGE("[NNRtTest] Create shared memory failed.");
        return HDF_FAILURE;
    }

    if (!ashptr->MapReadAndWriteAshmem()) {
        LOGE("[NNRtTest] Map allocate buffer failed.");
        return HDF_FAILURE;
    }

    buffer.fd = ashptr->GetAshmemFd();
    buffer.bufferSize = ashptr->GetAshmemSize();
    buffer.offset = 0;
    buffer.dataSize = length;

    m_ashmems[buffer.fd] = ashptr;
    m_bufferFd = buffer.fd;
    return HDF_SUCCESS;
}

int32_t MockIDevice::ReleaseBuffer(const SharedBuffer &buffer)
{
    std::lock_guard<std::mutex> lock(m_mtx);
    auto ash = m_ashmems[buffer.fd];
    ash->UnmapAshmem();
    return HDF_SUCCESS;
}

int32_t MockIDevice::MemoryCopy(float* data, uint32_t length)
{
    std::lock_guard<std::mutex> lock(m_mtx);
    auto memManager = NeuralNetworkRuntime::MemoryManager::GetInstance();
    auto memAddress = memManager->MapMemory(m_bufferFd, length);
    if (memAddress == nullptr) {
        LOGE("[NNRtTest] Map fd to address failed.");
        return HDF_FAILURE;
    }
    auto ret = memcpy_s(memAddress, length, data, length);
    if (ret != EOK) {
        LOGE("[NNRtTest] MockIDevice memory cop failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t MockIDevice::PrepareModel(const Model& model, const ModelConfig& config, sptr<IPreparedModel>& preparedModel)
{
    preparedModel = new (std::nothrow) V1_0::MockIPreparedModel();
    return HDF_SUCCESS;
}

int32_t MockIDevice::PrepareModelFromModelCache(const std::vector<SharedBuffer>& modelCache, const ModelConfig& config,
    sptr<IPreparedModel>& preparedModel)
{
    preparedModel = new (std::nothrow) V1_0::MockIPreparedModel();
    return HDF_SUCCESS;
}

int32_t MockIPreparedModel::ExportModelCache(std::vector<SharedBuffer>& modelCache)
{
    if (!modelCache.empty()) {
        LOGE("[NNRtTest] The parameters of ExportModelCache should be an empty vector.");
        return HDF_ERR_INVALID_PARAM;
    }
    uint8_t buffer[4] = {0, 1, 2, 3};
    uint32_t size = sizeof(buffer);
    sptr<Ashmem> cache = Ashmem::CreateAshmem("cache", size);
    if (cache == nullptr) {
        LOGE("[NNRtTest] Create shared memory failed.");
        return HDF_ERR_MALLOC_FAIL;
    }
    bool ret = cache->MapReadAndWriteAshmem();
    if (!ret) {
        LOGE("[NNRtTest] Map fd to write cache failed.");
        return HDF_FAILURE;
    }
    
    ret = cache->WriteToAshmem(buffer, size, 0);
    cache->UnmapAshmem();
    if (!ret) {
        LOGE("[NNRtTest] Write cache failed.");
        return HDF_FAILURE;
    }
    // SharedBuffer: fd, bufferSize, offset, dataSize
    modelCache.emplace_back(SharedBuffer {cache->GetAshmemFd(), cache->GetAshmemSize(), 0, cache->GetAshmemSize()});
    return HDF_SUCCESS;
}

int32_t MockIPreparedModel::GetVersion(uint32_t &majorVersion, uint32_t &minorVersion)
{
    majorVersion = 1;
    minorVersion = 0;
    return HDF_SUCCESS;
}

int32_t MockIPreparedModel::Run(const std::vector<IOTensor>& inputs, const std::vector<IOTensor>& outputs,
    std::vector<std::vector<int32_t>>& outputsDims, std::vector<bool>& isOutputBufferEnough)
{
    outputsDims = {{1, 2, 2, 1}};
    isOutputBufferEnough = {true};
    return HDF_SUCCESS;
}

} // namespace V1_0
} // namespace Nnrt
} // namespace HDI
} // namespace OHOS
