#include <GVK/device.hpp>

namespace GVK {

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
  assert(!data.empty());

  vk::DeviceSize bufferSize = data.size() * sizeof(data[0]);
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

} // namespace GVK
