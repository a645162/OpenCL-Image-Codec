// oic.h - OpenCL-Image-Codec 对外 C API（纯 C，C11）。
//
// 用法：
//   #include <OpenCLImageCodec/oic.h>
//   链接 OpenCLImageCodec::OpenCLImageCodec（或静态库 OpenCLImageCodec.Api）。
//
// 线程安全：本 API 未做内部加锁；同一 oic_codec* 句柄并发调用需调用方同步。
// 所有函数返回 oic_status；0(OIC_OK) 表示成功。
#pragma once

#include <stddef.h>
#include <stdint.h>

// ---- 导出宏 ----
// 静态库构建：OIC_API 为空。共享库构建（OIC_BUILD_SHARED）时，
// 构建库的编译单元需定义 OIC_BUILDING_LIB（dllexport），消费方自动 dllimport。
#if defined(_WIN32) && defined(OIC_BUILD_SHARED)
#  if defined(OIC_BUILDING_LIB)
#    define OIC_API __declspec(dllexport)
#  else
#    define OIC_API __declspec(dllimport)
#  endif
#else
#  define OIC_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ---- 状态码 ----
typedef enum oic_status {
  OIC_OK = 0,                    // 成功
  OIC_ERR_INVALID_ARGUMENT = 1,  // 参数非法（空指针/越界/不支持的参数组合）
  OIC_ERR_OUT_OF_MEMORY = 2,     // 内存不足（含输出缓冲过小）
  OIC_ERR_BACKEND_UNAVAILABLE = 3,  // 指定后端不可用（如缺 libvpl.dll）
  OIC_ERR_UNSUPPORTED_FORMAT = 4,   // 不支持的格式/组合
  OIC_ERR_DECODE_FAILED = 5,        // 解码失败
  OIC_ERR_ENCODE_FAILED = 6,        // 编码失败
  OIC_ERR_IO = 7,                   // 文件/临时文件 I/O 错误
  OIC_ERR_INTERNAL = 8              // 内部错误
} oic_status;

// 后端：优先选择顺序 opencl / onevpl / cpu。
typedef enum oic_backend {
  OIC_BACKEND_OPENCL = 0,  // OpenCL（GPU）
  OIC_BACKEND_ONEVPL = 1,  // oneVPL 硬件编解码（仅 JPEG，Windows）
  OIC_BACKEND_CPU = 2      // 纯 CPU
} oic_backend;

// 图像容器格式。
typedef enum oic_format {
  OIC_FORMAT_JPEG = 0,
  OIC_FORMAT_TIFF = 1
} oic_format;

// 解码产物/编码输入：24-bit RGB，top-down 行序（第 0 行为图像顶部）。
// data 指向 width*height*channels 字节；stride 为每行字节数（通常 = width*channels）。
typedef struct oic_image {
  uint32_t width;
  uint32_t height;
  uint32_t channels;  // 恒为 3（RGB）
  uint8_t *data;      // malloc'd，由 oic_image_free 释放
  size_t stride;      // 每行字节数
  size_t data_size;   // 像素数据字节数
} oic_image;

// 编码参数。字段按目标格式选用：
//   JPEG: jpeg_quality（1..100；ONEVPL 后端允许 0..100）
//   TIFF: tiff_compression（1=none, 5=LZW）、tiff_rows_per_strip（0=自动/整幅）
typedef struct oic_encode_params {
  int jpeg_quality;
  int tiff_compression;
  int tiff_rows_per_strip;
} oic_encode_params;

// 不透明编解码器句柄。
typedef struct oic_codec oic_codec;

// 状态码 -> 可读字符串（静态存储，勿 free）。
OIC_API const char *oic_status_string(oic_status status);

// 创建编解码器（绑定后端）。成功返回 OIC_OK 并写出句柄；失败返回错误码。
OIC_API oic_status oic_codec_create(oic_codec **out_codec, oic_backend backend);

// 销毁编解码器。NULL 安全。
OIC_API void oic_codec_destroy(oic_codec *codec);

// 解码内存 buffer -> oic_image（malloc'd，调用方用 oic_image_free 释放）。
// in/in_len 为编码后数据（JPEG 或 TIFF 字节流）。
OIC_API oic_status oic_codec_decode(oic_codec *codec, oic_format format,
                                    const uint8_t *in, size_t in_len,
                                    oic_image *out);

// 编码 oic_image -> 内存 buffer。
// out/out_capacity 为输出缓冲；*out_len 返回实际/所需字节数。
// 若缓冲过小返回 OIC_ERR_OUT_OF_MEMORY，*out_len 置为所需大小（支持先查后写）。
OIC_API oic_status oic_codec_encode(oic_codec *codec, oic_format format,
                                    const oic_image *img,
                                    const oic_encode_params *params,
                                    uint8_t *out, size_t *out_len,
                                    size_t out_capacity);

// 释放 oic_image（含 data）。NULL 安全。
OIC_API void oic_image_free(oic_image *img);

// 枚举 OpenCL 设备。成功返回 OIC_OK；无设备时 count=0、names=NULL 仍返回 OIC_OK。
// names 由调用方用 oic_device_list_free 释放。
OIC_API oic_status oic_device_list(char ***names, int *count);
OIC_API void oic_device_list_free(char **names, int count);

#ifdef __cplusplus
}  // extern "C"
#endif
