#pragma once
#include <GVKRender/instance.hpp>

namespace GVK {

bool isPhysicalDeviceSuitable(
    vk::raii::PhysicalDevice physicalDevice,
    const std::vector<const char *> &requiredDeviceExtensions,
    const vk::raii::SurfaceKHR &surface);

std::optional<vk::raii::PhysicalDevice>
pickPhysicalDevice(const std::vector<vk::raii::PhysicalDevice> &physicalDevices,
                   const std::vector<const char *> &requiredDeviceExtensions,
                   const vk::raii::SurfaceKHR &surface);

std::optional<uint32_t>
getPhysicalDeviceQueueFamilyIndex(vk::raii::PhysicalDevice physicalDevice,
                                  const vk::raii::SurfaceKHR &surface);

uint32_t findMemoryType(const vk::raii::PhysicalDevice &physicalDevice,
                        uint32_t typeFilter,
                        vk::MemoryPropertyFlags properties);

} // namespace GVK
