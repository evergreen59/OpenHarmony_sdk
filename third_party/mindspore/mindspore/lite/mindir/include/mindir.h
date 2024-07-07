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
#ifndef MINDSPORE_LITE_MINDIR_H
#define MINDSPORE_LITE_MINDIR_H
#include "mindir_types.h"
#include "mindir_lite_graph.h"
#include "mindir_tensor.h"
#include "mindir_primitive.h"
namespace OHOS {
namespace HDI {
namespace Nnrt {
namespace V1_0 {
struct Model;
}  // namespace V1_0
}  // namespace Nnrt
}  // namespace HDI
}  // namespace OHOS

namespace mindspore {
namespace lite {

// ********** Model **********
OHOS::HDI::Nnrt::V1_0::Model *MindIR_LiteGraph_To_Model(const LiteGraph *lite_graph,
                                                        const OHOS::HDI::Nnrt::V1_0::SharedBuffer &buffer);
void MindIR_Model_Destroy(OHOS::HDI::Nnrt::V1_0::Model **model);

// ********** Activation **********
PrimitivePtr MindIR_Activation_CreatePrimitive(ActivationType activation_type, float alpha, float min_val,
                                               float max_val, bool approximate);
ActivationType MindIR_Activation_GetActivationType(ConstPrimitivePtr primitive);
void MindIR_Activation_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type);
float MindIR_Activation_GetAlpha(ConstPrimitivePtr primitive);
void MindIR_Activation_SetAlpha(PrimitivePtr *primitive, float alpha);
float MindIR_Activation_GetMinVal(ConstPrimitivePtr primitive);
void MindIR_Activation_SetMinVal(PrimitivePtr *primitive, float min_val);
float MindIR_Activation_GetMaxVal(ConstPrimitivePtr primitive);
void MindIR_Activation_SetMaxVal(PrimitivePtr *primitive, float max_val);
bool MindIR_Activation_GetApproximate(ConstPrimitivePtr primitive);
void MindIR_Activation_SetApproximate(PrimitivePtr *primitive, bool approximate);

// ********** AddFusion **********
PrimitivePtr MindIR_AddFusion_CreatePrimitive(ActivationType activation_type);
ActivationType MindIR_AddFusion_GetActivationType(ConstPrimitivePtr primitive);
void MindIR_AddFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type);

// ********** ArgMaxFusion **********
PrimitivePtr MindIR_ArgMaxFusion_CreatePrimitive(int64_t axis, int64_t top_k, bool keep_dims, bool out_max_value);
int64_t MindIR_ArgMaxFusion_GetAxis(ConstPrimitivePtr primitive);
void MindIR_ArgMaxFusion_SetAxis(PrimitivePtr *primitive, int64_t axis);
int64_t MindIR_ArgMaxFusion_GetTopK(ConstPrimitivePtr primitive);
void MindIR_ArgMaxFusion_SetTopK(PrimitivePtr *primitive, int64_t top_k);
bool MindIR_ArgMaxFusion_GetKeepDims(ConstPrimitivePtr primitive);
void MindIR_ArgMaxFusion_SetKeepDims(PrimitivePtr *primitive, bool keep_dims);
bool MindIR_ArgMaxFusion_GetOutMaxValue(ConstPrimitivePtr primitive);
void MindIR_ArgMaxFusion_SetOutMaxValue(PrimitivePtr *primitive, bool out_max_value);

// ********** AvgPoolFusion **********
PrimitivePtr MindIR_AvgPoolFusion_CreatePrimitive(const std::vector<int64_t> &kernel_size,
                                                  const std::vector<int64_t> &strides, const std::vector<int64_t> &pad,
                                                  PadMode pad_mode, RoundMode round_mode, Format format, bool global,
                                                  ActivationType activation_type);
