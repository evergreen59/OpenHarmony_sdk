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
 * @file model.h
 * 
 * @brief Provides model-related APIs for model creation and inference.
 * 
 * @since 9
 */

#ifndef MINDSPORE_INCLUDE_C_API_MODEL_C_H
#define MINDSPORE_INCLUDE_C_API_MODEL_C_H

#include "mindspore/tensor.h"
#include "mindspore/context.h"
#include "mindspore/status.h"

#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief Defines the pointer to a model object.
 *
 * @since 9
 */
typedef void *OH_AI_ModelHandle;

/**
 * @brief Defines the tensor array structure, which is used to store the tensor array pointer and tensor array length.
 *
 * @since 9
 */
typedef struct OH_AI_TensorHandleArray
{
  /** Tensor array length */
  size_t handle_num;
  /** Tensor array pointer */
  OH_AI_TensorHandle *handle_list;
} OH_AI_TensorHandleArray;

/**
 * @brief Defines dimension information. The maximum dimension is set by {@link MS_MAX_SHAPE_NUM}.
 *
 * @since 9
 */
#define OH_AI_MAX_SHAPE_NUM 32
typedef struct OH_AI_ShapeInfo
{
  /** Dimension array length */
  size_t shape_num;
  /** Dimension array */
  int64_t shape[OH_AI_MAX_SHAPE_NUM];
} OH_AI_ShapeInfo;

/**
 * @brief Defines the operator information passed in a callback.
 * 
 * @since 9
 */
typedef struct OH_AI_CallBackParam
{
  /** Operator name */
  char *node_name;
  /** Operator type */
  char *node_type;
} OH_AI_CallBackParam;

/**
  * @brief Defines the pointer to a callback.
 *
 * This pointer is used to set the two callback functions in {@link OH_AI_ModelPredict}.
 * Each callback function must contain three parameters, where **inputs** and **outputs** indicate the input and output tensors of the operator, and **kernel_Info** indicates information about the current operator.
 * You can use the callback functions to monitor the operator execution status, for example, operator execution time and the operator correctness.
 * 
 * @since 9
 */
typedef bool (*OH_AI_KernelCallBack)(const OH_AI_TensorHandleArray inputs, const OH_AI_TensorHandleArray outputs,
                                      const OH_AI_CallBackParam kernel_Info);

/**
 * \brief Creates a model object.
 *
 * \return Pointer to the model object.
 * @since 9
 */
OH_AI_API OH_AI_ModelHandle OH_AI_ModelCreate();

/**
 * \brief Destroys a model object.
 *
 * \param model Pointer to the model object.
 * @since 9
 */
OH_AI_API void OH_AI_ModelDestroy(OH_AI_ModelHandle *model);

/**
 * \brief Loads and builds a MindSpore model from the memory buffer.
 *
 * Note that the same {@Link OH_AI_ContextHandle} object can only be passed to {@Link OH_AI_ModelBuild} or {@Link OH_AI_ModelBuildFromFile} once.
 * If you call this function multiple times, make sure that you create multiple {@Link OH_AI_ContextHandle} objects accordingly.
 * 
 * \param model Pointer to the model object.
 * \param model_data Address of the loaded model data in the memory.
 * \param data_size Length of the model data.
 * \param model_type Type of the model file. For details, see {@link OH_AI_ModelType}.
 * \param model_context Context for model running. For details, see {@link OH_AI_ContextHandle}.
 * \return Status code enumerated by {@link OH_AI_Status}. The value **MSStatus::kMSStatusSuccess** indicates that the operation is successful.
 * @since 9
 */
OH_AI_API OH_AI_Status OH_AI_ModelBuild(OH_AI_ModelHandle model, const void *model_data, size_t data_size,
                                        OH_AI_ModelType model_type, const OH_AI_ContextHandle model_context);

