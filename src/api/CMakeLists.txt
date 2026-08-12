# 对外 C API（include/OpenCLImageCodec/oic.h / oic.hpp 的实现库）。
# 桥接各模块文件入口（oic_*_test.h）与内存 buffer。
add_library(OpenCLImageCodec.Api STATIC
  oic.cpp
  ../../include/OpenCLImageCodec/oic.h
  ../../include/OpenCLImageCodec/oic.hpp)

target_include_directories(OpenCLImageCodec.Api PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/../../include")

target_link_libraries(OpenCLImageCodec.Api PUBLIC
  OpenCLImageCodec.Core
  OpenCLImageCodec.Tiff
  OpenCLImageCodec.Jpeg
  OpenCLImageCodec.Jpeg.OneVpl)

# 构建本库时 OIC_API 取 dllexport 分支（共享库场景）；静态库下为空。
target_compile_definitions(OpenCLImageCodec.Api PRIVATE OIC_BUILDING_LIB)