std::vector<int64_t> MindIR_AvgPoolFusion_GetKernelSize(ConstPrimitivePtr primitive);
void MindIR_AvgPoolFusion_SetKernelSize(PrimitivePtr *primitive, const std::vector<int64_t> &kernel_size);
std::vector<int64_t> MindIR_AvgPoolFusion_GetStrides(ConstPrimitivePtr primitive);
void MindIR_AvgPoolFusion_SetStrides(PrimitivePtr *primitive, const std::vector<int64_t> &strides);
std::vector<int64_t> MindIR_AvgPoolFusion_GetPad(ConstPrimitivePtr primitive);
void MindIR_AvgPoolFusion_SetPad(PrimitivePtr *primitive, const std::vector<int64_t> &pad);
PadMode MindIR_AvgPoolFusion_GetPadMode(ConstPrimitivePtr primitive);
void MindIR_AvgPoolFusion_SetPadMode(PrimitivePtr *primitive, PadMode pad_mode);
RoundMode MindIR_AvgPoolFusion_GetRoundMode(ConstPrimitivePtr primitive);
void MindIR_AvgPoolFusion_SetRoundMode(PrimitivePtr *primitive, RoundMode round_mode);
Format MindIR_AvgPoolFusion_GetFormat(ConstPrimitivePtr primitive);
void MindIR_AvgPoolFusion_SetFormat(PrimitivePtr *primitive, Format format);
bool MindIR_AvgPoolFusion_GetGlobal(ConstPrimitivePtr primitive);
void MindIR_AvgPoolFusion_SetGlobal(PrimitivePtr *primitive, bool global);
ActivationType MindIR_AvgPoolFusion_GetActivationType(ConstPrimitivePtr primitive);
void MindIR_AvgPoolFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type);

// ********** BatchToSpaceND **********
PrimitivePtr MindIR_BatchToSpaceND_CreatePrimitive(const std::vector<int64_t> &block_shape,
                                                   const std::vector<std::vector<int64_t>> &crops);
std::vector<int64_t> MindIR_BatchToSpaceND_GetBlockShape(ConstPrimitivePtr primitive);
void MindIR_BatchToSpaceND_SetBlockShape(PrimitivePtr *primitive, const std::vector<int64_t> &block_shape);
std::vector<std::vector<int64_t>> MindIR_BatchToSpaceND_GetCrops(ConstPrimitivePtr primitive);
void MindIR_BatchToSpaceND_SetCrops(PrimitivePtr *primitive, const std::vector<std::vector<int64_t>> &crops);

// ********** BiasAdd **********
PrimitivePtr MindIR_BiasAdd_CreatePrimitive();

// ********** Cast **********
PrimitivePtr MindIR_Cast_CreatePrimitive();

// ********** Concat **********
PrimitivePtr MindIR_Concat_CreatePrimitive(int64_t axis);
int64_t MindIR_Concat_GetAxis(ConstPrimitivePtr primitive);
void MindIR_Concat_SetAxis(PrimitivePtr *primitive, int64_t axis);

// ********** Conv2DFusion **********
PrimitivePtr MindIR_Conv2DFusion_CreatePrimitive(const std::vector<int64_t> &kernel_size,
                                                 const std::vector<int64_t> &stride,
                                                 const std::vector<int64_t> &dilation, PadMode pad_mode,
                                                 const std::vector<int64_t> &pad_list, int64_t group,
                                                 int64_t in_channel, int64_t out_channel,
                                                 ActivationType activation_type);
