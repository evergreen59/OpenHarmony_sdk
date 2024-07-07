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
 * @file format.h
 *
 * @brief Declares tensor data formats.
 *
 * @since 9
 */
#ifndef MINDSPORE_INCLUDE_C_API_FORMAT_C_H
#define MINDSPORE_INCLUDE_C_API_FORMAT_C_H

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Declares data formats supported by MSTensor.
 *
 * @since 9
 */
typedef enum OH_AI_Format {
  /** NCHW format */
  OH_AI_FORMAT_NCHW = 0,
  /** NHWC format */
  OH_AI_FORMAT_NHWC = 1,
  /** NHWC4 format */
  OH_AI_FORMAT_NHWC4 = 2,
  /** HWKC format */
  OH_AI_FORMAT_HWKC = 3,
  /** HWCK format */
  OH_AI_FORMAT_HWCK = 4,
  /** KCHW format */
  OH_AI_FORMAT_KCHW = 5,
  /** CKHW format */
  OH_AI_FORMAT_CKHW = 6,
  /** KHWC format */
  OH_AI_FORMAT_KHWC = 7,
  /** CHWK format */
  OH_AI_FORMAT_CHWK = 8,
  /** HW format */
  OH_AI_FORMAT_HW = 9,
  /** HW4 format */
  OH_AI_FORMAT_HW4 = 10,
  /** NC format */
  OH_AI_FORMAT_NC = 11,
  /** NC4 format */
  OH_AI_FORMAT_NC4 = 12,
  /** NC4HW4 format */
  OH_AI_FORMAT_NC4HW4 = 13,
  /** NCDHW format */
  OH_AI_FORMAT_NCDHW = 15,
  /** NWC format */
  OH_AI_FORMAT_NWC = 16,
  /** NCW format */
  OH_AI_FORMAT_NCW = 17
} OH_AI_Format;

#ifdef __cplusplus
}
#endif

/** @} */
#endif // MINDSPORE_INCLUDE_C_API_FORMAT_C_H
