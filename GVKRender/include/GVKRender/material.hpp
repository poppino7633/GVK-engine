#pragma once
#include <GVKRender/buffer.hpp>
#include <glm/vec4.hpp>
#include <stack>
#include <GVKRender/descriptor.hpp>

namespace GVK {
struct MaterialConstants {
  glm::vec4 baseColor = {1.0f, 1.0f, 1.0f, 1.0f};

  float metallic = 1.0f;
  float roughness = 1.0f;

  static vk::DescriptorSetLayoutBinding getBinding(uint32_t index) {
    return {.binding = index,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = 1,
            .stageFlags = vk::ShaderStageFlagBits::eFragment};
  }
  static vk::DescriptorPoolSize getPoolSize(uint32_t count) {
    return {.type = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = count};
  }
};

struct Material {

  const size_t descriptorSetIndex;
  const vk::DeviceSize uboOffset;

  static std::vector<vk::DescriptorSetLayoutBinding> getBindings() {
    return {MaterialConstants::getBinding(0)};
  }
  static std::vector<vk::DescriptorPoolSize> getPoolSizes(uint32_t count) {
    return {MaterialConstants::getPoolSize(count)};
  }
};

struct MaterialSystem {
  BufferMapped constantsUBO;
  size_t materialStride;
  vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
  std::vector<vk::raii::DescriptorSet> descriptorSets;
  std::stack<size_t> freeIndices;
};

MaterialSystem
createMaterialSystem(const vk::raii::Device &device,
                     const vk::raii::PhysicalDevice &physicalDevice,
                     const vk::raii::DescriptorPool &descriptorPool,
                     uint32_t materialCount);

Material createMaterial(const vk::raii::Device &device,
                        MaterialSystem &materialSystem,
                        MaterialConstants constants);

} // namespace GVK
