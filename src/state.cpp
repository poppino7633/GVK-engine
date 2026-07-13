#include <GVK/state.hpp>

namespace GVK {
State::State(GLFWwindow *window,
             const std::vector<const char *> &validationLayers,
             const std::vector<const char *> &deviceExtensions,
             uint32_t maxDescriptorCount) {
  instance = GVK::createInstance(context, validationLayers,
                                 GVK::getRequiredInstanceExtensions());
  if (!validationLayers.empty()) {
    debugMessenger = GVK::createDebugMessenger(instance, this);
  }
  surface = GVK::createSurface(instance, window);

  physicalDevice = [&]() {
    auto result = GVK::pickPhysicalDevice(instance.enumeratePhysicalDevices(),
                                          deviceExtensions, surface);
    if (!result)
      throw std::runtime_error("No suitable physical device found");
    queueFamilyIndex =
        GVK::getPhysicalDeviceQueueFamilyIndex(*result, surface).value();
    return std::move(*result);
  }();
  device =
      GVK::createDevice(physicalDevice, queueFamilyIndex, deviceExtensions);
  queue = vk::raii::Queue(device, queueFamilyIndex, 0);
  swapChain = GVK::createSwapChain(device, physicalDevice, window, surface);

  descriptorPool = GVK::createDescriptorPool(device, maxDescriptorCount);

  commandPool = GVK::createCommandPool(device, queueFamilyIndex);
  renderFinishedSemaphores =
      GVK::createSemaphores(device, swapChain.images.size());
}

std::vector<FrameState>
createFrameStates(const State &state,
                  const vk::raii::DescriptorSetLayout &descriptorSetLayout,
                  std::vector<GVK::BufferMapped> &uniformBuffers,
                  uint32_t count) {
  auto commandBuffers =
      GVK::createCommandBuffers(state.device, state.commandPool, count);
  auto descriptorSets = GVK::allocateDescriptorSets(
      state.device, state.descriptorPool, count, descriptorSetLayout);
  GVK::updateDescriptorSets(state.device, descriptorSets, uniformBuffers);
  auto presentCompleteSemaphores = GVK::createSemaphores(state.device, count);
  auto inFlightFences = GVK::createFences(state.device, count);

  std::vector<FrameState> frameStates;
  frameStates.reserve(count);

  for (uint32_t i = 0; i < count; ++i) {
    frameStates.emplace_back(FrameState{
        .commandBuffer = std::move(commandBuffers[i]),
        .descriptorSet = std::move(descriptorSets[i]),
        .presentCompleteSemaphore = std::move(presentCompleteSemaphores[i]),
        .inFlightFence = std::move(inFlightFences[i]),
        .ubo = uniformBuffers[i]
    });
  }

  return frameStates;
}

void recreateSwapChain(GVK::State &state, GLFWwindow *window) {

  state.device.waitIdle();
  state.swapChain =
      GVK::createSwapChain(state.device, state.physicalDevice, window,
                           state.surface, &state.swapChain);

  state.renderFinishedSemaphores =
      GVK::createSemaphores(state.device, state.swapChain.images.size());
}

} // namespace GVK
