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
  };
  for (size_t i = 0; i < materialCount; i++) {
    ms.freeIndices.push(i);
  }
  return ms;
}

Material createMaterial(const vk::raii::Device &device,
                        MaterialSystem &materialSystem,
                        MaterialConstants constants) {
  assert(!materialSystem.freeIndices.empty());
  size_t index = materialSystem.freeIndices.top();
  materialSystem.freeIndices.pop();

  memcpy((char *)materialSystem.constantsUBO.ptr +
             index * materialSystem.materialStride,
         &constants, sizeof(MaterialConstants));
  vk::DescriptorBufferInfo bufferInfo = {*materialSystem.constantsUBO.buffer,
                                         index * materialSystem.materialStride,
                                         sizeof(MaterialConstants)};
  std::array<vk::WriteDescriptorSet, 1> descriptorWrites{{
      {.dstSet = *materialSystem.descriptorSets[index],
       .dstBinding = 0,
       .dstArrayElement = 0,
       .descriptorCount = 1,
       .descriptorType = vk::DescriptorType::eUniformBuffer,
       .pBufferInfo = &bufferInfo},
  }};
  device.updateDescriptorSets(descriptorWrites, {});
  return {index, index * materialSystem.materialStride};
}

}; // namespace GVK
