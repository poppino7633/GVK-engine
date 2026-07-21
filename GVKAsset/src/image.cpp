#include <GVKAsset/image.hpp>
#include <stb/stb_image.h>
#include <stdexcept>

namespace GVK {
GVK::PixelData loadImage(std::string path) {
  int texWidth, texHeight, texChannels;
  stbi_uc *pixels = stbi_load(path.c_str(), &texWidth, &texHeight,
                              &texChannels, STBI_rgb_alpha);


  if (!pixels) {
    throw std::runtime_error("failed to load texture image!");
  }

  PixelData pixelData;
  pixelData.format = PixelFormat::RGBA;
  pixelData.width = texWidth;
  pixelData.height = texHeight;
  pixelData.data.assign(pixels, pixels + texWidth * texHeight * 4);

  return pixelData;
}
} // namespace GVK