/**
 * \brief Loads and builds a MindSpore model from a model file.
 *
 * Note that the same {@Link OH_AI_ContextHandle} object can only be passed to {@Link OH_AI_ModelBuild} or {@Link OH_AI_ModelBuildFromFile} once.
 * If you call this function multiple times, make sure that you create multiple {@Link OH_AI_ContextHandle} objects accordingly.
 * 
 * \param model Pointer to the model object.
 * \param model_path Path of the model file.
 * \param model_type Type of the model file. For details, see {@link OH_AI_ModelType}.
 * \param model_context Context for model running. For details, see {@link OH_AI_ContextHandle}.
 * \return Status code enumerated by {@link OH_AI_Status}. The value **MSStatus::kMSStatusSuccess** indicates that the operation is successful.
 * @since 9
 */
OH_AI_API OH_AI_Status OH_AI_ModelBuildFromFile(OH_AI_ModelHandle model, const char *model_path,
                                                OH_AI_ModelType model_type, const OH_AI_ContextHandle model_context);

/**
 * \brief Adjusts the input tensor shapes of a built model.
 *
 * \param model Pointer to the model object.
 * \param inputs Tensor array structure corresponding to the model input.
 * \param shape_infos Input shape array, which consists of tensor shapes arranged in the model input sequence. The model adjusts the tensor shapes in sequence.
 * \param shape_info_num Length of the input shape array.
 * \return Status code enumerated by {@link OH_AI_Status}. The value **MSStatus::kMSStatusSuccess** indicates that the operation is successful.
 * @since 9
 */
OH_AI_API OH_AI_Status OH_AI_ModelResize(OH_AI_ModelHandle model, const OH_AI_TensorHandleArray inputs,
                                          OH_AI_ShapeInfo *shape_infos, size_t shape_info_num);

/**
 * \brief Performs model inference.
 *
 * \param model Pointer to the model object.
 * \param inputs Tensor array structure corresponding to the model input.
 * \param outputs Pointer to the tensor array structure corresponding to the model output.
 * \param before Callback function executed before model inference.
 * \param after Callback function executed after model inference.
 * \return Status code enumerated by {@link OH_AI_Status}. The value **MSStatus::kMSStatusSuccess** indicates that the operation is successful.
 * @since 9
 */
OH_AI_API OH_AI_Status OH_AI_ModelPredict(OH_AI_ModelHandle model, const OH_AI_TensorHandleArray inputs,
                                          OH_AI_TensorHandleArray *outputs, const OH_AI_KernelCallBack before,
                                          const OH_AI_KernelCallBack after);

/**
 * \brief Obtains the input tensor array structure of a model.
 *
 * \param model Pointer to the model object.
 * \return Tensor array structure corresponding to the model input.
 * @since 9
 */
OH_AI_API OH_AI_TensorHandleArray OH_AI_ModelGetInputs(const OH_AI_ModelHandle model);

/**
 * \brief Obtains the output tensor array structure of a model.
 *
 * \param model Pointer to the model object.
 * \return Tensor array structure corresponding to the model output.
 * @since 9
 */
OH_AI_API OH_AI_TensorHandleArray OH_AI_ModelGetOutputs(const OH_AI_ModelHandle model);

/**
 * \brief Obtains the input tensor of a model by tensor name.
 *
 * \param model Pointer to the model object.
 * \param tensor_name Tensor name.
 * \return Pointer to the input tensor indicated by **tensor_name**. If the tensor does not exist in the input, **null** will be returned.
 * @since 9
 */
OH_AI_API OH_AI_TensorHandle OH_AI_ModelGetInputByTensorName(const OH_AI_ModelHandle model, const char *tensor_name);

/**
 * \brief Obtains the output tensor of a model by tensor name.
 *
 * \param model Pointer to the model object.
 * \param tensor_name Tensor name.
 * \return Pointer to the output tensor indicated by **tensor_name**. If the tensor does not exist in the input, **null** will be returned.
 * @since 9
 */
OH_AI_API OH_AI_TensorHandle OH_AI_ModelGetOutputByTensorName(const OH_AI_ModelHandle model, const char *tensor_name);

#ifdef __cplusplus
}
#endif

/** @} */
#endif // MINDSPORE_INCLUDE_C_API_MODEL_C_H
