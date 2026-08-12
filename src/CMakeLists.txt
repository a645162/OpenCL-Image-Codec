# src/ 下为多个独立静态库项目（参照主项目 src/libs 组织），用户可按需链接。
# 每个库提供命名空间别名，例如：
#   target_link_libraries(app PRIVATE OpenCLImageCodec::Tiff)   # 仅 TIFF（PUBLIC 传播 Core）
#   target_link_libraries(app PRIVATE OpenCLImageCodec::Jpeg)   # 仅 JPEG（含 Core）
# 依赖（PUBLIC 传播）：Tiff→Core；Jpeg→Core；OneVpl→Jpeg→Core；Api→Core+Tiff+Jpeg+OneVpl
add_subdirectory(OpenCL.Image.Codec.Core)     # OpenCLImageCodec::Core       平台抽象 + OpenCL 设备/上下文/kernel
add_subdirectory(OpenCL.Image.Codec.Tiff)     # OpenCLImageCodec::Tiff       TIFF 容器 + LZW 编码(GPU)/解码(CPU)
add_subdirectory(OpenCL.Image.Codec.Jpeg)     # OpenCLImageCodec::Jpeg       baseline JPEG 混合编解码（OpenCL）
add_subdirectory(OpenCL.Image.Codec.OneVpl)   # OpenCLImageCodec::Jpeg::OneVpl  oneVPL(libvpl.dll) 后端（依赖 Jpeg）
add_subdirectory(OpenCL.Image.Codec.Api)      # OpenCLImageCodec::Api        C API（oic.h）实现

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
