# 阶段0 占位；阶段2/3 填充 JPEG(OpenCL 混合 + oneVPL)实现。
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/onevpl/CMakeLists.txt")
  add_subdirectory(onevpl)
endif()
add_library(OpenCLImageCodec.Jpeg STATIC
  jpeg_stub.cpp)
target_include_directories(OpenCLImageCodec.Jpeg PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(OpenCLImageCodec.Jpeg PUBLIC
  OpenCLImageCodec.OpenCL
  OpenCLImageCodec.Platform)
