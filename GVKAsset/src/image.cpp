#include <GVKAsset/image.hpp>
#include <stb/stb_image.h>
#include <stdexcept>

namespace GVK {
GVK::ImageData loadImage(std::string path) {
  int texWidth, texHeight, texChannels;
  stbi_uc *pixels = stbi_load(path.c_str(), &texWidth, &texHeight,
                              &texChannels, STBI_rgb_alpha);


  if (!pixels) {
    throw std::runtime_error("failed to load texture image!");
  }

  ImageData image;
  image.format = PixelFormat::RGBA;
  image.width = texWidth;
  image.height = texHeight;
  image.data.assign(pixels, pixels + texWidth * texHeight * 4);

  return image;
}
} // namespace GVK
