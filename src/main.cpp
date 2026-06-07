#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <GVK/buffer.hpp>
#include <GVK/state.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>

constexpr uint32_t WIDTH = 600;
constexpr uint32_t HEIGHT = 600;
constexpr int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<GVK::BasicVertex> vertices = {
    // center
    {{0.0f,   0.0f},  {1.0f, 1.0f, 1.0f}},
    // ring
    {{0.0f,  -0.5f},  {1.0f, 0.0f, 0.0f}},   // red
    {{0.433f, -0.25f},{1.0f, 0.5f, 0.0f}},   // orange
    {{0.433f,  0.25f},{1.0f, 1.0f, 0.0f}},   // yellow
    {{0.0f,   0.5f},  {0.0f, 1.0f, 0.0f}},   // green
    {{-0.433f, 0.25f},{0.0f, 0.0f, 1.0f}},   // blue
    {{-0.433f,-0.25f},{0.5f, 0.0f, 1.0f}},   // violet
};

const std::vector<uint16_t> indices = {
    0, 1, 2,
    0, 2, 3,
    0, 3, 4,
    0, 4, 5,
    0, 5, 6,
    0, 6, 1,
};

std::vector<char> readFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  std::vector<char> buffer(file.tellg());

  file.seekg(0, std::ios::beg);
  file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
  file.close();
  return buffer;
}


void recordCommandBuffer(const vk::raii::CommandBuffer &commandBuffer,
                         const GVK::SwapChain &swapChain, uint32_t imageIndex,
                         const vk::raii::Pipeline &graphicsPipeline,
                         const vk::raii::Buffer &vertexBuffer,
                         const vk::raii::Buffer &indexBuffer) {
  commandBuffer.begin({});
  GVK::transition_image_layout(
      commandBuffer, swapChain.images[imageIndex], vk::ImageLayout::eUndefined,
      vk::ImageLayout::eColorAttachmentOptimal,
      {}, // srcAccessMask (no need to wait for previous operations)
      vk::AccessFlagBits2::eColorAttachmentWrite,         // dstAccessMask
      vk::PipelineStageFlagBits2::eColorAttachmentOutput, // srcStage
      vk::PipelineStageFlagBits2::eColorAttachmentOutput  // dstStage
  );

  vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
  vk::RenderingAttachmentInfo attachmentInfo = {
      .imageView = swapChain.imageViews[imageIndex],
      .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .clearValue = clearColor};

  vk::RenderingInfo renderingInfo = {
      .renderArea = {.offset = {0, 0}, .extent = swapChain.extent},
      .layerCount = 1,
      .colorAttachmentCount = 1,
      .pColorAttachments = &attachmentInfo};

  commandBuffer.beginRendering(renderingInfo);
  commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                             *graphicsPipeline);
  commandBuffer.bindVertexBuffers(0, *vertexBuffer, {0});
  commandBuffer.bindIndexBuffer(*indexBuffer, 0, vk::IndexType::eUint16);
  commandBuffer.setViewport(
      0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapChain.extent.width),
                      static_cast<float>(swapChain.extent.height), 0.0f, 1.0f));
  commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChain.extent));

  // Actual drawing code

  commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

  commandBuffer.endRendering();
  // After rendering, transition the swapchain image to
  // vk::ImageLayout::ePresentSrcKHR
  GVK::transition_image_layout(
      commandBuffer, swapChain.images[imageIndex],
      vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR,
      vk::AccessFlagBits2::eColorAttachmentWrite,         // srcAccessMask
      {},                                                 // dstAccessMask
      vk::PipelineStageFlagBits2::eColorAttachmentOutput, // srcStage
      vk::PipelineStageFlagBits2::eBottomOfPipe           // dstStage
  );
  commandBuffer.end();
}

