#include <demo/vertex.hpp>

namespace GVK {

template <> VertexDescription getVertexDescription<Vertex>() {
  return {{.binding = 0,
           .stride = sizeof(Vertex),
           .inputRate = vk::VertexInputRate::eVertex},

          {{.location = 0,
            .binding = 0,
            .format = vk::Format::eR32G32B32Sfloat,
            .offset = offsetof(Vertex, position)},
           {.location = 1,
            .binding = 0,
            .format = vk::Format::eR32G32B32Sfloat,
            .offset = offsetof(Vertex, normal)},
           {.location = 2,
            .binding = 0,
            .format = vk::Format::eR32G32Sfloat,
            .offset = offsetof(Vertex, uv)}}};
}

} // namespace GVK
