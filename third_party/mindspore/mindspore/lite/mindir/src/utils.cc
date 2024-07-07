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
#include "utils.h"
#include "src/common/log.h"
#include "mindir_memory_manager.h"
namespace mindspore {
namespace lite {

// ********** PrimitiveBase **********
NodeType MindIR_Primitive_GetType(PrimitivePtr primitive) {
  auto prim = flatbuffers::GetMutableRoot<schema::Primitive>(primitive);
  auto type = prim->value_type();
  return static_cast<NodeType>(type);
}

void MindIR_Primitive_Destroy(PrimitivePtr *primitive) {
  if (primitive != nullptr && *primitive != nullptr) {
    auto schema = static_cast<schema::Primitive *>(*primitive);
    MS_LOG(ERROR) << "schema=" << schema->value_type();
    MindIRMemoryManager::GetInstance()->DeletePrimitive(schema);
    *primitive = nullptr;
  }
}
PrimitivePtr MindIR_CreatePrimitiveFromBuilder(flatbuffers::FlatBufferBuilder &fbb) {
  auto buff = reinterpret_cast<uint8_t *>(malloc(fbb.GetSize()));
  if (buff == nullptr) {
    MS_LOG(ERROR) << "malloc memory for primitive failed!";
    fbb.Clear();
    return nullptr;
  }
  memcpy(buff, fbb.GetBufferPointer(), fbb.GetSize());
  fbb.Clear();
  return buff;
}
flatbuffers::Offset<schema::Vec2D> CreateVec2D(flatbuffers::FlatBufferBuilder &fbb,
                                               const std::vector<std::vector<int64_t>> &data) {
  std::vector<flatbuffers::Offset<schema::Vec>> vet2d;
  vet2d.reserve(data.size());
  for (const auto &data_one : data) {
    vet2d.emplace_back(schema::CreateVec(fbb, fbb.CreateVector<int64_t>(data_one)));
  }
  flatbuffers::Offset<schema::Vec2D> v2d = schema::CreateVec2D(fbb, fbb.CreateVector(vet2d));
  return v2d;
}
flatbuffers::Offset<schema::Vec2D> CreateVec2D(flatbuffers::FlatBufferBuilder &fbb,
                                               const mindspore::schema::Vec2D *data) {
  auto data_inner = data->data();
  std::vector<flatbuffers::Offset<schema::Vec>> vet2d;
  vet2d.reserve(data_inner->size());
  for (const auto data_one : *data_inner) {
    vet2d.emplace_back(schema::CreateVec(fbb, fbb.CreateVector(data_one->data()->data(), data_one->data()->size())));
  }
  flatbuffers::Offset<schema::Vec2D> v2d = schema::CreateVec2D(fbb, fbb.CreateVector(vet2d));
  return v2d;
}

flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<schema::QuantParam>>> ConvertQuantParams(
  flatbuffers::FlatBufferBuilder &fbb, const std::vector<QuantParam> &quant_params) {
  std::vector<flatbuffers::Offset<mindspore::schema::QuantParam>> tmp_vec;
  tmp_vec.reserve(quant_params.size());
  for (auto q_param : quant_params) {
    tmp_vec.emplace_back(schema::CreateQuantParam(fbb, q_param.scale, q_param.zeroPoint, 0, 0, true, q_param.numBits));
  }
  flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<schema::QuantParam>>> ret_quant_param =
    fbb.CreateVector(tmp_vec.data(), tmp_vec.size());
  return ret_quant_param;
}

flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<schema::QuantParam>>> ConvertQuantParams(
  flatbuffers::FlatBufferBuilder &fbb,
  const flatbuffers::Vector<flatbuffers::Offset<mindspore::schema::QuantParam>> *quant_params) {
  std::vector<flatbuffers::Offset<mindspore::schema::QuantParam>> tmp_vec;
  tmp_vec.reserve(quant_params->size());
  for (auto q_param : *quant_params) {
    tmp_vec.emplace_back(
      schema::CreateQuantParam(fbb, q_param->scale(), q_param->zeroPoint(), 0, 0, true, q_param->numBits()));
  }
  flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<schema::QuantParam>>> ret_quant_param =
    fbb.CreateVector(tmp_vec.data(), tmp_vec.size());
  return ret_quant_param;
}
}  // namespace lite
}  // namespace mindspore