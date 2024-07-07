#ifndef MIDIR_LITE_UTILS_H
#define MIDIR_LITE_UTILS_H
#include "mindir_types.h"
#include "mindir_lite_graph.h"
#include "schema/model_generated.h"
namespace mindspore {
namespace lite {

// ********** PrimitiveBase **********
PrimitivePtr MindIR_CreatePrimitiveFromBuilder(flatbuffers::FlatBufferBuilder &fbb);

flatbuffers::Offset<schema::Vec2D> CreateVec2D(flatbuffers::FlatBufferBuilder &fbb,
                                               const std::vector<std::vector<int64_t>> &data);
flatbuffers::Offset<schema::Vec2D> CreateVec2D(flatbuffers::FlatBufferBuilder &fbb,
                                               const mindspore::schema::Vec2D *data);

mindspore::schema::PrimitiveType MindIR_GetPrimitiveType(PrimitivePtr prim);

flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<schema::QuantParam>>> ConvertQuantParams(
  flatbuffers::FlatBufferBuilder &fbb, const std::vector<QuantParam> &quant_params);

flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<schema::QuantParam>>> ConvertQuantParams(
  flatbuffers::FlatBufferBuilder &fbb,
  const flatbuffers::Vector<flatbuffers::Offset<mindspore::schema::QuantParam>> *quant_params);

}  // namespace lite
}  // namespace mindspore
#endif  // MIDIR_LITE_UTILS_H
