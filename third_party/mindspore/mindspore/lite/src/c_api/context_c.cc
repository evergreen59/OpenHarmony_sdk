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
#include "include/c_api/context_c.h"
#include "src/c_api/context_c.h"
#include "src/common/log_adapter.h"

// ================ Context ================
OH_AI_ContextHandle OH_AI_ContextCreate() {
  auto impl = new (std::nothrow) mindspore::ContextC;
  if (impl == nullptr) {
    MS_LOG(ERROR) << "memory allocation failed.";
    return nullptr;
  }
  return static_cast<OH_AI_ContextHandle>(impl);
}

void OH_AI_ContextDestroy(OH_AI_ContextHandle *context) {
  if (context != nullptr && *context != nullptr) {
    auto impl = static_cast<mindspore::ContextC *>(*context);
    delete impl;
    *context = nullptr;
  }
}

void OH_AI_ContextSetThreadNum(OH_AI_ContextHandle context, int32_t thread_num) {
  if (context == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return;
  }
  auto impl = static_cast<mindspore::ContextC *>(context);
  impl->thread_num = thread_num;
}

int32_t OH_AI_ContextGetThreadNum(const OH_AI_ContextHandle context) {
  if (context == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return 0;
  }
  auto impl = static_cast<mindspore::ContextC *>(context);
  return impl->thread_num;
}

void OH_AI_ContextSetThreadAffinityMode(OH_AI_ContextHandle context, int mode) {
  if (context == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return;
  }
  auto impl = static_cast<mindspore::ContextC *>(context);
  impl->affinity_mode = mode;
  return;
}

int OH_AI_ContextGetThreadAffinityMode(const OH_AI_ContextHandle context) {
  if (context == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return 0;
  }
  auto impl = static_cast<mindspore::ContextC *>(context);
  return impl->affinity_mode;
}

void OH_AI_ContextSetThreadAffinityCoreList(OH_AI_ContextHandle context, const int32_t *core_list, size_t core_num) {
  if (context == nullptr || core_list == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return;
  }
  const std::vector<int32_t> vec_core_list(core_list, core_list + core_num);
  auto impl = static_cast<mindspore::ContextC *>(context);
  impl->affinity_core_list = vec_core_list;
  return;
}

const int32_t *OH_AI_ContextGetThreadAffinityCoreList(const OH_AI_ContextHandle context, size_t *core_num) {
  if (context == nullptr || core_num == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return nullptr;
  }
  auto impl = static_cast<mindspore::ContextC *>(context);
  *core_num = impl->affinity_core_list.size();
  return impl->affinity_core_list.data();
}

void OH_AI_ContextSetEnableParallel(OH_AI_ContextHandle context, bool is_parallel) {
  if (context == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return;
  }
  auto impl = static_cast<mindspore::ContextC *>(context);
  impl->enable_parallel = is_parallel;
}

bool OH_AI_ContextGetEnableParallel(const OH_AI_ContextHandle context) {
  if (context == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return false;
  }
  auto impl = static_cast<mindspore::ContextC *>(context);
  return impl->enable_parallel;
}

void OH_AI_ContextAddDeviceInfo(OH_AI_ContextHandle context, OH_AI_DeviceInfoHandle device_info) {
  if (context == nullptr || device_info == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return;
  }
  auto impl = static_cast<mindspore::ContextC *>(context);
  std::shared_ptr<mindspore::DeviceInfoC> device(static_cast<mindspore::DeviceInfoC *>(device_info));
  impl->device_info_list.push_back(device);
}

// ================ DeviceInfo ================
OH_AI_DeviceInfoHandle OH_AI_DeviceInfoCreate(OH_AI_DeviceType device_type) {
  mindspore::DeviceInfoC *impl = new (std::nothrow) mindspore::DeviceInfoC;
  if (impl == nullptr) {
    MS_LOG(ERROR) << "memory allocation failed.";
    return nullptr;
  }
  impl->device_type = device_type;
  return static_cast<OH_AI_DeviceInfoHandle>(impl);
}

