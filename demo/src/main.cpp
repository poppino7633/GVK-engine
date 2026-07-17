#include "vulkan/vulkan.hpp"
#include <filesystem>
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <GVKRender/state.hpp>
#include <GVKAsset/image.hpp>
#include <demo/vertex.hpp>

#include <chrono>
#include <cstdlib>
#include <fstream>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <stdexcept>

constexpr uint32_t WIDTH = 600;
constexpr uint32_t HEIGHT = 600;
constexpr int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<GVK::Vertex3D> vertices = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4
};

struct Matrices {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;

  static vk::DescriptorSetLayoutBinding getBinding() {
    return {.binding = 0,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = 1,
            .stageFlags = vk::ShaderStageFlagBits::eVertex};
  }
  static vk::DescriptorPoolSize getPoolSize(uint32_t count) {
    return {.type = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = count};
  }
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

void recordCommandBuffer(const GVK::FrameState &frameState,
                         const GVK::SwapChainImage &swapChainImage,
                         vk::Extent2D swapChainExtent,
                         const GVK::PipelineFamily &pipelineFamily,
                         const vk::raii::Buffer &vertexBuffer,
                         const vk::raii::Buffer &indexBuffer) {
  assert(pipelineFamily.pipelines.size() > 0);
  frameState.commandBuffer.begin({});
  GVK::transitionImageLayout(frameState.commandBuffer, swapChainImage.image,
                             vk::ImageLayout::eUndefined,
                             vk::ImageLayout::eColorAttachmentOptimal);

  vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
  vk::RenderingAttachmentInfo attachmentInfo = {
      .imageView = swapChainImage.imageView,
      .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .clearValue = clearColor};

  vk::RenderingInfo renderingInfo = {
      .renderArea = {.offset = {0, 0}, .extent = swapChainExtent},
      .layerCount = 1,
      .colorAttachmentCount = 1,
      .pColorAttachments = &attachmentInfo};

  frameState.commandBuffer.beginRendering(renderingInfo);
  frameState.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                        *pipelineFamily.pipelines[0]);
  frameState.commandBuffer.bindVertexBuffers(0, *vertexBuffer, {0});
  frameState.commandBuffer.bindIndexBuffer(*indexBuffer, 0,
                                           vk::IndexType::eUint16);
  frameState.commandBuffer.setViewport(
      0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapChainExtent.width),
                      static_cast<float>(swapChainExtent.height), 0.0f, 1.0f));
  frameState.commandBuffer.setScissor(
      0, vk::Rect2D(vk::Offset2D(0, 0), swapChainExtent));

  frameState.commandBuffer.bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics, pipelineFamily.pipelineLayout, 0,
      *frameState.descriptorSet, nullptr);

  // Actual drawing code

  frameState.commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1,
                                       0, 0, 0);

  frameState.commandBuffer.endRendering();
  // After rendering, transition the swapchain image to
  // vk::ImageLayout::ePresentSrcKHR
  GVK::transitionImageLayout(frameState.commandBuffer, swapChainImage.image,
                             vk::ImageLayout::eColorAttachmentOptimal,
                             vk::ImageLayout::ePresentSrcKHR);
  frameState.commandBuffer.end();
}

void updateMatricesUBO(GVK::BufferMapped &matricesBuffer,
                       vk::Extent2D swapChainExtent) {
  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(
                   currentTime - startTime)
                   .count();
  Matrices ubo{};
  ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
                          glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.proj = glm::perspective(glm::radians(90.0f),
                              static_cast<float>(swapChainExtent.width) /
                                  static_cast<float>(swapChainExtent.height),
                              0.1f, 10.0f);
  ubo.proj[1][1] *= -1;
  memcpy(matricesBuffer.ptr, &ubo, sizeof(ubo));
}