std::vector<int64_t> MindIR_Conv2DFusion_GetKernelSize(ConstPrimitivePtr primitive);
void MindIR_Conv2DFusion_SetKernelSize(PrimitivePtr *primitive, const std::vector<int64_t> &kernel_size);
std::vector<int64_t> MindIR_Conv2DFusion_GetStride(ConstPrimitivePtr primitive);
void MindIR_Conv2DFusion_SetStride(PrimitivePtr *primitive, const std::vector<int64_t> &stride);
std::vector<int64_t> MindIR_Conv2DFusion_GetDilation(ConstPrimitivePtr primitive);
void MindIR_Conv2DFusion_SetDilation(PrimitivePtr *primitive, const std::vector<int64_t> &dilation);
PadMode MindIR_Conv2DFusion_GetPadMode(ConstPrimitivePtr primitive);
void MindIR_Conv2DFusion_SetPadMode(PrimitivePtr *primitive, PadMode pad_mode);
std::vector<int64_t> MindIR_Conv2DFusion_GetPadList(ConstPrimitivePtr primitive);
void MindIR_Conv2DFusion_SetPadList(PrimitivePtr *primitive, const std::vector<int64_t> &pad_list);
int64_t MindIR_Conv2DFusion_GetGroup(ConstPrimitivePtr primitive);
void MindIR_Conv2DFusion_SetGroup(PrimitivePtr *primitive, int64_t group);
int64_t MindIR_Conv2DFusion_GetInChannel(ConstPrimitivePtr primitive);
void MindIR_Conv2DFusion_SetInChannel(PrimitivePtr *primitive, int64_t in_channel);
int64_t MindIR_Conv2DFusion_GetOutChannel(ConstPrimitivePtr primitive);
void MindIR_Conv2DFusion_SetOutChannel(PrimitivePtr *primitive, int64_t out_channel);
ActivationType MindIR_Conv2DFusion_GetActivationType(ConstPrimitivePtr primitive);
void MindIR_Conv2DFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type);

// ********** Conv2dTransposeFusion **********
PrimitivePtr MindIR_Conv2dTransposeFusion_CreatePrimitive(
  const std::vector<int64_t> &kernel_size, const std::vector<int64_t> &stride, const std::vector<int64_t> &dilation,
  PadMode pad_mode, const std::vector<int64_t> &pad_list, int64_t group, int64_t in_channel, int64_t out_channel,
  ActivationType activation_type, const std::vector<int64_t> &output_paddings);
std::vector<int64_t> MindIR_Conv2dTransposeFusion_GetKernelSize(ConstPrimitivePtr primitive);
void MindIR_Conv2dTransposeFusion_SetKernelSize(PrimitivePtr *primitive, const std::vector<int64_t> &kernel_size);
std::vector<int64_t> MindIR_Conv2dTransposeFusion_GetStride(ConstPrimitivePtr primitive);
void MindIR_Conv2dTransposeFusion_SetStride(PrimitivePtr *primitive, const std::vector<int64_t> &stride);
std::vector<int64_t> MindIR_Conv2dTransposeFusion_GetDilation(ConstPrimitivePtr primitive);
void MindIR_Conv2dTransposeFusion_SetDilation(PrimitivePtr *primitive, const std::vector<int64_t> &dilation);
PadMode MindIR_Conv2dTransposeFusion_GetPadMode(ConstPrimitivePtr primitive);
void MindIR_Conv2dTransposeFusion_SetPadMode(PrimitivePtr *primitive, PadMode pad_mode);
std::vector<int64_t> MindIR_Conv2dTransposeFusion_GetPadList(ConstPrimitivePtr primitive);
void MindIR_Conv2dTransposeFusion_SetPadList(PrimitivePtr *primitive, const std::vector<int64_t> &pad_list);
int64_t MindIR_Conv2dTransposeFusion_GetGroup(ConstPrimitivePtr primitive);
void MindIR_Conv2dTransposeFusion_SetGroup(PrimitivePtr *primitive, int64_t group);
int64_t MindIR_Conv2dTransposeFusion_GetInChannel(ConstPrimitivePtr primitive);
void MindIR_Conv2dTransposeFusion_SetInChannel(PrimitivePtr *primitive, int64_t in_channel);
int64_t MindIR_Conv2dTransposeFusion_GetOutChannel(ConstPrimitivePtr primitive);
void MindIR_Conv2dTransposeFusion_SetOutChannel(PrimitivePtr *primitive, int64_t out_channel);
ActivationType MindIR_Conv2dTransposeFusion_GetActivationType(ConstPrimitivePtr primitive);
void MindIR_Conv2dTransposeFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type);
std::vector<int64_t> MindIR_Conv2dTransposeFusion_GetOutputPaddings(ConstPrimitivePtr primitive);
void MindIR_Conv2dTransposeFusion_SetOutputPaddings(PrimitivePtr *primitive,
                                                    const std::vector<int64_t> &output_paddings);

