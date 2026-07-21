#pragma once
#include <GVKRender/buffer.hpp>

namespace GVK {

struct Image {
  vk::raii::Image handle;
  vk::raii::DeviceMemory memory;
  vk::raii::ImageView view;
};

void transitionImageLayout(const vk::raii::CommandBuffer &commandBuffer,
                           vk::Image image, vk::ImageLayout oldLayout,
                           vk::ImageLayout newLayout, vk::ImageAspectFlagBits aspectMask);

Image createImage(const vk::raii::Device &device,
                  const vk::raii::PhysicalDevice &physicalDevice,
                  uint32_t width, uint32_t height, vk::Format format,
                  vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                  vk::MemoryPropertyFlags properties,
                  vk::ImageAspectFlagBits aspectMask);

vk::raii::ImageView createImageView(const vk::raii::Device &device,
                                    const vk::Image &image, vk::Format format,
                                    vk::ImageAspectFlagBits aspectMask);

void copyBufferToImage(vk::raii::CommandBuffer &commandBuffer,
                       const vk::raii::Buffer &buffer, vk::raii::Image &image,
                       uint32_t width, uint32_t height);

} // namespace GVK
