#pragma once
#include <GVK/device.hpp>

namespace GVK {
vk::raii::CommandPool createCommandPool(const vk::raii::Device &device,
                                        uint32_t queueFamilyIndex);

std::vector<vk::raii::CommandBuffer>
createCommandBuffers(const vk::raii::Device &device,
                     const vk::raii::CommandPool &commandPool, uint32_t count);


struct SingleTimeCommand {
  vk::raii::CommandBuffer handle;
};

SingleTimeCommand
beginSingleTimeCommands(const vk::raii::Device &device,
                        const vk::raii::CommandPool &commandPool);

void endSingleTimeCommands(const vk::raii::Queue &queue,
                           SingleTimeCommand command);

} // namespace GVK
