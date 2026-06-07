#include <GVK/sync.hpp>

namespace GVK {
std::vector<vk::raii::Semaphore>
createSemaphores(const vk::raii::Device &device, uint32_t count) {
  std::vector<vk::raii::Semaphore> semaphores;
  for (size_t i = 0; i < count; i++) {
    semaphores.emplace_back(device, vk::SemaphoreCreateInfo());
  }
  return semaphores;
}

std::vector<vk::raii::Fence> createFences(const vk::raii::Device &device,
                                          uint32_t count, bool signaled) {

  std::vector<vk::raii::Fence> fences;
  for (size_t i = 0; i < count; i++) {
    fences.emplace_back(
        device, vk::FenceCreateInfo{
                    .flags = signaled ? vk::FenceCreateFlagBits::eSignaled
                                      : vk::FenceCreateFlags{}});
  }
  return fences;
}
} // namespace GVK
