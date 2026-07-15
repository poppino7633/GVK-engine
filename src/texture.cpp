#include "GVK/command.hpp"
#include <GVK/texture.hpp>

namespace GVK {
Texture createTexture(const vk::raii::Device &device,
                      const vk::raii::PhysicalDevice &physicalDevice,
                      const vk::raii::Queue &queue,
                      const vk::raii::CommandPool &commandPool,
                      std::string path) {
  int texWidth, texHeight, texChannels;
  stbi_uc *pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels,
                              STBI_rgb_alpha);
  vk::DeviceSize imageSize = texWidth * texHeight * 4;

  if (!pixels) {
    throw std::runtime_error("failed to load texture image!");
  }

  auto [stagingBuffer, stagingBufferMemory] = GVK::createBuffer(
      device, physicalDevice, imageSize, vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible |
          vk::MemoryPropertyFlagBits::eHostCoherent);

  void *data = stagingBufferMemory.mapMemory(0, imageSize);
  memcpy(data, pixels, imageSize);
  stagingBufferMemory.unmapMemory();
  stbi_image_free(pixels);

  auto [image, imageMemory] = createImage(
      device, physicalDevice, texWidth, texHeight, vk::Format::eR8G8B8A8Srgb,
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
      vk::MemoryPropertyFlagBits::eDeviceLocal);

  {
    SingleTimeCommand command = beginSingleTimeCommands(device, commandPool);
    transitionImageLayout(command.handle, image, vk::ImageLayout::eUndefined,
                          vk::ImageLayout::eTransferDstOptimal);
    copyBufferToImage(command.handle, stagingBuffer, image, texWidth,
                      texHeight);
    endSingleTimeCommands(queue, std::move(command));
  }

  vk::raii::ImageView imageView =
      createImageView(device, *image, vk::Format::eR8G8B8A8Srgb);

  return {std::move(image), std::move(imageMemory), std::move(imageView)};
}

} // namespace GVK
