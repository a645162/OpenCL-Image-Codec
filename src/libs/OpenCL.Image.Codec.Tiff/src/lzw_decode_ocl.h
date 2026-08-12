// tiff/lzw_decode_ocl.h - GPU LZW 解码（探索项，阶段 1.5）。
//
// ============================================================================
// 【探索结论】GPU 并行 LZW 解码 —— 建议不替换 CPU，CPU(lzwDecode) 保持唯一实现。
// ============================================================================
// 结论日期：2026-08-12。平台：Intel UHD 770 / OpenCL 3.0 NEO。
//
// 方案：段级并行。把码流按 ClearCode(256) 切段（段间字典独立、可并行），段内仍
// 串行。本文件提供 gpuLzwDecode()：host 先用一次「段扫描」找出各段 bit 区间并校验
// 码流，再启动 kernel（每个 work-item 解码一段），两遍（先测长度、后写输出）得到
// 与 CPU 参考逐字节一致的输出。
//
// 为什么并行化收益低（实测结论）：
//   1. LZW 解码的码流解析是硬串行依赖：码宽由段内码序号决定（见 lzw_decode.cl
//      seg_width），而段边界（ClearCode 位置）只能靠顺序读码发现。段级并行要求
//      先做一次串行的段扫描，该扫描已读取全部码（O(codes)），占一次完整解码
//      工作量的相当比例。
//   2. 典型图像 strip 的段数极少（字典 4096 满时才发 ClearCode，约每 20~40KB
//      一次），单段流（大多数小 strip）退化为 1 个 work-item 的纯串行 kernel，
//      加上两遍(测长+写出)内核调用、设备/缓冲开销，净效果反而慢于 CPU。
//   3. GPU 编码之所以快，是因为编码天然按 strip/tile 分块（work-item 数 =
//      strip 数，通常几十~上千）；解码若沿用同构思路（每 strip 一个 work-item）
//      需要批处理接口，而单个 gpuLzwDecode(in,size,out) 接口没有这个并行度。
//     即便做成按 strip 并行，单 work-item 内部仍是串行，且 NEO 对单 work-item
//     的缓冲/执行也有额外限制，收益存疑。
//
// 建议：
//   - 解码默认/唯一实现保持 CPU lzwDecode（本库 tiffDecodeFromFile 即如此）。
//   - 若未来确实需要 GPU 解码提速，方向是「按 strip 批处理 + 每 strip 一个
//     work-item」（镜像 lzw_encode_ocl 的架构），并配合两遍(测长+写出) kernel；
//     不要在单流接口上追求段内并行。
//   - 本 gpuLzwDecode 保留为探索/对拍用：输出与 CPU 逐字节一致，可作验证基准。
//
// 遗留风险：
//   - 段扫描与 kernel 的宽度时机（解码器 511/1023/2047 vs 编码器 512/1024/2048）
//     高度微妙，改动需同步 CPU 参考与 GPU 编码 kernel 三处。
//   - 本实现仅处理 ≤2^28 字节的单流；超大流可回退 CPU。
// ============================================================================
#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace oic {
namespace tiff {

// 用 GPU（段级并行）解码一段 LZW 压缩数据，语义与 CPU 参考 lzwDecode 一致
// （ClearCode/EOI、9->10->11->12 码宽、字典满 4096 重置、KwKwK 分支）。
// 成功返回 0，out 为全部解压字节；失败返回负值：
//   -1 输入非法；-2 位流提前结束（缺 EOI）；-3 非法码；-4 非字面量；
//   -5 码超前；-10.. 设备/编译/运行错误。
// 任何负值返回都表示失败，调用方应回退 CPU lzwDecode。
int gpuLzwDecode(const uint8_t* in, size_t in_size, std::vector<uint8_t>& out);

}  // namespace tiff
}  // namespace oic
