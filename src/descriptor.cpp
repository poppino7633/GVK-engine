#include <GVK/descriptor.hpp>

namespace GVK {

vk::raii::DescriptorPool createDescriptorPool(const vk::raii::Device &device,
                                              uint32_t descriptorCount) {
  vk::DescriptorPoolSize poolSize{.type = vk::DescriptorType::eUniformBuffer,
                                  .descriptorCount = descriptorCount};
  vk::DescriptorPoolCreateInfo poolInfo{
      .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
      .maxSets = descriptorCount,
      .poolSizeCount = 1,
      .pPoolSizes = &poolSize};

  return vk::raii::DescriptorPool(device, poolInfo);
}

std::vector<vk::raii::DescriptorSet>
allocateDescriptorSets(const vk::raii::Device &device,
                     const vk::raii::DescriptorPool &descriptorPool,
                     uint32_t count,
                     const vk::raii::DescriptorSetLayout &layout) {
  std::vector<vk::DescriptorSetLayout> layouts(count, *layout);
  vk::DescriptorSetAllocateInfo allocInfo{
      .descriptorPool = descriptorPool,
      .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
      .pSetLayouts = layouts.data()};

  return device.allocateDescriptorSets(allocInfo);
}

vk::raii::DescriptorSetLayout createDescriptorSetLayout(
    const vk::raii::Device &device,
    const std::vector<vk::DescriptorSetLayoutBinding> &bindings) {
  vk::DescriptorSetLayoutCreateInfo layoutInfo{
      .bindingCount = static_cast<uint32_t>(bindings.size()),
      .pBindings = bindings.data()};
  return vk::raii::DescriptorSetLayout(device, layoutInfo);
}

void updateDescriptorSets(
    const vk::raii::Device &device,
    const std::vector<vk::raii::DescriptorSet> &descriptorSets,
    const std::vector<GVK::BufferMapped> &uniformBuffers) {

  for (size_t i = 0; i < descriptorSets.size(); i++) {
    vk::DescriptorBufferInfo bufferInfo{
        .buffer = uniformBuffers[i].buffer, .offset = 0, .range = uniformBuffers[i].bufferSize};
    vk::WriteDescriptorSet descriptorWrite{
        .dstSet = descriptorSets[i],
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .pBufferInfo = &bufferInfo};
    device.updateDescriptorSets(descriptorWrite, {});
  }
}


} // namespace GVK
