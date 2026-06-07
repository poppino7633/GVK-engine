#include <GVK/state.hpp>

namespace GVK {
State::State(GLFWwindow *window,
             const std::vector<const char *> &validationLayers,
             const std::vector<const char *> &deviceExtensions,
             uint32_t maxFramesInFlight) {
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

  commandPool = GVK::createCommandPool(device, queueFamilyIndex);
  commandBuffers =
      GVK::createCommandBuffers(device, commandPool, maxFramesInFlight);
  renderFinishedSemaphores =
      GVK::createSemaphores(device, swapChain.images.size());

  presentCompleteSemaphores = GVK::createSemaphores(device, maxFramesInFlight);
  inFlightFences = GVK::createFences(device, maxFramesInFlight);
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
