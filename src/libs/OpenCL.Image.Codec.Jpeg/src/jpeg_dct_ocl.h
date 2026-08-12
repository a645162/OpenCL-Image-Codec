// src/jpeg/jpeg_dct_ocl.h - GPU DCT/颜色变换（OpenCL）与纯 CPU 参考路径。
// 解码：量化系数 -> GPU(反量化+IDCT+YCbCr->RGB+上采样) -> RGB。
// 编码：RGB -> GPU(RGB->YCbCr+下采样+FDCT+量化) -> 量化系数。
#pragma once

#include "jpeg_bitstream.h"

#include <cstdint>
#include <string>
#include <vector>

namespace oic {
namespace jpeg {

// 内嵌 kernel 源码（由 CMake 生成的 jpeg_cl_strings.cpp 提供）。
extern const char kJpegDecSource[];
extern const char kJpegEncSource[];

// GPU 解码：coeffs（自然序，布局同 img）-> RGB（top-down 行序，width*height*3）。
// 成功返回 0。需 OpenCL 设备可用。
int jpeg_gpu_decode(const JpegImage& img, const std::vector<float>& coeffs,
                    std::vector<uint8_t>* rgb, std::string* err);

// GPU 编码：RGB（top-down）-> 量化系数（自然序，布局同 img，float 整数）。
// 成功返回 0。
int jpeg_gpu_encode(const JpegImage& img, const uint8_t* rgb,
                    std::vector<float>* coeffs, std::string* err);

// 纯 CPU 解码（浮点 IDCT，验证 GPU 路径用；不依赖 OpenCL）。
int jpeg_cpu_decode(const JpegImage& img, const std::vector<float>& coeffs,
                    std::vector<uint8_t>* rgb, std::string* err);

}  // namespace jpeg
}  // namespace oic
