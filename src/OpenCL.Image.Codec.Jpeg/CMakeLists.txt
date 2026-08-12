# JPEG 模块：OpenCL 混合编解码（Huffman 熵编解码 CPU + DCT/颜色变换 GPU）。
# 硬件后端见平级模块 OpenCL.Image.Codec.OneVpl（oneVPL/libvpl.dll）。

# ---- 内嵌 OpenCL kernel 源码：file(READ) + configure_file 生成 C 字符串 ----
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/src/kernels/jpeg_dec.cl" JPEG_DEC_SOURCE)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/src/kernels/jpeg_enc.cl" JPEG_ENC_SOURCE)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/kernels/cl_strings.cpp.in"
               "${CMAKE_CURRENT_BINARY_DIR}/jpeg_cl_strings.cpp" @ONLY)

add_library(OpenCLImageCodec.Jpeg STATIC
  src/jpeg_bitstream.h
  src/jpeg_bitstream.cpp
  src/jpeg_huffman.h
  src/jpeg_huffman.cpp
  src/jpeg_dct_ocl.h
  src/jpeg_dct_ocl.cpp
  src/jpeg_codec.h
  src/jpeg_codec.cpp
  src/bmp_io.h
  src/bmp_io.cpp
  src/jpeg_test.h
  src/jpeg_test.cpp
  src/kernels/jpeg_dec.cl
  src/kernels/jpeg_enc.cl
  "${CMAKE_CURRENT_BINARY_DIR}/jpeg_cl_strings.cpp")
target_include_directories(OpenCLImageCodec.Jpeg PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(OpenCLImageCodec.Jpeg PUBLIC
  OpenCLImageCodec.Core)

# 命名空间别名：target_link_libraries(app PRIVATE OpenCLImageCodec::Jpeg)
add_library(OpenCLImageCodec::Jpeg ALIAS OpenCLImageCodec.Jpeg)

# 可选自测可执行（默认关闭；供本模块独立验证）。
option(OIC_BUILD_JPEG_SELFTEST "Build JPEG self-test executable" OFF)
if(OIC_BUILD_JPEG_SELFTEST)
  add_executable(oic_jpeg_selftest src/selftest/selftest.cpp)
  target_link_libraries(oic_jpeg_selftest PRIVATE OpenCLImageCodec.Jpeg)
endif()
