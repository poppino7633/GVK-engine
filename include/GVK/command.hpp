#pragma once
#include <GVK/device.hpp>

namespace GVK {
vk::raii::CommandPool createCommandPool(const vk::raii::Device &device,
                                        uint32_t queueFamilyIndex);

std::vector<vk::raii::CommandBuffer>
createCommandBuffers(const vk::raii::Device &device,
                     const vk::raii::CommandPool &commandPool, uint32_t count);

void transition_image_layout(const vk::raii::CommandBuffer &commandBuffer,
                             vk::Image image, vk::ImageLayout oldLayout,
                             vk::ImageLayout newLayout,
                             vk::AccessFlags2 srcAccessMask,
                             vk::AccessFlags2 dstAccessMask,
                             vk::PipelineStageFlags2 srcStageMask,
                             vk::PipelineStageFlags2 dstStageMask);

} // namespace GVK
