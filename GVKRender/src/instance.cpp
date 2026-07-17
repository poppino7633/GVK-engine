#include <GVKRender/instance.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace GVK {

std::vector<const char *> getRequiredInstanceExtensions() {
  uint32_t glfwExtensionCount = 0;
  auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

  return extensions;
}

vk::raii::Instance
createInstance(const vk::raii::Context &context,
               const std::vector<const char *> &validationLayers,
               const std::vector<const char *> &extensions) {

  const vk::ApplicationInfo appInfo{.pApplicationName = "GVK",
                                    .applicationVersion =
                                        VK_MAKE_VERSION(1, 0, 0),
                                    .pEngineName = "GVK",
                                    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                                    .apiVersion = vk::ApiVersion14};

  std::vector<char const *> requiredLayers = validationLayers;

  auto layerProperties = context.enumerateInstanceLayerProperties();
  auto unsupportedLayerIt = std::ranges::find_if(
      requiredLayers, [&layerProperties](auto const &requiredLayer) {
        return std::ranges::none_of(
            layerProperties, [requiredLayer](auto const &layerProperty) {
              return strcmp(layerProperty.layerName, requiredLayer) == 0;
            });
      });

  if (unsupportedLayerIt != requiredLayers.end()) {
    throw std::runtime_error("Required layer not supported: " +
                             std::string(*unsupportedLayerIt));
  }

  auto requiredExtensions = extensions;
  if (!requiredLayers.empty()) {
    requiredExtensions.push_back(vk::EXTDebugUtilsExtensionName);
  }

  auto extensionProperties = context.enumerateInstanceExtensionProperties();
  auto unsupportedPropertyIt = std::ranges::find_if(
      requiredExtensions,
      [&extensionProperties](auto const &requiredExtension) {
        return std::ranges::none_of(
            extensionProperties,
            [requiredExtension](auto const &extensionProperty) {
              return strcmp(extensionProperty.extensionName,
                            requiredExtension) == 0;
            });
      });

  if (unsupportedPropertyIt != requiredExtensions.end()) {
    throw std::runtime_error("Required extension not supported: " +
                             std::string(*unsupportedPropertyIt));
  }

  vk::InstanceCreateInfo createInfo{
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
      .ppEnabledLayerNames = requiredLayers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
      .ppEnabledExtensionNames = requiredExtensions.data(),

  };

  return vk::raii::Instance(context, createInfo);
}

} // namespace GVK
