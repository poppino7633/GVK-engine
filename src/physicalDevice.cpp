#include <GVK/physicalDevice.hpp>

namespace GVK {

std::optional<uint32_t>
getPhysicalDeviceQueueFamilyIndex(vk::raii::PhysicalDevice physicalDevice,
                                  const vk::raii::SurfaceKHR &surface) {

  auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
  uint32_t queueIndex = ~0;
  for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size();
       qfpIndex++) {
    if ((queueFamilyProperties[qfpIndex].queueFlags &
         vk::QueueFlagBits::eGraphics) &&
        physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface)) {
      // found a queue family that supports both graphics and present
      queueIndex = qfpIndex;
      break;
    }
  }
  if (queueIndex == ~0) {
    return std::nullopt;
  }
  return queueIndex;
}

bool isPhysicalDeviceSuitable(
    vk::raii::PhysicalDevice physicalDevice,
    const std::vector<const char *> &requiredDeviceExtensions,
    const vk::raii::SurfaceKHR &surface) {

  auto result = getPhysicalDeviceQueueFamilyIndex(physicalDevice, surface);
  if (!result)
    return false;

  auto deviceProperties = physicalDevice.getProperties();
  auto deviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();

  bool supportsRequiredExtensions = std::ranges::all_of(
      requiredDeviceExtensions,
      [&deviceExtensions](auto const &requiredDeviceExtension) {
        return std::ranges::any_of(
            deviceExtensions,
            [requiredDeviceExtension](auto const &deviceExtension) {
              return strcmp(deviceExtension.extensionName,
                            requiredDeviceExtension) == 0;
            });
      });

  auto requiredDeviceFeatures = physicalDevice.template getFeatures2<
      vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
      vk::PhysicalDeviceVulkan13Features,
      vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
  bool supportsRequiredFeatures =
      requiredDeviceFeatures.template get<vk::PhysicalDeviceVulkan11Features>()
          .shaderDrawParameters &&
      requiredDeviceFeatures.template get<vk::PhysicalDeviceVulkan13Features>()
          .dynamicRendering &&
      requiredDeviceFeatures.template get<vk::PhysicalDeviceVulkan13Features>()
          .synchronization2 &&
      requiredDeviceFeatures
          .template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()
          .extendedDynamicState;

  return supportsRequiredExtensions && supportsRequiredFeatures;
}

std::optional<vk::raii::PhysicalDevice>
pickPhysicalDevice(const std::vector<vk::raii::PhysicalDevice> &physicalDevices,
                   const std::vector<const char *> &requiredDeviceExtensions,
                   const vk::raii::SurfaceKHR &surface) {

  auto const pickedPhysicalDevice =
      std::ranges::find_if(physicalDevices, [&](auto const &physicalDevice) {
        return isPhysicalDeviceSuitable(physicalDevice,
                                        requiredDeviceExtensions, surface);
      });
  if (pickedPhysicalDevice == physicalDevices.end()) {
    return std::nullopt;
  }
  return *pickedPhysicalDevice;
}

uint32_t findMemoryType(const vk::raii::PhysicalDevice &physicalDevice,
                        uint32_t typeFilter,
                        vk::MemoryPropertyFlags properties) {

  vk::PhysicalDeviceMemoryProperties memProperties =
      physicalDevice.getMemoryProperties();
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return i;
    }
  }
  throw std::runtime_error("Failed to find suitable memory type!");
}



} // namespace GVK