// ********** DivFusion **********
PrimitivePtr MindIR_DivFusion_CreatePrimitive(ActivationType activation_type);
ActivationType MindIR_DivFusion_GetActivationType(ConstPrimitivePtr primitive);
void MindIR_DivFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type);

// ********** Eltwise **********
PrimitivePtr MindIR_Eltwise_CreatePrimitive(EltwiseMode mode);
EltwiseMode MindIR_Eltwise_GetMode(ConstPrimitivePtr primitive);
void MindIR_Eltwise_SetMode(PrimitivePtr *primitive, EltwiseMode mode);

// ********** ExpandDims **********
PrimitivePtr MindIR_ExpandDims_CreatePrimitive();

// ********** Fill **********
PrimitivePtr MindIR_Fill_CreatePrimitive();

// ********** FullConnection **********
PrimitivePtr MindIR_FullConnection_CreatePrimitive(bool has_bias, bool use_axis, int64_t axis,
                                                   ActivationType activation_type);
bool MindIR_FullConnection_GetHasBias(ConstPrimitivePtr primitive);
void MindIR_FullConnection_SetHasBias(PrimitivePtr *primitive, bool has_bias);
bool MindIR_FullConnection_GetUseAxis(ConstPrimitivePtr primitive);
void MindIR_FullConnection_SetUseAxis(PrimitivePtr *primitive, bool use_axis);
int64_t MindIR_FullConnection_GetAxis(ConstPrimitivePtr primitive);
void MindIR_FullConnection_SetAxis(PrimitivePtr *primitive, int64_t axis);
ActivationType MindIR_FullConnection_GetActivationType(ConstPrimitivePtr primitive);
void MindIR_FullConnection_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type);

// ********** FusedBatchNorm **********
PrimitivePtr MindIR_FusedBatchNorm_CreatePrimitive(float epsilon);
float MindIR_FusedBatchNorm_GetEpsilon(ConstPrimitivePtr primitive);
void MindIR_FusedBatchNorm_SetEpsilon(PrimitivePtr *primitive, float epsilon);

// ********** Gather **********
PrimitivePtr MindIR_Gather_CreatePrimitive();

// ********** LayerNormFusion **********
PrimitivePtr MindIR_LayerNormFusion_CreatePrimitive(int64_t begin_norm_axis, float epsilon, bool elementwise_affine,
                                                    int64_t begin_params_axis);
int64_t MindIR_LayerNormFusion_GetBeginNormAxis(ConstPrimitivePtr primitive);
void MindIR_LayerNormFusion_SetBeginNormAxis(PrimitivePtr *primitive, int64_t begin_norm_axis);
float MindIR_LayerNormFusion_GetEpsilon(ConstPrimitivePtr primitive);
void MindIR_LayerNormFusion_SetEpsilon(PrimitivePtr *primitive, float epsilon);
bool MindIR_LayerNormFusion_GetElementwiseAffine(ConstPrimitivePtr primitive);
void MindIR_LayerNormFusion_SetElementwiseAffine(PrimitivePtr *primitive, bool elementwise_affine);
int64_t MindIR_LayerNormFusion_GetBeginParamsAxis(ConstPrimitivePtr primitive);
void MindIR_LayerNormFusion_SetBeginParamsAxis(PrimitivePtr *primitive, int64_t begin_params_axis);

// ********** LessEqual **********
PrimitivePtr MindIR_LessEqual_CreatePrimitive();

