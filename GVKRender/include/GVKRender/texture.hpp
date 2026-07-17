#include <GVKCommon/data.hpp>
#include <GVKRender/command.hpp>
#include <GVKRender/image.hpp>

namespace GVK {

struct Texture {
  vk::raii::Image image;
  vk::raii::DeviceMemory memory;
  vk::raii::ImageView imageView;
  vk::raii::Sampler sampler;
  static vk::DescriptorSetLayoutBinding getBinding();
  static vk::DescriptorPoolSize getPoolSize(uint32_t count);
};

vk::DescriptorImageInfo getTextureImageInfo(const Texture &texture);

Texture createTexture(const vk::raii::Device &device,
                      const vk::raii::PhysicalDevice &physicalDevice,
                      const vk::raii::Queue &queue,
                      const vk::raii::CommandPool &commandPool,
                      GVK::ImageData imageData);

} // namespace GVK
