# 子项目组织：每个模块独立静态库，最后 INTERFACE 聚合为对外 target。
add_subdirectory(platform)
add_subdirectory(opencl)
add_subdirectory(tiff)
add_subdirectory(jpeg)

add_library(OpenCLImageCodec INTERFACE)
target_include_directories(OpenCLImageCodec INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>")
target_link_libraries(OpenCLImageCodec INTERFACE
  OpenCLImageCodec.Platform
  OpenCLImageCodec.OpenCL
  OpenCLImageCodec.Tiff
  OpenCLImageCodec.Jpeg)
add_library(OpenCLImageCodec::OpenCLImageCodec ALIAS OpenCLImageCodec)
