#pragma once
#include <GVK/device.hpp>
#include <glm/glm.hpp>

namespace GVK {

struct VertexDescription {
  vk::VertexInputBindingDescription bindingDescription;
  std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
};

struct BasicVertex {
  glm::vec2 pos;
  glm::vec3 color;

  static VertexDescription getDescription();
};

} // namespace GVK
