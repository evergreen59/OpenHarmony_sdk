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
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
__constant sampler_t smp_zero = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
#define NHWC4 2

__kernel void PRelu_scalar(__read_only image2d_t input, __write_only image2d_t output, float weight, int4 shape,
                           int data_format) {
  int nh = get_global_id(0);
  int w = get_global_id(1);
  int c = get_global_id(2);
  if (nh >= shape.x * shape.y || w >= shape.z || c >= shape.w || shape.y == 0) {
    return;
  }
  int n = nh / shape.y;
  int h = nh % shape.y;
  int x = w * shape.w + c;
  int y = n * shape.y + h;
  FLT4 out = READ_IMAGE(input, smp_zero, (int2)(x, y));
  if (out.x < 0) {
    out.x *= weight;
  }
  if (out.y < 0) {
    out.y *= weight;
  }
  if (out.z < 0) {
    out.z *= weight;
  }
  if (out.w < 0) {
    out.w *= weight;
  }
  WRITE_IMAGE(output, (int2)(x, y), out);
}

__kernel void PRelu_vector(__read_only image2d_t input, __write_only image2d_t output, __global FLT4 *weight_vector,
                           int4 shape, int data_format) {
  int nh = get_global_id(0);
  int w = get_global_id(1);
  int c = get_global_id(2);
  if (nh >= shape.x * shape.y || w >= shape.z || c >= shape.w || shape.y == 0) {
    return;
  }
  int n = nh / shape.y;
  int h = nh % shape.y;
  int x = w * shape.w + c;
  int y = n * shape.y + h;
  FLT4 weight = weight_vector[c];

  FLT4 out = READ_IMAGE(input, smp_zero, (int2)(x, y));
  if (out.x < 0) {
    out.x *= weight.x;
  }
  if (out.y < 0) {
    out.y *= weight.y;
  }
  if (out.z < 0) {
    out.z *= weight.z;
  }
  if (out.w < 0) {
    out.w *= weight.w;
  }
  WRITE_IMAGE(output, (int2)(x, y), out);
}
