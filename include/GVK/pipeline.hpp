#pragma once
#include <GVK/swapChain.hpp>

namespace GVK {
vk::raii::ShaderModule createShaderModule(const vk::raii::Device &device,
                                          const std::vector<char> &code);

vk::raii::PipelineLayout createPipelineLayout(const vk::raii::Device &device);

vk::raii::Pipeline createGraphicsPipeline(
    const vk::raii::Device &device, vk::raii::ShaderModule shaderModule,
    const vk::raii::PipelineLayout &pipelineLayout, const SwapChain &swapChain);

} // namespace GVK
