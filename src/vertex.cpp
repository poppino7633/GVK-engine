#include <GVK/vertex.hpp>

namespace GVK {

VertexDescription BasicVertex::getDescription() {
  return {{.binding = 0,
           .stride = sizeof(BasicVertex),
           .inputRate = vk::VertexInputRate::eVertex},

          {{.location = 0,
            .binding = 0,
            .format = vk::Format::eR32G32Sfloat,
            .offset = offsetof(BasicVertex, pos)},
           {.location = 1,
            .binding = 0,
            .format = vk::Format::eR32G32B32Sfloat,
            .offset = offsetof(BasicVertex, color)}}};
}

} // namespace GVK
