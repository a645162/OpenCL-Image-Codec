# TIFF 模块：容器读写 + LZW 编码(GPU)/解码(CPU)。

# ---- 内嵌 kernel 源码生成 C++ 字符串（file(READ) + configure_file）----
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/lzw_encode.cl" LZW_ENCODE_CL_STRING)
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/lzw_encode_cl.cpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/lzw_encode_cl.cpp"
  @ONLY)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/lzw_encode_tile.cl" LZW_ENCODE_TILE_CL_STRING)
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/lzw_encode_tile_cl.cpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/lzw_encode_tile_cl.cpp"
  @ONLY)
# LZW 解码 GPU 探索 kernel（段级并行；见 lzw_decode_ocl.h 探索结论）。
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/kernels/lzw_decode.cl" LZW_DECODE_CL_STRING)
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/lzw_decode_cl.cpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/lzw_decode_cl.cpp"
  @ONLY)

add_library(OpenCLImageCodec.Tiff STATIC
  tiff_container.h
  tiff_container.cpp
  lzw_encode_ocl.h
  lzw_encode_ocl.cpp
  lzw_decode_cpu.h
  lzw_decode_cpu.cpp
  lzw_decode_ocl.h
  lzw_decode_ocl.cpp
  lzw_codec.h
  lzw_codec.cpp
  bmp_io.h
  bmp_io.cpp
  tiff_test.h
  tiff_test.cpp
  "${CMAKE_CURRENT_BINARY_DIR}/lzw_encode_cl.cpp"
  "${CMAKE_CURRENT_BINARY_DIR}/lzw_encode_tile_cl.cpp"
  "${CMAKE_CURRENT_BINARY_DIR}/lzw_decode_cl.cpp")

target_include_directories(OpenCLImageCodec.Tiff PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(OpenCLImageCodec.Tiff PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(OpenCLImageCodec.Tiff PUBLIC
  OpenCLImageCodec.Core
  OpenCLImageCodec.Core)

# 命名空间别名：target_link_libraries(app PRIVATE OpenCLImageCodec::Tiff)
add_library(OpenCLImageCodec::Tiff ALIAS OpenCLImageCodec.Tiff)
