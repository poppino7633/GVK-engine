#pragma once
#include <GVKRender/swapChain.hpp>
#include <glm/mat4x4.hpp>

namespace GVK {

struct VertexDescription {
  vk::VertexInputBindingDescription bindingDescription;
  std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
};

struct PipelineConfig {
  vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;
};

template <typename PC> struct PipelineFamily {
  vk::raii::PipelineLayout pipelineLayout;
  std::vector<vk::raii::Pipeline> pipelines;
};

template<typename PC>
struct PipelineHandle {
  const vk::raii::PipelineLayout &layout;
  const vk::raii::Pipeline &pipeline;
};

vk::raii::ShaderModule createShaderModule(const vk::raii::Device &device,
                                          const std::vector<char> &code);


vk::raii::Pipeline
createGraphicsPipeline(const vk::raii::Device &device,
                       vk::raii::ShaderModule shaderModule,
                       const VertexDescription &vertexDescription,
                       const vk::raii::PipelineLayout &pipelineLayout,
                       PipelineConfig config, const SwapChain &swapChain);


template <typename PC>
void addGraphicsPipeline(const vk::raii::Device &device,
                         PipelineFamily<PC> &pipelineFamily,
                         vk::raii::ShaderModule shaderModule,
                         const VertexDescription &vertexDescription,
                         const SwapChain &swapChain,
                         PipelineConfig config = {}) {
  pipelineFamily.pipelines.emplace_back(std::move(createGraphicsPipeline(
      device, std::move(shaderModule), vertexDescription,
      pipelineFamily.pipelineLayout, config, swapChain)));
}

vk::raii::PipelineLayout createPipelineLayout(
    const vk::raii::Device &device,
    const std::vector<vk::DescriptorSetLayout> &descriptorSetLayouts,
    uint32_t pushConstantSize);

template <typename PC>
PipelineFamily<PC> createPipelineFamily(
    const vk::raii::Device &device,
    const std::vector<vk::DescriptorSetLayout> descriptorSetLayouts) {
  vk::raii::PipelineLayout pipelineLayout =
      createPipelineLayout(device, descriptorSetLayouts, sizeof(PC));

  return {.pipelineLayout = std::move(pipelineLayout), .pipelines = {}};
}

template <typename PC>
PipelineHandle<PC> getPipelineHandle(const PipelineFamily<PC> &family,
                                 size_t index) {

  return {family.pipelineLayout, family.pipelines[index]};
}

} // namespace GVK
