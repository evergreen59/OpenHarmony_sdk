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
 * @file status.h
 * 
 * @brief Provides the status codes of MindSpore Lite.
 * @since 9
 */
#ifndef MINDSPORE_INCLUDE_C_API_STATUS_C_H
#define MINDSPORE_INCLUDE_C_API_STATUS_C_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
  /**
   * @brief Defines MinSpore component codes.
   * 
   * @since 9
   */
  enum OH_AI_CompCode
  {
    /** MindSpore Core code */
    OH_AI_COMPCODE_CORE = 0x00000000u,
    /** MindSpore Lite code */
    OH_AI_COMPCODE_LITE = 0xF0000000u,
  };

  /**
   * @brief Defines MindSpore status codes.
   * 
   * @since 9
   */
  typedef enum OH_AI_Status
  {
    /** Success */
    OH_AI_STATUS_SUCCESS = 0,

    // Core
    /** MindSpore Core failure */
    OH_AI_STATUS_CORE_FAILED = OH_AI_COMPCODE_CORE | 0x1,

    // Lite
    /** MindSpore Lite error */
    OH_AI_STATUS_LITE_ERROR = OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -1),
    /** MindSpore Lite null pointer */
    OH_AI_STATUS_LITE_NULLPTR = OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -2),
    /** MindSpore Lite invalid parameters */
    OH_AI_STATUS_LITE_PARAM_INVALID = OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -3),
    /** MindSpore Lite no change */
    OH_AI_STATUS_LITE_NO_CHANGE = OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -4),
    /** MindSpore Lite exit without errors */
    OH_AI_STATUS_LITE_SUCCESS_EXIT = OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -5),
    /** MindSpore Lite memory allocation failure */
    OH_AI_STATUS_LITE_MEMORY_FAILED = OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -6),
    /** MindSpore Lite not supported */
    OH_AI_STATUS_LITE_NOT_SUPPORT = OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -7),
    /** MindSpore Lite thread pool error */
    OH_AI_STATUS_LITE_THREADPOOL_ERROR = OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -8),
    /** MindSpore Lite uninitialized */
    OH_AI_STATUS_LITE_UNINITIALIZED_OBJ = OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -9),

    // Executor-related error codes. The value range is **[-100,-200)**.
    /** MindSpore Lite tensor overflow */
    OH_AI_STATUS_LITE_OUT_OF_TENSOR_RANGE = OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -100),
    /** MindSpore Lite input tensor error */
    OH_AI_STATUS_LITE_INPUT_TENSOR_ERROR =
        OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -101),
    /** MindSpore Lite reentry error */
    OH_AI_STATUS_LITE_REENTRANT_ERROR = OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -102),

    // Graph-related error codes. The value range is **[-200,-300)**.
    /** MindSpore Lite file error */
    OH_AI_STATUS_LITE_GRAPH_FILE_ERROR = OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -200),

    // Operator-related error codes. The value range is **[-300,-400)**.
    /** MindSpore Lite operator not found */
    OH_AI_STATUS_LITE_NOT_FIND_OP = OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -300),
    /** MindSpore Lite invalid operators */
    OH_AI_STATUS_LITE_INVALID_OP_NAME = OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -301),
    /** MindSpore Lite invalid operator hyperparameters */
    OH_AI_STATUS_LITE_INVALID_OP_ATTR = OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -302),
    /** MindSpore Lite operator execution failure */
    OH_AI_STATUS_LITE_OP_EXECUTE_FAILURE =
        OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -303),

    // Tensor-related error codes. The value range is **[-400,-500)**.
    /** MindSpore Lite tensor format error */
    OH_AI_STATUS_LITE_FORMAT_ERROR = OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -400),

    // Shape inference-related error codes. The value range is **[-500,-600)**.
    /** MindSpore Lite shape inference error */
    OH_AI_STATUS_LITE_INFER_ERROR = OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -500),
    /** MindSpore Lite invalid shape inference */
    OH_AI_STATUS_LITE_INFER_INVALID =
        OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -501),

    // User input-related error codes. The value range is **[-600, 700)**.
    /** MindSpore Lite invalid input parameters */
    OH_AI_STATUS_LITE_INPUT_PARAM_INVALID =
        OH_AI_COMPCODE_LITE | (0x0FFFFFFF & -600),

  } OH_AI_Status;
#ifdef __cplusplus
}
#endif

/** @} */
#endif // MINDSPORE_INCLUDE_C_API_STATUS_C_H
