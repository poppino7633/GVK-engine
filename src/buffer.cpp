#include <GVK/buffer.hpp>

namespace GVK {
uint32_t findMemoryType(const vk::raii::PhysicalDevice &physicalDevice,
                        uint32_t typeFilter,
                        vk::MemoryPropertyFlags properties) {

  vk::PhysicalDeviceMemoryProperties memProperties =
      physicalDevice.getMemoryProperties();
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return i;
    }
  }
  throw std::runtime_error("Failed to find suitable memory type!");
}

std::pair<vk::raii::Buffer, vk::raii::DeviceMemory>
createBuffer(const vk::raii::Device &device,
             const vk::raii::PhysicalDevice &physicalDevice,
             vk::DeviceSize size, vk::BufferUsageFlags usage,
             vk::MemoryPropertyFlags properties) {
  vk::BufferCreateInfo bufferInfo{
      .size = size, .usage = usage, .sharingMode = vk::SharingMode::eExclusive};
  vk::raii::Buffer buffer = vk::raii::Buffer(device, bufferInfo);
  vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
  vk::MemoryAllocateInfo allocInfo{
      .allocationSize = memRequirements.size,
      .memoryTypeIndex = findMemoryType(
          physicalDevice, memRequirements.memoryTypeBits, properties)};
  vk::raii::DeviceMemory bufferMemory =
      vk::raii::DeviceMemory(device, allocInfo);
  buffer.bindMemory(*bufferMemory, 0);
  return {std::move(buffer), std::move(bufferMemory)};
}

void copyBuffer(const vk::raii::Device &device,
                const vk::raii::CommandPool &commandPool,
                const vk::raii::Queue &queue, vk::raii::Buffer &srcBuffer,
                vk::raii::Buffer &dstBuffer, vk::DeviceSize size) {

  vk::CommandBufferAllocateInfo allocInfo{.commandPool = commandPool,
                                          .level =
                                              vk::CommandBufferLevel::ePrimary,
                                          .commandBufferCount = 1};
  vk::raii::CommandBuffer copyCommandBuffer =
      std::move(device.allocateCommandBuffers(allocInfo).front());

  copyCommandBuffer.begin(
      {.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
  copyCommandBuffer.copyBuffer(*srcBuffer, *dstBuffer,
                               vk::BufferCopy(0, 0, size));
  copyCommandBuffer.end();
  queue.submit(vk::SubmitInfo{.commandBufferCount = 1,
                              .pCommandBuffers = &*copyCommandBuffer},
               nullptr);
  queue.waitIdle();
}

} // namespace GVK
