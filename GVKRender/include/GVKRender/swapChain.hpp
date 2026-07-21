#pragma once
#include <GVKRender/device.hpp>
#include <GVKRender/window.hpp>
#include <GVKRender/sync.hpp>
#include <GVKRender/image.hpp>

namespace GVK {

struct SwapChainImage {
  vk::Image image;
  vk::raii::ImageView imageView;
  Image depthImage;
};

struct SwapChain {
  vk::raii::SwapchainKHR handle = nullptr;
  vk::Extent2D extent;
  uint32_t minImageCount;
  vk::SurfaceFormatKHR surfaceFormat;
  vk::Format depthFormat;
  std::vector<SwapChainImage> images;
  std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
};

SwapChain createSwapChain(const vk::raii::Device &device,
                          vk::raii::PhysicalDevice physicalDevice,
                          GLFWwindow *window,
                          const vk::raii::SurfaceKHR &surface,
                          SwapChain *oldSwapChain = nullptr);
} // namespace GVK
