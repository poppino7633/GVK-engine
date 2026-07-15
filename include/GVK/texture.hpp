#include <GVK/command.hpp>
#include <GVK/image.hpp>

namespace GVK {

struct Texture {
  vk::raii::Image image;
  vk::raii::DeviceMemory memory;
  vk::raii::ImageView imageView;
};

Texture createTexture(const vk::raii::Device &device,
                      const vk::raii::PhysicalDevice &physicalDevice,
                      const vk::raii::Queue &queue,
                      const vk::raii::CommandPool &commandPool,
                      std::string path);

} // namespace GVK
