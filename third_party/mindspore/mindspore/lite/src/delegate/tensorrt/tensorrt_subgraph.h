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
#ifndef MINDSPORE_LITE_SRC_RUNTIME_DELEGATE_TENSORRT_SUB_GTAPH_
#define MINDSPORE_LITE_SRC_RUNTIME_DELEGATE_TENSORRT_SUB_GTAPH_
#include <utility>
#include <set>
#include <string>
#include <vector>
#include <memory>
#include "include/api/kernel.h"
#include "src/delegate/tensorrt/tensorrt_runtime.h"
#include "src/delegate/tensorrt/tensorrt_utils.h"
#include "include/api/context.h"

namespace mindspore::lite {
using mindspore::lite::RET_ERROR;
using mindspore::lite::RET_OK;
class TensorRTSubGraph : public kernel::Kernel {
 public:
  TensorRTSubGraph(std::vector<TensorRTOp *> ops, const std::vector<mindspore::MSTensor> &inputs,
                   const std::vector<mindspore::MSTensor> &outputs, const mindspore::Context *ctx,
                   std::shared_ptr<GPUDeviceInfo> device_info, TensorRTRuntime *runtime, bool support_hw_resize)
      : kernel::Kernel(inputs, outputs, nullptr, ctx),
        all_ops_(std::move(ops)),
        device_info_(device_info),
        runtime_(runtime) {
    trt_specific_weight_nodes_ = {
      schema::PrimitiveType_Conv2DFusion, schema::PrimitiveType_ReduceFusion,  schema::PrimitiveType_Transpose,
      schema::PrimitiveType_Gather,       schema::PrimitiveType_Reshape,       schema::PrimitiveType_PowFusion,
      schema::PrimitiveType_AddFusion,    schema::PrimitiveType_DivFusion,     schema::PrimitiveType_SubFusion,
      schema::PrimitiveType_MatMulFusion, schema::PrimitiveType_PowFusion,     schema::PrimitiveType_Eltwise,
      schema::PrimitiveType_ScaleFusion,  schema::PrimitiveType_MulFusion,     schema::PrimitiveType_StridedSlice,
      schema::PrimitiveType_PadFusion,    schema::PrimitiveType_FullConnection};
    if (!support_hw_resize) {
      input_hw_index_ = -1;
    }
  }

  ~TensorRTSubGraph() override;

  int Prepare() override;

  int Execute() override;

  int ReSize();

  int BuildTensorRTGraph();

  int Init();

 private:
  int BuildEngine();

  int SetDeviceConfig();

  bool SupportFP16();

  nvinfer1::ITensor *SetTensorRTNetworkInput(const mindspore::MSTensor &in_tensor);

  ITensorHelper FindTensorRTInputs(TensorRTOp *cur_op, const mindspore::MSTensor &in_tensor);

  int MarkOutputs();

  std::vector<TensorRTOp *> all_ops_{};
  // subgraph input nodes.
  std::vector<TensorRTOp *> in_ops_{};
  // subgraph output nodes.
  std::vector<TensorRTOp *> out_ops_{};

  void **tensor_bindings_{nullptr};

  std::shared_ptr<GPUDeviceInfo> device_info_{nullptr};

  TensorRTRuntime *runtime_{nullptr};  // all subgraph in one delegate share a runtime_

  std::set<mindspore::schema::PrimitiveType> trt_specific_weight_nodes_;

  // save in/out tensor name for subgraph isolate.
  std::vector<std::string> trt_in_tensor_name_;
  std::vector<std::string> trt_out_tensor_name_;

  nvinfer1::INetworkDefinition *network_{nullptr};
  nvinfer1::IBuilderConfig *config_{nullptr};
  nvinfer1::ICudaEngine *engine_{nullptr};
  nvinfer1::IExecutionContext *trt_context_{nullptr};
  nvinfer1::IOptimizationProfile *profile_{nullptr};

  int input_batchsize_index_{0};
  int output_batchsize_index_{0};

  // -1 means don't support hw resize
  int input_hw_index_{0};
};
}  // namespace mindspore::lite
#endif  // MINDSPORE_LITE_SRC_RUNTIME_DELEGATE_TENSORRT_SUB_GTAPH_