void drawFrame(GVK::State &state, const vk::raii::Pipeline &graphicsPipeline,
               const vk::raii::Buffer &vertexBuffer,
               const vk::raii::Buffer &indexBuffer, GVK::Window &window,
               uint32_t frameIndex) {
  auto fenceResult = state.device.waitForFences(
      *state.inFlightFences[frameIndex], vk::True, UINT64_MAX);
  if (fenceResult != vk::Result::eSuccess) {
    throw std::runtime_error("Failed to wait for fence!");
  }

  auto [result, imageIndex] = state.swapChain.handle.acquireNextImage(
      UINT64_MAX, *state.presentCompleteSemaphores[frameIndex], nullptr);

  if (result == vk::Result::eErrorOutOfDateKHR) {
    GVK::recreateSwapChain(state, window.handle);
    return;
  }
  if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
    assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
    throw std::runtime_error("Failed to acquire swap chain image!");
  }

  // Only reset the fence if we are submitting work
  state.device.resetFences(*state.inFlightFences[frameIndex]);
  state.commandBuffers[frameIndex].reset();
  recordCommandBuffer(state.commandBuffers[frameIndex], state.swapChain,
                      imageIndex, graphicsPipeline, vertexBuffer, indexBuffer);

  vk::PipelineStageFlags waitDestinationStageMask(
      vk::PipelineStageFlagBits::eColorAttachmentOutput);
  const vk::SubmitInfo submitInfo{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &*state.presentCompleteSemaphores[frameIndex],
      .pWaitDstStageMask = &waitDestinationStageMask,
      .commandBufferCount = 1,
      .pCommandBuffers = &*state.commandBuffers[frameIndex],
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &*state.renderFinishedSemaphores[imageIndex]};
  state.queue.submit(submitInfo, *state.inFlightFences[frameIndex]);

  vk::PresentInfoKHR presentInfo{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &*state.renderFinishedSemaphores[imageIndex],
      .swapchainCount = 1,
      .pSwapchains = &*state.swapChain.handle,
      .pImageIndices = &imageIndex};
  auto presentResult = state.queue.presentKHR(presentInfo);
  if ((presentResult == vk::Result::eSuboptimalKHR) ||
      (presentResult == vk::Result::eErrorOutOfDateKHR) ||
      window.framebufferResized) {
    window.framebufferResized = false;
    GVK::recreateSwapChain(state, window.handle);
  } else {
    // There are no other success codes than eSuccess; on any error code,
    // presentKHR already threw an exception.
    assert(presentResult == vk::Result::eSuccess);
  }
}

void mainLoop(GVK::State &state, GVK::Window &window,
              const vk::raii::Pipeline &graphicsPipeline,
              const vk::raii::Buffer &vertexBuffer,
              const vk::raii::Buffer &indexBuffer) {
  std::cout << "Starting main loop" << std::endl;
  uint32_t frameIndex = 0;
  while (!glfwWindowShouldClose(window.handle)) {
    glfwPollEvents();
    drawFrame(state, graphicsPipeline, vertexBuffer, indexBuffer, window,
              frameIndex);
    frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
  }
  state.device.waitIdle();
}

int main() {
  try {
    GVK::Window window(WIDTH, HEIGHT, "GVK", true);
#ifdef NDEBUG
    const std::vector<char const *> validationLayers = {};
#else
    const std::vector<char const *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"};
#endif
    {
      GVK::State state(window.handle, validationLayers,
                       {vk::KHRSwapchainExtensionName}, MAX_FRAMES_IN_FLIGHT);

      vk::raii::PipelineLayout pipelineLayout =
          GVK::createPipelineLayout(state.device);

      vk::raii::Pipeline graphicsPipeline = GVK::createGraphicsPipeline(
          state.device,
          GVK::createShaderModule(state.device, readFile("shaders/slang.spv")),
          GVK::BasicVertex::getDescription(), pipelineLayout, state.swapChain);

      auto [vertexBuffer, vertexBufferMemory] = GVK::createBufferFromVec(
          state.device, state.physicalDevice, state.commandPool, state.queue,
          vertices, vk::BufferUsageFlagBits::eVertexBuffer);

      auto [indexBuffer, indexBufferMemory] = GVK::createBufferFromVec(
          state.device, state.physicalDevice, state.commandPool, state.queue,
          indices, vk::BufferUsageFlagBits::eIndexBuffer);

      mainLoop(state, window, graphicsPipeline, vertexBuffer, indexBuffer);
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