void drawFrame(GVK::State &state, GVK::FrameState &frameState,
               const GVK::PipelineFamily &pipelineFamily,
               const vk::raii::Buffer &vertexBuffer,
               const vk::raii::Buffer &indexBuffer, GVK::Window &window) {
  auto fenceResult = state.device.waitForFences(*frameState.inFlightFence,
                                                vk::True, UINT64_MAX);
  if (fenceResult != vk::Result::eSuccess) {
    throw std::runtime_error("Failed to wait for fence!");
  }

  auto [acquireResult, swapChainImageIndex] =
      state.swapChain.handle.acquireNextImage(
          UINT64_MAX, *frameState.presentCompleteSemaphore, nullptr);

  if (acquireResult == vk::Result::eErrorOutOfDateKHR) {
    GVK::recreateSwapChain(state, window.handle);
    return;
  }
  if (acquireResult != vk::Result::eSuccess &&
      acquireResult != vk::Result::eSuboptimalKHR) {
    throw std::runtime_error("Failed to acquire swap chain image!");
  }

  // Only reset the fence if we are submitting work
  state.device.resetFences(*frameState.inFlightFence);
  frameState.commandBuffer.reset();
  recordCommandBuffer(frameState, state.swapChain.images[swapChainImageIndex],
                      state.swapChain.extent, pipelineFamily, vertexBuffer,
                      indexBuffer);

  updateMatricesUBO(frameState.ubo, state.swapChain.extent);

  vk::PipelineStageFlags waitDestinationStageMask(
      vk::PipelineStageFlagBits::eColorAttachmentOutput);
  const vk::SubmitInfo submitInfo{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &*frameState.presentCompleteSemaphore,
      .pWaitDstStageMask = &waitDestinationStageMask,
      .commandBufferCount = 1,
      .pCommandBuffers = &*frameState.commandBuffer,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores =
          &*state.swapChain.renderFinishedSemaphores[swapChainImageIndex]};
  state.queue.submit(submitInfo, *frameState.inFlightFence);

  vk::PresentInfoKHR presentInfo{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores =
          &*state.swapChain.renderFinishedSemaphores[swapChainImageIndex],
      .swapchainCount = 1,
      .pSwapchains = &*state.swapChain.handle,
      .pImageIndices = &swapChainImageIndex};
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

void mainLoop(GVK::State &state, std::vector<GVK::FrameState> &frameStates,
              GVK::Window &window, const GVK::PipelineFamily &pipelineFamily,
              const vk::raii::Buffer &vertexBuffer,
              const vk::raii::Buffer &indexBuffer) {
  std::cout << "Starting main loop" << std::endl;
  uint32_t frameIndex = 0;
  while (!glfwWindowShouldClose(window.handle)) {
    glfwPollEvents();
    drawFrame(state, frameStates[frameIndex], pipelineFamily, vertexBuffer,
              indexBuffer, window);
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
      std::vector<vk::DescriptorPoolSize> poolSizes = {
          Matrices::getPoolSize(MAX_FRAMES_IN_FLIGHT),
          GVK::Texture::getPoolSize(MAX_FRAMES_IN_FLIGHT)};
      GVK::State state(window.handle, validationLayers,
                       {vk::KHRSwapchainExtensionName}, poolSizes,
                       MAX_FRAMES_IN_FLIGHT);

      std::vector<vk::DescriptorSetLayoutBinding> bindings = {
          Matrices::getBinding(), GVK::Texture::getBinding()};

      GVK::PipelineFamily pipelineFamily =
          GVK::createPipelineFamily(state.device, bindings);

      GVK::addGraphicsPipeline(
          state.device, pipelineFamily,
          GVK::createShaderModule(state.device, readFile("shaders/slang.spv")),
          GVK::Vertex3D::getVertexDescription(), state.swapChain);

      auto [vertexBuffer, vertexBufferMemory] = GVK::createBufferFromVec(
          state.device, state.physicalDevice, state.commandPool, state.queue,
          vertices, vk::BufferUsageFlagBits::eVertexBuffer);

      auto [indexBuffer, indexBufferMemory] = GVK::createBufferFromVec(
          state.device, state.physicalDevice, state.commandPool, state.queue,
          indices, vk::BufferUsageFlagBits::eIndexBuffer);

      GVK::Texture texture =
          GVK::createTexture(state.device, state.physicalDevice, state.queue,
                             state.commandPool, GVK::loadImage("assets/textures/texture.jpg"));

      std::vector<GVK::FrameState> frameStates = GVK::createFrameStates(
          state, pipelineFamily.descriptorSetLayout,
          GVK::createUniformBuffers<Matrices>(
              state.device, state.physicalDevice, MAX_FRAMES_IN_FLIGHT),
          texture);

      mainLoop(state, frameStates, window, pipelineFamily, vertexBuffer,
               indexBuffer);
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
