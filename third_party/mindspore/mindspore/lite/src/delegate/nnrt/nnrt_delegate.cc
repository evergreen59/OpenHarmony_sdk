/**
 * Copyright 2022 Huawei Technologies Co., Ltd
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
#include "nnrt_delegate.h"
#include "checker/primitive_check.h"
#include "src/common/log_adapter.h"
#include "interfaces/kits/c/neural_network_runtime.h"
#include "interfaces/innerkits/c/neural_network_runtime_inner.h"
#include "nnrt_model_kernel.h"

mindspore::Status mindspore::NNRTDelegate::Build(DelegateModel<schema::Primitive> *model) {
  if (this->nnrt_lite_graph == nullptr) {
    MS_LOG(ERROR) << "nnrt_lite_graph is nullptr.";
    return mindspore::kLiteError;
  }
  if (this->nnrt_lite_graph->sub_graphs_.empty()) {
    // must have at lease one subgraph
    MS_LOG(ERROR) << "must have at lease one subgraph";
    return mindspore::kLiteError;
  }
  OH_NN_ReturnCode ret_code;
  OH_NNModel *oh_nnmodel = OH_NNModel_Construct();
  if (oh_nnmodel == nullptr) {
    MS_LOG(ERROR) << "Construct NNModel failed, oh_nnmodel is nullptr.";
    return mindspore::kLiteError;
  }

  ret_code = OH_NNModel_BuildFromLiteGraph(oh_nnmodel, this->nnrt_lite_graph);
  if (ret_code != OH_NN_SUCCESS) {
    MS_LOG(ERROR) << "Build NNModel failed, OH_NN_ReturnCode = " << ret_code;
    OH_NNModel_Destroy(&oh_nnmodel);
    return mindspore::kLiteError;
  }
  MS_LOG(INFO) << "NNRTDelegate creates NNModel success.";

  OH_NNCompilation *oh_nn_compilation = nullptr;
  oh_nn_compilation = OH_NNCompilation_Construct(oh_nnmodel);

  if (oh_nn_compilation == nullptr) {
    MS_LOG(ERROR) << "Construct NNCompilation failed";
    OH_NNModel_Destroy(&oh_nnmodel);
    return mindspore::kLiteError;
  }
  MS_LOG(INFO) << "NNRTDelegate creates NNCompilation success.";

  const size_t *allDevicesID = nullptr;
  uint32_t device_count = 0;
  ret_code = OH_NNDevice_GetAllDevicesID(&allDevicesID, &device_count);
  if (ret_code != OH_NN_SUCCESS) {
    MS_LOG(ERROR) << "NNModel GetAllDevicesID failed, OH_NN_ReturnCode = " << ret_code;
    OH_NNCompilation_Destroy(&oh_nn_compilation);
    OH_NNModel_Destroy(&oh_nnmodel);
    return mindspore::kLiteError;
  }

  if (device_count <= 0) {
    MS_LOG(WARNING) << "No NNRt Device found, fall back to CPU. ";
    // OH_NNCompilation_Destroy(&oh_nn_compilation);
    // OH_NNModel_Destroy(&oh_nnmodel);
    return mindspore::kSuccess;
  }
  MS_LOG(INFO) << "NNRTDelegate GetAllDevicesID success.";

  // check if model ops are supported
  const bool *issupported = nullptr;
  uint32_t op_count = 0;
  ret_code = OH_NNModel_GetAvailableOperations(oh_nnmodel, allDevicesID[0], &issupported, &op_count);
  if (ret_code != OH_NN_SUCCESS) {
    MS_LOG(ERROR) << "NNModel GetAvailableOperations failed, OH_NN_ReturnCode = " << ret_code
                  << ", maybe due to dataParcel data length limitaion. Fall back to CPU.";
    OH_NNCompilation_Destroy(&oh_nn_compilation);
    OH_NNModel_Destroy(&oh_nnmodel);
    return mindspore::kSuccess;
  }
  uint32_t supported_op_count = 0;
  for (uint32_t i = 0; i < op_count; i++) {
    if (issupported[i]) {
      supported_op_count++;
    }
  }
  if (op_count != supported_op_count) {
    MS_LOG(WARNING) << "this model has " << op_count << "ops, but NNRT only support " << supported_op_count
                    << " ops, fall back to CPU.";
    // must support all op, else fall back to CPU
    OH_NNCompilation_Destroy(&oh_nn_compilation);
    OH_NNModel_Destroy(&oh_nnmodel);
    return mindspore::kSuccess;
  }
  MS_LOG(INFO) << "NNRtDelegate supports all op in this model.";

  ret_code = OH_NNCompilation_SetDevice(oh_nn_compilation, allDevicesID[0]);

  if (ret_code != OH_NN_SUCCESS) {
    MS_LOG(ERROR) << "NNCompilation SetDevice failed, OH_NN_ReturnCode = " << ret_code;
    OH_NNCompilation_Destroy(&oh_nn_compilation);
    OH_NNModel_Destroy(&oh_nnmodel);
    return mindspore::kLiteError;
  }

  ret_code = OH_NNCompilation_Build(oh_nn_compilation);

  if (ret_code != OH_NN_SUCCESS) {
    MS_LOG(ERROR) << "Build NNCompilation failed, OH_NN_ReturnCode = " << ret_code;
    OH_NNCompilation_Destroy(&oh_nn_compilation);
    OH_NNModel_Destroy(&oh_nnmodel);
    return mindspore::kLiteError;
  }

  MS_LOG(DEBUG) << "NNRTDelegate SetDevice success.";

  OH_NNExecutor *oh_nn_executor = nullptr;
  oh_nn_executor = OH_NNExecutor_Construct(oh_nn_compilation);
  if (oh_nn_executor == nullptr) {
    MS_LOG(ERROR) << "Construct NNCompilation SetDevice failed, OH_NN_ReturnCode = " << ret_code;
    OH_NNCompilation_Destroy(&oh_nn_compilation);
    OH_NNModel_Destroy(&oh_nnmodel);
    return mindspore::kLiteError;
  }
  MS_LOG(DEBUG) << "NNRTDelegate creates NNExecutor success.";
  mindspore::Status prepare_data_ret;
  auto nnr_model_kernel = new (std::nothrow) NNRTModelKernel(oh_nn_executor, model->inputs(), model->outputs());
  if (nnr_model_kernel == nullptr) {
    MS_LOG(ERROR) << "new NNRTModelKernel failed";
    return mindspore::kLiteError;
  }
  OH_NNCompilation_Destroy(&oh_nn_compilation);
  OH_NNModel_Destroy(&oh_nnmodel);
  KernelIter from = model->BeginKernelIterator();
  KernelIter end = model->EndKernelIterator();
  model->Replace(from, end, nnr_model_kernel);

  MS_LOG(INFO) << "NNRTDelegate build  success.";
  return mindspore::kSuccess;
}

mindspore::Status mindspore::NNRTDelegate::Init() {
  MS_LOG(DEBUG) << "NNRTDelegate init success.";
  return mindspore::kSuccess;
}
mindspore::Status mindspore::NNRTDelegate::PrepareInputs(DelegateModel<schema::Primitive> *model,
                                                         OH_NNExecutor *oh_nn_executor) {
  auto input_tensors = model->inputs();
  for (size_t i = 0; i < input_tensors.size(); i++) {
    auto tensor = input_tensors[i];
    auto tensor_shape = tensor.Shape();
    auto tmp_quant_param = tensor.QuantParams();
    OH_NN_QuantParam *quant_param = nullptr;
    std::vector<uint32_t> bit_num;
    std::vector<double> scale;
    std::vector<int32_t> zero_point;
    if (!tmp_quant_param.empty()) {
      quant_param = new (std::nothrow) OH_NN_QuantParam;
      if (quant_param == nullptr) {
        MS_LOG(ERROR) << "new OH_NN_QuantParam failed.";
        return mindspore::kLiteError;
      }
      for (auto qparam : tmp_quant_param) {
        bit_num.emplace_back(qparam.bit_num);
        scale.emplace_back(qparam.scale);
        zero_point.emplace_back(qparam.zero_point);
      }
      quant_param->quantCount = tmp_quant_param.size();
      quant_param->numBits = bit_num.data();
      quant_param->scale = scale.data();
      quant_param->zeroPoint = zero_point.data();
    }
    auto oprend = new (std::nothrow) OH_NN_Tensor;
    if (oprend == nullptr) {
      MS_LOG(ERROR) << "new OH_NN_Tensor Failed";
      return mindspore::kLiteError;
    }
    oprend->dataType = ConvertDataType(tensor.DataType());
    oprend->dimensionCount = tensor_shape.size();

    std::vector<int32_t> dimensions_list;
    for (auto shape : tensor_shape) {
      if (shape < INT32_MAX) {
        dimensions_list.emplace_back(static_cast<int32_t>(shape));
      } else {
        MS_LOG(ERROR) << "NNExecutor SetInput failed,tensor dimension is is too large, max dim = " << INT32_MAX
                      << ", but get dimension = " << shape;
        return mindspore::kLiteError;
      }
    }
    oprend->dimensions = dimensions_list.data();
    oprend->quantParam = quant_param;
    oprend->type = OH_NN_TENSOR;
    OH_NN_ReturnCode ret_code =
      OH_NNExecutor_SetInput(oh_nn_executor, i, oprend, tensor.MutableData(), tensor.DataSize());
    delete (oprend);

    if (!tmp_quant_param.empty()) {
      delete (quant_param);
      quant_param = nullptr;
    }

    if (ret_code != OH_NN_SUCCESS) {
      MS_LOG(ERROR) << "NNExecutor SetInput failed, current input tensor is" << tensor.Name()
                    << "OH_NN_ReturnCode = " << ret_code;
      return mindspore::kLiteError;
    }
  }

  return mindspore::kSuccess;
}
OH_NN_DataType mindspore::NNRTDelegate::ConvertDataType(mindspore::DataType data_type) {
  OH_NN_DataType oh_data_type;
  switch (data_type) {
    case mindspore::DataType::kTypeUnknown:
    case mindspore::DataType::kObjectTypeString:
    case mindspore::DataType::kObjectTypeList:
    case mindspore::DataType::kObjectTypeTuple:
    case mindspore::DataType::kObjectTypeTensorType:
    case mindspore::DataType::kNumberTypeBegin:
    case mindspore::DataType::kNumberTypeEnd:
    case mindspore::DataType::kInvalidType:
      oh_data_type = OH_NN_UNKNOWN;
      break;
    case mindspore::DataType::kNumberTypeBool:
      oh_data_type = OH_NN_BOOL;
      break;
    case mindspore::DataType::kNumberTypeInt8:
      oh_data_type = OH_NN_INT8;
      break;
    case mindspore::DataType::kNumberTypeInt16:
      oh_data_type = OH_NN_INT16;
      break;
    case mindspore::DataType::kNumberTypeInt32:
      oh_data_type = OH_NN_INT32;
      break;
    case mindspore::DataType::kNumberTypeInt64:
      oh_data_type = OH_NN_INT64;
      break;
    case mindspore::DataType::kNumberTypeUInt8:
      oh_data_type = OH_NN_UINT8;
      break;
    case mindspore::DataType::kNumberTypeUInt16:
      oh_data_type = OH_NN_UINT16;
      break;
    case mindspore::DataType::kNumberTypeUInt32:
      oh_data_type = OH_NN_UINT32;
      break;
    case mindspore::DataType::kNumberTypeUInt64:
      oh_data_type = OH_NN_UINT64;
      break;
    case mindspore::DataType::kNumberTypeFloat16:
      oh_data_type = OH_NN_FLOAT16;
      break;
    case mindspore::DataType::kNumberTypeFloat32:
      oh_data_type = OH_NN_FLOAT32;
      break;
    case mindspore::DataType::kNumberTypeFloat64:
      oh_data_type = OH_NN_FLOAT64;
      break;
    default: {
      oh_data_type = OH_NN_UNKNOWN;
    }
  }
  return oh_data_type;
}

mindspore::Status mindspore::NNRTDelegate::PrepareOutputs(DelegateModel<schema::Primitive> *model,
                                                          OH_NNExecutor *oh_nn_executor) {
  auto output_tensors = model->outputs();
  for (size_t i = 0; i < output_tensors.size(); i++) {
    auto tensor = output_tensors[i];
    OH_NN_ReturnCode ret_code = OH_NNExecutor_SetOutput(oh_nn_executor, i, tensor.MutableData(), tensor.DataSize());
    if (ret_code != OH_NN_SUCCESS) {
      MS_LOG(ERROR) << "NNExecutor SetOutput failed, current out tensor is" << tensor.Name()
                    << ", OH_NN_ReturnCode = " << ret_code;
      return mindspore::kLiteError;
    }
  }
  return mindspore::kSuccess;
}

void mindspore::NNRTDelegate::ShallowCopyLiteGraph(const mindspore::lite::LiteGraph &lite_graph) {
  Status ret;
  for (auto node : lite_graph.all_nodes_) {
    ret = lite::CheckPrimitiveSupported(static_cast<const schema::Primitive *>(node->primitive_));
    if (ret == mindspore::kLiteError) {
      MS_LOG(ERROR) << " primitive supported check failed.";
      return;
    }
  }
  std::vector<LiteGraph::Node *> node_list;
  node_list.reserve(lite_graph.all_nodes_.size());
  // copy node
  for (auto node : lite_graph.all_nodes_) {
    auto new_node = new (std::nothrow) LiteGraph::Node;
    if (new_node == nullptr) {
      MS_LOG(ERROR) << " new LiteGraph::Node failed.";
      return;
    }
    new_node->name_ = node->name_;
    new_node->op_type_ = node->op_type_;
    new_node->node_type_ = node->node_type_;
    new_node->primitive_ = node->primitive_;
    new_node->base_operator_ = node->base_operator_;
    new_node->input_indices_ = node->input_indices_;
    new_node->output_indices_ = node->output_indices_;
    new_node->quant_type_ = node->quant_type_;
    new_node->device_type_ = node->device_type_;
    node_list.emplace_back(new_node);
  }
  // copy subgraph
  std::vector<LiteGraph::SubGraph *> subgraph_list;
  for (auto subgraph : lite_graph.sub_graphs_) {
    auto new_subgraph = new (std::nothrow) LiteGraph::SubGraph;
    if (new_subgraph == nullptr) {
      MS_LOG(ERROR) << "new LiteGraph::Subgraph failed.";
      return;
    }
    new_subgraph->name_ = subgraph->name_;
    new_subgraph->input_indices_ = subgraph->input_indices_;
    new_subgraph->output_indices_ = subgraph->output_indices_;
    new_subgraph->node_indices_ = subgraph->node_indices_;
    subgraph_list.emplace_back(new_subgraph);
  }
  for (auto tensor : lite_graph.all_tensors_) {
    ret = lite::CheckTensorSupported(static_cast<const schema::Tensor *>(tensor));
    if (ret == mindspore::kLiteError) {
      MS_LOG(ERROR) << "tensor supported check failed.";
      return;
    }
  }

  nnrt_lite_graph = new (std::nothrow) lite::LiteGraph();
  if (nnrt_lite_graph == nullptr) {
    MS_LOG(ERROR) << "new LiteGraph failed.";
    return;
  }

  nnrt_lite_graph->name_ = lite_graph.name_;
  nnrt_lite_graph->version_ = lite_graph.version_;
  nnrt_lite_graph->input_indices_ = lite_graph.input_indices_;
  nnrt_lite_graph->output_indices_ = lite_graph.output_indices_;
  nnrt_lite_graph->all_tensors_ = lite_graph.all_tensors_;
  nnrt_lite_graph->all_nodes_ = node_list;
  nnrt_lite_graph->sub_graphs_ = subgraph_list;
  MS_LOG(INFO) << "ShallowCopyLiteGraph success.";
}

mindspore::NNRTDelegate::~NNRTDelegate() {
  if (this->nnrt_lite_graph != nullptr) {
    MS_LOG(ERROR) << "Delete NNRTDelegate.";
  }
};
