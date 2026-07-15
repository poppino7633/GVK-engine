#pragma once
#include <GVK/buffer.hpp>
#include <GVK/device.hpp>

namespace GVK {

vk::raii::DescriptorSetLayout createDescriptorSetLayout(
    const vk::raii::Device &device,
    const std::vector<vk::DescriptorSetLayoutBinding> &bindings);

vk::raii::DescriptorPool createDescriptorPool(
    const vk::raii::Device &device,
    const std::vector<vk::DescriptorPoolSize> &descriptorPoolSizes,
    uint32_t descriptorCount);

std::vector<vk::raii::DescriptorSet>
allocateDescriptorSets(const vk::raii::Device &device,
                       const vk::raii::DescriptorPool &descriptorPool,
                       uint32_t count,
                       const vk::raii::DescriptorSetLayout &layout);

}; // namespace GVK
