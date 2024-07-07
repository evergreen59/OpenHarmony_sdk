#set(CMAKE_TOOLCHAIN_FILE /heaven/wty/devtools/ohos_sdk/native/build/cmake/ohos.toolchain.cmake)
set(OHOS_ARCH arm64-v8a)
set(OHOS_STL c++_static)
set(OHOS_PLATFORM rk3568)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
project(mindir)
cmake_minimum_required(VERSION 3.18)

file(GLOB source src/*.cc)
file(GLOB convert_source src/converter/*.cpp)
set(mindir_source ../src/common/log.cc)
include_directories(include)
include_directories(inner_headers)
include_directories(../)
include_directories(../../../../../out/rk3568/gen/drivers/interface)
include_directories(../../../../../third_party/flatbuffers/include)
include_directories(../../core)

include_directories(../../foundation/communication/ipc/interfaces/innerkits/ipc_core/include)
include_directories(../../../../../utils/native/base/include)
add_compile_definitions(MINDIR_INTERFACE)
add_library(mindir SHARED ${source} ${convert_source} ${mindir_source})
target_link_libraries(mindir ../../../../../out/rk3568/hdf/drivers_interface_nnrt/libnnrt_proxy_1.0.z.so
        ../../../../../out/rk3568/commonlibrary/c_utils/libutils.z.so
        hilog_ndk.z
        ../../../../../out/rk3568/communication/ipc/libipc_core.z.so
        )
file(GLOB test_sources tests/*.cc)

add_executable(mindir_test ${test_sources})
target_link_libraries(mindir_test mindir)