// ********** MatMulFusion **********
PrimitivePtr MindIR_MatMulFusion_CreatePrimitive(bool transpose_a, bool transpose_b, ActivationType activation_type);
bool MindIR_MatMulFusion_GetTransposeA(ConstPrimitivePtr primitive);
void MindIR_MatMulFusion_SetTransposeA(PrimitivePtr *primitive, bool transpose_a);
bool MindIR_MatMulFusion_GetTransposeB(ConstPrimitivePtr primitive);
void MindIR_MatMulFusion_SetTransposeB(PrimitivePtr *primitive, bool transpose_b);
ActivationType MindIR_MatMulFusion_GetActivationType(ConstPrimitivePtr primitive);
void MindIR_MatMulFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type);

// ********** Maximum **********
PrimitivePtr MindIR_Maximum_CreatePrimitive();

// ********** MaxPoolFusion **********
PrimitivePtr MindIR_MaxPoolFusion_CreatePrimitive(const std::vector<int64_t> &kernel_size,
                                                  const std::vector<int64_t> &strides, const std::vector<int64_t> &pad,
                                                  PadMode pad_mode, Format format, bool global,
                                                  ActivationType activation_type);
std::vector<int64_t> MindIR_MaxPoolFusion_GetKernelSize(ConstPrimitivePtr primitive);
void MindIR_MaxPoolFusion_SetKernelSize(PrimitivePtr *primitive, const std::vector<int64_t> &kernel_size);
std::vector<int64_t> MindIR_MaxPoolFusion_GetStrides(ConstPrimitivePtr primitive);
void MindIR_MaxPoolFusion_SetStrides(PrimitivePtr *primitive, const std::vector<int64_t> &strides);
std::vector<int64_t> MindIR_MaxPoolFusion_GetPad(ConstPrimitivePtr primitive);
void MindIR_MaxPoolFusion_SetPad(PrimitivePtr *primitive, const std::vector<int64_t> &pad);
PadMode MindIR_MaxPoolFusion_GetPadMode(ConstPrimitivePtr primitive);
void MindIR_MaxPoolFusion_SetPadMode(PrimitivePtr *primitive, PadMode pad_mode);
Format MindIR_MaxPoolFusion_GetFormat(ConstPrimitivePtr primitive);
void MindIR_MaxPoolFusion_SetFormat(PrimitivePtr *primitive, Format format);
bool MindIR_MaxPoolFusion_GetGlobal(ConstPrimitivePtr primitive);
void MindIR_MaxPoolFusion_SetGlobal(PrimitivePtr *primitive, bool global);
ActivationType MindIR_MaxPoolFusion_GetActivationType(ConstPrimitivePtr primitive);
void MindIR_MaxPoolFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type);

// ********** MulFusion **********
PrimitivePtr MindIR_MulFusion_CreatePrimitive(ActivationType activation_type);
ActivationType MindIR_MulFusion_GetActivationType(ConstPrimitivePtr primitive);
void MindIR_MulFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type);

// ********** OneHot **********
PrimitivePtr MindIR_OneHot_CreatePrimitive(int64_t axis);
int64_t MindIR_OneHot_GetAxis(ConstPrimitivePtr primitive);
void MindIR_OneHot_SetAxis(PrimitivePtr *primitive, int64_t axis);

// ********** PadFusion **********
PrimitivePtr MindIR_PadFusion_CreatePrimitive(const std::vector<std::vector<int64_t>> &paddings,
                                              PaddingMode padding_mode, float constant_value);
std::vector<std::vector<int64_t>> MindIR_PadFusion_GetPaddings(ConstPrimitivePtr primitive);
void MindIR_PadFusion_SetPaddings(PrimitivePtr *primitive, const std::vector<std::vector<int64_t>> &paddings);
PaddingMode MindIR_PadFusion_GetPaddingMode(ConstPrimitivePtr primitive);
void MindIR_PadFusion_SetPaddingMode(PrimitivePtr *primitive, PaddingMode padding_mode);
float MindIR_PadFusion_GetConstantValue(ConstPrimitivePtr primitive);
void MindIR_PadFusion_SetConstantValue(PrimitivePtr *primitive, float constant_value);

