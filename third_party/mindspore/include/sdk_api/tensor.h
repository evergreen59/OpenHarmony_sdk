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
 * @file tensor.h
 * 
 * @brief Provides APIs for creating and modifying tensor information.
 * 
 * @since 9
 */

#ifndef MINDSPORE_INCLUDE_C_API_TENSOE_C_H
#define MINDSPORE_INCLUDE_C_API_TENSOE_C_H

#include <stddef.h>
#include "mindspore/types.h"
#include "mindspore/data_type.h"
#include "mindspore/format.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines the handle of a tensor object.
 * 
 * @since 9
 */
typedef void *OH_AI_TensorHandle;

/**
 * \brief Creates a tensor object.
 *
 * \param name Tensor name.
 * \param type Tensor data type.
 * \param shape Tensor dimension array.
 * \param shape_num Length of the tensor dimension array.
 * \param data Data pointer.
 * \param data_len Data length.
 *
 * \return Handle of the tensor object.
 * 
 * @since 9
 */
OH_AI_API OH_AI_TensorHandle OH_AI_TensorCreate(const char *name, OH_AI_DataType type, const int64_t *shape,
                                                size_t shape_num, const void *data, size_t data_len);

/**
 * \brief Destroys a tensor object.
 *
 * \param tensor Level-2 pointer to the tensor handle.
 * 
 * @since 9
 */
OH_AI_API void OH_AI_TensorDestroy(OH_AI_TensorHandle *tensor);

/**
 * \brief Clones a tensor.
 *
 * \param tensor Pointer to the tensor to clone.
 *
 * \return Handle of the new tensor object.
 * 
 * @since 9
 */
OH_AI_API OH_AI_TensorHandle OH_AI_TensorClone(OH_AI_TensorHandle tensor);

/**
 * \brief Sets the name of a tensor.
 *
 * \param tensor Handle of the tensor object.
 * \param name Tensor name.
 * 
 * @since 9
 */
OH_AI_API void OH_AI_TensorSetName(OH_AI_TensorHandle tensor, const char *name);

/**
 * \brief Obtains the name of a tensor.
 *
 * \param tensor Handle of the tensor object.
 *
 * \return Tensor name.
 * 
 * @since 9
 */
OH_AI_API const char *OH_AI_TensorGetName(const OH_AI_TensorHandle tensor);

/**
 * \brief Sets the data type of a tensor.
 *
 * \param tensor Handle of the tensor object.
 * \param type Data type. For details, see {@link OH_AI_DataType}.
 * 
 * @since 9
 */
OH_AI_API void OH_AI_TensorSetDataType(OH_AI_TensorHandle tensor, OH_AI_DataType type);

/**
 * \brief Obtains the data type of a tensor.
 *
 * \param tensor Handle of the tensor object.
 *
 * \return Data type of the tensor.
 * 
 * @since 9
 */
OH_AI_API OH_AI_DataType OH_AI_TensorGetDataType(const OH_AI_TensorHandle tensor);

/**
 * \brief Sets the shape of a tensor.
 *
 * \param tensor Handle of the tensor object.
 * \param shape Tensor shape array.
 * \param shape_num Length of the tensor shape array.
 * 
 * @since 9
 */
OH_AI_API void OH_AI_TensorSetShape(OH_AI_TensorHandle tensor, const int64_t *shape, size_t shape_num);

/**
 * \brief Obtains the shape of a tensor.
 *
 * \param tensor Handle of the tensor object.
 * \param shape_num Length of the tensor shape array.
 *
 * \return Tensor shape array.
 * 
 * @since 9
 */
OH_AI_API const int64_t *OH_AI_TensorGetShape(const OH_AI_TensorHandle tensor, size_t *shape_num);

/**
 * \brief Sets the tensor data format.
 *
 * \param tensor Handle of the tensor object.
 * \param format Tensor data format.
 * 
 * @since 9
 */
OH_AI_API void OH_AI_TensorSetFormat(OH_AI_TensorHandle tensor, OH_AI_Format format);

/**
 * \brief Obtains the tensor data format.
 *
 * \param tensor Handle of the tensor object.
 *
 * \return Tensor data format.
 * 
 * @since 9
 */
OH_AI_API OH_AI_Format OH_AI_TensorGetFormat(const OH_AI_TensorHandle tensor);

/**
 * \brief Sets the tensor data.
 *
 * \param tensor Handle of the tensor object.
 * \param data Data pointer.
 * 
 * @since 9
 */
OH_AI_API void OH_AI_TensorSetData(OH_AI_TensorHandle tensor, void *data);

/**
 * \brief Obtains the pointer to tensor data.
 *
 * \param tensor Handle of the tensor object.
 *
 * \return Pointer to tensor data.
 * 
 * @since 9
 */
OH_AI_API const void *OH_AI_TensorGetData(const OH_AI_TensorHandle tensor);

/**
 * \brief Obtains the pointer to variable tensor data. If the data is empty, memory will be allocated.
 *
 * \param tensor Handle of the tensor object.
 *
 * \return Pointer to variable tensor data.
 * 
 * @since 9
 */
OH_AI_API void *OH_AI_TensorGetMutableData(const OH_AI_TensorHandle tensor);

/**
 * \brief Obtains the number of tensor elements.
 *
 * \param tensor Handle of the tensor object.
 *
 * \return Number of tensor elements.
 * 
 * @since 9
 */
OH_AI_API int64_t OH_AI_TensorGetElementNum(const OH_AI_TensorHandle tensor);

/**
 * \brief Obtains the number of bytes of the tensor data.
 *
 * \param tensor Handle of the tensor object.
 *
 * \return Number of bytes of the tensor data.
 * 
 * @since 9
 */
OH_AI_API size_t OH_AI_TensorGetDataSize(const OH_AI_TensorHandle tensor);

#ifdef __cplusplus
}
#endif

/** @} */
#endif  // MINDSPORE_INCLUDE_C_API_TENSOE_C_H
