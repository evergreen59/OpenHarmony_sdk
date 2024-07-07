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
#include "src/common/log.h"
#include "mindir.h"
#include "utils.h"
#include "mindir_memory_manager.h"
#include "nnrt/v1_0/nnrt_types.h"

using namespace OHOS::HDI::Nnrt::V1_0;

namespace mindspore {
namespace lite {
// ********** Tensor **********
TensorPtr MindIR_Tensor_Create() {
  flatbuffers::FlatBufferBuilder fbb;
  std::vector<int32_t> dims(1, 0);
  std::vector<uint8_t> data(1, 0);
  std::vector<QuantParam> quant_params(1, {0, 0, 8});
  std::string name = " ";
  auto ops_offset = schema::CreateTensor(fbb, 0, DataType::DATA_TYPE_INT32, 0, schema::Format::Format_NCHW, 0, 0, 0, 0,
                                         0, fbb.CreateString(name.c_str(), name.size()));
  fbb.Finish(ops_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreateTensorFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Tensor>(new_addr);
  return ret_value;
}

TensorPtr MindIR_Tensor_Create(const std::string &name, DataType data_type, const std::vector<int32_t> &dims,
                               Format format, const std::vector<uint8_t> &data,
                               const std::vector<QuantParam> &quant_params) {
  flatbuffers::FlatBufferBuilder fbb;

  auto ops_offset =
    schema::CreateTensor(fbb, 0, data_type, fbb.CreateVector(dims.data(), dims.size()),
                         static_cast<schema::Format>(format), 0, 0, fbb.CreateVector(data.data(), data.size()),
                         ConvertQuantParams(fbb, quant_params), 0, fbb.CreateString(name.c_str(), name.size()));
  fbb.Finish(ops_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreateTensorFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Tensor>(new_addr);
  return ret_value;
}

std::string MindIR_Tensor_GetName(ConstTensorPtr tensor) {
  if (tensor != nullptr) {
    auto value = static_cast<const schema::Tensor *>(tensor);
    if (value != nullptr) {
      return value->name()->str();
    } else {
      return "";
    }
  } else {
    return "";
  }
}

void MindIR_Tensor_SetName(TensorPtr *tensor, const std::string &name) {
  if (tensor != nullptr && *tensor != nullptr) {
    auto value = static_cast<schema::Tensor *>(*tensor);
    if (value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      flatbuffers::Offset<flatbuffers::Vector<int32_t>> dims;
      if (value->dims() == nullptr || value->dims()->size() <= 0) {
        dims = 0;
      } else {
        dims = fbb.CreateVector(value->dims()->data(), value->dims()->size());
      }
      flatbuffers::Offset<flatbuffers::Vector<uint8_t>> data;
      if (value->data() == nullptr || value->data()->size() <= 0) {
        data = 0;
      } else {
        data = fbb.CreateVector(value->data()->data(), value->data()->size());
      }
      auto ops_offset = schema::CreateTensor(
        fbb, 0, value->dataType(), dims, static_cast<schema::Format>(value->format()), 0, 0, data,
        ConvertQuantParams(fbb, value->quantParams()), 0, fbb.CreateString(name.c_str(), name.size()));
      fbb.Finish(ops_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreateTensorFromBuilder(fbb, value);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *tensor = ret_value;
    }
  }
}
DataType MindIR_Tensor_GetDataType(ConstTensorPtr tensor) {
  if (tensor != nullptr) {
    auto value = static_cast<const schema::Tensor *>(tensor);
    if (value != nullptr) {
      return static_cast<DataType>(value->dataType());
    } else {
      DataType en = DATA_TYPE_INT32;
      return en;
    }
  } else {
    DataType en = DATA_TYPE_INT32;
    return en;
  }
}

void MindIR_Tensor_SetDataType(TensorPtr *tensor, DataType data_type) {
  if (tensor != nullptr && *tensor != nullptr) {
    auto value = static_cast<schema::Tensor *>(*tensor);
    if (value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      flatbuffers::Offset<flatbuffers::Vector<int32_t>> dims;
      if (value->dims() == nullptr || value->dims()->size() <= 0) {
        dims = 0;
      } else {
        dims = fbb.CreateVector(value->dims()->data(), value->dims()->size());
      }
      flatbuffers::Offset<flatbuffers::Vector<uint8_t>> data;
      if (value->data() == nullptr || value->data()->size() <= 0) {
        data = 0;
      } else {
        data = fbb.CreateVector(value->data()->data(), value->data()->size());
      }
      flatbuffers::Offset<flatbuffers::String> name;
      if (value->name() == nullptr || value->name()->size() <= 0) {
        name = 0;
      } else {
        name = fbb.CreateString(value->name()->c_str(), value->name()->size());
      }
      auto ops_offset =
        schema::CreateTensor(fbb, 0, value->dataType(), dims, static_cast<schema::Format>(value->format()), 0, 0, data,
                             ConvertQuantParams(fbb, value->quantParams()), 0, name);
      fbb.Finish(ops_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreateTensorFromBuilder(fbb, value);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *tensor = ret_value;
    }
  }
}

std::vector<int32_t> MindIR_Tensor_GetDims(ConstTensorPtr tensor) {
  if (tensor != nullptr) {
    auto value = static_cast<const schema::Tensor *>(tensor);
    if (value != nullptr) {
      std::vector<int32_t> result;
      auto src = value->dims();
      if (src == nullptr) {
        return {};
      }
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int32_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_Tensor_SetDims(TensorPtr *tensor, const std::vector<int32_t> &dims) {
  if (tensor != nullptr && *tensor != nullptr) {
    auto value = static_cast<schema::Tensor *>(*tensor);
    if (value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      flatbuffers::Offset<flatbuffers::Vector<uint8_t>> data;
      if (value->data() == nullptr || value->data()->size() <= 0) {
        data = 0;
      } else {
        data = fbb.CreateVector(value->data()->data(), value->data()->size());
      }
      flatbuffers::Offset<flatbuffers::String> name;
      if (value->name() == nullptr || value->name()->size() <= 0) {
        name = 0;
      } else {
        name = fbb.CreateString(value->name()->c_str(), value->name()->size());
      }
      auto ops_offset = schema::CreateTensor(fbb, 0, value->dataType(), fbb.CreateVector(dims.data(), dims.size()),
                                             static_cast<schema::Format>(value->format()), 0, 0, data,
                                             ConvertQuantParams(fbb, value->quantParams()), 0, name);
      fbb.Finish(ops_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreateTensorFromBuilder(fbb, value);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *tensor = ret_value;
    }
  }
}
Format MindIR_Tensor_GetFormat(ConstTensorPtr tensor) {
  if (tensor != nullptr) {
    auto value = static_cast<const schema::Tensor *>(tensor);
    if (value != nullptr) {
      return static_cast<Format>(value->format());
    } else {
      Format en = FORMAT_NCHW;
      return en;
    }
  } else {
    Format en = FORMAT_NCHW;
    return en;
  }
}

void MindIR_Tensor_SetFormat(TensorPtr *tensor, Format format) {
  if (tensor != nullptr && *tensor != nullptr) {
    auto value = static_cast<schema::Tensor *>(*tensor);
    if (value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      flatbuffers::Offset<flatbuffers::Vector<int32_t>> dims;
      if (value->dims() == nullptr || value->dims()->size() <= 0) {
        dims = 0;
      } else {
        dims = fbb.CreateVector(value->dims()->data(), value->dims()->size());
      }
      flatbuffers::Offset<flatbuffers::Vector<uint8_t>> data;
      if (value->data() == nullptr || value->data()->size() <= 0) {
        data = 0;
      } else {
        data = fbb.CreateVector(value->data()->data(), value->data()->size());
      }
      flatbuffers::Offset<flatbuffers::String> name;
      if (value->name() == nullptr || value->name()->size() <= 0) {
        name = 0;
      } else {
        name = fbb.CreateString(value->name()->c_str(), value->name()->size());
      }
      auto ops_offset = schema::CreateTensor(fbb, 0, value->dataType(), dims, static_cast<schema::Format>(format), 0, 0,
                                             data, ConvertQuantParams(fbb, value->quantParams()), 0, name);
      fbb.Finish(ops_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreateTensorFromBuilder(fbb, value);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *tensor = ret_value;
    }
  }
}

SharedBuffer MindIR_Tensor_GetData(ConstTensorPtr tensor, const SharedBuffer &buffer_templete, uint8_t *mmap_ptr,
                                   unsigned int offset) {
  if (tensor != nullptr) {
    auto value = static_cast<const schema::Tensor *>(tensor);
    if (value != nullptr) {
      SharedBuffer result{};

      if (value->data() == nullptr || value->data()->size() == 0) {
        result.fd = -1;
        result.bufferSize = buffer_templete.bufferSize;
        result.offset = offset;
        result.dataSize = 0;
        return result;
      }
      if (mmap_ptr == nullptr) {
        MS_LOG(ERROR) << "Tensor GetData failed, mmap pointer should not be nullptr";
        return {-1, 0, offset, 0};
      }
      result.fd = buffer_templete.fd;
      result.bufferSize = buffer_templete.bufferSize;
      //      MS_LOG(ERROR) << "offset:" << offset << ",src->size():" << value->data()->size();
      memcpy(mmap_ptr + offset, value->data()->data(), value->data()->size());
      result.offset = offset;
      result.dataSize = value->data()->size();
      return result;
    } else {
      MS_LOG(WARNING) << "Tensor GetData failed, mmap pointer should not be nullptr";
      return {-1, 0, offset, 0};
    }
  } else {
    return {-1, 0, offset, 0};
  }
}

std::vector<uint8_t> MindIR_Tensor_GetData(ConstTensorPtr tensor) {
  if (tensor != nullptr) {
    auto value = static_cast<const schema::Tensor *>(tensor);
    if (value != nullptr) {
      std::vector<uint8_t> result;
      auto src = value->data();
      if (src == nullptr) {
        return {};
      }
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](uint8_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_Tensor_SetData(TensorPtr *tensor, const std::vector<uint8_t> &data) {
  if (tensor != nullptr && *tensor != nullptr) {
    auto value = static_cast<schema::Tensor *>(*tensor);
    if (value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      flatbuffers::Offset<flatbuffers::Vector<int32_t>> dims;
      if (value->dims() == nullptr || value->dims()->size() <= 0) {
        dims = 0;
      } else {
        dims = fbb.CreateVector(value->dims()->data(), value->dims()->size());
      }
      flatbuffers::Offset<flatbuffers::String> name;
      if (value->name() == nullptr || value->name()->size() <= 0) {
        name = 0;
      } else {
        name = fbb.CreateString(value->name()->c_str(), value->name()->size());
      }
      auto ops_offset = schema::CreateTensor(
        fbb, 0, value->dataType(), dims, static_cast<schema::Format>(value->format()), 0, 0,
        fbb.CreateVector(data.data(), data.size()), ConvertQuantParams(fbb, value->quantParams()), 0, name);
      fbb.Finish(ops_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreateTensorFromBuilder(fbb, value);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *tensor = ret_value;
    }
  }
}
std::vector<QuantParam> MindIR_Tensor_GetQuantParams(ConstTensorPtr tensor) {
  if (tensor != nullptr) {
    auto value = static_cast<const schema::Tensor *>(tensor);
    if (value != nullptr) {
      std::vector<QuantParam> result;
      auto src = value->quantParams();
      if (src == nullptr) {
        return {};
      }
      size_t size = src->size();
      result.reserve(src->size());
      for (size_t i = 0; i < size; i++) {
        auto tmp = src->Get(i);
        QuantParam q{tmp->zeroPoint(), tmp->scale(), tmp->numBits()};
        result.emplace_back(q);
      }
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_Tensor_SetQuantParams(TensorPtr *tensor, const std::vector<QuantParam> &quant_params) {
  if (tensor != nullptr && *tensor != nullptr) {
    auto value = static_cast<schema::Tensor *>(*tensor);
    if (value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      flatbuffers::Offset<flatbuffers::Vector<int32_t>> dims;
      if (value->dims() == nullptr || value->dims()->size() <= 0) {
        dims = 0;
      } else {
        dims = fbb.CreateVector(value->dims()->data(), value->dims()->size());
      }
      flatbuffers::Offset<flatbuffers::Vector<uint8_t>> data;
      if (value->data() == nullptr || value->data()->size() <= 0) {
        data = 0;
      } else {
        data = fbb.CreateVector(value->data()->data(), value->data()->size());
      }
      flatbuffers::Offset<flatbuffers::String> name;
      if (value->name() == nullptr || value->name()->size() <= 0) {
        name = 0;
      } else {
        name = fbb.CreateString(value->name()->c_str(), value->name()->size());
      }
      auto ops_offset =
        schema::CreateTensor(fbb, 0, value->dataType(), dims, static_cast<schema::Format>(value->format()), 0, 0, data,
                             ConvertQuantParams(fbb, quant_params), 0, name);
      fbb.Finish(ops_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreateTensorFromBuilder(fbb, value);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *tensor = ret_value;
    }
  }
}

void MindIR_Tensor_Destroy(TensorPtr *tensor) {
  if (tensor != nullptr && *tensor != nullptr) {
    auto schema = static_cast<schema::Tensor *>(*tensor);
    MindIRMemoryManager::GetInstance()->DeleteTensor(schema);
    *tensor = nullptr;
  }
  *tensor = nullptr;
}
}  // namespace lite
}  // namespace mindspore
