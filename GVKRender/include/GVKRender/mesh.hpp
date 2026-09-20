#include <GVKCommon/data.hpp>
#include <GVKRender/buffer.hpp>

namespace GVK {
struct Drawable {
  vk::raii::Buffer vertexBuffer;
  vk::raii::DeviceMemory vertexBufferMemory;
  vk::raii::Buffer indexBuffer;
  vk::raii::DeviceMemory indexBufferMemory;
  uint32_t indexCount;
  uint32_t firstIndex;
  uint32_t vertexOffset;
};

Drawable createDrawable(const vk::raii::Device &device,
                const vk::raii::PhysicalDevice &physicalDevice,
                const vk::raii::CommandPool &commandPool,
                const vk::raii::Queue &queue, const MeshData &data);

}; // namespace GVK
