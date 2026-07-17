#pragma once
#include <cstdint>
#include <vector>

namespace GVK {

enum class PixelFormat{
  RGBA
};

struct ImageData {
  std::vector<unsigned char> data;
  uint32_t width;
  uint32_t height;

  PixelFormat format;
};

}; // namespace GVK
