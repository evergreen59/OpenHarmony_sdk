#ifndef LITE_NNRT_NNRT_LITE_GRAPH_H
#define LITE_NNRT_NNRT_LITE_GRAPH_H
#include <memory>
#include <string>
#include <vector>
namespace mindspore {
namespace lite {

typedef void *PrimitivePtr;
typedef const void *ConstPrimitivePtr;

typedef void *TensorPtr;
typedef const void *ConstTensorPtr;

struct LiteGraph {
  struct Node {
    std::string name_;
    std::string op_type_;  // hnn no use
    int node_type_;        // hnn no use
    PrimitivePtr primitive_ = nullptr;
    std::shared_ptr<void> base_operator_ = nullptr;  // hnn no use
    std::vector<uint32_t> input_indices_;
    std::vector<uint32_t> output_indices_;
    int quant_type_;
    int device_type_ = -1;  // hnn no use
  };

  struct SubGraph {
    std::string name_;
    std::vector<uint32_t> input_indices_;
    std::vector<uint32_t> output_indices_;
    std::vector<uint32_t> node_indices_;
    std::vector<uint32_t> tensor_indices_;  // hnn no use
  };

  std::string name_;
  std::string version_;  // hnn no use
  std::vector<uint32_t> input_indices_;
  std::vector<uint32_t> output_indices_;
  std::vector<TensorPtr> all_tensors_;
  std::vector<Node *> all_nodes_;
  std::vector<SubGraph *> sub_graphs_;
#ifdef ENABLE_MODEL_OBF
  std::vector<uint32_t> all_prims_type_;      // hnn no use
  std::vector<uint32_t> all_nodes_stat_;      // hnn no use
  bool model_obfuscated_ = false;             // hnn no use
  std::vector<unsigned char *> deobf_prims_;  // hnn no use
#endif
};

void MindIR_LiteGraph_Destroy(LiteGraph **lite_graph);
size_t MindIR_LiteGraph_GetConstTensorSize(const LiteGraph *lite_graph);

}  // namespace lite
}  // namespace mindspore

#endif  // LITE_NNRT_NNRT_LITE_GRAPH_H
