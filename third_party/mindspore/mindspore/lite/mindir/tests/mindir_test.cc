#include "mindir.h"
#include "mindir_memory_manager.h"
#include <iostream>
#include <string>
using namespace mindspore::lite;
int main() {
  int loop = 0;
  int all = 100;
  while (loop < all) {
    auto tensor = MindIR_Tensor_Create();
    auto str = MindIR_Tensor_GetName(tensor);
    DataType dataType = DATA_TYPE_UINT16;
    MindIR_Tensor_SetDataType(&tensor, dataType);
    std::cout << "set data type" << std::endl;
    DataType d = MindIR_Tensor_GetDataType(tensor);
    std::cout << "loop = " << loop << ",data type = " << (int)d << std::endl;
    PrimitivePtr ret = nullptr;
    ret = MindIR_SquaredDifference_CreatePrimitive();
    std::cout << "MindIR_SquaredDifference_CreatePrimitive 1,PrimitivePtr = " << ret << std::endl;
    ret = MindIR_SquaredDifference_CreatePrimitive();
    std::cout << "MindIR_SquaredDifference_CreatePrimitive 1,PrimitivePtr = " << ret << std::endl;
    ret = MindIR_SubFusion_CreatePrimitive(ACTIVATION_TYPE_RELU6);
    std::cout << "MindIR_SubFusion_CreatePrimitive 2,PrimitivePtr = " << ret << std::endl;
    ret = MindIR_Activation_CreatePrimitive(ACTIVATION_TYPE_RELU6, .5, 0, 1, true);
    std::cout << "MindIR_Activation_CreatePrimitive 3,PrimitivePtr = " << ret << std::endl;
    MindIR_Primitive_Destroy(&ret);
    std::cout << "MindIR_Primitive_Destroy,PrimitivePtr = " << ret << std::endl;
    TensorPtr t_ret = nullptr;
    t_ret = MindIR_Tensor_Create();
    std::cout << "MindIR_Tensor_Create 3,TensorPtr = " << t_ret << std::endl;
    MindIR_Tensor_Destroy(&t_ret);
    std::cout << "MindIR_Tensor_Destroy,Tensor = " << t_ret << std::endl;
    ret = MindIR_SpaceToBatchND_CreatePrimitive({2, 2}, {{0}, {0}, {0}, {0}});
    auto blockshape = MindIR_SpaceToBatchND_GetBlockShape(ret);
    std::string bs_("");
    for (int i = 0; i < 2; i++) {
      bs_.append(std::to_string(blockshape[i]).c_str());
    }
    std::cout << "MindIR_SpaceToBatchND_GetBlockShape,blockshape = " << bs_ << std::endl;
    auto paddings = MindIR_SpaceToBatchND_GetPaddings(ret);
    std::string pad_("");
    for (auto item : paddings) {
      pad_.append(std::to_string(item[0]).c_str());
    }
    std::cout << "MindIR_SpaceToBatchND_GetPaddings,Paddings = " << pad_ << std::endl;
    loop++;
  }
  MindIRMemoryManager::GetInstance()->ClearAllMemory();
  std::cout << "MindIRMemoryManager::GetInstance()->ClearAllMemory()" << std::endl;
  loop++;
}