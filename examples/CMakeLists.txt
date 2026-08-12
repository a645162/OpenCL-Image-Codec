# 示例：纯 C 与 C++ 两种消费方式，均走对外 OpenCLImageCodec::OpenCLImageCodec。
add_executable(oic_example_c c_example.c)
target_link_libraries(oic_example_c PRIVATE OpenCLImageCodec::OpenCLImageCodec)
# 该可执行依赖 C++ 静态库，需用 C++ 链接器拉入 C++ 运行时。
set_target_properties(oic_example_c PROPERTIES
  C_STANDARD 11
  LINKER_LANGUAGE CXX)

add_executable(oic_example_cpp cpp_example.cpp)
target_link_libraries(oic_example_cpp PRIVATE OpenCLImageCodec::OpenCLImageCodec)
set_target_properties(oic_example_cpp PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON)
