// cli/main.cpp - OpenCL-Image-Codec 测试 CLI（单可执行 + 子命令）。
#include "ocl_device.h"
#include "oic_platform.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using namespace oic;

namespace {

void print_usage() {
  std::printf(
      "OpenCL-Image-Codec CLI\n"
      "Usage: oic <command> [options]\n"
      "Commands:\n"
      "  list-devices                      Enumerate OpenCL platforms/devices\n"
      "  info <in>                         Print file header info (jpeg/tiff)\n"
      "  decode <in> <out> [--backend opencl|onevpl|cpu]\n"
      "  encode <in> <out> [--backend opencl|onevpl] [--jpeg-quality N]\n"
      "  bench <in> [--iterations N] [--backend ...]\n");
}

int cmd_list_devices(int, char **) {
  const std::vector<OclDeviceInfo> devs = OclDevice::enumerate();
  if (devs.empty()) {
    std::fprintf(stderr, "No OpenCL platforms/devices found.\n");
    return 1;
  }
  std::printf("%zu OpenCL device(s):\n", devs.size());
  for (size_t i = 0; i < devs.size(); ++i) {
    const OclDeviceInfo &d = devs[i];
    std::printf("[%zu] %s | %s | %s | CL %u.%u\n", i, d.deviceName.c_str(),
                d.vendor.c_str(), d.clVersion.c_str(), d.major, d.minor);
  }
  return 0;
}

int cmd_not_implemented(int, char **) {
  std::fprintf(stderr, "not implemented yet (阶段1+)\n");
  return 1;
}

}  // namespace

int main(int argc, char **argv) {
  if (argc < 2) {
    print_usage();
    return 2;
  }
  const std::string cmd = argv[1];
  int rc = 2;
  if (cmd == "list-devices") {
    rc = cmd_list_devices(argc - 1, argv + 1);
  } else if (cmd == "info") {
    rc = cmd_not_implemented(argc - 1, argv + 1);
  } else if (cmd == "decode") {
    rc = cmd_not_implemented(argc - 1, argv + 1);
  } else if (cmd == "encode") {
    rc = cmd_not_implemented(argc - 1, argv + 1);
  } else if (cmd == "bench") {
    rc = cmd_not_implemented(argc - 1, argv + 1);
  } else {
    print_usage();
  }
  return rc;
}
