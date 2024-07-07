#ifndef OHOS_HDI_NNRT_V1_0_CPP_H
#define OHOS_HDI_NNRT_V1_0_CPP_H
#include "schema/model_generated.h"
#include "include/api/status.h"
namespace mindspore {
namespace lite {
Status CheckPrimitiveSupported(const schema::Primitive *primitive);
Status CheckTensorSupported(const schema::Tensor *primitive);
}  // namespace lite
}  // namespace mindspore

#endif  // OHOS_HDI_NNRT_V1_0_CPP_H