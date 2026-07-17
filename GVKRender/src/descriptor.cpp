#include "vulkan/vulkan.hpp"
#include <GVKRender/descriptor.hpp>

namespace GVK {

vk::raii::DescriptorPool
createDescriptorPool(const vk::raii::Device &device,
                     const std::vector<vk::DescriptorPoolSize> &poolSizes,
                     uint32_t descriptorCount) {
  vk::DescriptorPoolCreateInfo poolInfo{
      .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
      .maxSets = descriptorCount,
      .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
      .pPoolSizes = poolSizes.data()};

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


} // namespace GVK
