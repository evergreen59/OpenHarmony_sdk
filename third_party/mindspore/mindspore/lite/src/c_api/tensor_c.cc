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
#include "include/c_api/tensor_c.h"
#include "include/api/status.h"
#include "include/ms_tensor.h"
#include "src/cxx_api/tensor/tensor_impl.h"
#include "src/runtime/inner_allocator.h"

OH_AI_TensorHandle OH_AI_TensorCreate(const char *name, OH_AI_DataType type, const int64_t *shape, size_t shape_num,
                                      const void *data, size_t data_len) {
  if (name == nullptr || shape == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return nullptr;
  }
  std::vector<int32_t> vec_shape(shape_num);
  for (size_t i = 0; i < shape_num; i++) {
    vec_shape[i] = shape[i];
  }
  auto lite_tensor =
    mindspore::lite::Tensor::CreateTensor(name, static_cast<mindspore::TypeId>(type), vec_shape, data, data_len);
  auto impl = new (std::nothrow) mindspore::MSTensor::Impl(lite_tensor);
  if (impl == nullptr || impl->lite_tensor() == nullptr) {
    MS_LOG(ERROR) << "Failed to allocate tensor impl.";
    return nullptr;
  }
  impl->set_from_session(false);
  return impl;
}

void OH_AI_TensorDestroy(OH_AI_TensorHandle *tensor) {
  if (tensor != nullptr && *tensor != nullptr) {
    auto impl = static_cast<mindspore::MSTensor::Impl *>(*tensor);
    delete impl;
    *tensor = nullptr;
  }
}

OH_AI_TensorHandle OH_AI_TensorClone(OH_AI_TensorHandle tensor) {
  if (tensor == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return nullptr;
  }
  auto impl = static_cast<mindspore::MSTensor::Impl *>(tensor);
  auto lite_tensor = static_cast<mindspore::lite::Tensor *>(impl->lite_tensor());
  auto clone = mindspore::lite::Tensor::CopyTensor(*lite_tensor, true, lite_tensor->allocator());
  if (clone == nullptr) {
    MS_LOG(ERROR) << "Failed to allocate tensor.";
    return nullptr;
  }
  auto clone_impl = new (std::nothrow) mindspore::MSTensor::Impl(clone);
  if (clone_impl == nullptr) {
    delete clone;
    MS_LOG(ERROR) << "Failed to allocate tensor impl.";
    return nullptr;
  }
  clone_impl->set_from_session(false);
  return clone_impl;
}

void OH_AI_TensorSetName(OH_AI_TensorHandle tensor, const char *name) {
  if (tensor == nullptr || name == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return;
  }
  auto impl = static_cast<mindspore::MSTensor::Impl *>(tensor);
  impl->SetName(name);
}

const char *OH_AI_TensorGetName(const OH_AI_TensorHandle tensor) {
  if (tensor == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return nullptr;
  }
  auto impl = static_cast<mindspore::MSTensor::Impl *>(tensor);
  return impl->Name().c_str();
}

void OH_AI_TensorSetDataType(OH_AI_TensorHandle tensor, OH_AI_DataType type) {
  if (tensor == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return;
  }
  auto impl = static_cast<mindspore::MSTensor::Impl *>(tensor);
  impl->SetDataType(static_cast<mindspore::DataType>(type));
}

OH_AI_DataType OH_AI_TensorGetDataType(const OH_AI_TensorHandle tensor) {
  if (tensor == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return OH_AI_DATATYPE_UNKNOWN;
  }
  auto impl = static_cast<mindspore::MSTensor::Impl *>(tensor);
  auto dtype = impl->DataType();
  return static_cast<OH_AI_DataType>(dtype);
}

void OH_AI_TensorSetShape(OH_AI_TensorHandle tensor, const int64_t *shape, size_t shape_num) {
  if (tensor == nullptr || shape == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return;
  }
  auto impl = static_cast<mindspore::MSTensor::Impl *>(tensor);
  std::vector<int64_t> vec_shape(shape_num);
  for (size_t i = 0; i < shape_num; i++) {
    vec_shape[i] = shape[i];
  }
  impl->SetShape(vec_shape);
}

const int64_t *OH_AI_TensorGetShape(const OH_AI_TensorHandle tensor, size_t *shape_num) {
  if (tensor == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return nullptr;
  }
  auto impl = static_cast<mindspore::MSTensor::Impl *>(tensor);
  *shape_num = impl->Shape().size();
  return impl->Shape().data();
}

void OH_AI_TensorSetFormat(OH_AI_TensorHandle tensor, OH_AI_Format format) {
  if (tensor == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return;
  }
  auto impl = static_cast<mindspore::MSTensor::Impl *>(tensor);
  return impl->SetFormat(static_cast<mindspore::Format>(format));
}

OH_AI_Format OH_AI_TensorGetFormat(const OH_AI_TensorHandle tensor) {
  if (tensor == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return OH_AI_FORMAT_NHWC;
  }
  auto impl = static_cast<mindspore::MSTensor::Impl *>(tensor);
  return static_cast<OH_AI_Format>(impl->format());
}

void OH_AI_TensorSetData(OH_AI_TensorHandle tensor, void *data) {
  if (tensor == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return;
  }
  auto impl = static_cast<mindspore::MSTensor::Impl *>(tensor);
  return impl->SetData(data);
}

const void *OH_AI_TensorGetData(const OH_AI_TensorHandle tensor) {
  if (tensor == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return nullptr;
  }
  auto impl = static_cast<mindspore::MSTensor::Impl *>(tensor);
  return impl->Data().get();
}

void *OH_AI_TensorGetMutableData(const OH_AI_TensorHandle tensor) {
  if (tensor == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return nullptr;
  }
  auto impl = static_cast<mindspore::MSTensor::Impl *>(tensor);
  return impl->MutableData();
}

int64_t OH_AI_TensorGetElementNum(const OH_AI_TensorHandle tensor) {
  if (tensor == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return 0;
  }
  auto impl = static_cast<mindspore::MSTensor::Impl *>(tensor);
  return impl->ElementNum();
}

size_t OH_AI_TensorGetDataSize(const OH_AI_TensorHandle tensor) {
  if (tensor == nullptr) {
    MS_LOG(ERROR) << "param is nullptr.";
    return 0;
  }
  auto impl = static_cast<mindspore::MSTensor::Impl *>(tensor);
  return impl->DataSize();
}
