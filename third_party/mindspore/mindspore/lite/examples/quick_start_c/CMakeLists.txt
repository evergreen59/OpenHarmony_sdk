cmake_minimum_required(VERSION 3.14)
project(QuickStartC)

add_executable(demo main.c)

target_link_libraries(
        demo
        mindspore-lite.huawei
        pthread
        dl
)

