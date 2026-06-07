#pragma once
#include <GVK/physicalDevice.hpp>

namespace GVK {
vk::raii::Device createDevice(vk::raii::PhysicalDevice physicalDevice,
                              uint32_t queueFamilyIndex,
                              const std::vector<const char *> &extensions);

}
