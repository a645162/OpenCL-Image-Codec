add_library(OpenCLImageCodec.OpenCL STATIC
  ocl_device.h
  ocl_device.cpp
  ocl_program.h
  ocl_program.cpp)
target_include_directories(OpenCLImageCodec.OpenCL PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(OpenCLImageCodec.OpenCL PUBLIC OpenCL::OpenCL)
