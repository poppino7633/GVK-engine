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
    transitionImageLayout(command.handle, image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    endSingleTimeCommands(queue, std::move(command));
  }

  vk::raii::ImageView imageView =
      createImageView(device, *image, vk::Format::eR8G8B8A8Srgb);

  vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
  vk::SamplerCreateInfo samplerInfo{
      .magFilter = vk::Filter::eLinear,
      .minFilter = vk::Filter::eLinear,
      .mipmapMode = vk::SamplerMipmapMode::eLinear,
      .addressModeU = vk::SamplerAddressMode::eRepeat,
      .addressModeV = vk::SamplerAddressMode::eRepeat,
      .addressModeW = vk::SamplerAddressMode::eRepeat,
      .mipLodBias = 0.0f,
      .anisotropyEnable = vk::True,
      .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
      .compareEnable = vk::False,
      .compareOp = vk::CompareOp::eAlways,
      .minLod = 0.0f,
      .maxLod = 0.0f,
      .borderColor = vk::BorderColor::eIntOpaqueBlack,
      .unnormalizedCoordinates = vk::False,

  };

  vk::raii::Sampler sampler{device, samplerInfo};

  return {std::move(image), std::move(imageMemory), std::move(imageView),
          std::move(sampler)};
}

vk::DescriptorSetLayoutBinding Texture::getBinding() {
  return {.binding = 1,
          .descriptorType = vk::DescriptorType::eCombinedImageSampler,
          .descriptorCount = 1,
          .stageFlags = vk::ShaderStageFlagBits::eFragment};
}

vk::DescriptorPoolSize Texture::getPoolSize(uint32_t count) {
  return {.type = vk::DescriptorType::eCombinedImageSampler,
          .descriptorCount = count};
}

vk::DescriptorImageInfo getTextureImageInfo(const Texture &texture) {
  return {.sampler = texture.sampler,
          .imageView = texture.imageView,
          .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal};
}

} // namespace GVK
