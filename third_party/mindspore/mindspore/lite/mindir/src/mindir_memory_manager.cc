#include "mindir_memory_manager.h"
#include "src/common/log.h"
#include "utils.h"
#include <iostream>
namespace mindspore {
namespace lite {
namespace {
template <typename KEY_TYPE, typename VALUE_TYPE>
void ClearMap(std::map<KEY_TYPE, VALUE_TYPE> &map) {
  for (auto iter = map.begin(); iter != map.end();) {
    if (iter->second != nullptr) {
      free(iter->second);
      map.erase(iter++);
    } else {
      iter++;
    }
  }
}
}  // namespace
MindIRMemoryManager *MindIRMemoryManager::GetInstance() {
  static MindIRMemoryManager instance;
  return &instance;
}

void *MindIRMemoryManager::CopyFbbToNewMemory(flatbuffers::FlatBufferBuilder &fbb) {
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
void *MindIRMemoryManager::CreateTensorFromBuilder(flatbuffers::FlatBufferBuilder &fbb_new, schema::Tensor *tensor) {
  std::lock_guard<std::mutex> lck(mutex);
  if (tensor != nullptr) {
    // find primitive exist
    if (tensor_map.find(tensor) != tensor_map.end()) {
      // if find, then delete
      void *flatbuffer_ptr = tensor_map[tensor];
      if (flatbuffer_ptr != nullptr) {
        free(flatbuffer_ptr);
        tensor_map[tensor] = nullptr;
        tensor_map.erase(tensor_map.find(tensor));
      }
    }
  }
  // then copy fbb
  auto new_memory_ptr = CopyFbbToNewMemory(fbb_new);
  auto tensor_root = flatbuffers::GetMutableRoot<schema::Tensor>(new_memory_ptr);
  tensor_map[tensor_root] = new_memory_ptr;
  return new_memory_ptr;
}

void *MindIRMemoryManager::CreatePrimitiveFromBuilder(flatbuffers::FlatBufferBuilder &fbb_new,
                                                      schema::Primitive *primitive) {
  std::lock_guard<std::mutex> lck(mutex);
  if (primitive != nullptr) {
    // find primitive exist
    if (primitive_map.find(primitive) != primitive_map.end()) {
      // if find, then delete
      void *flatbuffer_ptr = primitive_map[primitive];
      if (flatbuffer_ptr != nullptr) {
        free(flatbuffer_ptr);
        primitive_map[primitive] = nullptr;
        primitive_map.erase(primitive_map.find(primitive));
      }
    }
  }
  // then copy fbb
  auto new_memory_ptr = CopyFbbToNewMemory(fbb_new);
  auto primitive_root = flatbuffers::GetMutableRoot<schema::Primitive>(new_memory_ptr);
  primitive_map[primitive_root] = new_memory_ptr;
  return new_memory_ptr;
}

void MindIRMemoryManager::DeletePrimitive(schema::Primitive *primitive) {
  std::lock_guard<std::mutex> lck(mutex);
  if (primitive == nullptr) {
    MS_LOG(ERROR) << "primitive is nullptr, no need to delete.";
    return;
  }
  if (primitive_map.find(primitive) != primitive_map.end()) {
    // if find, then delete
    void *flatbuffer_ptr = primitive_map[primitive];
    if (flatbuffer_ptr != nullptr) {
      free(flatbuffer_ptr);
      primitive_map[primitive] = nullptr;
      primitive_map.erase(primitive_map.find(primitive));
    }
  }
}

void MindIRMemoryManager::DeleteTensor(schema::Tensor *tensor) {
  std::lock_guard<std::mutex> lck(mutex);
  if (tensor == nullptr) {
    MS_LOG(ERROR) << "tensor is nullptr, no need to delete.";
    return;
  }
  if (tensor != nullptr) {
    // find primitive exist
    if (tensor_map.find(tensor) != tensor_map.end()) {
      // if find, then delete
      void *flatbuffer_ptr = tensor_map[tensor];
      if (flatbuffer_ptr != nullptr) {
        free(flatbuffer_ptr);
        tensor_map[tensor] = nullptr;
        tensor_map.erase(tensor_map.find(tensor));
      }
    }
  }
}

void MindIRMemoryManager::ClearAllMemory() {
  std::lock_guard<std::mutex> lck(mutex);
  ClearMap(primitive_map);
  ClearMap(tensor_map);
}
}  // namespace lite
}  // namespace mindspore