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
#include "mindir.h"
#include "utils.h"
#include "schema/model_generated.h"
#include "mindir_memory_manager.h"
//----TODO---write an example to run MindIRMemoryManager
namespace mindspore {
namespace lite {

// ********** Activation **********
PrimitivePtr MindIR_Activation_CreatePrimitive(ActivationType activation_type, float alpha, float min_val,
                                               float max_val, bool approximate) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateActivation(fbb, static_cast<schema::ActivationType>(activation_type), alpha, min_val,
                                             max_val, approximate);
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_ACTIVATION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
ActivationType MindIR_Activation_GetActivationType(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Activation();
    if (prim != nullptr && value != nullptr) {
      return static_cast<ActivationType>(value->activation_type());
    } else {
      ActivationType en = static_cast<ActivationType>(0);
      return en;
    }
  } else {
    ActivationType en = static_cast<ActivationType>(0);
    return en;
  }
}

void MindIR_Activation_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Activation();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset =
        schema::CreateActivation(fbb, static_cast<schema::ActivationType>(activation_type), value->alpha(),
                                 value->min_val(), value->max_val(), value->approximate());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_ACTIVATION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
float MindIR_Activation_GetAlpha(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Activation();
    if (prim != nullptr && value != nullptr) {
      return value->alpha();
    } else {
      return .0;
    }
  } else {
    return .0;
  }
}

void MindIR_Activation_SetAlpha(PrimitivePtr *primitive, float alpha) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Activation();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateActivation(fbb, static_cast<schema::ActivationType>(value->activation_type()),
                                                 alpha, value->min_val(), value->max_val(), value->approximate());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_ACTIVATION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
float MindIR_Activation_GetMinVal(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Activation();
    if (prim != nullptr && value != nullptr) {
      return value->min_val();
    } else {
      return .0;
    }
  } else {
    return .0;
  }
}

void MindIR_Activation_SetMinVal(PrimitivePtr *primitive, float min_val) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Activation();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateActivation(fbb, static_cast<schema::ActivationType>(value->activation_type()),
                                                 value->alpha(), min_val, value->max_val(), value->approximate());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_ACTIVATION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
float MindIR_Activation_GetMaxVal(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Activation();
    if (prim != nullptr && value != nullptr) {
      return value->max_val();
    } else {
      return .0;
    }
  } else {
    return .0;
  }
}

void MindIR_Activation_SetMaxVal(PrimitivePtr *primitive, float max_val) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Activation();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateActivation(fbb, static_cast<schema::ActivationType>(value->activation_type()),
                                                 value->alpha(), value->min_val(), max_val, value->approximate());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_ACTIVATION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
bool MindIR_Activation_GetApproximate(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Activation();
    if (prim != nullptr && value != nullptr) {
      return value->approximate();
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void MindIR_Activation_SetApproximate(PrimitivePtr *primitive, bool approximate) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Activation();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateActivation(fbb, static_cast<schema::ActivationType>(value->activation_type()),
                                                 value->alpha(), value->min_val(), value->max_val(), approximate);
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_ACTIVATION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** AddFusion **********
PrimitivePtr MindIR_AddFusion_CreatePrimitive(ActivationType activation_type) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateAddFusion(fbb, static_cast<schema::ActivationType>(activation_type));
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_ADD_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
ActivationType MindIR_AddFusion_GetActivationType(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_AddFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<ActivationType>(value->activation_type());
    } else {
      ActivationType en = static_cast<ActivationType>(0);
      return en;
    }
  } else {
    ActivationType en = static_cast<ActivationType>(0);
    return en;
  }
}

