#include <GVK/buffer.hpp>
#include <stb/stb_image.h>

namespace GVK {


void transitionImageLayout(const vk::raii::CommandBuffer &commandBuffer,
                           vk::Image image, vk::ImageLayout oldLayout,
                           vk::ImageLayout newLayout);

std::pair<vk::raii::Image, vk::raii::DeviceMemory>
createImage(const vk::raii::Device &device,
            const vk::raii::PhysicalDevice &physicalDevice, uint32_t width,
            uint32_t height, vk::Format format, vk::ImageTiling tiling,
            vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);

void copyBufferToImage(vk::raii::CommandBuffer &commandBuffer,
                       const vk::raii::Buffer &buffer, vk::raii::Image &image,
                       uint32_t width, uint32_t height);

vk::raii::ImageView createImageView(const vk::raii::Device &device,
                                    const vk::Image &image, vk::Format format);


} // namespace GVK
