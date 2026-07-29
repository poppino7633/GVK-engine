#pragma once
#include <GVKRender/mesh.hpp>
#include <GVKRender/state.hpp>

namespace GVK {
void beginFrame(GVK::State &state, GVK::FrameState &frameState,
                const GVK::Window &window);

void bindPipeline(const FrameState &frameState,
                  const PipelineHandle &pipeline);

void endFrame(GVK::State &state, GVK::FrameState &frameState,
              GVK::Window &window);

void drawMesh(const GVK::FrameState &frameState, const Mesh &mesh,
              const PipelineHandle &pipeline, const PushConstants &pushConstants);
}; // namespace GVK
