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
void bindPipeline(const FrameState &frameState,
                  const PipelineHandle<PC> &pipeline) {
  frameState.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                        pipeline.pipeline);
}
template <typename PC>
void draw(const GVK::FrameState &frameState,
          const std::vector<DrawCommand<PC>> &drawCommands,
          const PipelineHandle<PC> &pipeline) {
  bindPipeline(frameState, pipeline);
  for (const DrawCommand<PC> &drawCommand : drawCommands) {
    frameState.commandBuffer.bindVertexBuffers(
        0, *drawCommand.drawable->vertexBuffer, {0});
    frameState.commandBuffer.bindIndexBuffer(*drawCommand.drawable->indexBuffer,
                                             0, vk::IndexType::eUint32);
    frameState.commandBuffer.pushConstants(
        pipeline.layout, vk::ShaderStageFlagBits::eAllGraphics, 0,
        sizeof(drawCommand.pushConstants), &drawCommand.pushConstants);
    frameState.commandBuffer.drawIndexed(
        drawCommand.drawable->indexCount, drawCommand.instanceCount,
        drawCommand.drawable->firstIndex, drawCommand.drawable->vertexOffset,
        drawCommand.firstInstance);
  }
}

void bindMaterial(const GVK::FrameState &frameState,
                  const vk::raii::PipelineLayout &pipelineLayout,
                  const MaterialSystem &materialSystem,
                  const Material &material);

}; // namespace GVK
