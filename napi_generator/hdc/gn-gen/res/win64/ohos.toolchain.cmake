set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_CXX_COMPILER_ID Clang)
SET (CMAKE_SYSTEM_PROCESSOR arm)

SET ( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET ( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET ( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET ( CROSS_COMPILATION_ARCHITECTURE armv7-a)

set(CMAKE_TOOLCHAIN_PREFIX llvm-)

set(PROJECT_ROOT CC_REPLACE_OHOS_ROOT)

#指定c编译工具（确保工具链所在路径已经添加到了PATH环境变量中）和编译标志，使用clang编译时标志中必须指定--target，否则无法交叉编译。
set(CMAKE_C_COMPILER ${PROJECT_ROOT}/prebuilts/clang/ohos/windows-x86_64/llvm/bin/clang.exe)
set(CMAKE_C_FLAGS "--target=arm-linux-ohosmusl -D__clang__ -march=armv7-a  -mfloat-abi=softfp -mfpu=neon-vfpv4 -w")
#指定c++编译工具（确保工具链所在路径已经添加到了PATH环境变量中）和编译标志，必须指定--target，否则无法交叉编译。
set(CMAKE_CXX_COMPILER ${PROJECT_ROOT}/prebuilts/clang/ohos/windows-x86_64/llvm/bin/clang++.exe) 
set(CMAKE_CXX_FLAGS "--target=arm-linux-ohosmusl -D__clang__ -march=armv7-a  -mfloat-abi=softfp -mfpu=neon-vfpv4 -w")
#指定链接工具和链接标志，必须指定--target和--sysroot，其中OHOS_ROOT_PATH可通过cmake命令后缀参数来指定。
set(MY_LINK_FLAGS "--target=arm-linux-ohosmusl --sysroot=${PROJECT_ROOT}/CC_REPLACE_OHOS_TARGET/obj/third_party/musl/")
set(CMAKE_LINKER ${PROJECT_ROOT}/prebuilts/clang/ohos/windows-x86_64/llvm/bin/clang.exe)
set(CMAKE_CXX_LINKER ${PROJECT_ROOT}/prebuilts/clang/ohos/windows-x86_64/llvm/bin/clang++.exe)
set(CMAKE_C_LINKER ${PROJECT_ROOT}/prebuilts/clang/ohos/windows-x86_64/llvm/bin/clang.exe)

#指定链接库的查找路径。
set(CMAKE_SYSROOT ${PROJECT_ROOT}/CC_REPLACE_OHOS_TARGET/obj/third_party/musl/)
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--dynamic-linker,/lib/ld-musl-arm.so.1")
