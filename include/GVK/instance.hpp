#pragma once
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif


namespace GVK {
std::vector<const char *> getRequiredInstanceExtensions(); 
vk::raii::Instance
createInstance(const vk::raii::Context &context,
               const std::vector<const char *> &validationLayers = {},
               const std::vector<const char *> &extensions = {});

}
