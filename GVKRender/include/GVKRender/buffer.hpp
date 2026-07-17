#pragma once
#include <GVKRender/device.hpp>

namespace GVK {

struct BufferMapped {
  vk::raii::Buffer buffer;
  vk::raii::DeviceMemory memory;
  vk::DeviceSize bufferSize;

  void *ptr;
};

vk::DescriptorBufferInfo getBufferMappedInfo(const BufferMapped &buffer, vk::DeviceSize offset);

std::pair<vk::raii::Buffer, vk::raii::DeviceMemory>
createBuffer(const vk::raii::Device &device,
             const vk::raii::PhysicalDevice &physicalDevice,
             vk::DeviceSize size, vk::BufferUsageFlags usage,
             vk::MemoryPropertyFlags properties);

void copyBuffer(const vk::raii::Device &device,
                const vk::raii::CommandPool &commandPool,
                const vk::raii::Queue &queue, vk::raii::Buffer &srcBuffer,
                vk::raii::Buffer &dstBuffer, vk::DeviceSize size);

template <typename T>
std::pair<vk::raii::Buffer, vk::raii::DeviceMemory>
createBufferFromVec(const vk::raii::Device &device,
                    const vk::raii::PhysicalDevice &physicalDevice,
                    const vk::raii::CommandPool &commandPool,
                    const vk::raii::Queue &queue, std::vector<T> data,
                    vk::BufferUsageFlags usageFlags) {

  vk::DeviceSize bufferSize = data.size() * sizeof(T);
  auto [stagingBuffer, stagingBufferMemory] = GVK::createBuffer(
      device, physicalDevice, bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible |
          vk::MemoryPropertyFlagBits::eHostCoherent);

  void *dataStaging = stagingBufferMemory.mapMemory(0, bufferSize);
  memcpy(dataStaging, data.data(), bufferSize);
  stagingBufferMemory.unmapMemory();

  auto [buffer, bufferMemory] =
      GVK::createBuffer(device, physicalDevice, bufferSize,
                        usageFlags | vk::BufferUsageFlagBits::eTransferDst,
                        vk::MemoryPropertyFlagBits::eDeviceLocal);

  GVK::copyBuffer(device, commandPool, queue, stagingBuffer, buffer,
                  bufferSize);
  return {std::move(buffer), std::move(bufferMemory)};
}

template <typename T>
std::vector<BufferMapped>
createUniformBuffers(const vk::raii::Device &device,
                     const vk::raii::PhysicalDevice &physicalDevice,
                     size_t count) {
  std::vector<BufferMapped> uniformBuffers;
  for (size_t i = 0; i < count; i++) {
    vk::DeviceSize bufferSize = sizeof(T);
    auto [buffer, bufferMem] =
        createBuffer(device, physicalDevice, bufferSize,
                     vk::BufferUsageFlagBits::eUniformBuffer,
                     vk::MemoryPropertyFlagBits::eHostVisible |
                         vk::MemoryPropertyFlagBits::eHostCoherent);
    uniformBuffers.emplace_back(BufferMapped{
        std::move(buffer), std::move(bufferMem), bufferSize, nullptr});
    uniformBuffers.back().ptr =
        uniformBuffers.back().memory.mapMemory(0, bufferSize);
  }
  return uniformBuffers;
}

} // namespace GVK
