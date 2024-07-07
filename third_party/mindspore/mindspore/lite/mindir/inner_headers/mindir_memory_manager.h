#ifndef LITE_MINDIR_MEMORY_MANAGER_H
#define LITE_MINDIR_MEMORY_MANAGER_H
#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include "include/errorcode.h"
#include "schema/model_generated.h"
#include "mindir_lite_graph.h"
// using namespace OHOS::HDI::Nnrt::V1_0;

namespace mindspore {
namespace lite {
class MindIRMemoryManager {
 public:
  static MindIRMemoryManager *GetInstance();
  ~MindIRMemoryManager() = default;
  void *CreateTensorFromBuilder(flatbuffers::FlatBufferBuilder &fbb_new, schema::Tensor *tensor);
  void DeleteTensor(schema::Tensor *tensor);
  void *CreatePrimitiveFromBuilder(flatbuffers::FlatBufferBuilder &fbb_new, schema::Primitive *primitive);
  void DeletePrimitive(schema::Primitive *primitive);
  void ClearAllMemory();

 private:
  MindIRMemoryManager() = default;
  static void *CopyFbbToNewMemory(flatbuffers::FlatBufferBuilder &fbb_new);
  std::map<schema::Primitive *, PrimitivePtr> primitive_map;
  std::map<schema::Tensor *, TensorPtr> tensor_map;
  std::mutex mutex;
};
}  // namespace lite
}  // namespace mindspore
#endif  // LITE_MINDIR_MEMORY_MANAGER_H
