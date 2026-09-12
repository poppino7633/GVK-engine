#include <GVKRender/render.hpp>

namespace GVK {
void beginFrame(State &state, FrameState &frameState, const Window &window) {
  auto fenceResult = state.device.waitForFences(*frameState.inFlightFence,
                                                vk::True, UINT64_MAX);
  if (fenceResult != vk::Result::eSuccess) {
    throw std::runtime_error("Failed to wait for fence!");
  }

  auto [acquireResult, swapChainImageIndex] =
      state.swapChain.handle.acquireNextImage(
          UINT64_MAX, *frameState.presentCompleteSemaphore, nullptr);
  frameState.swapChainImageIndex = swapChainImageIndex;
  SwapChainImage &swapChainImage = state.swapChain.images[swapChainImageIndex];

  if (acquireResult == vk::Result::eErrorOutOfDateKHR) {
    recreateSwapChain(state, window.handle);
  }
  if (acquireResult != vk::Result::eSuccess &&
      acquireResult != vk::Result::eSuboptimalKHR) {
    throw std::runtime_error("Failed to acquire swap chain image!");
  }

  // Only reset the fence if we are submitting work
  state.device.resetFences(*frameState.inFlightFence);
  frameState.commandBuffer.reset();
  frameState.commandBuffer.begin({});

  transitionImageLayout(frameState.commandBuffer, swapChainImage.image,
                        vk::ImageLayout::eUndefined,
                        vk::ImageLayout::eColorAttachmentOptimal,
                        vk::ImageAspectFlagBits::eColor);
  transitionImageLayout(
      frameState.commandBuffer, swapChainImage.depthImage.handle,
      vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthAttachmentOptimal,
      vk::ImageAspectFlagBits::eDepth);

  vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.05f, 0.05f, 1.0f);
  vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);
  vk::RenderingAttachmentInfo colorAttachmentInfo = {
      .imageView = swapChainImage.imageView,
      .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .clearValue = clearColor};

  vk::RenderingAttachmentInfo depthAttachmentInfo = {
      .imageView = swapChainImage.depthImage.view,
      .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eDontCare,
      .clearValue = clearDepth};

  vk::RenderingInfo renderingInfo = {
      .renderArea = {.offset = {0, 0}, .extent = state.swapChain.extent},
      .layerCount = 1,
      .colorAttachmentCount = 1,
      .pColorAttachments = &colorAttachmentInfo,
      .pDepthAttachment = &depthAttachmentInfo};

  frameState.commandBuffer.beginRendering(renderingInfo);
  frameState.commandBuffer.setViewport(
      0, vk::Viewport(
             0.0f, 0.0f, static_cast<float>(state.swapChain.extent.width),
             static_cast<float>(state.swapChain.extent.height), 0.0f, 1.0f));
  frameState.commandBuffer.setScissor(
      0, vk::Rect2D(vk::Offset2D(0, 0), state.swapChain.extent));
}

void bindPipeline(const FrameState &frameState,
                  const PipelineHandle &pipeline) {
  frameState.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                        pipeline.pipeline);
}

void endFrame(State &state, FrameState &frameState, Window &window) {

  frameState.commandBuffer.endRendering();
  SwapChainImage &swapChainImage =
      state.swapChain.images[frameState.swapChainImageIndex];
  transitionImageLayout(frameState.commandBuffer, swapChainImage.image,
                        vk::ImageLayout::eColorAttachmentOptimal,
                        vk::ImageLayout::ePresentSrcKHR,
                        vk::ImageAspectFlagBits::eColor);
  frameState.commandBuffer.end();
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
          &*state.swapChain
                .renderFinishedSemaphores[frameState.swapChainImageIndex]};
  state.queue.submit(submitInfo, *frameState.inFlightFence);

  vk::PresentInfoKHR presentInfo{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores =
          &*state.swapChain
                .renderFinishedSemaphores[frameState.swapChainImageIndex],
      .swapchainCount = 1,
      .pSwapchains = &*state.swapChain.handle,
      .pImageIndices = &frameState.swapChainImageIndex};
  auto presentResult = state.queue.presentKHR(presentInfo);
  if ((presentResult == vk::Result::eSuboptimalKHR) ||
      (presentResult == vk::Result::eErrorOutOfDateKHR) ||
      window.framebufferResized) {
    window.framebufferResized = false;
    recreateSwapChain(state, window.handle);
  } else {
    // There are no other success codes than eSuccess; on any error code,
    // presentKHR already threw an exception.
    assert(presentResult == vk::Result::eSuccess);
  }
}

void drawMesh(const GVK::FrameState &frameState, const Mesh &mesh,
              const PipelineHandle &pipeline,
              const PushConstants &pushConstants) {
  frameState.commandBuffer.bindVertexBuffers(0, *mesh.vertexBuffer, {0});
  frameState.commandBuffer.bindIndexBuffer(*mesh.indexBuffer, 0,
                                           vk::IndexType::eUint32);
  frameState.commandBuffer.pushConstants(
      pipeline.layout, vk::ShaderStageFlagBits::eAllGraphics, 0,
      sizeof(PushConstants), &pushConstants);
  frameState.commandBuffer.drawIndexed(mesh.indexCount, 1, 0, 0, 0);
}

void bindMaterial(const GVK::FrameState &frameState,
                  const PipelineHandle &pipeline,
                  const MaterialSystem &materialSystem,
                  const Material &material) {
  frameState.commandBuffer.bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics, pipeline.layout, 1,
      *materialSystem.descriptorSets[material.descriptorSetIndex], {});
}

}; // namespace GVK
