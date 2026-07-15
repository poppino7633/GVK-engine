#include <GVK/command.hpp>
#include <GVK/image.hpp>

namespace GVK {
std::pair<vk::raii::Image, vk::raii::DeviceMemory>
createTextureImage(const vk::raii::Device &device,
                   const vk::raii::PhysicalDevice &physicalDevice,
                   const vk::raii::Queue &queue,
                   const vk::raii::CommandPool &commandPool, std::string path);

}
