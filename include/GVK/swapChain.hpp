#pragma once
#include <GVK/device.hpp>
#include <GVK/window.hpp>

namespace GVK {

struct SwapChainImage {
  vk::raii::ImageView imageView;
  vk::Image image;
};

struct SwapChain {
  vk::raii::SwapchainKHR handle = nullptr;
  vk::Extent2D extent;
  uint32_t minImageCount;
  vk::SurfaceFormatKHR surfaceFormat;
  std::vector<SwapChainImage> images;
};

SwapChain createSwapChain(const vk::raii::Device &device,
                          vk::raii::PhysicalDevice physicalDevice,
                          GLFWwindow *window,
                          const vk::raii::SurfaceKHR &surface,
                          SwapChain *oldSwapChain = nullptr);
} // namespace GVK
