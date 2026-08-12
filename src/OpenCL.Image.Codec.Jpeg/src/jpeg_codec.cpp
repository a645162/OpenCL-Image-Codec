// src/jpeg/jpeg_codec.cpp - 混合编解码编排实现。
#include "jpeg_codec.h"

#include "bmp_io.h"
#include "jpeg_bitstream.h"
#include "jpeg_dct_ocl.h"
#include "jpeg_huffman.h"

#include <cstdio>

namespace oic {
namespace jpeg {

namespace {

void infer_subsampling(const JpegImage& img, int* out) {
  int hs = 1, vs = 1;
  for (int c = 0; c < img.ncomp; c++) {
    if (c == 0) {
      hs = img.comp[c].h_samp;
      vs = img.comp[c].v_samp;
    }
  }
  const int hr = img.max_h / hs;
  const int vr = img.max_v / vs;
  if (img.ncomp == 1) {
    *out = 444;
  } else if (hr == 1 && vr == 1) {
    *out = 444;
  } else if (hr == 2 && vr == 2) {
    *out = 420;
  } else if (hr == 2 && vr == 1) {
    *out = 422;
  } else {
    *out = 444;
  }
}

}  // namespace

int jpeg_decode_buffer(const std::vector<uint8_t>& file, int backend,
                       JpegDecodeResult* out) {
  out->rgb.clear();
  out->error.clear();
  JpegImage img;
  std::string err;
  if (jpeg_parse(file, &img, &err) != 0) {
    out->error = "parse failed: " + err;
    return -1;
  }
  std::vector<float> coeffs;
  if (jpeg_entropy_decode(img, &coeffs, &err) != 0) {
    out->error = "entropy decode failed: " + err;
    return -1;
  }
  std::vector<uint8_t> rgb;
  int rc;
  if (backend == 0) {
    rc = jpeg_gpu_decode(img, coeffs, &rgb, &err);
  } else {
    rc = jpeg_cpu_decode(img, coeffs, &rgb, &err);
  }
  if (rc != 0) {
    out->error = "DCT/color decode failed: " + err;
    return -1;
  }
  out->width = img.width;
  out->height = img.height;
  out->ncomp = img.ncomp;
  infer_subsampling(img, &out->subsampling);
  out->rgb = std::move(rgb);
  return 0;
}

int jpeg_encode_buffer(const std::vector<uint8_t>& rgb, int width, int height,
                       int quality, int subsampling, int backend,
                       std::vector<uint8_t>* jpeg_out, std::string* err) {
  if (quality < 1) quality = 1;
  if (quality > 100) quality = 100;
  if (subsampling != 444 && subsampling != 420 && subsampling != 422) {
    subsampling = (quality >= 92) ? 444 : 420;
  }
  (void)backend;  // 当前仅 OpenCL 后端

  JpegImage img;
  img.width = width;
  img.height = height;
  img.ncomp = 3;
  img.max_h = 2;
  img.max_v = 2;
  img.comp[0].id = 1;
  img.comp[1].id = 2;
  img.comp[2].id = 3;
  img.comp[0].tq = 0;
  img.comp[1].tq = 1;
  img.comp[2].tq = 1;
  img.comp[0].td = 0;
  img.comp[0].ta = 0;
  img.comp[1].td = 1;
  img.comp[1].ta = 1;
  img.comp[2].td = 1;
  img.comp[2].ta = 1;
  if (subsampling == 444) {
    img.max_h = img.max_v = 1;
    for (int c = 0; c < 3; c++) img.comp[c].h_samp = img.comp[c].v_samp = 1;
  } else if (subsampling == 420) {
    img.comp[0].h_samp = 2;
    img.comp[0].v_samp = 2;
    img.comp[1].h_samp = img.comp[1].v_samp = 1;
    img.comp[2].h_samp = img.comp[2].v_samp = 1;
  } else {  // 422
    img.comp[0].h_samp = 2;
    img.comp[0].v_samp = 1;
    img.comp[1].h_samp = img.comp[1].v_samp = 1;
    img.comp[2].h_samp = img.comp[2].v_samp = 1;
  }
  jpeg_compute_layout(&img);
  jpeg_make_quant_tables(quality, img.quant);
  img.huff_dc[0] = jpeg_std_dc_lum();
  img.huff_ac[0] = jpeg_std_ac_lum();
  img.huff_dc[1] = jpeg_std_dc_chroma();
  img.huff_ac[1] = jpeg_std_ac_chroma();

  std::vector<float> coeffs;
  if (jpeg_gpu_encode(img, rgb.data(), &coeffs, err) != 0) {
    return -1;
  }
  std::vector<uint8_t> entropy;
  if (jpeg_entropy_encode(img, coeffs, &entropy, err) != 0) {
    return -1;
  }
  if (jpeg_write_file(img, entropy, jpeg_out, err) != 0) {
    return -1;
  }
  return 0;
}

int jpeg_decode_file(const std::string& in_path, int backend,
                     JpegDecodeResult* out) {
  FILE* f = std::fopen(in_path.c_str(), "rb");
  if (!f) {
    out->error = "cannot open file: " + in_path;
    return -1;
  }
  std::vector<uint8_t> file;
  uint8_t buf[65536];
  size_t n;
  while ((n = std::fread(buf, 1, sizeof(buf), f)) > 0) {
    file.insert(file.end(), buf, buf + n);
  }
  std::fclose(f);
  return jpeg_decode_buffer(file, backend, out);
}

int jpeg_encode_bmp_file(const std::string& in_bmp, const std::string& out_jpg,
                         int backend, int quality, std::string* err) {
  std::vector<uint8_t> rgb;
  int w = 0, h = 0;
  if (bmp_read(in_bmp, &rgb, &w, &h, err) != 0) return -1;
  std::vector<uint8_t> jpeg;
  if (jpeg_encode_buffer(rgb, w, h, quality, 0, backend, &jpeg, err) != 0) {
    return -1;
  }
  FILE* f = std::fopen(out_jpg.c_str(), "wb");
  if (!f) {
    if (err) *err = "cannot write file: " + out_jpg;
    return -1;
  }
  const size_t written = std::fwrite(jpeg.data(), 1, jpeg.size(), f);
  std::fclose(f);
  if (written != jpeg.size()) {
    if (err) *err = "short write: " + out_jpg;
    return -1;
  }
  return 0;
}

}  // namespace jpeg
}  // namespace oic
