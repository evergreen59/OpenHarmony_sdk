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
#include <include/errorcode.h>
#include "nnrt_model_kernel.h"
int mindspore::NNRTModelKernel::Prepare() { return 0; }
int mindspore::NNRTModelKernel::Execute() {
  lite::STATUS ret_val = PrepareInputs();
  if (ret_val != lite::RET_OK) {
    MS_LOG(ERROR) << "NNRTModelKernel PrepareInputs failed, STATUS is " << ret_val;
    return ret_val;
  }
  ret_val = TransferOutputs();
  if (ret_val != lite::RET_OK) {
    MS_LOG(ERROR) << "NNRTModelKernel TransferOutputs failed, STATUS is " << ret_val;
    return ret_val;
  }
  MS_LOG(INFO) << "Running NNRtModel Kernel...";
  OH_NN_ReturnCode ret_code;
  ret_code = OH_NNExecutor_Run(this->oh_nn_executor);

  if (ret_code != OH_NN_SUCCESS) {
    MS_LOG(ERROR) << "NNExecutor Run failed, OH_NN_ReturnCode = " << ret_code;
    return lite::RET_ERROR;
  }
  MS_LOG(INFO) << "Run NNRtModel Kernel success.";

  return lite::RET_OK;
}

OH_NN_DataType mindspore::NNRTModelKernel::ConvertDataType(mindspore::DataType data_type) {
  OH_NN_DataType oh_data_type;
  switch (data_type) {
    case DataType::kTypeUnknown:
    case DataType::kObjectTypeString:
    case DataType::kObjectTypeList:
    case DataType::kObjectTypeTuple:
    case DataType::kObjectTypeTensorType:
    case DataType::kNumberTypeBegin:
    case DataType::kNumberTypeEnd:
    case DataType::kInvalidType:
      oh_data_type = OH_NN_UNKNOWN;
      break;
    case DataType::kNumberTypeBool:
      oh_data_type = OH_NN_BOOL;
      break;
    case DataType::kNumberTypeInt8:
      oh_data_type = OH_NN_INT8;
      break;
    case DataType::kNumberTypeInt16:
      oh_data_type = OH_NN_INT16;
      break;
    case DataType::kNumberTypeInt32:
      oh_data_type = OH_NN_INT32;
      break;
    case DataType::kNumberTypeInt64:
      oh_data_type = OH_NN_INT64;
      break;
    case DataType::kNumberTypeUInt8:
      oh_data_type = OH_NN_UINT8;
      break;
    case DataType::kNumberTypeUInt16:
      oh_data_type = OH_NN_UINT16;
      break;
    case DataType::kNumberTypeUInt32:
      oh_data_type = OH_NN_UINT32;
      break;
    case DataType::kNumberTypeUInt64:
      oh_data_type = OH_NN_UINT64;
      break;
    case DataType::kNumberTypeFloat16:
      oh_data_type = OH_NN_FLOAT16;
      break;
    case DataType::kNumberTypeFloat32:
      oh_data_type = OH_NN_FLOAT32;
      break;
    case DataType::kNumberTypeFloat64:
      oh_data_type = OH_NN_FLOAT64;
      break;
    default: {
      oh_data_type = OH_NN_UNKNOWN;
    }
  }
  return oh_data_type;
}
int mindspore::NNRTModelKernel::PrepareInputs() {
  auto input_tensors = this->inputs();
  for (int i = 0; i < input_tensors.size(); i++) {
    auto tensor = input_tensors[i];
    auto tensor_shape = tensor.Shape();
    auto tmp_quant_param = tensor.QuantParams();
    OH_NN_QuantParam *quant_param = nullptr;
    std::vector<uint32_t> bit_num;
    std::vector<double> scale;
    std::vector<int32_t> zero_point;
    if (!tmp_quant_param.empty()) {
      quant_param = (new (std::nothrow) OH_NN_QuantParam);
      if (quant_param == nullptr) {
        MS_LOG(ERROR) << "new OH_NN_QuantParam failed.";
        return lite::RET_NULL_PTR;
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
      return lite::RET_ERROR;
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
        return lite::RET_ERROR;
      }
    }
    oprend->dimensions = dimensions_list.data();
    oprend->quantParam = quant_param;
    oprend->type = OH_NN_TENSOR;
    OH_NN_ReturnCode ret_code =
      OH_NNExecutor_SetInput(oh_nn_executor, i, oprend, tensor.MutableData(), tensor.DataSize());
    delete (oprend);

    if (!tmp_quant_param.empty()) {
      free(quant_param);
      quant_param = nullptr;
    }

    if (ret_code != OH_NN_SUCCESS) {
      MS_LOG(ERROR) << "NNExecutor SetInput failed, current input tensor is" << tensor.Name()
                    << "OH_NN_ReturnCode = " << ret_code;
      return lite::RET_ERROR;
    }
  }

  return lite::RET_OK;
}
int mindspore::NNRTModelKernel::TransferOutputs() {
  auto output_tensors = this->outputs();
  for (size_t i = 0; i < output_tensors.size(); i++) {
    auto tensor = output_tensors[i];
    OH_NN_ReturnCode ret_code = OH_NNExecutor_SetOutput(oh_nn_executor, i, tensor.MutableData(), tensor.DataSize());
    if (ret_code != OH_NN_SUCCESS) {
      MS_LOG(ERROR) << "NNExecutor SetOutput failed, current out tensor is" << tensor.Name()
                    << ", OH_NN_ReturnCode = " << ret_code;
      return lite::RET_ERROR;
    }
  }
  return lite::RET_OK;
}
