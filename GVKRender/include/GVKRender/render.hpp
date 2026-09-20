#pragma once
#include <GVKRender/mesh.hpp>
#include <GVKRender/state.hpp>

namespace GVK {

template <typename PC> struct DrawCommand {
  const Drawable *drawable;
  PC pushConstants;
  uint32_t instanceCount = 1;
  uint32_t firstInstance = 0;
};

void beginFrame(GVK::State &state, GVK::FrameState &frameState,
                const GVK::Window &window);

void endFrame(GVK::State &state, GVK::FrameState &frameState,
              GVK::Window &window);

template <typename PC>
void bindPipeline(FrameState &frameState,
                  const PipelineHandle<PC> &pipeline) {
  if(frameState.currentPipeline == *pipeline.pipeline) {
    return;
  }
  frameState.currentPipeline = *pipeline.pipeline;
  frameState.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                        pipeline.pipeline);
}

template<typename PC>
void draw(GVK::FrameState &frameState, const Drawable &drawable,
          const PipelineHandle<PC> &pipeline, PC pushConstants,
          uint32_t instanceCount, uint32_t firstInstance) {
  bindPipeline(frameState, pipeline);
  frameState.commandBuffer.bindVertexBuffers(0, *drawable.vertexBuffer, {0});
  frameState.commandBuffer.bindIndexBuffer(*drawable.indexBuffer, 0,
                                           vk::IndexType::eUint32);
  frameState.commandBuffer.pushConstants(
      pipeline.layout, vk::ShaderStageFlagBits::eAllGraphics, 0,
      sizeof(pushConstants), &pushConstants);
  frameState.commandBuffer.drawIndexed(drawable.indexCount, instanceCount,
                                       drawable.firstIndex,
                                       drawable.vertexOffset, firstInstance);
}


void bindMaterial(const GVK::FrameState &frameState,
                  const vk::raii::PipelineLayout &pipelineLayout,
                  const MaterialSystem &materialSystem,
                  const Material &material);
}
; // namespace GVK
