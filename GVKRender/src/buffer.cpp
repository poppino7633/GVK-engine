#include <GVKRender/buffer.hpp>
#include <GVKRender/command.hpp>

namespace GVK {
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

  auto copyCommandBuffer = GVK::beginSingleTimeCommands(device, commandPool);

  copyCommandBuffer.handle.copyBuffer(*srcBuffer, *dstBuffer,
                                   vk::BufferCopy(0, 0, size));
  GVK::endSingleTimeCommands(queue, std::move(copyCommandBuffer));
}

vk::DescriptorBufferInfo getBufferMappedInfo(const BufferMapped &buffer, vk::DeviceSize offset){
  return { .buffer = buffer.buffer, .offset = offset, .range = buffer.bufferSize };
}


} // namespace GVK
