// tiff/bmp_io.h - 24-bit BMP 读写（内部 top-down BGR 表示）。
#pragma once

#include <cstdint>
#include <vector>

namespace oic {
namespace tiff {

struct BmpImage {
  int width = 0;
  int height = 0;              // 绝对值（正数）
  std::vector<uint8_t> bgr;    // top-down BGR24（width*3 字节/行）
  bool bottom_up = true;       // 原始 BMP 的存储方向（bottom-up 标准）
};

// 读取 24-bit BMP。成功返回 0；失败返回负值。
// 不支持的格式（非 24-bit / 压缩 / 位图掩码等）返回 -2。
int bmpLoad(const char* path, BmpImage& img);

// 写出 24-bit BMP（标准 bottom-up，BITMAPFILEHEADER+INFOHEADER+BGR 像素）。
// 成功返回 0。
int bmpSave(const char* path, const BmpImage& img);

}  // namespace tiff
}  // namespace oic
