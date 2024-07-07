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
#include <stdlib.h>
#include <stdio.h>
#include "mindspore/model.h"

//生成随机的输入
int GenerateInputDataWithRandom(OH_AI_TensorHandleArray inputs) {
  for (size_t i = 0; i < inputs.handle_num; ++i) {
    float *input_data = (float *)OH_AI_TensorGetMutableData(inputs.handle_list[i]);
    if (input_data == NULL) {
      printf("MSTensorGetMutableData failed.\n");
      return OH_AI_STATUS_LITE_ERROR;
    }
    int64_t num = OH_AI_TensorGetElementNum(inputs.handle_list[i]);
    const int divisor = 10;
    for (size_t j = 0; j < num; j++) {
      input_data[j] = (float)(rand() % divisor) / divisor;  // 0--0.9f
    }
  }
  return OH_AI_STATUS_SUCCESS;
}

int QuickStart(int argc, const char **argv) {
  if (argc < 2) {
    printf("Model file must be provided.\n");
    return OH_AI_STATUS_LITE_ERROR;
  }

  // 创建并配置上下文，设置运行时的线程数量为2，绑核策略为大核优先
  OH_AI_ContextHandle context = OH_AI_ContextCreate();
  if (context == NULL) {
    printf("OH_AI_ContextCreate failed.\n");
    return OH_AI_STATUS_LITE_ERROR;
  }
  const int thread_num = 2;
  OH_AI_ContextSetThreadNum(context, thread_num);
  OH_AI_ContextSetThreadAffinityMode(context, 1);
  //设置运行设备为CPU，不使用Float16推理
  OH_AI_DeviceInfoHandle cpu_device_info = OH_AI_DeviceInfoCreate(OH_AI_DEVICETYPE_CPU);
  if (cpu_device_info == NULL) {
    printf("OH_AI_DeviceInfoCreate failed.\n");
    OH_AI_ContextDestroy(&context);
    return OH_AI_STATUS_LITE_ERROR;
  }
  OH_AI_DeviceInfoSetEnableFP16(cpu_device_info, false);
  OH_AI_ContextAddDeviceInfo(context, cpu_device_info);

  // 创建模型
  OH_AI_ModelHandle model = OH_AI_ModelCreate();
  if (model == NULL) {
    printf("OH_AI_ModelCreate failed.\n");
    OH_AI_ContextDestroy(&context);
    return OH_AI_STATUS_LITE_ERROR;
  }

  // 加载与编译模型，模型的类型为OH_AI_ModelTypeMindIR
  int ret = OH_AI_ModelBuildFromFile(model, argv[1], OH_AI_MODELTYPE_MINDIR, context);
  if (ret != OH_AI_STATUS_SUCCESS) {
    printf("OH_AI_ModelBuildFromFile failed, ret: %d.\n", ret);
    OH_AI_ModelDestroy(&model);
    return ret;
  }

  // 获得输入张量
  OH_AI_TensorHandleArray inputs = OH_AI_ModelGetInputs(model);
  if (inputs.handle_list == NULL) {
    printf("OH_AI_ModelGetInputs failed, ret: %d.\n", ret);
    OH_AI_ModelDestroy(&model);
    return ret;
  }
  // 使用随机数据填充张量
  ret = GenerateInputDataWithRandom(inputs);
  if (ret != OH_AI_STATUS_SUCCESS) {
    printf("GenerateInputDataWithRandom failed, ret: %d.\n", ret);
    OH_AI_ModelDestroy(&model);
    return ret;
  }

  // 执行模型推理
  OH_AI_TensorHandleArray outputs;
  ret = OH_AI_ModelPredict(model, inputs, &outputs, NULL, NULL);
  if (ret != OH_AI_STATUS_SUCCESS) {
    printf("OH_AI_ModelPredict failed, ret: %d.\n", ret);
    OH_AI_ModelDestroy(&model);
    return ret;
  }

  // 获取模型的输出张量，并打印
  for (size_t i = 0; i < outputs.handle_num; ++i) {
    OH_AI_TensorHandle tensor = outputs.handle_list[i];
    int64_t element_num = OH_AI_TensorGetElementNum(tensor);
    printf("Tensor name: %s, tensor size is %zu ,elements num: %lld.\n", OH_AI_TensorGetName(tensor),
           OH_AI_TensorGetDataSize(tensor), element_num);
    const float *data = (const float *)OH_AI_TensorGetData(tensor);
    printf("output data is:\n");
    const int max_print_num = 50;
    for (int j = 0; j < element_num && j <= max_print_num; ++j) {
      printf("%f ", data[j]);
    }
    printf("\n");
  }

  // 释放模型
  OH_AI_ModelDestroy(&model);
  return OH_AI_STATUS_SUCCESS;
}

int main(int argc, const char **argv) { return QuickStart(argc, argv); }
