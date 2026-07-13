#pragma once
#include <GVK/swapChain.hpp>
#include <GVK/vertex.hpp>

namespace GVK {

struct PipelineFamily {
  vk::raii::DescriptorSetLayout descriptorSetLayout;
  vk::raii::PipelineLayout pipelineLayout;

  std::vector<vk::raii::Pipeline> pipelines;
};

vk::raii::ShaderModule createShaderModule(const vk::raii::Device &device,
                                          const std::vector<char> &code);

void addGraphicsPipeline(const vk::raii::Device &device,
                         PipelineFamily &pipelineFamily,
                         vk::raii::ShaderModule shaderModule,
                         const VertexDescription &vertexDescription,
                         const SwapChain &swapChain);

PipelineFamily createPipelineFamily(
    const vk::raii::Device &device,
    const std::vector<vk::DescriptorSetLayoutBinding> &bindings);

} // namespace GVK
