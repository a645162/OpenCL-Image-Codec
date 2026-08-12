// src/jpeg/jpeg_test.h - JPEG CLI 测试入口（供上层集成，签名固定，C 链接）。
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// 打印 JPEG 文件信息：尺寸/分量/子采样/量化表/重启间隔。成功返回 0。
int oic_jpeg_info(const char* path);

// 解码 JPEG -> 24-bit BMP。backend: 0=OpenCL。成功返回 0。
int oic_jpeg_decode(const char* in, const char* out_bmp, int backend);

// 编码 24-bit BMP -> JPEG。quality 1..100。backend: 0=OpenCL。成功返回 0。
int oic_jpeg_encode(const char* in_bmp, const char* out, int backend, int quality);

#ifdef __cplusplus
}
#endif
