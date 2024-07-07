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
#ifndef MINDSPORE_INCLUDE_C_API_MODEL_C_H
#define MINDSPORE_INCLUDE_C_API_MODEL_C_H

#include "include/c_api/tensor_c.h"
#include "include/c_api/context_c.h"
#include "include/c_api/status_c.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *OH_AI_ModelHandle;

typedef struct OH_AI_TensorHandleArray {
  size_t handle_num;
  OH_AI_TensorHandle *handle_list;
} OH_AI_TensorHandleArray;

#define OH_AI_MAX_SHAPE_NUM 32
typedef struct OH_AI_ShapeInfo {
  size_t shape_num;
  int64_t shape[OH_AI_MAX_SHAPE_NUM];
} OH_AI_ShapeInfo;

typedef struct OH_AI_CallBackParam {
  char *node_name;
  char *node_type;
} OH_AI_CallBackParam;

typedef bool (*OH_AI_KernelCallBack)(const OH_AI_TensorHandleArray inputs, const OH_AI_TensorHandleArray outputs,
                                     const OH_AI_CallBackParam kernel_Info);

/// \brief Create a model object. Only valid for Lite.
///
/// \return Model object handle.
OH_AI_API OH_AI_ModelHandle OH_AI_ModelCreate();

/// \brief Destroy the model object. Only valid for Lite.
///
/// \param[in] model Model object handle address.
OH_AI_API void OH_AI_ModelDestroy(OH_AI_ModelHandle *model);

/// \brief Set workspace for the model object. Only valid for Iot.
///
/// \param[in] model Model object handle.
/// \param[in] workspace Define the workspace address.
/// \param[in] workspace_size Define the workspace size.
OH_AI_API void OH_AI_ModelSetWorkspace(OH_AI_ModelHandle model, void *workspace, size_t workspace_size);

/// \brief Build the model from model file buffer so that it can run on a device. Only valid for Lite.
///
/// \param[in] model Model object handle.
/// \param[in] model_data Define the buffer read from a model file.
/// \param[in] data_size Define bytes number of model file buffer.
/// \param[in] model_type Define The type of model file.
/// \param[in] model_context Define the context used to store options during execution.
///
/// \return OH_AI_Status.
OH_AI_API OH_AI_Status OH_AI_ModelBuild(OH_AI_ModelHandle model, const void *model_data, size_t data_size,
                                        OH_AI_ModelType model_type, const OH_AI_ContextHandle model_context);

/// \brief Load and build the model from model path so that it can run on a device. Only valid for Lite.
///
/// \param[in] model Model object handle.
/// \param[in] model_path Define the model file path.
/// \param[in] model_type Define The type of model file.
/// \param[in] model_context Define the context used to store options during execution.
///
/// \return OH_AI_Status.
OH_AI_API OH_AI_Status OH_AI_ModelBuildFromFile(OH_AI_ModelHandle model, const char *model_path,
                                                OH_AI_ModelType model_type, const OH_AI_ContextHandle model_context);

/// \brief Resizes the shapes of inputs.
///
/// \param[in] model Model object handle.
/// \param[in] inputs The array that includes all input tensor handles.
/// \param[in] shape_infos Defines the new shapes of inputs, should be consistent with inputs.
/// \param[in] shape_info_num The num of shape_infos.
///
/// \return OH_AI_Status.
OH_AI_API OH_AI_Status OH_AI_ModelResize(OH_AI_ModelHandle model, const OH_AI_TensorHandleArray inputs,
                                         OH_AI_ShapeInfo *shape_infos, size_t shape_info_num);

/// \brief Inference model.
///
/// \param[in] model Model object handle.
/// \param[in] inputs The array that includes all input tensor handles.
/// \param[out] outputs The array that includes all output tensor handles.
/// \param[in] before CallBack before predict.
/// \param[in] after CallBack after predict.
///
/// \return OH_AI_Status.
OH_AI_API OH_AI_Status OH_AI_ModelPredict(OH_AI_ModelHandle model, const OH_AI_TensorHandleArray inputs,
                                          OH_AI_TensorHandleArray *outputs, const OH_AI_KernelCallBack before,
                                          const OH_AI_KernelCallBack after);

/// \brief Obtains all input tensor handles of the model.
///
/// \param[in] model Model object handle.
///
/// \return The array that includes all input tensor handles.
OH_AI_API OH_AI_TensorHandleArray OH_AI_ModelGetInputs(const OH_AI_ModelHandle model);

/// \brief Obtains all output tensor handles of the model.
///
/// \param[in] model Model object handle.
///
/// \return The array that includes all output tensor handles.
OH_AI_API OH_AI_TensorHandleArray OH_AI_ModelGetOutputs(const OH_AI_ModelHandle model);

/// \brief Obtains the input tensor handle of the model by name.
///
/// \param[in] model Model object handle.
/// \param[in] tensor_name The name of tensor.
///
/// \return The input tensor handle with the given name, if the name is not found, an NULL is returned.
OH_AI_API OH_AI_TensorHandle OH_AI_ModelGetInputByTensorName(const OH_AI_ModelHandle model, const char *tensor_name);

/// \brief Obtains the output tensor handle of the model by name.
///
/// \param[in] model Model object handle.
/// \param[in] tensor_name The name of tensor.
///
/// \return The output tensor handle with the given name, if the name is not found, an NULL is returned.
OH_AI_API OH_AI_TensorHandle OH_AI_ModelGetOutputByTensorName(const OH_AI_ModelHandle model, const char *tensor_name);

#ifdef __cplusplus
}
#endif
#endif  // MINDSPORE_INCLUDE_C_API_MODEL_C_H
