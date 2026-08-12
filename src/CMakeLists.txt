# src/ 下为多个独立静态库项目（参照主项目 src/libs 组织），用户可按需链接。
# 每个库提供命名空间别名，例如：
#   target_link_libraries(app PRIVATE OpenCLImageCodec::Tiff)   # 仅 TIFF（PUBLIC 传播 Core）
#   target_link_libraries(app PRIVATE OpenCLImageCodec::Jpeg)   # 仅 JPEG（含 Core）
# 依赖（PUBLIC 传播）：Tiff→Core；Jpeg→Core；OneVpl→Jpeg→Core；Api→Core+Tiff+Jpeg+OneVpl
add_subdirectory(libs)   # libs/ 下 5 个独立静态库（见 libs/CMakeLists.txt，用户可按需链接）

# 全量聚合便捷入口（可选）：一次链接全部库。不需要全部时可省略。
add_library(OpenCLImageCodec INTERFACE)
target_include_directories(OpenCLImageCodec INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>")
target_link_libraries(OpenCLImageCodec INTERFACE
  OpenCLImageCodec::Core
  OpenCLImageCodec::Tiff
  OpenCLImageCodec::Jpeg
  OpenCLImageCodec::Jpeg::OneVpl
  OpenCLImageCodec::Api)
add_library(OpenCLImageCodec::OpenCLImageCodec ALIAS OpenCLImageCodec)
