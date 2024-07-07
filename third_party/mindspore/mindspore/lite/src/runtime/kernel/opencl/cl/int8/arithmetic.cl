/* Copyright 2021 Huawei Technologies Co., Ltd
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
__constant sampler_t smp_none = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

__kernel void ElementAddInt8(__read_only image2d_t input_a, __read_only image2d_t input_b,
                             __write_only image2d_t output, const int2 output_shape, float act_min, float act_max,
                             const float4 scale, const char4 zero_point) {
  int X = get_global_id(0);
  int Y = get_global_id(1);
  if (X >= output_shape.x || Y >= output_shape.y) {
    return;
  }
  char4 a = convert_char4(read_imagei(input_a, smp_none, (int2)(X, Y)));
  char4 b = convert_char4(read_imagei(input_b, smp_none, (int2)(X, Y)));

  float4 real_a = convert_float4(a - zero_point.x) * scale.x;
  float4 real_b = convert_float4(b - zero_point.y) * scale.y;
  int4 result = convert_int4(round((real_a + real_b) / scale.z)) + zero_point.z;
  result = clamp(result, (int)(act_min), (int)(act_max));
  write_imagei(output, (int2)(X, Y), result);
}
