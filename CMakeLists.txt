# Copyright (c) 2020 Wind River Systems, Inc. All Rights Reserved.
#
cmake_minimum_required(VERSION 3.10.2)
project(dkm)
if(CODE_COVERAGE)
    LINK_LIBRARIES(${WIND_SDK_PATH}/vxsdk/sysroot/usr/lib/common/libllvm_cov.a)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs  -ftest-coverage -mllvm -gcov-const-priority=50")
endif(CODE_COVERAGE)
set(CMAKE_EXECUTABLE_SUFFIX ".out")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
aux_source_directory("src" DIR_SRCS)
add_executable (dkm ${DIR_SRCS} )
target_include_directories(dkm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)