// src/jpeg/bmp_io.h - 24-bit BMP 读写（内部自实现，供测试/编解码使用）。
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace oic {
namespace jpeg {

// 读取 24-bit BMP。rgb 为 top-down 行序（width*height*3）。成功返回 0。
int bmp_read(const std::string& path, std::vector<uint8_t>* rgb, int* width,
             int* height, std::string* err);

// 写出 24-bit BMP（bottom-up 行序，4 字节对齐）。成功返回 0。
int bmp_write(const std::string& path, const uint8_t* rgb, int width, int height,
              std::string* err);

}  // namespace jpeg
}  // namespace oic
