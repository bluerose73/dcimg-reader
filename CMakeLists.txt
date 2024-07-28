cmake_minimum_required(VERSION 3.10)

project(DCIMG_MEX VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

find_package(Matlab)

matlab_add_mex(NAME dcimg_get_size
               SRC dcimg_mex/mex_interface/dcimg_get_size.cpp
                   dcimg_mex/impl/dcimg_ops.cpp)
target_include_directories(dcimg_get_size PRIVATE .)
target_link_libraries(dcimg_get_size "${CMAKE_SOURCE_DIR}/dcimgapi/dcimgapi.lib")

matlab_add_mex(NAME dcimg_read_frame
               SRC dcimg_mex/mex_interface/dcimg_read_frame.cpp
                   dcimg_mex/impl/dcimg_ops.cpp)
target_include_directories(dcimg_read_frame PRIVATE .)
target_link_libraries(dcimg_read_frame "${CMAKE_SOURCE_DIR}/dcimgapi/dcimgapi.lib")

add_executable(test_read test/test_read.cpp dcimg_mex/impl/dcimg_ops.cpp)
target_include_directories(test_read PRIVATE .)
target_link_libraries(test_read "${CMAKE_SOURCE_DIR}/dcimgapi/dcimgapi.lib")

add_custom_command(
    TARGET dcimg_get_size POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/dcimgapi/dcimgapi.dll"
    "$<TARGET_FILE_DIR:dcimg_get_size>"
    COMMENT "Copying dcimgapi.dll to output directory"
)


find_package(JNI REQUIRED)

add_library(dcimg_jni SHARED fiji_plugin/src/main/cpp/io_bluerose73_fiji_dcimg_DcimgReader.cpp
                            dcimg_mex/impl/dcimg_ops.cpp)
target_include_directories(dcimg_jni PRIVATE . "${JNI_INCLUDE_DIRS}")
target_link_libraries(dcimg_jni "${CMAKE_SOURCE_DIR}/dcimgapi/dcimgapi.lib")