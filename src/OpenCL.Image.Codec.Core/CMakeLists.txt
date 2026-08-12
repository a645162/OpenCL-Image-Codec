# 核心基础设施：平台抽象（exe_dir/计时/休眠）+ OpenCL 设备/上下文/kernel 构建。
add_library(OpenCLImageCodec.Core STATIC
  src/oic_platform.h
  src/win_platform.cpp
  src/ocl_device.h
  src/ocl_device.cpp
  src/ocl_program.h
  src/ocl_program.cpp)
target_include_directories(OpenCLImageCodec.Core PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(OpenCLImageCodec.Core PUBLIC OpenCL::OpenCL)

# 命名空间别名：target_link_libraries(app PRIVATE OpenCLImageCodec::Core)
add_library(OpenCLImageCodec::Core ALIAS OpenCLImageCodec.Core)
