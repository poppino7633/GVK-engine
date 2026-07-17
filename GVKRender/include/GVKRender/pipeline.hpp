#pragma once
#include <GVKRender/swapChain.hpp>

namespace GVK {

struct VertexDescription {
  vk::VertexInputBindingDescription bindingDescription;
  std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
};

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
