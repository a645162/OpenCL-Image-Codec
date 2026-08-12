add_library(OpenCLImageCodec.Platform STATIC
  oic_platform.h
  win_platform.cpp)
target_include_directories(OpenCLImageCodec.Platform PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}")
