cmake_minimum_required(VERSION 3.10)

project(DCIMG_MEX VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

find_package(Matlab)

matlab_add_mex(NAME dcimg_get_size
               SRC dcimg_mex/mex_interface/dcimg_get_size.cpp
                   dcimg_mex/impl/dcimg_ops.cpp)

target_include_directories(dcimg_get_size PRIVATE .)
target_link_libraries(dcimg_get_size "${CMAKE_SOURCE_DIR}/dcimgapi/dcimgapi.lib")