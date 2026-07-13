#pragma once
#include <GVK/swapChain.hpp>
#include <GVK/vertex.hpp>

namespace GVK {
vk::raii::ShaderModule createShaderModule(const vk::raii::Device &device,
                                          const std::vector<char> &code);

vk::raii::PipelineLayout createPipelineLayout(const vk::raii::Device &device, const vk::raii::DescriptorSetLayout &descriptorSetLayout);

vk::raii::Pipeline createGraphicsPipeline(
    const vk::raii::Device &device, vk::raii::ShaderModule shaderModule,
    const VertexDescription &VertexDescription,
    const vk::raii::PipelineLayout &pipelineLayout, const SwapChain &swapChain);

} // namespace GVK
