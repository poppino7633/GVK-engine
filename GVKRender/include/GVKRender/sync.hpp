#pragma once
#include <GVKRender/device.hpp>

namespace GVK {
std::vector<vk::raii::Semaphore>
createSemaphores(const vk::raii::Device &device, uint32_t count);
std::vector<vk::raii::Fence> createFences(const vk::raii::Device &device,
                                          uint32_t count, bool signaled = true);

} // namespace GVK
