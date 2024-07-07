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
#ifndef LITE_NNRT_MODEL_KERNEL_H
#define LITE_NNRT_MODEL_KERNEL_H
#include <vector>
#include <queue>
#include <map>
#include <utility>
#include "include/api/kernel.h"
#include "interfaces/kits/c/neural_network_runtime.h"
#include "src/common/log_adapter.h"
#include "include/errorcode.h"

namespace mindspore {

class NNRTModelKernel : public kernel::Kernel {
  /**
   * Because nnr can't run single op, but the whole model. So we decide to make the whole model into one kernel.
   * */
 public:
  NNRTModelKernel(OH_NNExecutor *oh_nn_executor, const std::vector<mindspore::MSTensor> &inputs,
                  const std::vector<mindspore::MSTensor> &outputs)
      : kernel::Kernel(inputs, outputs, nullptr, nullptr), oh_nn_executor(oh_nn_executor) {}
  int Prepare() override;
  int Execute() override;
  int ReSize() override {
    MS_LOG(ERROR) << "NNRT does not support the resize function temporarily.";
    return lite::RET_ERROR;
  };
  OH_NN_DataType ConvertDataType(mindspore::DataType data_type);
  int PrepareInputs();
  int TransferOutputs();
  ~NNRTModelKernel() override {
    MS_LOG(INFO) << "start NNExecutor Destroy.";
    OH_NNExecutor_Destroy(&oh_nn_executor);
    MS_LOG(INFO) << "start NNExecutor Finish.";
  }

 protected:
  OH_NNExecutor *oh_nn_executor = nullptr;
};
}  // namespace mindspore

#endif  // LITE_NNRTT_MODEL_KERNEL_H
