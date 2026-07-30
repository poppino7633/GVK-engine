#pragma once
#include <GVKCommon/data.hpp>
#include <GVKRender/command.hpp>
#include <GVKRender/image.hpp>
#include <unordered_map>

namespace GVK {

struct Texture {
  std::string name;
  Image image;
  vk::raii::Sampler sampler;
  static vk::DescriptorSetLayoutBinding getBinding(uint32_t index);
  static vk::DescriptorPoolSize getPoolSize(uint32_t count);
};

vk::DescriptorImageInfo getTextureImageInfo(const Texture &texture);

Texture createTexture(const vk::raii::Device &device,
                      const vk::raii::PhysicalDevice &physicalDevice,
                      const vk::raii::Queue &queue,
                      const vk::raii::CommandPool &commandPool,
                      GVK::PixelData imageData, std::string name);

struct TextureManager {
  std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
};

void addTexture(TextureManager &manager, Texture texture);
std::shared_ptr<Texture> getTexture(TextureManager &manager, std::string name);




} // namespace GVK
