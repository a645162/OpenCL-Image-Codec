// src/jpeg/bmp_io.cpp - 24-bit BMP 读写实现。
#include "bmp_io.h"

#include <cstdio>
#include <cstring>
#include <vector>

namespace oic {
namespace jpeg {

namespace {

struct U32LE {
  static uint32_t get(const uint8_t* p) {
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
           (static_cast<uint32_t>(p[2]) << 16) |
           (static_cast<uint32_t>(p[3]) << 24);
  }
  static void put(std::vector<uint8_t>* v, uint32_t x) {
    v->push_back(static_cast<uint8_t>(x & 0xFF));
    v->push_back(static_cast<uint8_t>((x >> 8) & 0xFF));
    v->push_back(static_cast<uint8_t>((x >> 16) & 0xFF));
    v->push_back(static_cast<uint8_t>((x >> 24) & 0xFF));
  }
};
struct U16LE {
  static uint16_t get(const uint8_t* p) {
    return static_cast<uint16_t>(p[0] | (p[1] << 8));
  }
  static void put(std::vector<uint8_t>* v, uint16_t x) {
    v->push_back(static_cast<uint8_t>(x & 0xFF));
    v->push_back(static_cast<uint8_t>((x >> 8) & 0xFF));
  }
};

}  // namespace

int bmp_read(const std::string& path, std::vector<uint8_t>* rgb, int* width,
             int* height, std::string* err) {
  FILE* f = std::fopen(path.c_str(), "rb");
  if (!f) {
    if (err) *err = "cannot open file: " + path;
    return -1;
  }
  std::vector<uint8_t> data;
  uint8_t buf[4096];
  size_t n;
  while ((n = std::fread(buf, 1, sizeof(buf), f)) > 0) {
    data.insert(data.end(), buf, buf + n);
  }
  std::fclose(f);
  if (data.size() < 54 || data[0] != 'B' || data[1] != 'M') {
    if (err) *err = "not a BMP file";
    return -1;
  }
  const uint32_t data_offset = U32LE::get(&data[10]);
  const uint32_t dib_size = U32LE::get(&data[14]);
  if (dib_size != 40 && dib_size != 12) {
    if (err) *err = "unsupported BMP DIB header size";
    return -1;
  }
  int w = 0, h = 0, bpp = 0;
  uint32_t compression = 0;
  if (dib_size == 40) {
    const int32_t iw = static_cast<int32_t>(U32LE::get(&data[18]));
    const int32_t ih = static_cast<int32_t>(U32LE::get(&data[22]));
    w = (iw < 0) ? -iw : iw;
    h = (ih < 0) ? -ih : ih;
    bpp = U16LE::get(&data[28]);
    compression = U32LE::get(&data[30]);
  } else {  // BITMAPCOREHEADER
    w = U16LE::get(&data[18]);
    h = U16LE::get(&data[20]);
    bpp = U16LE::get(&data[24]);
  }
  if (w <= 0 || h <= 0 || bpp != 24 || compression != 0) {
    if (err) *err = "only 24-bit uncompressed BMP supported";
    return -1;
  }
  const int row_stride = ((w * 3 + 3) / 4) * 4;
  if (data_offset + static_cast<uint64_t>(row_stride) * h > data.size()) {
    if (err) *err = "truncated BMP pixel data";
    return -1;
  }
  rgb->assign(static_cast<size_t>(w) * h * 3, 0);
  const bool top_down = (dib_size == 40) &&
                        (static_cast<int32_t>(U32LE::get(&data[22])) < 0);
  for (int y = 0; y < h; y++) {
    const int src_row = top_down ? y : (h - 1 - y);
    const uint8_t* src = &data[data_offset + static_cast<size_t>(src_row) * row_stride];
    uint8_t* dst = &(*rgb)[static_cast<size_t>(y) * w * 3];
    for (int x = 0; x < w; x++) {
      dst[x * 3 + 0] = src[x * 3 + 2];  // BGR -> RGB
      dst[x * 3 + 1] = src[x * 3 + 1];
      dst[x * 3 + 2] = src[x * 3 + 0];
    }
  }
  *width = w;
  *height = h;
  return 0;
}

int bmp_write(const std::string& path, const uint8_t* rgb, int width, int height,
              std::string* err) {
  if (width <= 0 || height <= 0) {
    if (err) *err = "invalid dimensions";
    return -1;
  }
  const int row_stride = ((width * 3 + 3) / 4) * 4;
  const uint32_t pixel_bytes = static_cast<uint32_t>(row_stride) * height;
  const uint32_t file_size = 54 + pixel_bytes;

  std::vector<uint8_t> out;
  out.reserve(file_size);
  out.push_back('B');
  out.push_back('M');
  U32LE::put(&out, file_size);
  U16LE::put(&out, 0);
  U16LE::put(&out, 0);
  U32LE::put(&out, 54);  // data offset
  U32LE::put(&out, 40);  // BITMAPINFOHEADER
  U32LE::put(&out, static_cast<uint32_t>(width));
  U32LE::put(&out, static_cast<uint32_t>(height));  // bottom-up
  U16LE::put(&out, 1);   // planes
  U16LE::put(&out, 24);  // bpp
  U32LE::put(&out, 0);   // BI_RGB
  U32LE::put(&out, pixel_bytes);
  U32LE::put(&out, 2835);
  U32LE::put(&out, 2835);
  U32LE::put(&out, 0);
  U32LE::put(&out, 0);
  for (int y = height - 1; y >= 0; y--) {
    const uint8_t* src = &rgb[static_cast<size_t>(y) * width * 3];
    for (int x = 0; x < width; x++) {
      out.push_back(src[x * 3 + 2]);  // RGB -> BGR
      out.push_back(src[x * 3 + 1]);
      out.push_back(src[x * 3 + 0]);
    }
    for (int p = width * 3; p < row_stride; p++) out.push_back(0);
  }

  FILE* f = std::fopen(path.c_str(), "wb");
  if (!f) {
    if (err) *err = "cannot write file: " + path;
    return -1;
  }
  const size_t written = std::fwrite(out.data(), 1, out.size(), f);
  std::fclose(f);
  if (written != out.size()) {
    if (err) *err = "short write: " + path;
    return -1;
  }
  return 0;
}

}  // namespace jpeg
}  // namespace oic
