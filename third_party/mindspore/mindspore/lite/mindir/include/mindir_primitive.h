#ifndef MINDIR_MINDIR_PRIMITIVE_H
#define MINDIR_MINDIR_PRIMITIVE_H
#include "mindir_lite_graph.h"
#include "mindir_types.h"

namespace mindspore {
namespace lite {

// ********** PrimitiveBase **********
NodeType MindIR_Primitive_GetType(PrimitivePtr primitive);
void MindIR_Primitive_Destroy(PrimitivePtr *primitive);

}  // namespace lite
}  // namespace mindspore
#endif  // MINDIR_MINDIR_PRIMITIVE_H