// ********** PowFusion **********
PrimitivePtr MindIR_PowFusion_CreatePrimitive(float scale, float shift);
float MindIR_PowFusion_GetScale(ConstPrimitivePtr primitive);
void MindIR_PowFusion_SetScale(PrimitivePtr *primitive, float scale);
float MindIR_PowFusion_GetShift(ConstPrimitivePtr primitive);
void MindIR_PowFusion_SetShift(PrimitivePtr *primitive, float shift);

// ********** PReLUFusion **********
PrimitivePtr MindIR_PReLUFusion_CreatePrimitive(bool channel_shared);
bool MindIR_PReLUFusion_GetChannelShared(ConstPrimitivePtr primitive);
void MindIR_PReLUFusion_SetChannelShared(PrimitivePtr *primitive, bool channel_shared);

// ********** QuantDTypeCast **********
PrimitivePtr MindIR_QuantDTypeCast_CreatePrimitive(int64_t src_t, int64_t dst_t);
int64_t MindIR_QuantDTypeCast_GetSrcT(ConstPrimitivePtr primitive);
void MindIR_QuantDTypeCast_SetSrcT(PrimitivePtr *primitive, int64_t src_t);
int64_t MindIR_QuantDTypeCast_GetDstT(ConstPrimitivePtr primitive);
void MindIR_QuantDTypeCast_SetDstT(PrimitivePtr *primitive, int64_t dst_t);

// ********** ReduceFusion **********
PrimitivePtr MindIR_ReduceFusion_CreatePrimitive(bool keep_dims, ReduceMode mode, bool reduce_to_end, float coeff);
bool MindIR_ReduceFusion_GetKeepDims(ConstPrimitivePtr primitive);
void MindIR_ReduceFusion_SetKeepDims(PrimitivePtr *primitive, bool keep_dims);
ReduceMode MindIR_ReduceFusion_GetMode(ConstPrimitivePtr primitive);
void MindIR_ReduceFusion_SetMode(PrimitivePtr *primitive, ReduceMode mode);
bool MindIR_ReduceFusion_GetReduceToEnd(ConstPrimitivePtr primitive);
void MindIR_ReduceFusion_SetReduceToEnd(PrimitivePtr *primitive, bool reduce_to_end);
float MindIR_ReduceFusion_GetCoeff(ConstPrimitivePtr primitive);
void MindIR_ReduceFusion_SetCoeff(PrimitivePtr *primitive, float coeff);

// ********** Reshape **********
PrimitivePtr MindIR_Reshape_CreatePrimitive();

// ********** Resize **********
PrimitivePtr MindIR_Resize_CreatePrimitive(ResizeMethod method, int64_t new_height, int64_t new_width,
                                           bool preserve_aspect_ratio,
                                           CoordinateTransformMode coordinate_transform_mode, float cubic_coeff,
                                           int64_t exclude_outside, float extrapolation_value,
                                           NearestMode nearest_mode);
ResizeMethod MindIR_Resize_GetMethod(ConstPrimitivePtr primitive);
void MindIR_Resize_SetMethod(PrimitivePtr *primitive, ResizeMethod method);
int64_t MindIR_Resize_GetNewHeight(ConstPrimitivePtr primitive);
void MindIR_Resize_SetNewHeight(PrimitivePtr *primitive, int64_t new_height);
int64_t MindIR_Resize_GetNewWidth(ConstPrimitivePtr primitive);
void MindIR_Resize_SetNewWidth(PrimitivePtr *primitive, int64_t new_width);
bool MindIR_Resize_GetPreserveAspectRatio(ConstPrimitivePtr primitive);
void MindIR_Resize_SetPreserveAspectRatio(PrimitivePtr *primitive, bool preserve_aspect_ratio);
CoordinateTransformMode MindIR_Resize_GetCoordinateTransformMode(ConstPrimitivePtr primitive);
void MindIR_Resize_SetCoordinateTransformMode(PrimitivePtr *primitive,
                                              CoordinateTransformMode coordinate_transform_mode);
