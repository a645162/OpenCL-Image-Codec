# 阶段0 占位；阶段1 填充 TIFF 编解码实现。
add_library(OpenCLImageCodec.Tiff STATIC
  tiff_stub.cpp)
target_include_directories(OpenCLImageCodec.Tiff PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(OpenCLImageCodec.Tiff PUBLIC
  OpenCLImageCodec.OpenCL
  OpenCLImageCodec.Platform)
