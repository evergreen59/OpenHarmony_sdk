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
#include "mindir_lite_graph.h"
#include "mindir_tensor.h"
#include "mindir_primitive.h"
#include "src/common/log.h"
#include "schema/model_generated.h"
#include "mindir_memory_manager.h"
#include <string>
namespace mindspore {
namespace lite {
void MindIR_LiteGraph_Destroy(LiteGraph **lite_graph) {
  if (lite_graph != nullptr && *lite_graph != nullptr) {
    MS_LOG(INFO) << "start to destroy LiteGraph.";
    auto graph = *lite_graph;
    graph->name_.clear();
    graph->input_indices_.clear();
    graph->output_indices_.clear();
    MS_LOG(INFO) << "Destroying  nodes.";
    // node
    for (size_t idx = 0; idx < graph->all_nodes_.size(); idx++) {
      if (graph->all_nodes_[idx] != nullptr) {
        MindIRMemoryManager::GetInstance()->DeletePrimitive(
          static_cast<schema::Primitive *>(graph->all_nodes_[idx]->primitive_));
        delete graph->all_nodes_[idx];
      }
    }
    MS_LOG(INFO) << "Destroying  subgraphs.";
    // subgraph
    for (size_t idx = 0; idx < graph->sub_graphs_.size(); idx++) {
      if (graph->sub_graphs_[idx] != nullptr) {
        delete graph->sub_graphs_[idx];
      }
    }
    MS_LOG(INFO) << "Destroying  tensors.";
    // tensor
    for (size_t idx = 0; idx < graph->all_tensors_.size(); idx++) {
      if (graph->all_tensors_[idx] != nullptr) {
        MindIRMemoryManager::GetInstance()->DeleteTensor(static_cast<schema::Tensor *>(graph->all_tensors_[idx]));
      }
    }
    // graph
    delete graph;
    *lite_graph = nullptr;
  } else {
    MS_LOG(ERROR) << "nnrt_lite_graph is nullptr, can not delete.";
  }
}

size_t MindIR_LiteGraph_GetConstTensorSize(const LiteGraph *lite_graph) {
  if (lite_graph != nullptr) {
    size_t size = 0;
    for (auto tensor : lite_graph->all_tensors_) {
      if (tensor != nullptr) {
        auto value = static_cast<schema::Tensor *>(tensor);
        if (value != nullptr) {
          auto src = value->data();
          if (src == nullptr) {
            continue;
          }
          size += src->size();
        }
      }
    }
    MS_LOG(DEBUG) << "lite_graph has " << lite_graph->all_tensors_.size() << "tensors ,const tensor size = " << size;
    return size;
  } else {
    MS_LOG(ERROR) << "lite_graph is nullptr";
    return 0;
  }
}

}  // namespace lite
}  // namespace mindspore