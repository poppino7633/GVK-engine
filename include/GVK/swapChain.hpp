#pragma once
#include <GVK/device.hpp>
#include <GVK/window.hpp>

namespace GVK {

struct SwapChain {
  vk::raii::SwapchainKHR handle = nullptr;
  vk::Extent2D extent;
  uint32_t minImageCount;
  vk::SurfaceFormatKHR surfaceFormat;
  std::vector<vk::Image> images;
  std::vector<vk::raii::ImageView> imageViews;
};

SwapChain createSwapChain(const vk::raii::Device &device,
                          vk::raii::PhysicalDevice physicalDevice,
                          GLFWwindow *window,
                          const vk::raii::SurfaceKHR &surface,
                          SwapChain *oldSwapChain = nullptr);

} // namespace GVK
