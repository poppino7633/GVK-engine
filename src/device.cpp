#include <GVK/device.hpp>

namespace GVK {
vk::raii::Device
createDevice(vk::raii::PhysicalDevice physicalDevice, uint32_t queueFamilyIndex,
             const std::vector<const char *> &extensions) {

  float queuePriority = 0.5f;
  vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
      .queueFamilyIndex = queueFamilyIndex,
      .queueCount = 1,
      .pQueuePriorities = &queuePriority};

  vk::StructureChain<vk::PhysicalDeviceFeatures2,
                     vk::PhysicalDeviceVulkan11Features,
                     vk::PhysicalDeviceVulkan13Features,
                     vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
      featureChain = {
          {}, // vk::PhysicalDeviceFeatures2 (empty for now)
          {.shaderDrawParameters = true},
          {.synchronization2 = true,
           .dynamicRendering =
               true}, // Enable dynamic rendering from Vulkan 1.3
          {.extendedDynamicState =
               true} // Enable extended dynamic state from the extension
      };

  vk::DeviceCreateInfo deviceCreateInfo{
      .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &deviceQueueCreateInfo,
      .enabledExtensionCount =
          static_cast<uint32_t>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),
  };

  return vk::raii::Device(physicalDevice, deviceCreateInfo);
}
} // namespace GVK