void OH_AI_DeviceInfoDestroy(OH_AI_DeviceInfoHandle *device_info) {
  if (device_info != nullptr && *device_info != nullptr) {
    auto impl = static_cast<mindspore::DeviceInfoC *>(*device_info);
    delete impl;
    *device_info = nullptr;
  }
}

void OH_AI_DeviceInfoSetProvider(OH_AI_DeviceInfoHandle device_info, const char *provider) {
  if (device_info == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return;
  }
  auto impl = static_cast<mindspore::DeviceInfoC *>(device_info);
  impl->provider = provider;
}

const char *OH_AI_DeviceInfoGetProvider(const OH_AI_DeviceInfoHandle device_info) {
  if (device_info == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return nullptr;
  }
  auto impl = static_cast<mindspore::DeviceInfoC *>(device_info);
  return impl->provider.c_str();
}

void OH_AI_DeviceInfoSetProviderDevice(OH_AI_DeviceInfoHandle device_info, const char *device) {
  if (device_info == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return;
  }
  auto impl = static_cast<mindspore::DeviceInfoC *>(device_info);
  impl->provider_device = device;
}

const char *OH_AI_DeviceInfoGetProviderDevice(const OH_AI_DeviceInfoHandle device_info) {
  if (device_info == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return nullptr;
  }
  auto impl = static_cast<mindspore::DeviceInfoC *>(device_info);
  return impl->provider_device.c_str();
}

OH_AI_DeviceType OH_AI_DeviceInfoGetDeviceType(const OH_AI_DeviceInfoHandle device_info) {
  if (device_info == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return OH_AI_DEVICETYPE_INVALID;
  }
  auto impl = static_cast<mindspore::DeviceInfoC *>(device_info);
  return impl->device_type;
}

void OH_AI_DeviceInfoSetEnableFP16(OH_AI_DeviceInfoHandle device_info, bool is_fp16) {
  if (device_info == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return;
  }
  auto impl = static_cast<mindspore::DeviceInfoC *>(device_info);
  if (impl->device_type == OH_AI_DEVICETYPE_CPU || impl->device_type == OH_AI_DEVICETYPE_GPU) {
    impl->enable_fp16 = is_fp16;
  } else {
    MS_LOG(ERROR) << "Unsupported Feature.";
  }
}

bool OH_AI_DeviceInfoGetEnableFP16(const OH_AI_DeviceInfoHandle device_info) {
  if (device_info == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return false;
  }
  auto impl = static_cast<mindspore::DeviceInfoC *>(device_info);
  if (impl->device_type == OH_AI_DEVICETYPE_CPU || impl->device_type == OH_AI_DEVICETYPE_GPU) {
    return impl->enable_fp16;
  } else {
    MS_LOG(ERROR) << "Unsupported Feature. device_type: " << impl->device_type;
    return false;
  }
}

void OH_AI_DeviceInfoSetFrequency(OH_AI_DeviceInfoHandle device_info, int frequency) {  // only for KirinNPU
  if (device_info == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return;
  }
  auto impl = static_cast<mindspore::DeviceInfoC *>(device_info);
  if (impl->device_type == OH_AI_DEVICETYPE_KIRIN_NPU) {
    impl->frequency = frequency;
  } else {
    MS_LOG(ERROR) << "Unsupported Feature.";
  }
}

int OH_AI_DeviceInfoGetFrequency(const OH_AI_DeviceInfoHandle device_info) {  // only for KirinNPU
  if (device_info == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return -1;
  }
  auto impl = static_cast<mindspore::DeviceInfoC *>(device_info);
  if (impl->device_type == OH_AI_DEVICETYPE_KIRIN_NPU) {
    return impl->frequency;
  } else {
    MS_LOG(ERROR) << "Unsupported Feature.";
    return -1;
  }
}
