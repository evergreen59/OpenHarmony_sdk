/**
 * Copyright 2021 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "src/common/context_util.h"
#include <map>
#include <memory>
#include <set>
#include <string>
#include "src/common/log_adapter.h"

namespace mindspore {
namespace lite {
namespace {
template <class T>
void PassBasicProperties(std::shared_ptr<T> device_info, const lite::DeviceContext &device_context) {
  MS_ASSERT(device_info != nullptr);
  device_info->SetProvider(device_context.provider_);
  device_info->SetProviderDevice(device_context.provider_device_);
  device_info->SetAllocator(device_context.allocator_);
}

std::shared_ptr<mindspore::CPUDeviceInfo> CPUDeviceInfoFromCPUDeviceContext(const lite::DeviceContext &cpu_context) {
  if (cpu_context.device_type_ != DT_CPU) {
    MS_LOG(ERROR) << "function input parameter is not cpu context.";
    return nullptr;
  }
  auto cpu_info = std::make_shared<mindspore::CPUDeviceInfo>();
  MS_CHECK_TRUE_RET(cpu_info != nullptr, nullptr);
  cpu_info->SetEnableFP16(cpu_context.device_info_.cpu_device_info_.enable_float16_);
  PassBasicProperties(cpu_info, cpu_context);
  return cpu_info;
}

std::shared_ptr<mindspore::GPUDeviceInfo> GPUDeviceInfoFromGPUDeviceContext(const lite::DeviceContext &gpu_context) {
  if (gpu_context.device_type_ != DT_GPU) {
    MS_LOG(ERROR) << "function input parameter is not gpu context.";
    return nullptr;
  }
  auto gpu_info = std::make_shared<mindspore::GPUDeviceInfo>();
  MS_CHECK_TRUE_RET(gpu_info != nullptr, nullptr);
  gpu_info->SetEnableFP16(gpu_context.device_info_.gpu_device_info_.enable_float16_);
  gpu_info->SetDeviceID(gpu_context.device_info_.gpu_device_info_.gpu_device_id_);
  PassBasicProperties(gpu_info, gpu_context);
  return gpu_info;
}

std::shared_ptr<mindspore::KirinNPUDeviceInfo> NPUDeviceInfoFromNPUDeviceContext(
  const lite::DeviceContext &npu_context) {
  if (npu_context.device_type_ != DT_NPU) {
    MS_LOG(ERROR) << "function input parameter is not npu context.";
    return nullptr;
  }
  auto npu_info = std::make_shared<mindspore::KirinNPUDeviceInfo>();
  MS_CHECK_TRUE_RET(npu_info != nullptr, nullptr);
  npu_info->SetFrequency(npu_context.device_info_.npu_device_info_.frequency_);
  PassBasicProperties(npu_info, npu_context);
  return npu_info;
}

std::shared_ptr<mindspore::Ascend310DeviceInfo> Ascend310DeviceInfoFromAscend310DeviceContext(
  const lite::DeviceContext &ascend310_context) {
  if (ascend310_context.device_type_ != DT_ASCEND310) {
    MS_LOG(ERROR) << "Function input parameter is not ascend310 context.";
    return nullptr;
  }
  auto ascend310_info = std::make_shared<mindspore::Ascend310DeviceInfo>();
  MS_CHECK_TRUE_RET(ascend310_info != nullptr, nullptr);
  ascend310_info->SetDeviceID(ascend310_context.device_info_.ascend310_device_info_.device_id_);
  return ascend310_info;
}

std::shared_ptr<mindspore::NNRTDeviceInfo> NNRtDeviceInfoFromNNRtDeviceContext(
  const lite::DeviceContext &nnrt_context) {
  if (nnrt_context.device_type_ != DT_NNRT) {
    MS_LOG(ERROR) << "Function input parameter is not NNRt context.";
    return nullptr;
  }
  auto nnrt_info = std::make_shared<mindspore::NNRTDeviceInfo>();
  MS_CHECK_TRUE_RET(nnrt_info != nullptr, nullptr);
  return nnrt_info;
}
}  // namespace

mindspore::Context *MSContextFromContext(const lite::Context *context) {
  if (context == nullptr) {
    MS_LOG(ERROR) << "context is nullptr";
    return nullptr;
  }
  auto ms_context = new (std::nothrow) mindspore::Context();
  if (ms_context == nullptr) {
    MS_LOG(ERROR) << "New Context failed";
    return nullptr;
  }
  ms_context->SetThreadNum(context->thread_num_);
  ms_context->SetThreadAffinity(context->affinity_core_list_);
  ms_context->SetEnableParallel(context->enable_parallel_);
  ms_context->SetDelegate(context->delegate);
  auto &device_infos = ms_context->MutableDeviceInfo();
  std::map<DeviceType, std::function<std::shared_ptr<mindspore::DeviceInfoContext>(const lite::DeviceContext &)>>
    transfer_funcs = {{DT_CPU, CPUDeviceInfoFromCPUDeviceContext},
                      {DT_GPU, GPUDeviceInfoFromGPUDeviceContext},
                      {DT_NPU, NPUDeviceInfoFromNPUDeviceContext},
                      {DT_ASCEND310, Ascend310DeviceInfoFromAscend310DeviceContext},
                      {DT_NNRT, NNRtDeviceInfoFromNNRtDeviceContext}};
  for (auto &device_context : context->device_list_) {
    auto device_type = device_context.device_type_;
    if (transfer_funcs.find(device_type) == transfer_funcs.end()) {
      MS_LOG(ERROR) << "device type is invalid.";
      return nullptr;
    }
    auto device_info = transfer_funcs[device_type](device_context);
    if (device_info == nullptr) {
      MS_LOG(ERROR) << "transfer device context to device info failed.";
      return nullptr;
    }
    if (device_type == DT_CPU) {
      ms_context->SetThreadAffinity(device_context.device_info_.cpu_device_info_.cpu_bind_mode_);
    }
    device_infos.push_back(device_info);
  }
  return ms_context;
}

bool DeviceTypePriority(const lite::Context *context, int device_type1, int device_type2) {
  /* dt1 > dt2    true
   * dt1 < dt2    false    */

  if (context == nullptr) {
    return false;
  }
  DeviceContextVector device_infos = context->device_list_;
  for (DeviceContext device_info : device_infos) {
    if (device_info.device_type_ == device_type1) {
      return true;
    }
    if (device_info.device_type_ == device_type2) {
      return false;
    }
  }
  return false;
}

DeviceType KernelArchToDeviceType(kernel::KERNEL_ARCH kernel_arch) {
  switch (kernel_arch) {
    case kernel::KERNEL_ARCH::kCPU:
      return DT_CPU;
    case kernel::KERNEL_ARCH::kGPU:
      return DT_GPU;
    case kernel::KERNEL_ARCH::kNPU:
      return DT_NPU;
    default:
      return DT_CPU;
  }
}
}  // namespace lite
}  // namespace mindspore
