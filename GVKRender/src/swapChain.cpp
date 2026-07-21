#include "vulkan/vulkan.hpp"
#include <GVKRender/image.hpp>
#include <GVKRender/swapChain.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace GVK {
vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
  const auto formatIt =
      std::ranges::find_if(availableFormats, [](const auto &format) {
        return format.format == vk::Format::eB8G8R8A8Srgb &&
               format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
      });
  return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
}

vk::PresentModeKHR chooseSwapPresentMode(
    std::vector<vk::PresentModeKHR> const &availablePresentModes) {
  assert(std::ranges::any_of(availablePresentModes, [](auto presentMode) {
    return presentMode == vk::PresentModeKHR::eFifo;
  }));
  return std::ranges::any_of(availablePresentModes,
                             [](const vk::PresentModeKHR value) {
                               return vk::PresentModeKHR::eMailbox == value;
                             })
             ? vk::PresentModeKHR::eMailbox
             : vk::PresentModeKHR::eFifo;
}

vk::Extent2D chooseSwapExtent(GLFWwindow *window,
                              vk::SurfaceCapabilitiesKHR const &capabilities) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  }
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);

  return {std::clamp<uint32_t>(width, capabilities.minImageExtent.width,
                               capabilities.maxImageExtent.width),
          std::clamp<uint32_t>(height, capabilities.minImageExtent.height,
                               capabilities.maxImageExtent.height)};
}

uint32_t
chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const &surfaceCapabilities) {
  auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
  if ((0 < surfaceCapabilities.maxImageCount) &&
      (surfaceCapabilities.maxImageCount < minImageCount)) {
    minImageCount = surfaceCapabilities.maxImageCount;
  }
  return minImageCount;
}

vk::Format findDepthFormat(const vk::raii::PhysicalDevice &physicalDevice,
                           const std::vector<vk::Format> &candidates,
                           vk::ImageTiling tiling,
                           vk::FormatFeatureFlags features) {
  for (const auto format : candidates) {
    vk::FormatProperties props = physicalDevice.getFormatProperties(format);

    if (((tiling == vk::ImageTiling::eLinear) &&
         ((props.linearTilingFeatures & features) == features)) ||
        ((tiling == vk::ImageTiling::eOptimal) &&
         ((props.optimalTilingFeatures & features) == features))) {
      return format;
    }
  }

  throw std::runtime_error("failed to find supported format!");
}

SwapChain createSwapChain(const vk::raii::Device &device,
                          vk::raii::PhysicalDevice physicalDevice,
                          GLFWwindow *window,
                          const vk::raii::SurfaceKHR &surface,
                          SwapChain *oldSwapChain) {
  vk::SurfaceCapabilitiesKHR surfaceCapabilities =
      physicalDevice.getSurfaceCapabilitiesKHR(*surface);
  auto swapChainExtent = chooseSwapExtent(window, surfaceCapabilities);
  uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);

  std::vector<vk::SurfaceFormatKHR> availableFormats =
      physicalDevice.getSurfaceFormatsKHR(*surface);
  auto swapChainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);

  std::vector<vk::PresentModeKHR> availablePresentModes =
      physicalDevice.getSurfacePresentModesKHR(*surface);

  vk::SwapchainCreateInfoKHR swapChainCreateInfo{
      .surface = *surface,
      .minImageCount = minImageCount,
      .imageFormat = swapChainSurfaceFormat.format,
      .imageColorSpace = swapChainSurfaceFormat.colorSpace,
      .imageExtent = swapChainExtent,
      .imageArrayLayers = 1,
      .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
      .imageSharingMode = vk::SharingMode::eExclusive,
      .preTransform = surfaceCapabilities.currentTransform,
      .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
      .presentMode = chooseSwapPresentMode(availablePresentModes),
      .clipped = true};

  if (oldSwapChain) {
    swapChainCreateInfo.oldSwapchain = *oldSwapChain->handle;
    oldSwapChain->images.clear();
  }

  auto handle = vk::raii::SwapchainKHR(device, swapChainCreateInfo);
  auto swapChainImages = handle.getImages();

  vk::Format depthFormat =
      findDepthFormat(physicalDevice,
                      {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint,
                       vk::Format::eD24UnormS8Uint},
                      vk::ImageTiling::eOptimal,
                      vk::FormatFeatureFlagBits::eDepthStencilAttachment);

  SwapChain swapChain{
      .handle = std::move(handle),
      .extent = swapChainExtent,
      .minImageCount = minImageCount,
      .surfaceFormat = swapChainSurfaceFormat,
      .depthFormat = depthFormat,
  };


  for (auto &image : swapChainImages) {
    swapChain.images.emplace_back(SwapChainImage{
        image,
        createImageView(device, image, swapChainSurfaceFormat.format,
                        vk::ImageAspectFlagBits::eColor),
        createImage(device, physicalDevice, swapChainExtent.width,
                    swapChainExtent.height, depthFormat,
                    vk::ImageTiling::eOptimal,
                    vk::ImageUsageFlagBits::eDepthStencilAttachment,
                    vk::MemoryPropertyFlagBits::eDeviceLocal,
                    vk::ImageAspectFlagBits::eDepth)});
  }
  swapChain.renderFinishedSemaphores =
      GVK::createSemaphores(device, swapChain.images.size());

  return swapChain;
}

} // namespace GVK
