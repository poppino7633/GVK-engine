#include <GVK/debug.hpp>
#include <iostream>
#include <vulkan/vulkan_raii.hpp>

static VKAPI_ATTR vk::Bool32 VKAPI_CALL
debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
              vk::DebugUtilsMessageTypeFlagsEXT type,
              const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData) {
  if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
    const char *colorCode =
        (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
            ? "\033[31m"  // red
            : "\033[33m"; // yellow
    std::cerr << colorCode << "Validation layer: Type " << to_string(type)
              << " Msg: " << pCallbackData->pMessage << "\033[0m" << std::endl;
  }
  return vk::False;
}
namespace GVK {
vk::raii::DebugUtilsMessengerEXT
createDebugMessenger(const vk::raii::Instance &instance, void *userData) {
  vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
  vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
      vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
      vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
  vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{
      .messageSeverity = severityFlags,
      .messageType = messageTypeFlags,
      .pfnUserCallback = &debugCallback,
      .pUserData = userData};

  return instance.createDebugUtilsMessengerEXT(
      debugUtilsMessengerCreateInfoEXT);
}

} // namespace GVK
