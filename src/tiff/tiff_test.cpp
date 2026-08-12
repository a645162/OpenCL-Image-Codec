// tiff/tiff_test.cpp - CLI 测试入口实现（BMP <-> TIFF）。
#include "tiff_test.h"

#include "bmp_io.h"
#include "lzw_codec.h"
#include "tiff_container.h"

#include <cstdio>

namespace {

// BGR <-> RGB 原地互换。
void swapChannels(std::vector<uint8_t>& px) {
  const size_t n = px.size() - (px.size() % 3);
  for (size_t i = 0; i + 2 < n; i += 3) {
    uint8_t t = px[i];
    px[i] = px[i + 2];
    px[i + 2] = t;
  }
}

}  // namespace

int oic_tiff_info(const char* path) {
  if (path == nullptr) return -1;
  oic::tiff::TiffReader r;
  if (!r.openFile(path)) {
    std::fprintf(stderr, "oic_tiff_info: 无法解析 TIFF 文件: %s\n", path);
    return -2;
  }
  std::printf("TIFF info: %s\n", path);
  r.printInfo(stdout);
  return 0;
}

int oic_tiff_decode(const char* in, const char* out_bmp, int backend) {
  (void)backend;  // 解码统一走 CPU；参数保留以便 CLI 语义一致。
  if (in == nullptr || out_bmp == nullptr) return -1;
  int w = 0, h = 0;
  std::vector<uint8_t> rgb;
  const int rc = oic::tiff::tiffDecodeFromFile(in, w, h, rgb);
  if (rc != 0) {
    std::fprintf(stderr, "oic_tiff_decode: TIFF 解码失败 (%d)\n", rc);
    return rc;
  }
  oic::tiff::BmpImage img;
  img.width = w;
  img.height = h;
  img.bottom_up = true;
  img.bgr = rgb;  // RGB -> BGR
  swapChannels(img.bgr);
  const int rc2 = oic::tiff::bmpSave(out_bmp, img);
  if (rc2 != 0) {
    std::fprintf(stderr, "oic_tiff_decode: BMP 写出失败 (%d)\n", rc2);
    return rc2;
  }
  return 0;
}

int oic_tiff_encode(const char* in_bmp, const char* out, int backend,
                    int compression, int rows_per_strip) {
  if (in_bmp == nullptr || out == nullptr) return -1;
  if (compression != 1 && compression != 5) {
    std::fprintf(stderr, "oic_tiff_encode: 不支持的压缩 %d（1=none, 5=LZW）\n",
                 compression);
    return -2;
  }
  oic::tiff::BmpImage img;
  const int rc = oic::tiff::bmpLoad(in_bmp, img);
  if (rc != 0) {
    std::fprintf(stderr, "oic_tiff_encode: BMP 读取失败 (%d): %s\n", rc, in_bmp);
    return rc;
  }
  if (rows_per_strip <= 0) rows_per_strip = img.height;
  // BGR -> RGB
  swapChannels(img.bgr);
  const int rc2 = oic::tiff::tiffEncodeToFile(
      out, img.width, img.height, img.bgr.data(), compression, rows_per_strip,
      backend);
  if (rc2 != 0) {
    std::fprintf(stderr, "oic_tiff_encode: TIFF 编码失败 (%d): %s\n", rc2, out);
    return rc2;
  }
  return 0;
}
