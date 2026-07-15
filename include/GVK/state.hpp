#include <GVK/buffer.hpp>
#include <GVK/command.hpp>
#include <GVK/descriptor.hpp>
#include <GVK/image.hpp>
#include <GVK/pipeline.hpp>
#include <GVK/surface.hpp>
#include <GVK/sync.hpp>
#include <GVK/texture.hpp>
#include <GVK/window.hpp>
#ifndef NDEBUG
#include <GVK/debug.hpp>
#endif

namespace GVK {

struct State {
  vk::raii::Context context;
  vk::raii::Instance instance = nullptr;
  vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
  vk::raii::SurfaceKHR surface = nullptr;
  vk::raii::PhysicalDevice physicalDevice = nullptr;
  uint32_t queueFamilyIndex;
  vk::raii::Device device = nullptr;
  vk::raii::Queue queue = nullptr;
  GVK::SwapChain swapChain;
  vk::raii::DescriptorPool descriptorPool = nullptr;
  vk::raii::CommandPool commandPool = nullptr;

  State(GLFWwindow *window, const std::vector<const char *> &validationLayers,
        const std::vector<const char *> &deviceExtensions,
        const std::vector<vk::DescriptorPoolSize> &descriptorPoolSizes,
        uint32_t maxDescriptorCount);
};

struct FrameState {
  vk::raii::CommandBuffer commandBuffer;
  vk::raii::DescriptorSet descriptorSet;
  vk::raii::Semaphore presentCompleteSemaphore;
  vk::raii::Fence inFlightFence;
  BufferMapped ubo;
};

void recreateSwapChain(GVK::State &state, GLFWwindow *window);

std::vector<FrameState>
createFrameStates(const State &state,
                  const vk::raii::DescriptorSetLayout &descriptorSetLayout,
                  std::vector<BufferMapped> uniformBuffers,
                  const Texture &texture);
} // namespace GVK
