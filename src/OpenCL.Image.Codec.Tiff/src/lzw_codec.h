// tiff/lzw_codec.h - 统一编解码入口（encode 选 GPU/CPU，decode 选 CPU）。
#pragma once

#include <cstdint>
#include <vector>

namespace oic {
namespace tiff {

// 把 top-down RGB 图像（width*3 字节/行）写为 TIFF 文件。
// compression: 1=none, 5=LZW。backend: 0=OpenCL(GPU), 2=CPU。
// LZW 时 rows_per_strip 会被自动调整为 height 的最大 ≤rps 因子（保证 strip 等长）。
// 返回 0 成功，负值失败。
int tiffEncodeToFile(const char* path, int width, int height, const uint8_t* rgb,
                     int compression, int rows_per_strip, int backend);

// 读 TIFF 文件并解出 top-down RGB（仅支持 8-bit RGB 非平面）。
// 返回 0 成功，负值失败。
int tiffDecodeFromFile(const char* path, int& width, int& height,
                       std::vector<uint8_t>& rgb);

// CPU LZW 编码单段（供 backend=CPU 与测试复用；与 GPU kernel 逻辑一致）。
int cpuLzwEncode(const uint8_t* in, size_t in_size, std::vector<uint8_t>& out);

}  // namespace tiff
}  // namespace oic