float MindIR_Resize_GetCubicCoeff(ConstPrimitivePtr primitive);
void MindIR_Resize_SetCubicCoeff(PrimitivePtr *primitive, float cubic_coeff);
int64_t MindIR_Resize_GetExcludeOutside(ConstPrimitivePtr primitive);
void MindIR_Resize_SetExcludeOutside(PrimitivePtr *primitive, int64_t exclude_outside);
float MindIR_Resize_GetExtrapolationValue(ConstPrimitivePtr primitive);
void MindIR_Resize_SetExtrapolationValue(PrimitivePtr *primitive, float extrapolation_value);
NearestMode MindIR_Resize_GetNearestMode(ConstPrimitivePtr primitive);
void MindIR_Resize_SetNearestMode(PrimitivePtr *primitive, NearestMode nearest_mode);

// ********** Rsqrt **********
PrimitivePtr MindIR_Rsqrt_CreatePrimitive();

// ********** ScaleFusion **********
PrimitivePtr MindIR_ScaleFusion_CreatePrimitive(int64_t axis, ActivationType activation_type);
int64_t MindIR_ScaleFusion_GetAxis(ConstPrimitivePtr primitive);
void MindIR_ScaleFusion_SetAxis(PrimitivePtr *primitive, int64_t axis);
ActivationType MindIR_ScaleFusion_GetActivationType(ConstPrimitivePtr primitive);
void MindIR_ScaleFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type);

// ********** Shape **********
PrimitivePtr MindIR_Shape_CreatePrimitive();

// ********** SliceFusion **********
PrimitivePtr MindIR_SliceFusion_CreatePrimitive(const std::vector<int64_t> &axes);
std::vector<int64_t> MindIR_SliceFusion_GetAxes(ConstPrimitivePtr primitive);
void MindIR_SliceFusion_SetAxes(PrimitivePtr *primitive, const std::vector<int64_t> &axes);

// ********** Softmax **********
PrimitivePtr MindIR_Softmax_CreatePrimitive(const std::vector<int64_t> &axis);
std::vector<int64_t> MindIR_Softmax_GetAxis(ConstPrimitivePtr primitive);
void MindIR_Softmax_SetAxis(PrimitivePtr *primitive, const std::vector<int64_t> &axis);

// ********** SpaceToBatchND **********
PrimitivePtr MindIR_SpaceToBatchND_CreatePrimitive(const std::vector<int64_t> &block_shape,
                                                   const std::vector<std::vector<int64_t>> &paddings);
std::vector<int64_t> MindIR_SpaceToBatchND_GetBlockShape(ConstPrimitivePtr primitive);
void MindIR_SpaceToBatchND_SetBlockShape(PrimitivePtr *primitive, const std::vector<int64_t> &block_shape);
std::vector<std::vector<int64_t>> MindIR_SpaceToBatchND_GetPaddings(ConstPrimitivePtr primitive);
void MindIR_SpaceToBatchND_SetPaddings(PrimitivePtr *primitive, const std::vector<std::vector<int64_t>> &paddings);

// ********** Split **********
PrimitivePtr MindIR_Split_CreatePrimitive(int64_t output_num, const std::vector<int64_t> &size_splits, int64_t axis);
int64_t MindIR_Split_GetOutputNum(ConstPrimitivePtr primitive);
void MindIR_Split_SetOutputNum(PrimitivePtr *primitive, int64_t output_num);
std::vector<int64_t> MindIR_Split_GetSizeSplits(ConstPrimitivePtr primitive);
void MindIR_Split_SetSizeSplits(PrimitivePtr *primitive, const std::vector<int64_t> &size_splits);
int64_t MindIR_Split_GetAxis(ConstPrimitivePtr primitive);
void MindIR_Split_SetAxis(PrimitivePtr *primitive, int64_t axis);

// ********** Sqrt **********
PrimitivePtr MindIR_Sqrt_CreatePrimitive();

