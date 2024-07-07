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
 * @file types.h
 *
 * @brief Provides the model file types and device types supported by MindSpore Lite.
 *
 * @since 9
 */

#ifndef MINDSPORE_INCLUDE_C_API_TYPES_C_H
#define MINDSPORE_INCLUDE_C_API_TYPES_C_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OH_AI_API
#ifdef _WIN32
#define OH_AI_API __declspec(dllexport)
#else
#define OH_AI_API __attribute__((visibility("default")))
#endif
#endif
/**
 * @brief Defines model file types.
 *
 * @since 9
 */
typedef enum OH_AI_ModelType {
  /** Model type: MindIR
   *
   * @since 9
   */
  OH_AI_MODELTYPE_MINDIR = 0,
  /** Invalid model type
   *
   * @since 9
   */
  OH_AI_MODELTYPE_INVALID = 0xFFFFFFFF
} OH_AI_ModelType;

/**
 * @brief Defines the supported device types.
 *
 * @since 9
 */
typedef enum OH_AI_DeviceType {
  /** Device type: CPU
   *
   * @since 9
   * 
   */
  OH_AI_DEVICETYPE_CPU = 0,
  /** Device type: GPU
   * 
   * Reserved, not support yet.
   * @since 9
   */
  OH_AI_DEVICETYPE_GPU,
  /** Device type: Kirin NPU
   *
   * Reserved, not support yet.
   * @since 9
   * 
   */
  OH_AI_DEVICETYPE_KIRIN_NPU,
  /** Device type: NNRt
   *
   * @since 9
   * 
   */
  OH_AI_DEVICETYPE_NNRT = 60,
  /** Invalid device type
   *
   * @since 9
   * 
   */
  OH_AI_DEVICETYPE_INVALID = 100,
} OH_AI_DeviceType;

#ifdef __cplusplus
}
#endif

/** @} */
#endif // MINDSPORE_INCLUDE_C_API_TYPES_C_H
