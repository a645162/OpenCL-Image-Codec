add_executable(oic main.cpp)
target_link_libraries(oic PRIVATE
  OpenCLImageCodec::OpenCLImageCodec)
