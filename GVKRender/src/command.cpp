#include <GVKRender/command.hpp>

namespace GVK {
vk::raii::CommandPool createCommandPool(const vk::raii::Device &device,
                                        uint32_t queueFamilyIndex) {
  vk::CommandPoolCreateInfo poolInfo{
      .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      .queueFamilyIndex = queueFamilyIndex};

  return vk::raii::CommandPool(device, poolInfo);
}

std::vector<vk::raii::CommandBuffer>
createCommandBuffers(const vk::raii::Device &device,
                     const vk::raii::CommandPool &commandPool, uint32_t count) {
  vk::CommandBufferAllocateInfo allocInfo{.commandPool = commandPool,
                                          .level =
                                              vk::CommandBufferLevel::ePrimary,
                                          .commandBufferCount = count};
  return vk::raii::CommandBuffers(device, allocInfo);
}


SingleTimeCommand
beginSingleTimeCommands(const vk::raii::Device &device,
                       const vk::raii::CommandPool &commandPool) {
  vk::CommandBufferAllocateInfo allocInfo{.commandPool = commandPool,
                                          .level =
                                              vk::CommandBufferLevel::ePrimary,
                                          .commandBufferCount = 1};
  vk::raii::CommandBuffer commandBuffer =
      std::move(vk::raii::CommandBuffers(device, allocInfo).front());

  vk::CommandBufferBeginInfo beginInfo{
      .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
  commandBuffer.begin(beginInfo);

  return {std::move(commandBuffer)};
}

void endSingleTimeCommands(const vk::raii::Queue &queue, SingleTimeCommand command)
{
  command.handle.end();

  vk::SubmitInfo submitInfo{.commandBufferCount = 1, .pCommandBuffers = &*command.handle};
  queue.submit(submitInfo, nullptr);
  queue.waitIdle();
}

} // namespace GVK