// ********** SquaredDifference **********
PrimitivePtr MindIR_SquaredDifference_CreatePrimitive();

// ********** Squeeze **********
PrimitivePtr MindIR_Squeeze_CreatePrimitive(const std::vector<int64_t> &axis);
std::vector<int64_t> MindIR_Squeeze_GetAxis(ConstPrimitivePtr primitive);
void MindIR_Squeeze_SetAxis(PrimitivePtr *primitive, const std::vector<int64_t> &axis);

// ********** Stack **********
PrimitivePtr MindIR_Stack_CreatePrimitive(int64_t axis);
int64_t MindIR_Stack_GetAxis(ConstPrimitivePtr primitive);
void MindIR_Stack_SetAxis(PrimitivePtr *primitive, int64_t axis);

// ********** StridedSlice **********
PrimitivePtr MindIR_StridedSlice_CreatePrimitive(int64_t begin_mask, int64_t end_mask, int64_t ellipsis_mask,
                                                 int64_t new_axis_mask, int64_t shrink_axis_mask);
int64_t MindIR_StridedSlice_GetBeginMask(ConstPrimitivePtr primitive);
void MindIR_StridedSlice_SetBeginMask(PrimitivePtr *primitive, int64_t begin_mask);
int64_t MindIR_StridedSlice_GetEndMask(ConstPrimitivePtr primitive);
void MindIR_StridedSlice_SetEndMask(PrimitivePtr *primitive, int64_t end_mask);
int64_t MindIR_StridedSlice_GetEllipsisMask(ConstPrimitivePtr primitive);
void MindIR_StridedSlice_SetEllipsisMask(PrimitivePtr *primitive, int64_t ellipsis_mask);
int64_t MindIR_StridedSlice_GetNewAxisMask(ConstPrimitivePtr primitive);
void MindIR_StridedSlice_SetNewAxisMask(PrimitivePtr *primitive, int64_t new_axis_mask);
int64_t MindIR_StridedSlice_GetShrinkAxisMask(ConstPrimitivePtr primitive);
void MindIR_StridedSlice_SetShrinkAxisMask(PrimitivePtr *primitive, int64_t shrink_axis_mask);

// ********** SubFusion **********
PrimitivePtr MindIR_SubFusion_CreatePrimitive(ActivationType activation_type);
ActivationType MindIR_SubFusion_GetActivationType(ConstPrimitivePtr primitive);
void MindIR_SubFusion_SetActivationType(PrimitivePtr *primitive, ActivationType activation_type);

// ********** TileFusion **********
PrimitivePtr MindIR_TileFusion_CreatePrimitive(const std::vector<int64_t> &dims);
std::vector<int64_t> MindIR_TileFusion_GetDims(ConstPrimitivePtr primitive);
void MindIR_TileFusion_SetDims(PrimitivePtr *primitive, const std::vector<int64_t> &dims);

// ********** TopKFusion **********
PrimitivePtr MindIR_TopKFusion_CreatePrimitive(bool sorted, int64_t axis);
bool MindIR_TopKFusion_GetSorted(ConstPrimitivePtr primitive);
void MindIR_TopKFusion_SetSorted(PrimitivePtr *primitive, bool sorted);
int64_t MindIR_TopKFusion_GetAxis(ConstPrimitivePtr primitive);
void MindIR_TopKFusion_SetAxis(PrimitivePtr *primitive, int64_t axis);

// ********** Transpose **********
PrimitivePtr MindIR_Transpose_CreatePrimitive();

// ********** Unsqueeze **********
PrimitivePtr MindIR_Unsqueeze_CreatePrimitive(const std::vector<int64_t> &axis);
std::vector<int64_t> MindIR_Unsqueeze_GetAxis(ConstPrimitivePtr primitive);
void MindIR_Unsqueeze_SetAxis(PrimitivePtr *primitive, const std::vector<int64_t> &axis);

}  // namespace lite
}  // namespace mindspore
#endif
