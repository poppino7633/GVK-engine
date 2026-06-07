#include <GVK/command.hpp>
#include <GVK/pipeline.hpp>
#include <GVK/surface.hpp>
#include <GVK/sync.hpp>
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
  vk::raii::PipelineLayout pipelineLayout = nullptr;
  vk::raii::Pipeline graphicsPipeline = nullptr;
  vk::raii::CommandPool commandPool = nullptr;
  std::vector<vk::raii::CommandBuffer> commandBuffers;
  std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
  std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
  std::vector<vk::raii::Fence> inFlightFences;

  State(GLFWwindow *window, const std::vector<const char *> &validationLayers,
        const std::vector<const char *> &deviceExtensions,
        uint32_t maxFramesInFlight);
};

void recreateSwapChain(GVK::State &state, GLFWwindow *window);

} // namespace GVK
