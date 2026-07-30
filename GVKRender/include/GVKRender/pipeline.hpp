#pragma once
#include <GVKRender/swapChain.hpp>
#include <glm/mat4x4.hpp>

namespace GVK {

struct VertexDescription {
  vk::VertexInputBindingDescription bindingDescription;
  std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
};

struct PushConstants {
  glm::mat4 modelMatrix;
  glm::mat4 normalMatrix;
};

struct PipelineFamily {
  vk::raii::PipelineLayout pipelineLayout;
  std::vector<vk::raii::Pipeline> pipelines;
};

struct PipelineHandle {
  const vk::raii::PipelineLayout &layout;
  const vk::raii::Pipeline &pipeline;
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
    const std::vector<vk::DescriptorSetLayout> descriptorSetLayouts);

PipelineHandle getPipelineHandle(const PipelineFamily &family, size_t index);

} // namespace GVK
