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

/**
 * @addtogroup MindSpore
 * @{
 * 
 * @brief Provides APIs related to MindSpore Lite model inference.
 * 
 * @Syscap SystemCapability.Ai.MindSpore
 * @since 9
 */

/**
 * @file context.h
 * 
 * @brief Provides **Context** APIs for configuring runtime information.
 * 
 * @since 9
 */
#ifndef MINDSPORE_INCLUDE_C_API_CONTEXT_C_H
#define MINDSPORE_INCLUDE_C_API_CONTEXT_C_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "mindspore/types.h"

#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief Defines the pointer to the MindSpore context. 
 *
 * @since 9
 */
typedef void *OH_AI_ContextHandle;

/**
 * @brief Defines the pointer to the MindSpore device information.
 * 
 * @since 9
 */
typedef void *OH_AI_DeviceInfoHandle;

/**
 * \brief Creates a context object.
 *
 * @return {@link OH_AI_ContextHandle} that points to the context.
 * @since 9
 */
OH_AI_API OH_AI_ContextHandle OH_AI_ContextCreate();

/**
 * \brief Destroys a context object.
 *
 * \param context Level-2 pointer to {@link OH_AI_ContextHandle}. After the context is destroyed, the pointer is set to null.
 * @since 9
 */
OH_AI_API void OH_AI_ContextDestroy(OH_AI_ContextHandle *context);

/**
 * \brief Sets the number of runtime threads.
 *
 * \param context {@link OH_AI_ContextHandle} that points to the context instance.
 * \param thread_num Number of runtime threads.
 * @since 9
 */
OH_AI_API void OH_AI_ContextSetThreadNum(OH_AI_ContextHandle context, int32_t thread_num);

/**
 * \brief Obtains the number of threads.
 *
 * \param context {@link OH_AI_ContextHandle} that points to the context instance.
 * \return Number of threads.
 * @since 9
 */
OH_AI_API int32_t OH_AI_ContextGetThreadNum(const OH_AI_ContextHandle context);

/**
 * \brief Sets the affinity mode for binding runtime threads to CPU cores, which are categorized into middle cores and big cores depending on the CPU frequency.
 *
 * \param context {@link OH_AI_ContextHandle} that points to the context instance.
 * \param mode Affinity mode. **0**: no affinities; **1**: big cores first; **2**: middle cores first
 * @since 9
 */
OH_AI_API void OH_AI_ContextSetThreadAffinityMode(OH_AI_ContextHandle context, int mode);

/**
 * \brief Obtains the affinity mode for binding runtime threads to CPU cores.
 *
 * \param context {@link OH_AI_ContextHandle} that points to the context instance.
 * \return Affinity mode. **0**: no affinities; **1**: big cores first; **2**: middle cores first
 * @since 9
 */
OH_AI_API int OH_AI_ContextGetThreadAffinityMode(const OH_AI_ContextHandle context);

/**
 * \brief Sets the list of CPU cores bound to a runtime thread.
 * 
 * For example, if **core_list** is set to **[2,6,8]**, threads run on the 2nd, 6th, and 8th CPU cores.
 * If {@link OH_AI_ContextSetThreadAffinityMode} and {@link OH_AI_ContextSetThreadAffinityCoreList} are called for the same context object,
 * the **core_list** parameter of {@link OH_AI_ContextSetThreadAffinityCoreList} takes effect, but the **mode** parameter of {@link OH_AI_ContextSetThreadAffinityMode} does not.
 * 
 * \param context {@link OH_AI_ContextHandle} that points to the context instance.
 * \param core_list List of bound CPU cores.
 * \param core_num Number of cores, which indicates the length of {@link core_list}.
 * @since 9
 */
OH_AI_API void OH_AI_ContextSetThreadAffinityCoreList(OH_AI_ContextHandle context, const int32_t *core_list,
                                                        size_t core_num);

/**
 * \brief Obtains the list of bound CPU cores.
 *
 * \param context {@link OH_AI_ContextHandle} that points to the context instance.
 * \param core_num Number of CPU cores.
 * \return List of bound CPU cores.
 * @since 9
 */
OH_AI_API const int32_t *OH_AI_ContextGetThreadAffinityCoreList(const OH_AI_ContextHandle context, size_t *core_num);

/**
 * \brief Sets whether to enable parallelism between operators.
 *
 * \param context {@link OH_AI_ContextHandle} that points to the context instance.
 * \param is_parallel Whether to enable parallelism between operators. The value **true** means to enable parallelism between operators, and the value **false** means the opposite.
 * @since 9
 */
OH_AI_API void OH_AI_ContextSetEnableParallel(OH_AI_ContextHandle context, bool is_parallel);

/**
 * \brief Checks whether parallelism between operators is supported.
 *
 * \param context {@link OH_AI_ContextHandle} that points to the context instance.
 * \return Whether parallelism between operators is supported. The value **true** means that parallelism between operators is supported, and the value **false** means the opposite.
 * @since 9
 */
