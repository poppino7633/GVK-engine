#include <GVKCommon/data.hpp>
#include <string>
namespace GVK {
  GVK::PixelData loadImage(std::string path);
  GVK::PixelData createDefaultImage(glm::vec4 color);
};
