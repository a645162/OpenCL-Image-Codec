# 阶段3 占位；oneVPL(libvpl.dll 动态加载) JPEG 后端，仅 WIN32 编译。
add_library(OpenCLImageCodec.Jpeg.OneVpl STATIC
  onevpl_stub.cpp)
target_include_directories(OpenCLImageCodec.Jpeg.OneVpl PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(OpenCLImageCodec.Jpeg.OneVpl PUBLIC
  OpenCLImageCodec.Jpeg)
