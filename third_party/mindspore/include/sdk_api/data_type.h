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
 * @file data_type.h
 *
 * @brief Declares tensor data types.
 *
 * @since 9
 */
#ifndef MINDSPORE_INCLUDE_C_API_DATA_TYPE_C_H
#define MINDSPORE_INCLUDE_C_API_DATA_TYPE_C_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Declares data types supported by MSTensor.
 *
 * @since 9
 */
typedef enum OH_AI_DataType {
  /** Unknown data type */
  OH_AI_DATATYPE_UNKNOWN = 0,
  /** String data type */
  OH_AI_DATATYPE_OBJECTTYPE_STRING = 12,
  /** List data type */
  OH_AI_DATATYPE_OBJECTTYPE_LIST = 13,
  /** Tuple data type */
  OH_AI_DATATYPE_OBJECTTYPE_TUPLE = 14,
  /** TensorList data type */
  OH_AI_DATATYPE_OBJECTTYPE_TENSOR = 17,
  /** Beginning of the number type */
  OH_AI_DATATYPE_NUMBERTYPE_BEGIN = 29,
  /** Bool data type */
  OH_AI_DATATYPE_NUMBERTYPE_BOOL = 30,
  /** Int8 data type */
  OH_AI_DATATYPE_NUMBERTYPE_INT8 = 32,
  /** Int16 data type */
  OH_AI_DATATYPE_NUMBERTYPE_INT16 = 33,
  /** Int32 data type */
  OH_AI_DATATYPE_NUMBERTYPE_INT32 = 34,
  /** Int64 data type */
  OH_AI_DATATYPE_NUMBERTYPE_INT64 = 35,
  /** UInt8 data type */
  OH_AI_DATATYPE_NUMBERTYPE_UINT8 = 37,
  /** UInt16 data type */
  OH_AI_DATATYPE_NUMBERTYPE_UINT16 = 38,
  /** UInt32 data type */
  OH_AI_DATATYPE_NUMBERTYPE_UINT32 = 39,
  /** UInt64 data type */
  OH_AI_DATATYPE_NUMBERTYPE_UINT64 = 40,
  /** Float16 data type */
  OH_AI_DATATYPE_NUMBERTYPE_FLOAT16 = 42,
  /** Float32 data type */
  OH_AI_DATATYPE_NUMBERTYPE_FLOAT32 = 43,
  /** Float64 data type */
  OH_AI_DATATYPE_NUMBERTYPE_FLOAT64 = 44,
  /** End of the number type */
  OH_AI_DATATYPE_NUMBERTYPE_END = 46,
  /** Invalid data type */
  OH_AI_DataTypeInvalid = INT32_MAX,
} OH_AI_DataType;

#ifdef __cplusplus
}
#endif

/** @} */
#endif // MINDSPORE_INCLUDE_C_API_DATA_TYPE_C_H
