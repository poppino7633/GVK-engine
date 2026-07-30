#include <GVKRender/state.hpp>

namespace GVK {
State::State(GLFWwindow *window,
             const std::vector<const char *> &validationLayers,
             const std::vector<const char *> &deviceExtensions,
             const std::vector<vk::DescriptorPoolSize> &descriptorPoolSizes,
             uint32_t maxMaterials, uint32_t maxDescriptorCount) {
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

  std::vector<vk::DescriptorPoolSize> materialPoolSizes =
      Material::getPoolSizes(maxMaterials);
  std::vector<vk::DescriptorPoolSize> poolSizes;
  poolSizes.reserve(descriptorPoolSizes.size() + materialPoolSizes.size());
  poolSizes.insert(poolSizes.begin(), descriptorPoolSizes.begin(),
                   descriptorPoolSizes.end());
  poolSizes.insert(poolSizes.begin() + descriptorPoolSizes.size(),
                   materialPoolSizes.begin(), materialPoolSizes.end());
  descriptorPool =
      GVK::createDescriptorPool(device, poolSizes, maxDescriptorCount);

  commandPool = GVK::createCommandPool(device, queueFamilyIndex);

  materialSystem = createMaterialSystem(device, physicalDevice, descriptorPool,
                                        maxMaterials);
}

std::vector<FrameState>
createFrameStates(const State &state,
                  const vk::raii::DescriptorSetLayout &descriptorSetLayout,
                  std::vector<BufferMapped> uniformBuffers) {

  const uint32_t count = uniformBuffers.size();
  auto commandBuffers =
      createCommandBuffers(state.device, state.commandPool, count);
  auto descriptorSets = allocateDescriptorSets(
      state.device, state.descriptorPool, count, descriptorSetLayout);

  for (size_t i = 0; i < descriptorSets.size(); i++) {
    vk::DescriptorBufferInfo bufferInfo = {*uniformBuffers[i].buffer, 0,
                                           uniformBuffers[i].bufferSize};
    std::array<vk::WriteDescriptorSet, 1> descriptorWrites{
        {{.dstSet = descriptorSets[i],
          .dstBinding = 0,
          .dstArrayElement = 0,
          .descriptorCount = 1,
          .descriptorType = vk::DescriptorType::eUniformBuffer,
          .pBufferInfo = &bufferInfo}}};

    state.device.updateDescriptorSets(descriptorWrites, {});
  }

  auto presentCompleteSemaphores = createSemaphores(state.device, count);
  auto inFlightFences = createFences(state.device, count);

  std::vector<FrameState> frameStates;
  frameStates.reserve(count);

  for (uint32_t i = 0; i < count; ++i) {
    frameStates.emplace_back(FrameState{
        .commandBuffer = std::move(commandBuffers[i]),
        .descriptorSet = std::move(descriptorSets[i]),
        .presentCompleteSemaphore = std::move(presentCompleteSemaphores[i]),
        .inFlightFence = std::move(inFlightFences[i]),
        .ubo = std::move(uniformBuffers[i])});
  }

  return frameStates;
}

void recreateSwapChain(GVK::State &state, GLFWwindow *window) {

  state.device.waitIdle();
  state.swapChain =
      GVK::createSwapChain(state.device, state.physicalDevice, window,
                           state.surface, &state.swapChain);
}

} // namespace GVK
