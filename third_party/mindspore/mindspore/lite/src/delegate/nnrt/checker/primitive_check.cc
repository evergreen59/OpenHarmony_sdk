#include <string>
#include <vector>
#include "primitive_check.h"
#include "dtype/type_id.h"
#include "src/weight_decoder.h"
#include "src/common/log.h"
#include "src/common/utils.h"
namespace mindspore {
namespace lite {

Status CheckPrimitiveSupported(const schema::Primitive *primitive) {
  if (primitive != nullptr) {
    auto prim = primitive;
    auto type = prim->value_type();
    switch (type) {
      case schema::PrimitiveType_Activation:
        return mindspore::kSuccess;
      case schema::PrimitiveType_AddFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_ArgMaxFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_AvgPoolFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_BatchToSpaceND:
        return mindspore::kSuccess;
      case schema::PrimitiveType_BiasAdd:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Cast:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Concat:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Conv2DFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Conv2dTransposeFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_DivFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Eltwise:
        return mindspore::kSuccess;
      case schema::PrimitiveType_ExpandDims:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Fill:
        return mindspore::kSuccess;
      case schema::PrimitiveType_FullConnection:
        return mindspore::kSuccess;
      case schema::PrimitiveType_FusedBatchNorm:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Gather:
        return mindspore::kSuccess;
      case schema::PrimitiveType_LayerNormFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_LessEqual:
        return mindspore::kSuccess;
      case schema::PrimitiveType_MatMulFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Maximum:
        return mindspore::kSuccess;
      case schema::PrimitiveType_MaxPoolFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_MulFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_OneHot:
        return mindspore::kSuccess;
      case schema::PrimitiveType_PadFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_PowFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_PReLUFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_QuantDTypeCast:
        return mindspore::kSuccess;
      case schema::PrimitiveType_ReduceFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Reshape:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Resize:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Rsqrt:
        return mindspore::kSuccess;
      case schema::PrimitiveType_ScaleFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Shape:
        return mindspore::kSuccess;
      case schema::PrimitiveType_SliceFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Softmax:
        return mindspore::kSuccess;
      case schema::PrimitiveType_SpaceToBatchND:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Split:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Sqrt:
        return mindspore::kSuccess;
      case schema::PrimitiveType_SquaredDifference:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Squeeze:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Stack:
        return mindspore::kSuccess;
      case schema::PrimitiveType_StridedSlice:
        return mindspore::kSuccess;
      case schema::PrimitiveType_SubFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_TileFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_TopKFusion:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Transpose:
        return mindspore::kSuccess;
      case schema::PrimitiveType_Unsqueeze:
        return mindspore::kSuccess;
      default: {
        MS_LOG(WARNING) << "No primitive type :" << (int)(type);
        return mindspore::kLiteSuccessExit;
      }
    }
    return mindspore::kSuccess;
  } else {
    MS_LOG(ERROR) << "primitive is nullptr.";
    return mindspore::kLiteError;
  }
}
namespace {
bool NeedBitUppackCheck(const schema::Tensor &src_tensor) {
  if (src_tensor.enableHuffmanCode()) {
    return true;
  }
  bool need_bit_unpack = src_tensor.quantParams() != nullptr && src_tensor.quantParams()->size() > 0 &&
                         src_tensor.quantParams()->Get(0) != nullptr;
  if (need_bit_unpack) {
    auto num_bits = src_tensor.quantParams()->Get(0)->numBits();
    need_bit_unpack = ((num_bits >= kBitNum1 && num_bits < kBitNum8) || (num_bits > kBitNum8 && num_bits < kBitNum16));
  }

  return need_bit_unpack;
}
int DecompressTensor(const schema::Tensor &src_tensor) {
  if (src_tensor.weightQunatCompressType() == schema::WeightQunatCompressType_FSE ||
      src_tensor.weightQunatCompressType() == schema::WeightQunatCompressType_INDEXING ||
      src_tensor.weightQunatCompressType() == schema::WeightQunatCompressType_SPARSE) {
    return RET_NOT_SUPPORT;
  }
  if (!NeedBitUppackCheck(src_tensor)) {
    return RET_NO_CHANGE;
  }
  MS_LOG(ERROR) << "DecompressTensor Error.";
  return RET_ERROR;
}
}  // namespace

Status CheckTensorSupported(const schema::Tensor *primitive) {
  if (primitive == nullptr) {
    MS_LOG(ERROR) << "primitive is nullptr, which type is Tensor.";
    return mindspore::kLiteSuccessExit;
  }

  int32_t data_type = primitive->dataType();
  if (data_type <= kTypeUnknown || data_type >= kMonadTypeEnd) {
    MS_LOG(ERROR) << "invalid data type. " << data_type;
    return mindspore::kLiteSuccessExit;
  }

  if (primitive->dims() == nullptr) {
    MS_LOG(DEBUG) << "Dims of tensor is nullptr";
  }

  if (data_type == kObjectTypeTensorType) {
    MS_LOG(ERROR) << "Not support TensorList.";
    return mindspore::kLiteNotSupport;
  }

  if (primitive->data() == nullptr || primitive->data()->size() <= 0) {
    MS_LOG(DEBUG) << "No valid data converted.";
    return mindspore::kSuccess;
  } else {
    auto ret = DecompressTensor(*primitive);
    if (ret == RET_NO_CHANGE) {
    } else {
      MS_LOG(ERROR) << "Not support Decompress Tensor.";
      return mindspore::kLiteNotSupport;
    }
  }
  return mindspore::kSuccess;
  ;
}
}  // namespace lite
}  // namespace mindspore
