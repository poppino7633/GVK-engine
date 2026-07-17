#include <GVK/vertex.hpp>

namespace GVK {

VertexDescription BasicVertex::getVertexDescription() {
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

VertexDescription TexVertex::getVertexDescription() {
  return {{.binding = 0,
           .stride = sizeof(TexVertex),
           .inputRate = vk::VertexInputRate::eVertex},

          {{.location = 0,
            .binding = 0,
            .format = vk::Format::eR32G32Sfloat,
            .offset = offsetof(TexVertex, pos)},
           {.location = 1,
            .binding = 0,
            .format = vk::Format::eR32G32Sfloat,
            .offset = offsetof(TexVertex, texCoords)}}};
}

VertexDescription Vertex::getVertexDescription() {
  return {{.binding = 0,
           .stride = sizeof(Vertex),
           .inputRate = vk::VertexInputRate::eVertex},

          {{.location = 0,
            .binding = 0,
            .format = vk::Format::eR32G32B32Sfloat,
            .offset = offsetof(Vertex, pos)},
           {.location = 1,
            .binding = 0,
            .format = vk::Format::eR32G32B32Sfloat,
            .offset = offsetof(Vertex, color)},
           {.location = 2,
            .binding = 0,
            .format = vk::Format::eR32G32Sfloat,
            .offset = offsetof(Vertex, texCoords)}}};
}

} // namespace GVK
