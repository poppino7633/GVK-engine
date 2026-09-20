#include <GVKRender/mesh.hpp>

namespace GVK {
Drawable createDrawable(const vk::raii::Device &device,
                const vk::raii::PhysicalDevice &physicalDevice,
                const vk::raii::CommandPool &commandPool,
                const vk::raii::Queue &queue, const MeshData &data) {

  auto [vertexBuffer, vertexBufferMemory] = GVK::createBufferFromVec(
      device, physicalDevice, commandPool, queue, data.vertices,
      vk::BufferUsageFlagBits::eVertexBuffer);

  auto [indexBuffer, indexBufferMemory] = GVK::createBufferFromVec(
      device, physicalDevice, commandPool, queue, data.indices,
      vk::BufferUsageFlagBits::eIndexBuffer);

  uint32_t count = data.indices.size();
  return {
    std::move(vertexBuffer),
    std::move(vertexBufferMemory),
    std::move(indexBuffer),
    std::move(indexBufferMemory),
    count
  };
}

} // namespace GVK
