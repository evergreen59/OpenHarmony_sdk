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

typedef enum OH_AI_ModelType {
  OH_AI_MODELTYPE_MINDIR = 0,
  // insert new data type here
  OH_AI_MODELTYPE_INVALID = 0xFFFFFFFF
} OH_AI_ModelType;

typedef enum OH_AI_DeviceType {
  OH_AI_DEVICETYPE_CPU = 0,
  OH_AI_DEVICETYPE_GPU,
  OH_AI_DEVICETYPE_KIRIN_NPU,
  // add new type here
  //ohos-only device range[60,80)
  OH_AI_DEVICETYPE_NNRT = 60,
  OH_AI_DEVICETYPE_INVALID = 100,
} OH_AI_DeviceType;

#ifdef __cplusplus
}
#endif
#endif  // MINDSPORE_INCLUDE_C_API_TYPES_C_H
