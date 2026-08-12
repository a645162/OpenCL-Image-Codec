# JPEG 模块：OpenCL 混合编解码（Huffman 熵编解码 CPU + DCT/颜色变换 GPU）。
# 阶段2 实现；阶段3 通过 onevpl 子目录接入硬件编解码。

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/onevpl/CMakeLists.txt")
  add_subdirectory(onevpl)
endif()

# ---- 内嵌 OpenCL kernel 源码：file(READ) + configure_file 生成 C 字符串 ----
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/kernels/jpeg_dec.cl" JPEG_DEC_SOURCE)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/kernels/jpeg_enc.cl" JPEG_ENC_SOURCE)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/kernels/cl_strings.cpp.in"
               "${CMAKE_CURRENT_BINARY_DIR}/jpeg_cl_strings.cpp" @ONLY)

add_library(OpenCLImageCodec.Jpeg STATIC
  jpeg_bitstream.h
  jpeg_bitstream.cpp
  jpeg_huffman.h
  jpeg_huffman.cpp
  jpeg_dct_ocl.h
  jpeg_dct_ocl.cpp
  jpeg_codec.h
  jpeg_codec.cpp
  bmp_io.h
  bmp_io.cpp
  jpeg_test.h
  jpeg_test.cpp
  kernels/jpeg_dec.cl
  kernels/jpeg_enc.cl
  "${CMAKE_CURRENT_BINARY_DIR}/jpeg_cl_strings.cpp")
target_include_directories(OpenCLImageCodec.Jpeg PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(OpenCLImageCodec.Jpeg PUBLIC
  OpenCLImageCodec.OpenCL
  OpenCLImageCodec.Platform)

# 可选自测可执行（默认关闭；供本模块独立验证）。
option(OIC_BUILD_JPEG_SELFTEST "Build JPEG self-test executable" OFF)
if(OIC_BUILD_JPEG_SELFTEST)
  add_executable(oic_jpeg_selftest selftest/selftest.cpp)
  target_link_libraries(oic_jpeg_selftest PRIVATE OpenCLImageCodec.Jpeg)
endif()
