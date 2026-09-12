#include "GVKRender/texture.hpp"
#include <GVKRender/material.hpp>

size_t alignUp(size_t value, size_t alignment) {
  return (value + alignment - 1) / alignment * alignment;
}

namespace GVK {
MaterialSystem
createMaterialSystem(const vk::raii::Device &device,
                     const vk::raii::PhysicalDevice &physicalDevice,
                     const vk::raii::DescriptorPool &descriptorPool,
                     uint32_t materialCount) {

  vk::raii::DescriptorSetLayout descriptorSetLayout =
      createDescriptorSetLayout(device, Material::getBindings());
  size_t stride = alignUp(
      sizeof(MaterialConstants),
      physicalDevice.getProperties().limits.minUniformBufferOffsetAlignment);
  BufferMapped constantsUBO =
      createUniformBufferArray(device, physicalDevice, stride, materialCount);
  std::vector<vk::raii::DescriptorSet> descriptorSets = allocateDescriptorSets(
      device, descriptorPool, materialCount, descriptorSetLayout);
  MaterialSystem ms = {
      .constantsUBO = std::move(constantsUBO),
      .materialStride = stride,
      .descriptorSetLayout = std::move(descriptorSetLayout),
      .descriptorSets = std::move(descriptorSets),
      .baseColorTextures = std::vector<std::shared_ptr<Texture>>(materialCount),
      .normalTextures = std::vector<std::shared_ptr<Texture>>(materialCount),
      .metallicRoughnessTextures =
          std::vector<std::shared_ptr<Texture>>(materialCount)};

  for (size_t i = 0; i < materialCount; i++) {
    ms.freeIndices.push(i);
  }
  return ms;
}

Material createMaterial(const vk::raii::Device &device,
                        MaterialSystem &materialSystem,
                        MaterialConstants constants,
                        std::shared_ptr<Texture> baseColorTexture,
                        std::shared_ptr<Texture> normalTexture,
                        std::shared_ptr<Texture> metallicRoughnessTexture) {
  assert(!materialSystem.freeIndices.empty());

  size_t index = materialSystem.freeIndices.top();
  materialSystem.freeIndices.pop();
  materialSystem.baseColorTextures[index] = baseColorTexture;
  materialSystem.normalTextures[index] = normalTexture;
  materialSystem.metallicRoughnessTextures[index] = metallicRoughnessTexture;

  memcpy((char *)materialSystem.constantsUBO.ptr +
             index * materialSystem.materialStride,
         &constants, sizeof(MaterialConstants));
  vk::DescriptorBufferInfo bufferInfo = {*materialSystem.constantsUBO.buffer,
                                         index * materialSystem.materialStride,
                                         sizeof(MaterialConstants)};

  vk::DescriptorImageInfo baseColorImageInfo =
      getTextureImageInfo(*baseColorTexture);
  vk::DescriptorImageInfo normalImageInfo = getTextureImageInfo(*normalTexture);
  vk::DescriptorImageInfo metallicRoughnessImageInfo =
      getTextureImageInfo(*metallicRoughnessTexture);

  std::array<vk::WriteDescriptorSet, 4> descriptorWrites{{
      {.dstSet = *materialSystem.descriptorSets[index],
       .dstBinding = 0,
       .dstArrayElement = 0,
       .descriptorCount = 1,
       .descriptorType = vk::DescriptorType::eUniformBuffer,
       .pBufferInfo = &bufferInfo},
      {.dstSet = *materialSystem.descriptorSets[index],
       .dstBinding = 1,
       .dstArrayElement = 0,
       .descriptorCount = 1,
       .descriptorType = vk::DescriptorType::eCombinedImageSampler,
       .pImageInfo = &baseColorImageInfo},
      {.dstSet = *materialSystem.descriptorSets[index],
       .dstBinding = 2,
       .dstArrayElement = 0,
       .descriptorCount = 1,
       .descriptorType = vk::DescriptorType::eCombinedImageSampler,
       .pImageInfo = &normalImageInfo},
      {.dstSet = *materialSystem.descriptorSets[index],
       .dstBinding = 3,
       .dstArrayElement = 0,
       .descriptorCount = 1,
       .descriptorType = vk::DescriptorType::eCombinedImageSampler,
       .pImageInfo = &metallicRoughnessImageInfo},
  }};
  device.updateDescriptorSets(descriptorWrites, {});
  return {index, index * materialSystem.materialStride};
}

}; // namespace GVK