void MindIR_AddFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_AddFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateAddFusion(fbb, static_cast<schema::ActivationType>(activation_type));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_ADD_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** ArgMaxFusion **********
PrimitivePtr MindIR_ArgMaxFusion_CreatePrimitive(int64_t axis, int64_t top_k, bool keep_dims, bool out_max_value) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateArgMaxFusion(fbb, axis, top_k, keep_dims, out_max_value);
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_ARGMAX_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
int64_t MindIR_ArgMaxFusion_GetAxis(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_ArgMaxFusion();
    if (prim != nullptr && value != nullptr) {
      return value->axis();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_ArgMaxFusion_SetAxis(PrimitivePtr *primitive, int64_t axis) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_ArgMaxFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset =
        schema::CreateArgMaxFusion(fbb, axis, value->top_k(), value->keep_dims(), value->out_max_value());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_ARGMAX_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_ArgMaxFusion_GetTopK(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_ArgMaxFusion();
    if (prim != nullptr && value != nullptr) {
      return value->top_k();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_ArgMaxFusion_SetTopK(PrimitivePtr *primitive, int64_t top_k) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_ArgMaxFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset =
        schema::CreateArgMaxFusion(fbb, value->axis(), top_k, value->keep_dims(), value->out_max_value());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_ARGMAX_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
bool MindIR_ArgMaxFusion_GetKeepDims(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_ArgMaxFusion();
    if (prim != nullptr && value != nullptr) {
      return value->keep_dims();
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void MindIR_ArgMaxFusion_SetKeepDims(PrimitivePtr *primitive, bool keep_dims) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_ArgMaxFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset =
        schema::CreateArgMaxFusion(fbb, value->axis(), value->top_k(), keep_dims, value->out_max_value());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_ARGMAX_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
bool MindIR_ArgMaxFusion_GetOutMaxValue(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_ArgMaxFusion();
    if (prim != nullptr && value != nullptr) {
      return value->out_max_value();
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void MindIR_ArgMaxFusion_SetOutMaxValue(PrimitivePtr *primitive, bool out_max_value) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_ArgMaxFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset =
        schema::CreateArgMaxFusion(fbb, value->axis(), value->top_k(), value->keep_dims(), out_max_value);
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_ARGMAX_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** AvgPoolFusion **********
PrimitivePtr MindIR_AvgPoolFusion_CreatePrimitive(const std::vector<int64_t> &kernel_size,
                                                  const std::vector<int64_t> &strides, const std::vector<int64_t> &pad,
                                                  PadMode pad_mode, RoundMode round_mode, Format format, bool global,
                                                  ActivationType activation_type) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateAvgPoolFusion(
    fbb, fbb.CreateVector(kernel_size.data(), kernel_size.size()), fbb.CreateVector(strides.data(), strides.size()),
    fbb.CreateVector(pad.data(), pad.size()), static_cast<schema::PadMode>(pad_mode),
    static_cast<schema::RoundMode>(round_mode), static_cast<schema::Format>(format), global,
    static_cast<schema::ActivationType>(activation_type));
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_AVG_POOL_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
std::vector<int64_t> MindIR_AvgPoolFusion_GetKernelSize(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_AvgPoolFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->kernel_size();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_AvgPoolFusion_SetKernelSize(PrimitivePtr *primitive, const std::vector<int64_t> &kernel_size) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_AvgPoolFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateAvgPoolFusion(
        fbb, fbb.CreateVector(kernel_size.data(), kernel_size.size()),
        fbb.CreateVector(value->strides()->data(), value->strides()->size()),
        fbb.CreateVector(value->pad()->data(), value->pad()->size()), static_cast<schema::PadMode>(value->pad_mode()),
        static_cast<schema::RoundMode>(value->round_mode()), static_cast<schema::Format>(value->format()),
        value->global(), static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_AVG_POOL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
std::vector<int64_t> MindIR_AvgPoolFusion_GetStrides(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_AvgPoolFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->strides();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_AvgPoolFusion_SetStrides(PrimitivePtr *primitive, const std::vector<int64_t> &strides) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_AvgPoolFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateAvgPoolFusion(
        fbb, fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(strides.data(), strides.size()), fbb.CreateVector(value->pad()->data(), value->pad()->size()),
        static_cast<schema::PadMode>(value->pad_mode()), static_cast<schema::RoundMode>(value->round_mode()),
        static_cast<schema::Format>(value->format()), value->global(),
        static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_AVG_POOL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
std::vector<int64_t> MindIR_AvgPoolFusion_GetPad(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_AvgPoolFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->pad();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_AvgPoolFusion_SetPad(PrimitivePtr *primitive, const std::vector<int64_t> &pad) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_AvgPoolFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateAvgPoolFusion(
        fbb, fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->strides()->data(), value->strides()->size()), fbb.CreateVector(pad.data(), pad.size()),
        static_cast<schema::PadMode>(value->pad_mode()), static_cast<schema::RoundMode>(value->round_mode()),
        static_cast<schema::Format>(value->format()), value->global(),
        static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_AVG_POOL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
PadMode MindIR_AvgPoolFusion_GetPadMode(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_AvgPoolFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<PadMode>(value->pad_mode());
    } else {
      PadMode en = static_cast<PadMode>(0);
      return en;
    }
  } else {
    PadMode en = static_cast<PadMode>(0);
    return en;
  }
}

void MindIR_AvgPoolFusion_SetPadMode(PrimitivePtr *primitive, PadMode pad_mode) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_AvgPoolFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateAvgPoolFusion(
        fbb, fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->strides()->data(), value->strides()->size()),
        fbb.CreateVector(value->pad()->data(), value->pad()->size()), static_cast<schema::PadMode>(pad_mode),
        static_cast<schema::RoundMode>(value->round_mode()), static_cast<schema::Format>(value->format()),
        value->global(), static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_AVG_POOL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
RoundMode MindIR_AvgPoolFusion_GetRoundMode(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_AvgPoolFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<RoundMode>(value->round_mode());
    } else {
      RoundMode en = static_cast<RoundMode>(0);
      return en;
    }
  } else {
    RoundMode en = static_cast<RoundMode>(0);
    return en;
  }
}

void MindIR_AvgPoolFusion_SetRoundMode(PrimitivePtr *primitive, RoundMode round_mode) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_AvgPoolFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateAvgPoolFusion(
        fbb, fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->strides()->data(), value->strides()->size()),
        fbb.CreateVector(value->pad()->data(), value->pad()->size()), static_cast<schema::PadMode>(value->pad_mode()),
        static_cast<schema::RoundMode>(round_mode), static_cast<schema::Format>(value->format()), value->global(),
        static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_AVG_POOL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
Format MindIR_AvgPoolFusion_GetFormat(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_AvgPoolFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<Format>(value->format());
    } else {
      Format en = static_cast<Format>(0);
      return en;
    }
  } else {
    Format en = static_cast<Format>(0);
    return en;
  }
}

void MindIR_AvgPoolFusion_SetFormat(PrimitivePtr *primitive, Format format) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_AvgPoolFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateAvgPoolFusion(
        fbb, fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->strides()->data(), value->strides()->size()),
        fbb.CreateVector(value->pad()->data(), value->pad()->size()), static_cast<schema::PadMode>(value->pad_mode()),
        static_cast<schema::RoundMode>(value->round_mode()), static_cast<schema::Format>(format), value->global(),
        static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_AVG_POOL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
bool MindIR_AvgPoolFusion_GetGlobal(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_AvgPoolFusion();
    if (prim != nullptr && value != nullptr) {
      return value->global();
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void MindIR_AvgPoolFusion_SetGlobal(PrimitivePtr *primitive, bool global) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_AvgPoolFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateAvgPoolFusion(
        fbb, fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->strides()->data(), value->strides()->size()),
        fbb.CreateVector(value->pad()->data(), value->pad()->size()), static_cast<schema::PadMode>(value->pad_mode()),
        static_cast<schema::RoundMode>(value->round_mode()), static_cast<schema::Format>(value->format()), global,
        static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_AVG_POOL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
ActivationType MindIR_AvgPoolFusion_GetActivationType(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_AvgPoolFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<ActivationType>(value->activation_type());
    } else {
      ActivationType en = static_cast<ActivationType>(0);
      return en;
    }
  } else {
    ActivationType en = static_cast<ActivationType>(0);
    return en;
  }
}

void MindIR_AvgPoolFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_AvgPoolFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateAvgPoolFusion(
        fbb, fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->strides()->data(), value->strides()->size()),
        fbb.CreateVector(value->pad()->data(), value->pad()->size()), static_cast<schema::PadMode>(value->pad_mode()),
        static_cast<schema::RoundMode>(value->round_mode()), static_cast<schema::Format>(value->format()),
        value->global(), static_cast<schema::ActivationType>(activation_type));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_AVG_POOL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** BatchToSpaceND **********
PrimitivePtr MindIR_BatchToSpaceND_CreatePrimitive(const std::vector<int64_t> &block_shape,
                                                   const std::vector<std::vector<int64_t>> &crops) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateBatchToSpaceND(fbb, fbb.CreateVector(block_shape.data(), block_shape.size()),
                                                 CreateVec2D(fbb, crops));
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_BATCH_TO_SPACE_ND), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
std::vector<int64_t> MindIR_BatchToSpaceND_GetBlockShape(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_BatchToSpaceND();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->block_shape();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_BatchToSpaceND_SetBlockShape(PrimitivePtr *primitive, const std::vector<int64_t> &block_shape) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_BatchToSpaceND();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateBatchToSpaceND(fbb, fbb.CreateVector(block_shape.data(), block_shape.size()),
                                                     CreateVec2D(fbb, value->crops()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_BATCH_TO_SPACE_ND), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
std::vector<std::vector<int64_t>> MindIR_BatchToSpaceND_GetCrops(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_BatchToSpaceND();
    if (prim != nullptr && value != nullptr) {
      std::vector<std::vector<int64_t>> out;
      auto src = value->crops();
      for (auto sub_list : *src->data()) {
        std::vector<int64_t> result_tmp;
        result_tmp.resize(sub_list->data()->size());
        std::transform(sub_list->data()->begin(), sub_list->data()->end(), result_tmp.begin(),
                       [](int64_t item) { return item; });
        out.emplace_back(result_tmp);
      }
      return out;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_BatchToSpaceND_SetCrops(PrimitivePtr *primitive, const std::vector<std::vector<int64_t>> &crops) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_BatchToSpaceND();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateBatchToSpaceND(
        fbb, fbb.CreateVector(value->block_shape()->data(), value->block_shape()->size()), CreateVec2D(fbb, crops));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_BATCH_TO_SPACE_ND), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** BiasAdd **********
PrimitivePtr MindIR_BiasAdd_CreatePrimitive() {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateBiasAdd(fbb);
  auto prim_offset = schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_BIAS_ADD), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}

// ********** Cast **********
PrimitivePtr MindIR_Cast_CreatePrimitive() {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateCast(fbb);
  auto prim_offset = schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CAST), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}

// ********** Concat **********
PrimitivePtr MindIR_Concat_CreatePrimitive(int64_t axis) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateConcat(fbb, axis);
  auto prim_offset = schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONCAT), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
int64_t MindIR_Concat_GetAxis(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Concat();
    if (prim != nullptr && value != nullptr) {
      return value->axis();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_Concat_SetAxis(PrimitivePtr *primitive, int64_t axis) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Concat();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConcat(fbb, axis);
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONCAT), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** Conv2DFusion **********
PrimitivePtr MindIR_Conv2DFusion_CreatePrimitive(const std::vector<int64_t> &kernel_size,
                                                 const std::vector<int64_t> &stride,
                                                 const std::vector<int64_t> &dilation, PadMode pad_mode,
                                                 const std::vector<int64_t> &pad_list, int64_t group,
                                                 int64_t in_channel, int64_t out_channel,
                                                 ActivationType activation_type) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateConv2DFusion(
    fbb, mindspore::schema::Format_NCHW, fbb.CreateVector(kernel_size.data(), kernel_size.size()),
    fbb.CreateVector(stride.data(), stride.size()), fbb.CreateVector(dilation.data(), dilation.size()),
    static_cast<schema::PadMode>(pad_mode), fbb.CreateVector(pad_list.data(), pad_list.size()), 0, group, in_channel,
    out_channel, static_cast<schema::ActivationType>(activation_type));
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
std::vector<int64_t> MindIR_Conv2DFusion_GetKernelSize(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->kernel_size();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_Conv2DFusion_SetKernelSize(PrimitivePtr *primitive, const std::vector<int64_t> &kernel_size) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2DFusion(
        fbb, mindspore::schema::Format_NCHW, fbb.CreateVector(kernel_size.data(), kernel_size.size()),
        fbb.CreateVector(value->stride()->data(), value->stride()->size()),
        fbb.CreateVector(value->dilation()->data(), value->dilation()->size()),
        static_cast<schema::PadMode>(value->pad_mode()),
        fbb.CreateVector(value->pad_list()->data(), value->pad_list()->size()), 0, value->group(), value->in_channel(),
        value->out_channel(), static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
std::vector<int64_t> MindIR_Conv2DFusion_GetStride(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->stride();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_Conv2DFusion_SetStride(PrimitivePtr *primitive, const std::vector<int64_t> &stride) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2DFusion(
        fbb, mindspore::schema::Format_NCHW,
        fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(stride.data(), stride.size()),
        fbb.CreateVector(value->dilation()->data(), value->dilation()->size()),
        static_cast<schema::PadMode>(value->pad_mode()),
        fbb.CreateVector(value->pad_list()->data(), value->pad_list()->size()), 0, value->group(), value->in_channel(),
        value->out_channel(), static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
std::vector<int64_t> MindIR_Conv2DFusion_GetDilation(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->dilation();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_Conv2DFusion_SetDilation(PrimitivePtr *primitive, const std::vector<int64_t> &dilation) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2DFusion(
        fbb, mindspore::schema::Format_NCHW,
        fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->stride()->data(), value->stride()->size()),
        fbb.CreateVector(dilation.data(), dilation.size()), static_cast<schema::PadMode>(value->pad_mode()),
        fbb.CreateVector(value->pad_list()->data(), value->pad_list()->size()), 0, value->group(), value->in_channel(),
        value->out_channel(), static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
PadMode MindIR_Conv2DFusion_GetPadMode(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<PadMode>(value->pad_mode());
    } else {
      PadMode en = static_cast<PadMode>(0);
      return en;
    }
  } else {
    PadMode en = static_cast<PadMode>(0);
    return en;
  }
}

void MindIR_Conv2DFusion_SetPadMode(PrimitivePtr *primitive, PadMode pad_mode) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2DFusion(
        fbb, mindspore::schema::Format_NCHW,
        fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->stride()->data(), value->stride()->size()),
        fbb.CreateVector(value->dilation()->data(), value->dilation()->size()), static_cast<schema::PadMode>(pad_mode),
        fbb.CreateVector(value->pad_list()->data(), value->pad_list()->size()), 0, value->group(), value->in_channel(),
        value->out_channel(), static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
std::vector<int64_t> MindIR_Conv2DFusion_GetPadList(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->pad_list();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_Conv2DFusion_SetPadList(PrimitivePtr *primitive, const std::vector<int64_t> &pad_list) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2DFusion(
        fbb, mindspore::schema::Format_NCHW,
        fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->stride()->data(), value->stride()->size()),
        fbb.CreateVector(value->dilation()->data(), value->dilation()->size()),
        static_cast<schema::PadMode>(value->pad_mode()), fbb.CreateVector(pad_list.data(), pad_list.size()), 0,
        value->group(), value->in_channel(), value->out_channel(),
        static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_Conv2DFusion_GetGroup(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      return value->group();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_Conv2DFusion_SetGroup(PrimitivePtr *primitive, int64_t group) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2DFusion(
        fbb, mindspore::schema::Format_NCHW,
        fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->stride()->data(), value->stride()->size()),
        fbb.CreateVector(value->dilation()->data(), value->dilation()->size()),
        static_cast<schema::PadMode>(value->pad_mode()),
        fbb.CreateVector(value->pad_list()->data(), value->pad_list()->size()), 0, group, value->in_channel(),
        value->out_channel(), static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_Conv2DFusion_GetInChannel(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      return value->in_channel();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_Conv2DFusion_SetInChannel(PrimitivePtr *primitive, int64_t in_channel) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2DFusion(
        fbb, mindspore::schema::Format_NCHW,
        fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->stride()->data(), value->stride()->size()),
        fbb.CreateVector(value->dilation()->data(), value->dilation()->size()),
        static_cast<schema::PadMode>(value->pad_mode()),
        fbb.CreateVector(value->pad_list()->data(), value->pad_list()->size()), 0, value->group(), in_channel,
        value->out_channel(), static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_Conv2DFusion_GetOutChannel(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      return value->out_channel();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_Conv2DFusion_SetOutChannel(PrimitivePtr *primitive, int64_t out_channel) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2DFusion(
        fbb, mindspore::schema::Format_NCHW,
        fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->stride()->data(), value->stride()->size()),
        fbb.CreateVector(value->dilation()->data(), value->dilation()->size()),
        static_cast<schema::PadMode>(value->pad_mode()),
        fbb.CreateVector(value->pad_list()->data(), value->pad_list()->size()), 0, value->group(), value->in_channel(),
        out_channel, static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
ActivationType MindIR_Conv2DFusion_GetActivationType(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<ActivationType>(value->activation_type());
    } else {
      ActivationType en = static_cast<ActivationType>(0);
      return en;
    }
  } else {
    ActivationType en = static_cast<ActivationType>(0);
    return en;
  }
}

void MindIR_Conv2DFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2DFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2DFusion(
        fbb, mindspore::schema::Format_NCHW,
        fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->stride()->data(), value->stride()->size()),
        fbb.CreateVector(value->dilation()->data(), value->dilation()->size()),
        static_cast<schema::PadMode>(value->pad_mode()),
        fbb.CreateVector(value->pad_list()->data(), value->pad_list()->size()), 0, value->group(), value->in_channel(),
        value->out_channel(), static_cast<schema::ActivationType>(activation_type));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** Conv2dTransposeFusion **********
PrimitivePtr MindIR_Conv2dTransposeFusion_CreatePrimitive(
  const std::vector<int64_t> &kernel_size, const std::vector<int64_t> &stride, const std::vector<int64_t> &dilation,
  PadMode pad_mode, const std::vector<int64_t> &pad_list, int64_t group, int64_t in_channel, int64_t out_channel,
  ActivationType activation_type, const std::vector<int64_t> &output_paddings) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateConv2dTransposeFusion(
    fbb, mindspore::schema::Format_NCHW, fbb.CreateVector(kernel_size.data(), kernel_size.size()),
    fbb.CreateVector(stride.data(), stride.size()), fbb.CreateVector(dilation.data(), dilation.size()),
    static_cast<schema::PadMode>(pad_mode), 0, fbb.CreateVector(pad_list.data(), pad_list.size()), 0, group, in_channel,
    out_channel, static_cast<schema::ActivationType>(activation_type),
    fbb.CreateVector(output_paddings.data(), output_paddings.size()));
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_TRANSPOSE_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
std::vector<int64_t> MindIR_Conv2dTransposeFusion_GetKernelSize(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->kernel_size();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_Conv2dTransposeFusion_SetKernelSize(PrimitivePtr *primitive, const std::vector<int64_t> &kernel_size) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2dTransposeFusion(
        fbb, mindspore::schema::Format_NCHW, fbb.CreateVector(kernel_size.data(), kernel_size.size()),
        fbb.CreateVector(value->stride()->data(), value->stride()->size()),
        fbb.CreateVector(value->dilation()->data(), value->dilation()->size()),
        static_cast<schema::PadMode>(value->pad_mode()), 0,
        fbb.CreateVector(value->pad_list()->data(), value->pad_list()->size()), 0, value->group(), value->in_channel(),
        value->out_channel(), static_cast<schema::ActivationType>(value->activation_type()),
        fbb.CreateVector(value->output_paddings()->data(), value->output_paddings()->size()));
      auto prim_offset = schema::CreatePrimitive(
        fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_TRANSPOSE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
std::vector<int64_t> MindIR_Conv2dTransposeFusion_GetStride(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->stride();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_Conv2dTransposeFusion_SetStride(PrimitivePtr *primitive, const std::vector<int64_t> &stride) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2dTransposeFusion(
        fbb, mindspore::schema::Format_NCHW,
        fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(stride.data(), stride.size()),
        fbb.CreateVector(value->dilation()->data(), value->dilation()->size()),
        static_cast<schema::PadMode>(value->pad_mode()), 0,
        fbb.CreateVector(value->pad_list()->data(), value->pad_list()->size()), 0, value->group(), value->in_channel(),
        value->out_channel(), static_cast<schema::ActivationType>(value->activation_type()),
        fbb.CreateVector(value->output_paddings()->data(), value->output_paddings()->size()));
      auto prim_offset = schema::CreatePrimitive(
        fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_TRANSPOSE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
std::vector<int64_t> MindIR_Conv2dTransposeFusion_GetDilation(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->dilation();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_Conv2dTransposeFusion_SetDilation(PrimitivePtr *primitive, const std::vector<int64_t> &dilation) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2dTransposeFusion(
        fbb, mindspore::schema::Format_NCHW,
        fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->stride()->data(), value->stride()->size()),
        fbb.CreateVector(dilation.data(), dilation.size()), static_cast<schema::PadMode>(value->pad_mode()), 0,
        fbb.CreateVector(value->pad_list()->data(), value->pad_list()->size()), 0, value->group(), value->in_channel(),
        value->out_channel(), static_cast<schema::ActivationType>(value->activation_type()),
        fbb.CreateVector(value->output_paddings()->data(), value->output_paddings()->size()));
      auto prim_offset = schema::CreatePrimitive(
        fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_TRANSPOSE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
PadMode MindIR_Conv2dTransposeFusion_GetPadMode(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<PadMode>(value->pad_mode());
    } else {
      PadMode en = static_cast<PadMode>(0);
      return en;
    }
  } else {
    PadMode en = static_cast<PadMode>(0);
    return en;
  }
}

void MindIR_Conv2dTransposeFusion_SetPadMode(PrimitivePtr *primitive, PadMode pad_mode) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2dTransposeFusion(
        fbb, mindspore::schema::Format_NCHW,
        fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->stride()->data(), value->stride()->size()),
        fbb.CreateVector(value->dilation()->data(), value->dilation()->size()), static_cast<schema::PadMode>(pad_mode),
        0, fbb.CreateVector(value->pad_list()->data(), value->pad_list()->size()), 0, value->group(),
        value->in_channel(), value->out_channel(), static_cast<schema::ActivationType>(value->activation_type()),
        fbb.CreateVector(value->output_paddings()->data(), value->output_paddings()->size()));
      auto prim_offset = schema::CreatePrimitive(
        fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_TRANSPOSE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
std::vector<int64_t> MindIR_Conv2dTransposeFusion_GetPadList(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->pad_list();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_Conv2dTransposeFusion_SetPadList(PrimitivePtr *primitive, const std::vector<int64_t> &pad_list) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2dTransposeFusion(
        fbb, mindspore::schema::Format_NCHW,
        fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->stride()->data(), value->stride()->size()),
        fbb.CreateVector(value->dilation()->data(), value->dilation()->size()),
        static_cast<schema::PadMode>(value->pad_mode()), 0, fbb.CreateVector(pad_list.data(), pad_list.size()), 0,
        value->group(), value->in_channel(), value->out_channel(),
        static_cast<schema::ActivationType>(value->activation_type()),
        fbb.CreateVector(value->output_paddings()->data(), value->output_paddings()->size()));
      auto prim_offset = schema::CreatePrimitive(
        fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_TRANSPOSE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_Conv2dTransposeFusion_GetGroup(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      return value->group();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_Conv2dTransposeFusion_SetGroup(PrimitivePtr *primitive, int64_t group) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2dTransposeFusion(
        fbb, mindspore::schema::Format_NCHW,
        fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->stride()->data(), value->stride()->size()),
        fbb.CreateVector(value->dilation()->data(), value->dilation()->size()),
        static_cast<schema::PadMode>(value->pad_mode()), 0,
        fbb.CreateVector(value->pad_list()->data(), value->pad_list()->size()), 0, group, value->in_channel(),
        value->out_channel(), static_cast<schema::ActivationType>(value->activation_type()),
        fbb.CreateVector(value->output_paddings()->data(), value->output_paddings()->size()));
      auto prim_offset = schema::CreatePrimitive(
        fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_TRANSPOSE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_Conv2dTransposeFusion_GetInChannel(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      return value->in_channel();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_Conv2dTransposeFusion_SetInChannel(PrimitivePtr *primitive, int64_t in_channel) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2dTransposeFusion(
        fbb, mindspore::schema::Format_NCHW,
        fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->stride()->data(), value->stride()->size()),
        fbb.CreateVector(value->dilation()->data(), value->dilation()->size()),
        static_cast<schema::PadMode>(value->pad_mode()), 0,
        fbb.CreateVector(value->pad_list()->data(), value->pad_list()->size()), 0, value->group(), in_channel,
        value->out_channel(), static_cast<schema::ActivationType>(value->activation_type()),
        fbb.CreateVector(value->output_paddings()->data(), value->output_paddings()->size()));
      auto prim_offset = schema::CreatePrimitive(
        fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_TRANSPOSE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_Conv2dTransposeFusion_GetOutChannel(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      return value->out_channel();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_Conv2dTransposeFusion_SetOutChannel(PrimitivePtr *primitive, int64_t out_channel) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2dTransposeFusion(
        fbb, mindspore::schema::Format_NCHW,
        fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->stride()->data(), value->stride()->size()),
        fbb.CreateVector(value->dilation()->data(), value->dilation()->size()),
        static_cast<schema::PadMode>(value->pad_mode()), 0,
        fbb.CreateVector(value->pad_list()->data(), value->pad_list()->size()), 0, value->group(), value->in_channel(),
        out_channel, static_cast<schema::ActivationType>(value->activation_type()),
        fbb.CreateVector(value->output_paddings()->data(), value->output_paddings()->size()));
      auto prim_offset = schema::CreatePrimitive(
        fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_TRANSPOSE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
ActivationType MindIR_Conv2dTransposeFusion_GetActivationType(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<ActivationType>(value->activation_type());
    } else {
      ActivationType en = static_cast<ActivationType>(0);
      return en;
    }
  } else {
    ActivationType en = static_cast<ActivationType>(0);
    return en;
  }
}

void MindIR_Conv2dTransposeFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2dTransposeFusion(
        fbb, mindspore::schema::Format_NCHW,
        fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->stride()->data(), value->stride()->size()),
        fbb.CreateVector(value->dilation()->data(), value->dilation()->size()),
        static_cast<schema::PadMode>(value->pad_mode()), 0,
        fbb.CreateVector(value->pad_list()->data(), value->pad_list()->size()), 0, value->group(), value->in_channel(),
        value->out_channel(), static_cast<schema::ActivationType>(activation_type),
        fbb.CreateVector(value->output_paddings()->data(), value->output_paddings()->size()));
      auto prim_offset = schema::CreatePrimitive(
        fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_TRANSPOSE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
std::vector<int64_t> MindIR_Conv2dTransposeFusion_GetOutputPaddings(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->output_paddings();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_Conv2dTransposeFusion_SetOutputPaddings(PrimitivePtr *primitive,
                                                    const std::vector<int64_t> &output_paddings) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Conv2dTransposeFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateConv2dTransposeFusion(
        fbb, mindspore::schema::Format_NCHW,
        fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->stride()->data(), value->stride()->size()),
        fbb.CreateVector(value->dilation()->data(), value->dilation()->size()),
        static_cast<schema::PadMode>(value->pad_mode()), 0,
        fbb.CreateVector(value->pad_list()->data(), value->pad_list()->size()), 0, value->group(), value->in_channel(),
        value->out_channel(), static_cast<schema::ActivationType>(value->activation_type()),
        fbb.CreateVector(output_paddings.data(), output_paddings.size()));
      auto prim_offset = schema::CreatePrimitive(
        fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_CONV2D_TRANSPOSE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** DivFusion **********
PrimitivePtr MindIR_DivFusion_CreatePrimitive(ActivationType activation_type) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateDivFusion(fbb, static_cast<schema::ActivationType>(activation_type));
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_DIV_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
ActivationType MindIR_DivFusion_GetActivationType(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_DivFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<ActivationType>(value->activation_type());
    } else {
      ActivationType en = static_cast<ActivationType>(0);
      return en;
    }
  } else {
    ActivationType en = static_cast<ActivationType>(0);
    return en;
  }
}

void MindIR_DivFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_DivFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateDivFusion(fbb, static_cast<schema::ActivationType>(activation_type));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_DIV_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** Eltwise **********
PrimitivePtr MindIR_Eltwise_CreatePrimitive(EltwiseMode mode) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateEltwise(fbb, static_cast<schema::EltwiseMode>(mode));
  auto prim_offset = schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_ELTWISE), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
EltwiseMode MindIR_Eltwise_GetMode(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Eltwise();
    if (prim != nullptr && value != nullptr) {
      return static_cast<EltwiseMode>(value->mode());
    } else {
      EltwiseMode en = static_cast<EltwiseMode>(0);
      return en;
    }
  } else {
    EltwiseMode en = static_cast<EltwiseMode>(0);
    return en;
  }
}

void MindIR_Eltwise_SetMode(PrimitivePtr *primitive, EltwiseMode mode) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Eltwise();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateEltwise(fbb, static_cast<schema::EltwiseMode>(mode));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_ELTWISE), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** ExpandDims **********
PrimitivePtr MindIR_ExpandDims_CreatePrimitive() {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateExpandDims(fbb);
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_EXPAND_DIMS), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}

// ********** Fill **********
PrimitivePtr MindIR_Fill_CreatePrimitive() {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateFill(fbb);
  auto prim_offset = schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_FILL), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}

// ********** FullConnection **********
PrimitivePtr MindIR_FullConnection_CreatePrimitive(bool has_bias, bool use_axis, int64_t axis,
                                                   ActivationType activation_type) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset =
    schema::CreateFullConnection(fbb, has_bias, use_axis, axis, static_cast<schema::ActivationType>(activation_type));
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_FULL_CONNECTION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
bool MindIR_FullConnection_GetHasBias(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_FullConnection();
    if (prim != nullptr && value != nullptr) {
      return value->has_bias();
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void MindIR_FullConnection_SetHasBias(PrimitivePtr *primitive, bool has_bias) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_FullConnection();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateFullConnection(fbb, has_bias, value->use_axis(), value->axis(),
                                                     static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_FULL_CONNECTION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
bool MindIR_FullConnection_GetUseAxis(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_FullConnection();
    if (prim != nullptr && value != nullptr) {
      return value->use_axis();
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void MindIR_FullConnection_SetUseAxis(PrimitivePtr *primitive, bool use_axis) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_FullConnection();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateFullConnection(fbb, value->has_bias(), use_axis, value->axis(),
                                                     static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_FULL_CONNECTION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_FullConnection_GetAxis(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_FullConnection();
    if (prim != nullptr && value != nullptr) {
      return value->axis();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_FullConnection_SetAxis(PrimitivePtr *primitive, int64_t axis) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_FullConnection();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateFullConnection(fbb, value->has_bias(), value->use_axis(), axis,
                                                     static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_FULL_CONNECTION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
ActivationType MindIR_FullConnection_GetActivationType(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_FullConnection();
    if (prim != nullptr && value != nullptr) {
      return static_cast<ActivationType>(value->activation_type());
    } else {
      ActivationType en = static_cast<ActivationType>(0);
      return en;
    }
  } else {
    ActivationType en = static_cast<ActivationType>(0);
    return en;
  }
}

void MindIR_FullConnection_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_FullConnection();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateFullConnection(fbb, value->has_bias(), value->use_axis(), value->axis(),
                                                     static_cast<schema::ActivationType>(activation_type));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_FULL_CONNECTION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** FusedBatchNorm **********
PrimitivePtr MindIR_FusedBatchNorm_CreatePrimitive(float epsilon) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateFusedBatchNorm(fbb, 0.9, 0);
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_FUSED_BATCH_NORM), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
float MindIR_FusedBatchNorm_GetEpsilon(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_FusedBatchNorm();
    if (prim != nullptr && value != nullptr) {
      return value->epsilon();
    } else {
      return .0;
    }
  } else {
    return .0;
  }
}

void MindIR_FusedBatchNorm_SetEpsilon(PrimitivePtr *primitive, float epsilon) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_FusedBatchNorm();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateFusedBatchNorm(fbb, epsilon, 0.9, 0);
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_FUSED_BATCH_NORM), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** Gather **********
PrimitivePtr MindIR_Gather_CreatePrimitive() {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateGather(fbb);
  auto prim_offset = schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_GATHER), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}

// ********** LayerNormFusion **********
PrimitivePtr MindIR_LayerNormFusion_CreatePrimitive(int64_t begin_norm_axis, float epsilon, bool elementwise_affine,
                                                    int64_t begin_params_axis) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateLayerNormFusion(fbb, begin_norm_axis, epsilon, elementwise_affine, begin_params_axis);
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_LAYER_NORM_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
int64_t MindIR_LayerNormFusion_GetBeginNormAxis(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_LayerNormFusion();
    if (prim != nullptr && value != nullptr) {
      return value->begin_norm_axis();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_LayerNormFusion_SetBeginNormAxis(PrimitivePtr *primitive, int64_t begin_norm_axis) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_LayerNormFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateLayerNormFusion(fbb, begin_norm_axis, value->epsilon(),
                                                      value->elementwise_affine(), value->begin_params_axis());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_LAYER_NORM_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
float MindIR_LayerNormFusion_GetEpsilon(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_LayerNormFusion();
    if (prim != nullptr && value != nullptr) {
      return value->epsilon();
    } else {
      return .0;
    }
  } else {
    return .0;
  }
}

void MindIR_LayerNormFusion_SetEpsilon(PrimitivePtr *primitive, float epsilon) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_LayerNormFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateLayerNormFusion(fbb, value->begin_norm_axis(), epsilon,
                                                      value->elementwise_affine(), value->begin_params_axis());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_LAYER_NORM_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
bool MindIR_LayerNormFusion_GetElementwiseAffine(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_LayerNormFusion();
    if (prim != nullptr && value != nullptr) {
      return value->elementwise_affine();
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void MindIR_LayerNormFusion_SetElementwiseAffine(PrimitivePtr *primitive, bool elementwise_affine) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_LayerNormFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateLayerNormFusion(fbb, value->begin_norm_axis(), value->epsilon(),
                                                      elementwise_affine, value->begin_params_axis());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_LAYER_NORM_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_LayerNormFusion_GetBeginParamsAxis(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_LayerNormFusion();
    if (prim != nullptr && value != nullptr) {
      return value->begin_params_axis();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_LayerNormFusion_SetBeginParamsAxis(PrimitivePtr *primitive, int64_t begin_params_axis) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_LayerNormFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateLayerNormFusion(fbb, value->begin_norm_axis(), value->epsilon(),
                                                      value->elementwise_affine(), begin_params_axis);
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_LAYER_NORM_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** LessEqual **********
PrimitivePtr MindIR_LessEqual_CreatePrimitive() {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateLessEqual(fbb);
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_LESS_EQUAL), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}

// ********** MatMulFusion **********
PrimitivePtr MindIR_MatMulFusion_CreatePrimitive(bool transpose_a, bool transpose_b, ActivationType activation_type) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset =
    schema::CreateMatMulFusion(fbb, transpose_a, transpose_b, static_cast<schema::ActivationType>(activation_type));
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_MATMUL_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
bool MindIR_MatMulFusion_GetTransposeA(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_MatMulFusion();
    if (prim != nullptr && value != nullptr) {
      return value->transpose_a();
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void MindIR_MatMulFusion_SetTransposeA(PrimitivePtr *primitive, bool transpose_a) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_MatMulFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateMatMulFusion(fbb, transpose_a, value->transpose_b(),
                                                   static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_MATMUL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
bool MindIR_MatMulFusion_GetTransposeB(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_MatMulFusion();
    if (prim != nullptr && value != nullptr) {
      return value->transpose_b();
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void MindIR_MatMulFusion_SetTransposeB(PrimitivePtr *primitive, bool transpose_b) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_MatMulFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateMatMulFusion(fbb, value->transpose_a(), transpose_b,
                                                   static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_MATMUL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
ActivationType MindIR_MatMulFusion_GetActivationType(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_MatMulFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<ActivationType>(value->activation_type());
    } else {
      ActivationType en = static_cast<ActivationType>(0);
      return en;
    }
  } else {
    ActivationType en = static_cast<ActivationType>(0);
    return en;
  }
}

void MindIR_MatMulFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_MatMulFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateMatMulFusion(fbb, value->transpose_a(), value->transpose_b(),
                                                   static_cast<schema::ActivationType>(activation_type));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_MATMUL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** Maximum **********
PrimitivePtr MindIR_Maximum_CreatePrimitive() {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateMaximum(fbb);
  auto prim_offset = schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_MAXIMUM), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}

// ********** MaxPoolFusion **********
PrimitivePtr MindIR_MaxPoolFusion_CreatePrimitive(const std::vector<int64_t> &kernel_size,
                                                  const std::vector<int64_t> &strides, const std::vector<int64_t> &pad,
                                                  PadMode pad_mode, Format format, bool global,
                                                  ActivationType activation_type) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateMaxPoolFusion(
    fbb, fbb.CreateVector(kernel_size.data(), kernel_size.size()), fbb.CreateVector(strides.data(), strides.size()),
    fbb.CreateVector(pad.data(), pad.size()), static_cast<schema::PadMode>(pad_mode),
    mindspore::schema::RoundMode_FLOOR, static_cast<schema::Format>(format), global,
    static_cast<schema::ActivationType>(activation_type));
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_MAX_POOL_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
std::vector<int64_t> MindIR_MaxPoolFusion_GetKernelSize(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_MaxPoolFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->kernel_size();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_MaxPoolFusion_SetKernelSize(PrimitivePtr *primitive, const std::vector<int64_t> &kernel_size) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_MaxPoolFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateMaxPoolFusion(
        fbb, fbb.CreateVector(kernel_size.data(), kernel_size.size()),
        fbb.CreateVector(value->strides()->data(), value->strides()->size()),
        fbb.CreateVector(value->pad()->data(), value->pad()->size()), static_cast<schema::PadMode>(value->pad_mode()),
        mindspore::schema::RoundMode_FLOOR, static_cast<schema::Format>(value->format()), value->global(),
        static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_MAX_POOL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
std::vector<int64_t> MindIR_MaxPoolFusion_GetStrides(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_MaxPoolFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->strides();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_MaxPoolFusion_SetStrides(PrimitivePtr *primitive, const std::vector<int64_t> &strides) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_MaxPoolFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateMaxPoolFusion(
        fbb, fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(strides.data(), strides.size()), fbb.CreateVector(value->pad()->data(), value->pad()->size()),
        static_cast<schema::PadMode>(value->pad_mode()), mindspore::schema::RoundMode_FLOOR,
        static_cast<schema::Format>(value->format()), value->global(),
        static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_MAX_POOL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
std::vector<int64_t> MindIR_MaxPoolFusion_GetPad(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_MaxPoolFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->pad();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_MaxPoolFusion_SetPad(PrimitivePtr *primitive, const std::vector<int64_t> &pad) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_MaxPoolFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateMaxPoolFusion(
        fbb, fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->strides()->data(), value->strides()->size()), fbb.CreateVector(pad.data(), pad.size()),
        static_cast<schema::PadMode>(value->pad_mode()), mindspore::schema::RoundMode_FLOOR,
        static_cast<schema::Format>(value->format()), value->global(),
        static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_MAX_POOL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
PadMode MindIR_MaxPoolFusion_GetPadMode(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_MaxPoolFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<PadMode>(value->pad_mode());
    } else {
      PadMode en = static_cast<PadMode>(0);
      return en;
    }
  } else {
    PadMode en = static_cast<PadMode>(0);
    return en;
  }
}

void MindIR_MaxPoolFusion_SetPadMode(PrimitivePtr *primitive, PadMode pad_mode) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_MaxPoolFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateMaxPoolFusion(
        fbb, fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->strides()->data(), value->strides()->size()),
        fbb.CreateVector(value->pad()->data(), value->pad()->size()), static_cast<schema::PadMode>(pad_mode),
        mindspore::schema::RoundMode_FLOOR, static_cast<schema::Format>(value->format()), value->global(),
        static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_MAX_POOL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
Format MindIR_MaxPoolFusion_GetFormat(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_MaxPoolFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<Format>(value->format());
    } else {
      Format en = static_cast<Format>(0);
      return en;
    }
  } else {
    Format en = static_cast<Format>(0);
    return en;
  }
}

void MindIR_MaxPoolFusion_SetFormat(PrimitivePtr *primitive, Format format) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_MaxPoolFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateMaxPoolFusion(
        fbb, fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->strides()->data(), value->strides()->size()),
        fbb.CreateVector(value->pad()->data(), value->pad()->size()), static_cast<schema::PadMode>(value->pad_mode()),
        mindspore::schema::RoundMode_FLOOR, static_cast<schema::Format>(format), value->global(),
        static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_MAX_POOL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
bool MindIR_MaxPoolFusion_GetGlobal(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_MaxPoolFusion();
    if (prim != nullptr && value != nullptr) {
      return value->global();
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void MindIR_MaxPoolFusion_SetGlobal(PrimitivePtr *primitive, bool global) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_MaxPoolFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateMaxPoolFusion(
        fbb, fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->strides()->data(), value->strides()->size()),
        fbb.CreateVector(value->pad()->data(), value->pad()->size()), static_cast<schema::PadMode>(value->pad_mode()),
        mindspore::schema::RoundMode_FLOOR, static_cast<schema::Format>(value->format()), global,
        static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_MAX_POOL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
ActivationType MindIR_MaxPoolFusion_GetActivationType(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_MaxPoolFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<ActivationType>(value->activation_type());
    } else {
      ActivationType en = static_cast<ActivationType>(0);
      return en;
    }
  } else {
    ActivationType en = static_cast<ActivationType>(0);
    return en;
  }
}

void MindIR_MaxPoolFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_MaxPoolFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateMaxPoolFusion(
        fbb, fbb.CreateVector(value->kernel_size()->data(), value->kernel_size()->size()),
        fbb.CreateVector(value->strides()->data(), value->strides()->size()),
        fbb.CreateVector(value->pad()->data(), value->pad()->size()), static_cast<schema::PadMode>(value->pad_mode()),
        mindspore::schema::RoundMode_FLOOR, static_cast<schema::Format>(value->format()), value->global(),
        static_cast<schema::ActivationType>(activation_type));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_MAX_POOL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** MulFusion **********
PrimitivePtr MindIR_MulFusion_CreatePrimitive(ActivationType activation_type) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateMulFusion(fbb, static_cast<schema::ActivationType>(activation_type));
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_MUL_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
ActivationType MindIR_MulFusion_GetActivationType(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_MulFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<ActivationType>(value->activation_type());
    } else {
      ActivationType en = static_cast<ActivationType>(0);
      return en;
    }
  } else {
    ActivationType en = static_cast<ActivationType>(0);
    return en;
  }
}

void MindIR_MulFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_MulFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateMulFusion(fbb, static_cast<schema::ActivationType>(activation_type));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_MUL_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** OneHot **********
PrimitivePtr MindIR_OneHot_CreatePrimitive(int64_t axis) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateOneHot(fbb, axis);
  auto prim_offset = schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_ONE_HOT), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
int64_t MindIR_OneHot_GetAxis(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_OneHot();
    if (prim != nullptr && value != nullptr) {
      return value->axis();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_OneHot_SetAxis(PrimitivePtr *primitive, int64_t axis) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_OneHot();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateOneHot(fbb, axis);
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_ONE_HOT), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** PadFusion **********
PrimitivePtr MindIR_PadFusion_CreatePrimitive(const std::vector<std::vector<int64_t>> &paddings,
                                              PaddingMode padding_mode, float constant_value) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreatePadFusion(fbb, CreateVec2D(fbb, paddings),
                                            static_cast<schema::PaddingMode>(padding_mode), constant_value);
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_PAD_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
std::vector<std::vector<int64_t>> MindIR_PadFusion_GetPaddings(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_PadFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<std::vector<int64_t>> out;
      auto src = value->paddings();
      for (auto sub_list : *src->data()) {
        std::vector<int64_t> result_tmp;
        result_tmp.resize(sub_list->data()->size());
        std::transform(sub_list->data()->begin(), sub_list->data()->end(), result_tmp.begin(),
                       [](int64_t item) { return item; });
        out.emplace_back(result_tmp);
      }
      return out;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_PadFusion_SetPaddings(PrimitivePtr *primitive, const std::vector<std::vector<int64_t>> &paddings) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_PadFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset =
        schema::CreatePadFusion(fbb, CreateVec2D(fbb, paddings),
                                static_cast<schema::PaddingMode>(value->padding_mode()), value->constant_value());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_PAD_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
PaddingMode MindIR_PadFusion_GetPaddingMode(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_PadFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<PaddingMode>(value->padding_mode());
    } else {
      PaddingMode en = static_cast<PaddingMode>(0);
      return en;
    }
  } else {
    PaddingMode en = static_cast<PaddingMode>(0);
    return en;
  }
}

void MindIR_PadFusion_SetPaddingMode(PrimitivePtr *primitive, PaddingMode padding_mode) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_PadFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset =
        schema::CreatePadFusion(fbb, CreateVec2D(fbb, value->paddings()),
                                static_cast<schema::PaddingMode>(padding_mode), value->constant_value());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_PAD_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
float MindIR_PadFusion_GetConstantValue(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_PadFusion();
    if (prim != nullptr && value != nullptr) {
      return value->constant_value();
    } else {
      return .0;
    }
  } else {
    return .0;
  }
}

void MindIR_PadFusion_SetConstantValue(PrimitivePtr *primitive, float constant_value) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_PadFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset =
        schema::CreatePadFusion(fbb, CreateVec2D(fbb, value->paddings()),
                                static_cast<schema::PaddingMode>(value->padding_mode()), constant_value);
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_PAD_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** PowFusion **********
PrimitivePtr MindIR_PowFusion_CreatePrimitive(float scale, float shift) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreatePowFusion(fbb, scale, shift);
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_POW_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
float MindIR_PowFusion_GetScale(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_PowFusion();
    if (prim != nullptr && value != nullptr) {
      return value->scale();
    } else {
      return .0;
    }
  } else {
    return .0;
  }
}

void MindIR_PowFusion_SetScale(PrimitivePtr *primitive, float scale) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_PowFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreatePowFusion(fbb, scale, value->shift());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_POW_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
float MindIR_PowFusion_GetShift(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_PowFusion();
    if (prim != nullptr && value != nullptr) {
      return value->shift();
    } else {
      return .0;
    }
  } else {
    return .0;
  }
}

void MindIR_PowFusion_SetShift(PrimitivePtr *primitive, float shift) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_PowFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreatePowFusion(fbb, value->scale(), shift);
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_POW_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** PReLUFusion **********
PrimitivePtr MindIR_PReLUFusion_CreatePrimitive(bool channel_shared) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreatePReLUFusion(fbb, channel_shared);
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_PRELU_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
bool MindIR_PReLUFusion_GetChannelShared(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_PReLUFusion();
    if (prim != nullptr && value != nullptr) {
      return value->channel_shared();
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void MindIR_PReLUFusion_SetChannelShared(PrimitivePtr *primitive, bool channel_shared) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_PReLUFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreatePReLUFusion(fbb, channel_shared);
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_PRELU_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** QuantDTypeCast **********
PrimitivePtr MindIR_QuantDTypeCast_CreatePrimitive(int64_t src_t, int64_t dst_t) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateQuantDTypeCast(fbb, src_t, dst_t);
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_QUANT_DTYPE_CAST), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
int64_t MindIR_QuantDTypeCast_GetSrcT(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_QuantDTypeCast();
    if (prim != nullptr && value != nullptr) {
      return value->src_t();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_QuantDTypeCast_SetSrcT(PrimitivePtr *primitive, int64_t src_t) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_QuantDTypeCast();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateQuantDTypeCast(fbb, src_t, value->dst_t());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_QUANT_DTYPE_CAST), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_QuantDTypeCast_GetDstT(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_QuantDTypeCast();
    if (prim != nullptr && value != nullptr) {
      return value->dst_t();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_QuantDTypeCast_SetDstT(PrimitivePtr *primitive, int64_t dst_t) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_QuantDTypeCast();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateQuantDTypeCast(fbb, value->src_t(), dst_t);
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_QUANT_DTYPE_CAST), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** ReduceFusion **********
PrimitivePtr MindIR_ReduceFusion_CreatePrimitive(bool keep_dims, ReduceMode mode, bool reduce_to_end, float coeff) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset =
    schema::CreateReduceFusion(fbb, keep_dims, static_cast<schema::ReduceMode>(mode), reduce_to_end, coeff);
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_REDUCE_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
bool MindIR_ReduceFusion_GetKeepDims(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_ReduceFusion();
    if (prim != nullptr && value != nullptr) {
      return value->keep_dims();
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void MindIR_ReduceFusion_SetKeepDims(PrimitivePtr *primitive, bool keep_dims) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_ReduceFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateReduceFusion(fbb, keep_dims, static_cast<schema::ReduceMode>(value->mode()),
                                                   value->reduce_to_end(), value->coeff());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_REDUCE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
ReduceMode MindIR_ReduceFusion_GetMode(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_ReduceFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<ReduceMode>(value->mode());
    } else {
      ReduceMode en = static_cast<ReduceMode>(0);
      return en;
    }
  } else {
    ReduceMode en = static_cast<ReduceMode>(0);
    return en;
  }
}

void MindIR_ReduceFusion_SetMode(PrimitivePtr *primitive, ReduceMode mode) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_ReduceFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateReduceFusion(fbb, value->keep_dims(), static_cast<schema::ReduceMode>(mode),
                                                   value->reduce_to_end(), value->coeff());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_REDUCE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
bool MindIR_ReduceFusion_GetReduceToEnd(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_ReduceFusion();
    if (prim != nullptr && value != nullptr) {
      return value->reduce_to_end();
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void MindIR_ReduceFusion_SetReduceToEnd(PrimitivePtr *primitive, bool reduce_to_end) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_ReduceFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateReduceFusion(
        fbb, value->keep_dims(), static_cast<schema::ReduceMode>(value->mode()), reduce_to_end, value->coeff());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_REDUCE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
float MindIR_ReduceFusion_GetCoeff(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_ReduceFusion();
    if (prim != nullptr && value != nullptr) {
      return value->coeff();
    } else {
      return .0;
    }
  } else {
    return .0;
  }
}

void MindIR_ReduceFusion_SetCoeff(PrimitivePtr *primitive, float coeff) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_ReduceFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateReduceFusion(
        fbb, value->keep_dims(), static_cast<schema::ReduceMode>(value->mode()), value->reduce_to_end(), coeff);
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_REDUCE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** Reshape **********
PrimitivePtr MindIR_Reshape_CreatePrimitive() {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateReshape(fbb);
  auto prim_offset = schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_RESHAPE), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}

// ********** Resize **********
PrimitivePtr MindIR_Resize_CreatePrimitive(ResizeMethod method, int64_t new_height, int64_t new_width,
                                           bool preserve_aspect_ratio,
                                           CoordinateTransformMode coordinate_transform_mode, float cubic_coeff,
                                           int64_t exclude_outside, float extrapolation_value,
                                           NearestMode nearest_mode) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateResize(
    fbb, mindspore::schema::Format_NCHW, static_cast<schema::ResizeMethod>(method), new_height, new_width,
    preserve_aspect_ratio, static_cast<schema::CoordinateTransformMode>(coordinate_transform_mode), cubic_coeff,
    exclude_outside, extrapolation_value, static_cast<schema::NearestMode>(nearest_mode));
  auto prim_offset = schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_RESIZE), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
ResizeMethod MindIR_Resize_GetMethod(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      return static_cast<ResizeMethod>(value->method());
    } else {
      ResizeMethod en = static_cast<ResizeMethod>(0);
      return en;
    }
  } else {
    ResizeMethod en = static_cast<ResizeMethod>(0);
    return en;
  }
}

void MindIR_Resize_SetMethod(PrimitivePtr *primitive, ResizeMethod method) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset =
        schema::CreateResize(fbb, mindspore::schema::Format_NCHW, static_cast<schema::ResizeMethod>(method),
                             value->new_height(), value->new_width(), value->preserve_aspect_ratio(),
                             static_cast<schema::CoordinateTransformMode>(value->coordinate_transform_mode()),
                             value->cubic_coeff(), value->exclude_outside(), value->extrapolation_value(),
                             static_cast<schema::NearestMode>(value->nearest_mode()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_RESIZE), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_Resize_GetNewHeight(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      return value->new_height();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_Resize_SetNewHeight(PrimitivePtr *primitive, int64_t new_height) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset =
        schema::CreateResize(fbb, mindspore::schema::Format_NCHW, static_cast<schema::ResizeMethod>(value->method()),
                             new_height, value->new_width(), value->preserve_aspect_ratio(),
                             static_cast<schema::CoordinateTransformMode>(value->coordinate_transform_mode()),
                             value->cubic_coeff(), value->exclude_outside(), value->extrapolation_value(),
                             static_cast<schema::NearestMode>(value->nearest_mode()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_RESIZE), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_Resize_GetNewWidth(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      return value->new_width();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_Resize_SetNewWidth(PrimitivePtr *primitive, int64_t new_width) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset =
        schema::CreateResize(fbb, mindspore::schema::Format_NCHW, static_cast<schema::ResizeMethod>(value->method()),
                             value->new_height(), new_width, value->preserve_aspect_ratio(),
                             static_cast<schema::CoordinateTransformMode>(value->coordinate_transform_mode()),
                             value->cubic_coeff(), value->exclude_outside(), value->extrapolation_value(),
                             static_cast<schema::NearestMode>(value->nearest_mode()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_RESIZE), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
bool MindIR_Resize_GetPreserveAspectRatio(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      return value->preserve_aspect_ratio();
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void MindIR_Resize_SetPreserveAspectRatio(PrimitivePtr *primitive, bool preserve_aspect_ratio) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset =
        schema::CreateResize(fbb, mindspore::schema::Format_NCHW, static_cast<schema::ResizeMethod>(value->method()),
                             value->new_height(), value->new_width(), preserve_aspect_ratio,
                             static_cast<schema::CoordinateTransformMode>(value->coordinate_transform_mode()),
                             value->cubic_coeff(), value->exclude_outside(), value->extrapolation_value(),
                             static_cast<schema::NearestMode>(value->nearest_mode()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_RESIZE), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
CoordinateTransformMode MindIR_Resize_GetCoordinateTransformMode(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      return static_cast<CoordinateTransformMode>(value->coordinate_transform_mode());
    } else {
      CoordinateTransformMode en = static_cast<CoordinateTransformMode>(0);
      return en;
    }
  } else {
    CoordinateTransformMode en = static_cast<CoordinateTransformMode>(0);
    return en;
  }
}

void MindIR_Resize_SetCoordinateTransformMode(PrimitivePtr *primitive,
                                              CoordinateTransformMode coordinate_transform_mode) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset =
        schema::CreateResize(fbb, mindspore::schema::Format_NCHW, static_cast<schema::ResizeMethod>(value->method()),
                             value->new_height(), value->new_width(), value->preserve_aspect_ratio(),
                             static_cast<schema::CoordinateTransformMode>(coordinate_transform_mode),
                             value->cubic_coeff(), value->exclude_outside(), value->extrapolation_value(),
                             static_cast<schema::NearestMode>(value->nearest_mode()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_RESIZE), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
float MindIR_Resize_GetCubicCoeff(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      return value->cubic_coeff();
    } else {
      return .0;
    }
  } else {
    return .0;
  }
}

void MindIR_Resize_SetCubicCoeff(PrimitivePtr *primitive, float cubic_coeff) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset =
        schema::CreateResize(fbb, mindspore::schema::Format_NCHW, static_cast<schema::ResizeMethod>(value->method()),
                             value->new_height(), value->new_width(), value->preserve_aspect_ratio(),
                             static_cast<schema::CoordinateTransformMode>(value->coordinate_transform_mode()),
                             cubic_coeff, value->exclude_outside(), value->extrapolation_value(),
                             static_cast<schema::NearestMode>(value->nearest_mode()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_RESIZE), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_Resize_GetExcludeOutside(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      return value->exclude_outside();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_Resize_SetExcludeOutside(PrimitivePtr *primitive, int64_t exclude_outside) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateResize(
        fbb, mindspore::schema::Format_NCHW, static_cast<schema::ResizeMethod>(value->method()), value->new_height(),
        value->new_width(), value->preserve_aspect_ratio(),
        static_cast<schema::CoordinateTransformMode>(value->coordinate_transform_mode()), value->cubic_coeff(),
        exclude_outside, value->extrapolation_value(), static_cast<schema::NearestMode>(value->nearest_mode()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_RESIZE), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
float MindIR_Resize_GetExtrapolationValue(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      return value->extrapolation_value();
    } else {
      return .0;
    }
  } else {
    return .0;
  }
}

void MindIR_Resize_SetExtrapolationValue(PrimitivePtr *primitive, float extrapolation_value) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateResize(
        fbb, mindspore::schema::Format_NCHW, static_cast<schema::ResizeMethod>(value->method()), value->new_height(),
        value->new_width(), value->preserve_aspect_ratio(),
        static_cast<schema::CoordinateTransformMode>(value->coordinate_transform_mode()), value->cubic_coeff(),
        value->exclude_outside(), extrapolation_value, static_cast<schema::NearestMode>(value->nearest_mode()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_RESIZE), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
NearestMode MindIR_Resize_GetNearestMode(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      return static_cast<NearestMode>(value->nearest_mode());
    } else {
      NearestMode en = static_cast<NearestMode>(0);
      return en;
    }
  } else {
    NearestMode en = static_cast<NearestMode>(0);
    return en;
  }
}

void MindIR_Resize_SetNearestMode(PrimitivePtr *primitive, NearestMode nearest_mode) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Resize();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateResize(
        fbb, mindspore::schema::Format_NCHW, static_cast<schema::ResizeMethod>(value->method()), value->new_height(),
        value->new_width(), value->preserve_aspect_ratio(),
        static_cast<schema::CoordinateTransformMode>(value->coordinate_transform_mode()), value->cubic_coeff(),
        value->exclude_outside(), value->extrapolation_value(), static_cast<schema::NearestMode>(nearest_mode));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_RESIZE), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** Rsqrt **********
PrimitivePtr MindIR_Rsqrt_CreatePrimitive() {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateRsqrt(fbb);
  auto prim_offset = schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_RSQRT), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}

// ********** ScaleFusion **********
PrimitivePtr MindIR_ScaleFusion_CreatePrimitive(int64_t axis, ActivationType activation_type) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateScaleFusion(fbb, axis, static_cast<schema::ActivationType>(activation_type));
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SCALE_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
int64_t MindIR_ScaleFusion_GetAxis(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_ScaleFusion();
    if (prim != nullptr && value != nullptr) {
      return value->axis();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_ScaleFusion_SetAxis(PrimitivePtr *primitive, int64_t axis) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_ScaleFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset =
        schema::CreateScaleFusion(fbb, axis, static_cast<schema::ActivationType>(value->activation_type()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SCALE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
ActivationType MindIR_ScaleFusion_GetActivationType(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_ScaleFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<ActivationType>(value->activation_type());
    } else {
      ActivationType en = static_cast<ActivationType>(0);
      return en;
    }
  } else {
    ActivationType en = static_cast<ActivationType>(0);
    return en;
  }
}

void MindIR_ScaleFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_ScaleFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset =
        schema::CreateScaleFusion(fbb, value->axis(), static_cast<schema::ActivationType>(activation_type));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SCALE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** Shape **********
PrimitivePtr MindIR_Shape_CreatePrimitive() {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateShape(fbb);
  auto prim_offset = schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SHAPE), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}

// ********** SliceFusion **********
PrimitivePtr MindIR_SliceFusion_CreatePrimitive(const std::vector<int64_t> &axes) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateSliceFusion(fbb, fbb.CreateVector(axes.data(), axes.size()));
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SLICE_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
std::vector<int64_t> MindIR_SliceFusion_GetAxes(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_SliceFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->axes();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_SliceFusion_SetAxes(PrimitivePtr *primitive, const std::vector<int64_t> &axes) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_SliceFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateSliceFusion(fbb, fbb.CreateVector(axes.data(), axes.size()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SLICE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** Softmax **********
PrimitivePtr MindIR_Softmax_CreatePrimitive(const std::vector<int64_t> &axis) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateSoftmax(fbb, fbb.CreateVector(axis.data(), axis.size()));
  auto prim_offset = schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SOFTMAX), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
std::vector<int64_t> MindIR_Softmax_GetAxis(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Softmax();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->axis();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_Softmax_SetAxis(PrimitivePtr *primitive, const std::vector<int64_t> &axis) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Softmax();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateSoftmax(fbb, fbb.CreateVector(axis.data(), axis.size()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SOFTMAX), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** SpaceToBatchND **********
PrimitivePtr MindIR_SpaceToBatchND_CreatePrimitive(const std::vector<int64_t> &block_shape,
                                                   const std::vector<std::vector<int64_t>> &paddings) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateSpaceToBatchND(fbb, fbb.CreateVector(block_shape.data(), block_shape.size()),
                                                 CreateVec2D(fbb, paddings));
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SPACE_TO_BATCH_ND), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
std::vector<int64_t> MindIR_SpaceToBatchND_GetBlockShape(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_SpaceToBatchND();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->block_shape();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_SpaceToBatchND_SetBlockShape(PrimitivePtr *primitive, const std::vector<int64_t> &block_shape) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_SpaceToBatchND();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateSpaceToBatchND(fbb, fbb.CreateVector(block_shape.data(), block_shape.size()),
                                                     CreateVec2D(fbb, value->paddings()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SPACE_TO_BATCH_ND), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
std::vector<std::vector<int64_t>> MindIR_SpaceToBatchND_GetPaddings(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_SpaceToBatchND();
    if (prim != nullptr && value != nullptr) {
      std::vector<std::vector<int64_t>> out;
      auto src = value->paddings();
      for (auto sub_list : *src->data()) {
        std::vector<int64_t> result_tmp;
        result_tmp.resize(sub_list->data()->size());
        std::transform(sub_list->data()->begin(), sub_list->data()->end(), result_tmp.begin(),
                       [](int64_t item) { return item; });
        out.emplace_back(result_tmp);
      }
      return out;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_SpaceToBatchND_SetPaddings(PrimitivePtr *primitive, const std::vector<std::vector<int64_t>> &paddings) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_SpaceToBatchND();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateSpaceToBatchND(
        fbb, fbb.CreateVector(value->block_shape()->data(), value->block_shape()->size()), CreateVec2D(fbb, paddings));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SPACE_TO_BATCH_ND), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** Split **********
PrimitivePtr MindIR_Split_CreatePrimitive(int64_t output_num, const std::vector<int64_t> &size_splits, int64_t axis) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset =
    schema::CreateSplit(fbb, output_num, fbb.CreateVector(size_splits.data(), size_splits.size()), axis);
  auto prim_offset = schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SPLIT), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
int64_t MindIR_Split_GetOutputNum(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Split();
    if (prim != nullptr && value != nullptr) {
      return value->output_num();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_Split_SetOutputNum(PrimitivePtr *primitive, int64_t output_num) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Split();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateSplit(
        fbb, output_num, fbb.CreateVector(value->size_splits()->data(), value->size_splits()->size()), value->axis());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SPLIT), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
std::vector<int64_t> MindIR_Split_GetSizeSplits(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Split();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->size_splits();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_Split_SetSizeSplits(PrimitivePtr *primitive, const std::vector<int64_t> &size_splits) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Split();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateSplit(fbb, value->output_num(),
                                            fbb.CreateVector(size_splits.data(), size_splits.size()), value->axis());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SPLIT), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_Split_GetAxis(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Split();
    if (prim != nullptr && value != nullptr) {
      return value->axis();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_Split_SetAxis(PrimitivePtr *primitive, int64_t axis) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Split();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateSplit(
        fbb, value->output_num(), fbb.CreateVector(value->size_splits()->data(), value->size_splits()->size()), axis);
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SPLIT), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** Sqrt **********
PrimitivePtr MindIR_Sqrt_CreatePrimitive() {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateSqrt(fbb);
  auto prim_offset = schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SQRT), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}

// ********** SquaredDifference **********
PrimitivePtr MindIR_SquaredDifference_CreatePrimitive() {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateSquaredDifference(fbb);
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SQUARED_DIFFERENCE), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}

// ********** Squeeze **********
PrimitivePtr MindIR_Squeeze_CreatePrimitive(const std::vector<int64_t> &axis) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateSqueeze(fbb, fbb.CreateVector(axis.data(), axis.size()));
  auto prim_offset = schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SQUEEZE), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
std::vector<int64_t> MindIR_Squeeze_GetAxis(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Squeeze();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->axis();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_Squeeze_SetAxis(PrimitivePtr *primitive, const std::vector<int64_t> &axis) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Squeeze();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateSqueeze(fbb, fbb.CreateVector(axis.data(), axis.size()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SQUEEZE), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** Stack **********
PrimitivePtr MindIR_Stack_CreatePrimitive(int64_t axis) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateStack(fbb, axis);
  auto prim_offset = schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_STACK), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
int64_t MindIR_Stack_GetAxis(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Stack();
    if (prim != nullptr && value != nullptr) {
      return value->axis();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_Stack_SetAxis(PrimitivePtr *primitive, int64_t axis) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Stack();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateStack(fbb, axis);
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_STACK), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** StridedSlice **********
PrimitivePtr MindIR_StridedSlice_CreatePrimitive(int64_t begin_mask, int64_t end_mask, int64_t ellipsis_mask,
                                                 int64_t new_axis_mask, int64_t shrink_axis_mask) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset =
    schema::CreateStridedSlice(fbb, begin_mask, end_mask, ellipsis_mask, new_axis_mask, shrink_axis_mask);
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_STRIDED_SLICE), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
int64_t MindIR_StridedSlice_GetBeginMask(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_StridedSlice();
    if (prim != nullptr && value != nullptr) {
      return value->begin_mask();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_StridedSlice_SetBeginMask(PrimitivePtr *primitive, int64_t begin_mask) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_StridedSlice();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateStridedSlice(fbb, begin_mask, value->end_mask(), value->ellipsis_mask(),
                                                   value->new_axis_mask(), value->shrink_axis_mask());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_STRIDED_SLICE), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_StridedSlice_GetEndMask(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_StridedSlice();
    if (prim != nullptr && value != nullptr) {
      return value->end_mask();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_StridedSlice_SetEndMask(PrimitivePtr *primitive, int64_t end_mask) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_StridedSlice();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateStridedSlice(fbb, value->begin_mask(), end_mask, value->ellipsis_mask(),
                                                   value->new_axis_mask(), value->shrink_axis_mask());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_STRIDED_SLICE), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_StridedSlice_GetEllipsisMask(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_StridedSlice();
    if (prim != nullptr && value != nullptr) {
      return value->ellipsis_mask();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_StridedSlice_SetEllipsisMask(PrimitivePtr *primitive, int64_t ellipsis_mask) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_StridedSlice();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateStridedSlice(fbb, value->begin_mask(), value->end_mask(), ellipsis_mask,
                                                   value->new_axis_mask(), value->shrink_axis_mask());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_STRIDED_SLICE), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_StridedSlice_GetNewAxisMask(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_StridedSlice();
    if (prim != nullptr && value != nullptr) {
      return value->new_axis_mask();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_StridedSlice_SetNewAxisMask(PrimitivePtr *primitive, int64_t new_axis_mask) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_StridedSlice();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateStridedSlice(fbb, value->begin_mask(), value->end_mask(), value->ellipsis_mask(),
                                                   new_axis_mask, value->shrink_axis_mask());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_STRIDED_SLICE), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_StridedSlice_GetShrinkAxisMask(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_StridedSlice();
    if (prim != nullptr && value != nullptr) {
      return value->shrink_axis_mask();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_StridedSlice_SetShrinkAxisMask(PrimitivePtr *primitive, int64_t shrink_axis_mask) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_StridedSlice();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateStridedSlice(fbb, value->begin_mask(), value->end_mask(), value->ellipsis_mask(),
                                                   value->new_axis_mask(), shrink_axis_mask);
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_STRIDED_SLICE), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** SubFusion **********
PrimitivePtr MindIR_SubFusion_CreatePrimitive(ActivationType activation_type) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateSubFusion(fbb, static_cast<schema::ActivationType>(activation_type));
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SUB_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
ActivationType MindIR_SubFusion_GetActivationType(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_SubFusion();
    if (prim != nullptr && value != nullptr) {
      return static_cast<ActivationType>(value->activation_type());
    } else {
      ActivationType en = static_cast<ActivationType>(0);
      return en;
    }
  } else {
    ActivationType en = static_cast<ActivationType>(0);
    return en;
  }
}

void MindIR_SubFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_SubFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateSubFusion(fbb, static_cast<schema::ActivationType>(activation_type));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_SUB_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** TileFusion **********
PrimitivePtr MindIR_TileFusion_CreatePrimitive(const std::vector<int64_t> &dims) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateTileFusion(fbb, fbb.CreateVector(dims.data(), dims.size()));
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_TILE_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
std::vector<int64_t> MindIR_TileFusion_GetDims(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_TileFusion();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->dims();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_TileFusion_SetDims(PrimitivePtr *primitive, const std::vector<int64_t> &dims) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_TileFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateTileFusion(fbb, fbb.CreateVector(dims.data(), dims.size()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_TILE_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** TopKFusion **********
PrimitivePtr MindIR_TopKFusion_CreatePrimitive(bool sorted, int64_t axis) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateTopKFusion(fbb, sorted, axis);
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_TOPK_FUSION), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
bool MindIR_TopKFusion_GetSorted(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_TopKFusion();
    if (prim != nullptr && value != nullptr) {
      return value->sorted();
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void MindIR_TopKFusion_SetSorted(PrimitivePtr *primitive, bool sorted) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_TopKFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateTopKFusion(fbb, sorted, value->axis());
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_TOPK_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}
int64_t MindIR_TopKFusion_GetAxis(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_TopKFusion();
    if (prim != nullptr && value != nullptr) {
      return value->axis();
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void MindIR_TopKFusion_SetAxis(PrimitivePtr *primitive, int64_t axis) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_TopKFusion();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateTopKFusion(fbb, value->sorted(), axis);
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_TOPK_FUSION), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

// ********** Transpose **********
PrimitivePtr MindIR_Transpose_CreatePrimitive() {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateTranspose(fbb);
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_TRANSPOSE), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}

// ********** Unsqueeze **********
PrimitivePtr MindIR_Unsqueeze_CreatePrimitive(const std::vector<int64_t> &axis) {
  flatbuffers::FlatBufferBuilder fbb;
  auto ops_offset = schema::CreateUnsqueeze(fbb, fbb.CreateVector(axis.data(), axis.size()));
  auto prim_offset =
    schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_UNSQUEEZE), ops_offset.o);
  fbb.Finish(prim_offset);
  auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, nullptr);
  auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
  return ret_value;
}
std::vector<int64_t> MindIR_Unsqueeze_GetAxis(ConstPrimitivePtr primitive) {
  if (primitive != nullptr) {
    auto prim = static_cast<const schema::Primitive *>(primitive);
    auto value = prim->value_as_Unsqueeze();
    if (prim != nullptr && value != nullptr) {
      std::vector<int64_t> result;
      auto src = value->axis();
      result.resize(src->size());
      std::transform(src->begin(), src->end(), result.begin(), [](int64_t item) { return item; });
      return result;
    } else {
      return {};
    }
  } else {
    return {};
  }
}

void MindIR_Unsqueeze_SetAxis(PrimitivePtr *primitive, const std::vector<int64_t> &axis) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto prim = static_cast<schema::Primitive *>(*primitive);
    auto value = prim->value_as_Unsqueeze();
    if (prim != nullptr && value != nullptr) {
      flatbuffers::FlatBufferBuilder fbb;
      auto ops_offset = schema::CreateUnsqueeze(fbb, fbb.CreateVector(axis.data(), axis.size()));
      auto prim_offset =
        schema::CreatePrimitive(fbb, static_cast<schema::PrimitiveType>(NODE_TYPE_UNSQUEEZE), ops_offset.o);
      fbb.Finish(prim_offset);
      auto new_addr = MindIRMemoryManager::GetInstance()->CreatePrimitiveFromBuilder(fbb, prim);
      auto ret_value = flatbuffers::GetMutableRoot<schema::Primitive>(new_addr);
      *primitive = ret_value;
    }
  }
}

}  // namespace lite
}  // namespace mindspore
