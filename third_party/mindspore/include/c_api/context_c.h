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
#ifndef MINDSPORE_INCLUDE_C_API_CONTEXT_C_H
#define MINDSPORE_INCLUDE_C_API_CONTEXT_C_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "include/c_api/types_c.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *OH_AI_ContextHandle;
typedef void *OH_AI_DeviceInfoHandle;

/// \brief Create a context object.
///
/// \return Context object handle.
OH_AI_API OH_AI_ContextHandle OH_AI_ContextCreate();

/// \brief Destroy the context object.
///
/// \param[in] context Context object handle address.
OH_AI_API void OH_AI_ContextDestroy(OH_AI_ContextHandle *context);

/// \brief Set the number of threads at runtime.
///
/// \param[in] context Context object handle.
/// \param[in] thread_num the number of threads at runtime.
OH_AI_API void OH_AI_ContextSetThreadNum(OH_AI_ContextHandle context, int32_t thread_num);

/// \brief Obtain the current thread number setting.
///
/// \param[in] context Context object handle.
///
/// \return The current thread number setting.
OH_AI_API int32_t OH_AI_ContextGetThreadNum(const OH_AI_ContextHandle context);

/// \brief Set the thread affinity to CPU cores.
///
/// \param[in] context Context object handle.
/// \param[in] mode: 0: no affinities, 1: big cores first, 2: middle cores first
OH_AI_API void OH_AI_ContextSetThreadAffinityMode(OH_AI_ContextHandle context, int mode);

/// \brief Obtain the thread affinity of CPU cores.
///
/// \param[in] context Context object handle.
///
/// \return Thread affinity to CPU cores. 0: no affinities, 1: big cores first, 2: middle cores first
OH_AI_API int OH_AI_ContextGetThreadAffinityMode(const OH_AI_ContextHandle context);

/// \brief Set the thread lists to CPU cores.
///
/// \note If core_list and mode are set by OH_AI_ContextSetThreadAffinityMode at the same time,
/// the core_list is effective, but the mode is not effective.
///
/// \param[in] context Context object handle.
/// \param[in] core_list: a array of thread core lists.
/// \param[in] core_num The number of core.
OH_AI_API void OH_AI_ContextSetThreadAffinityCoreList(OH_AI_ContextHandle context, const int32_t *core_list,
                                                      size_t core_num);

/// \brief Obtain the thread lists of CPU cores.
///
/// \param[in] context Context object handle.
/// \param[out] core_num The number of core.
///
/// \return a array of thread core lists.
OH_AI_API const int32_t *OH_AI_ContextGetThreadAffinityCoreList(const OH_AI_ContextHandle context, size_t *core_num);

/// \brief Set the status whether to perform model inference or training in parallel.
///
/// \param[in] context Context object handle.
/// \param[in] is_parallel: true, parallel; false, not in parallel.
OH_AI_API void OH_AI_ContextSetEnableParallel(OH_AI_ContextHandle context, bool is_parallel);

/// \brief Obtain the status whether to perform model inference or training in parallel.
///
/// \param[in] context Context object handle.
///
/// \return Bool value that indicates whether in parallel.
OH_AI_API bool OH_AI_ContextGetEnableParallel(const OH_AI_ContextHandle context);

/// \brief Add device info to context object.
///
/// \param[in] context Context object handle.
/// \param[in] device_info Device info object handle.
OH_AI_API void OH_AI_ContextAddDeviceInfo(OH_AI_ContextHandle context, OH_AI_DeviceInfoHandle device_info);

/// \brief Create a device info object.
///
/// \param[in] device_info Device info object handle.
///
/// \return Device info object handle.
OH_AI_API OH_AI_DeviceInfoHandle OH_AI_DeviceInfoCreate(OH_AI_DeviceType device_type);

/// \brief Destroy the device info object.
///
/// \param[in] device_info Device info object handle address.
OH_AI_API void OH_AI_DeviceInfoDestroy(OH_AI_DeviceInfoHandle *device_info);

/// \brief Set provider's name.
///
/// \param[in] device_info Device info object handle.
/// \param[in] provider define the provider's name.
OH_AI_API void OH_AI_DeviceInfoSetProvider(OH_AI_DeviceInfoHandle device_info, const char *provider);

/// \brief Obtain provider's name
///
/// \param[in] device_info Device info object handle.
///
/// \return provider's name.
OH_AI_API const char *OH_AI_DeviceInfoGetProvider(const OH_AI_DeviceInfoHandle device_info);

/// \brief Set provider's device type.
///
/// \param[in] device_info Device info object handle.
/// \param[in] device define the provider's device type. EG: CPU.
OH_AI_API void OH_AI_DeviceInfoSetProviderDevice(OH_AI_DeviceInfoHandle device_info, const char *device);

/// \brief Obtain provider's device type.
///
/// \param[in] device_info Device info object handle.
///
/// \return provider's device type.
OH_AI_API const char *OH_AI_DeviceInfoGetProviderDevice(const OH_AI_DeviceInfoHandle device_info);

/// \brief Obtain the device type of the device info.
///
/// \param[in] device_info Device info object handle.
///
/// \return Device Type of the device info.
OH_AI_API OH_AI_DeviceType OH_AI_DeviceInfoGetDeviceType(const OH_AI_DeviceInfoHandle device_info);

/// \brief Set enables to perform the float16 inference, Only valid for CPU/GPU.
///
/// \param[in] device_info Device info object handle.
/// \param[in] is_fp16 Enable float16 inference or not.
OH_AI_API void OH_AI_DeviceInfoSetEnableFP16(OH_AI_DeviceInfoHandle device_info, bool is_fp16);

/// \brief Obtain enables to perform the float16 inference, Only valid for CPU/GPU.
///
/// \param[in] device_info Device info object handle.
///
/// \return Whether enable float16 inference.
OH_AI_API bool OH_AI_DeviceInfoGetEnableFP16(const OH_AI_DeviceInfoHandle device_info);

/// \brief Set the NPU frequency, Only valid for NPU.
///
/// \param[in] device_info Device info object handle.
/// \param[in] frequency Can be set to 1 (low power consumption), 2 (balanced), 3 (high performance), 4 (extreme
/// performance), default as 3.
OH_AI_API void OH_AI_DeviceInfoSetFrequency(OH_AI_DeviceInfoHandle device_info, int frequency);

/// \brief Obtain the NPU frequency, Only valid for NPU.
///
/// \param[in] device_info Device info object handle.
///
/// \return NPU frequency
OH_AI_API int OH_AI_DeviceInfoGetFrequency(const OH_AI_DeviceInfoHandle device_info);

#ifdef __cplusplus
}
#endif
#endif  // MINDSPORE_INCLUDE_C_API_CONTEXT_C_H