OH_AI_API bool OH_AI_ContextGetEnableParallel(const OH_AI_ContextHandle context);

/**
 * \brief Adds information about a running device.
 *
 * \param context {@link OH_AI_ContextHandle} that points to the context instance.
 * \param device_info {@link OH_AI_DeviceInfoHandle} that points to a device information instance.
 * @since 9
 */
OH_AI_API void OH_AI_ContextAddDeviceInfo(OH_AI_ContextHandle context, OH_AI_DeviceInfoHandle device_info);

/**
 * \brief Creates a device information object.
 *
 * \param device_type Device type. For details, see {@link OH_AI_DeviceType}.
 *
 * \return {@link OH_AI_DeviceInfoHandle} that points to the device information instance.
 * @since 9
 */
OH_AI_API OH_AI_DeviceInfoHandle OH_AI_DeviceInfoCreate(OH_AI_DeviceType device_type);

/**
 * \brief Destroys a device information instance.
 *
 * \param device_info {@link OH_AI_DeviceInfoHandle} that points to a device information instance.
 * @since 9
 */
OH_AI_API void OH_AI_DeviceInfoDestroy(OH_AI_DeviceInfoHandle *device_info);

/**
 * \brief Sets the name of a provider.
 *
 * \param device_info {@link OH_AI_DeviceInfoHandle} that points to a device information instance.
 * \param provider Provider name.
 * @since 9
 */
OH_AI_API void OH_AI_DeviceInfoSetProvider(OH_AI_DeviceInfoHandle device_info, const char *provider);

/**
 * \brief Obtains the provider name.
 *
 * \param device_info {@link OH_AI_DeviceInfoHandle} that points to a device information instance.
 *
 * \return Provider name.
 * @since 9
 */
OH_AI_API const char *OH_AI_DeviceInfoGetProvider(const OH_AI_DeviceInfoHandle device_info);

/**
 * \brief Sets the name of a provider device.
 *
 * \param device_info {@link OH_AI_DeviceInfoHandle} that points to a device information instance.
 * \param device Name of the provider device, for example, CPU.
 * @since 9
 */
OH_AI_API void OH_AI_DeviceInfoSetProviderDevice(OH_AI_DeviceInfoHandle device_info, const char *device);

/**
 * \brief Obtains the name of a provider device.
 *
 * \param device_info {@link OH_AI_DeviceInfoHandle} that points to a device information instance.
 *
 * \return Name of the provider device.
 * @since 9
 */
OH_AI_API const char *OH_AI_DeviceInfoGetProviderDevice(const OH_AI_DeviceInfoHandle device_info);

/**
 * \brief Obtains the type of a provider device.
 *
 * \param device_info {@link OH_AI_DeviceInfoHandle} that points to a device information instance.
 * \return Type of the provider device.
 * @since 9
 */
OH_AI_API OH_AI_DeviceType OH_AI_DeviceInfoGetDeviceType(const OH_AI_DeviceInfoHandle device_info);

/**
 * \brief Sets whether to enable float16 inference. This function is available only for CPU/GPU devices.
 *
 * \param device_info {@link OH_AI_DeviceInfoHandle} that points to a device information instance.
 * \param is_fp16 Whether to enable float16 inference.
 * @since 9
 */
OH_AI_API void OH_AI_DeviceInfoSetEnableFP16(OH_AI_DeviceInfoHandle device_info, bool is_fp16);

/**
 * \brief Checks whether float16 inference is enabled. This function is available only for CPU/GPU devices.
 *
 * \param device_info {@link OH_AI_DeviceInfoHandle} that points to a device information instance.
 * \return Whether float16 inference is enabled.
 * @since 9
 */
OH_AI_API bool OH_AI_DeviceInfoGetEnableFP16(const OH_AI_DeviceInfoHandle device_info);

/**
 * \brief Sets the NPU frequency type. This function is available only for NPU devices.
 *
 * \param device_info {@link OH_AI_DeviceInfoHandle} that points to a device information instance.
 * \param frequency NPU frequency type. The value ranges from **0** to **4**. The default value is **3**. **1**: low power consumption; **2**: balanced; **3**: high performance; **4**: ultra-high performance
 * @since 9
 */
OH_AI_API void OH_AI_DeviceInfoSetFrequency(OH_AI_DeviceInfoHandle device_info, int frequency);

/**
 * \brief Obtains the NPU frequency type. This function is available only for NPU devices.
 *
 * \param device_info {@link OH_AI_DeviceInfoHandle} that points to a device information instance.
 *
 * \return Frequency type of the NPU. The value ranges from **0** to **4**. **1**: low power consumption; **2**: balanced; **3**: high performance; **4**: ultra-high performance
 * @since 9
 */
OH_AI_API int OH_AI_DeviceInfoGetFrequency(const OH_AI_DeviceInfoHandle device_info);

#ifdef __cplusplus
}
#endif

/** @} */
#endif // MINDSPORE_INCLUDE_C_API_CONTEXT_C_H
