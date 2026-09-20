#include <GVKRender/pipeline.hpp>

namespace GVK {
[[nodiscard]] vk::raii::ShaderModule
createShaderModule(const vk::raii::Device &device,
                   const std::vector<char> &code) {
  vk::ShaderModuleCreateInfo createInfo{
      .codeSize = code.size() * sizeof(char),
      .pCode = reinterpret_cast<const uint32_t *>(code.data())};
  return vk::raii::ShaderModule(device, createInfo);
#include "vulkan/vulkan.hpp"
}

vk::raii::PipelineLayout createPipelineLayout(
    const vk::raii::Device &device,
    const std::vector<vk::DescriptorSetLayout> &descriptorSetLayouts, uint32_t pushConstantSize) {

  vk::PushConstantRange pushConstantRange = {
      .stageFlags = vk::ShaderStageFlagBits::eAllGraphics,
      .offset = 0,
      .size = pushConstantSize};

  vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
      .setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
      .pSetLayouts = descriptorSetLayouts.data(),
      .pushConstantRangeCount = 1,
      .pPushConstantRanges = &pushConstantRange};

  return vk::raii::PipelineLayout(device, pipelineLayoutInfo);
}

vk::raii::Pipeline
createGraphicsPipeline(const vk::raii::Device &device,
                       vk::raii::ShaderModule shaderModule,
                       const VertexDescription &vertexDescription,
                       const vk::raii::PipelineLayout &pipelineLayout,
                       PipelineConfig config, const SwapChain &swapChain) {

  vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
      .stage = vk::ShaderStageFlagBits::eVertex,
      .module = shaderModule,
      .pName = "vertMain"};

  vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
      .stage = vk::ShaderStageFlagBits::eFragment,
      .module = shaderModule,
      .pName = "fragMain"};

  vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                      fragShaderStageInfo};

  std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport,
                                                 vk::DynamicState::eScissor};

  vk::PipelineDynamicStateCreateInfo dynamicState{
      .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
      .pDynamicStates = dynamicStates.data()};

  vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
      .vertexBindingDescriptionCount = 1,
      .pVertexBindingDescriptions = &vertexDescription.bindingDescription,
      .vertexAttributeDescriptionCount =
          static_cast<uint32_t>(vertexDescription.attributeDescriptions.size()),
      .pVertexAttributeDescriptions =
          vertexDescription.attributeDescriptions.data()};

  vk::PipelineInputAssemblyStateCreateInfo inputAssembly{.topology =
                                                             config.topology};

  vk::Viewport viewport{0.0f,
                        0.0f,
                        static_cast<float>(swapChain.extent.width),
                        static_cast<float>(swapChain.extent.height),
                        0.0f,
                        1.0f};

  vk::Rect2D scissor{vk::Offset2D{0, 0}, swapChain.extent};
  vk::PipelineViewportStateCreateInfo viewportState{.viewportCount = 1,
                                                    .scissorCount = 1};

  vk::PipelineRasterizationStateCreateInfo rasterizer{
      .depthClampEnable = vk::False,
      .rasterizerDiscardEnable = vk::False,
      .polygonMode = vk::PolygonMode::eFill,
      .cullMode = vk::CullModeFlagBits::eBack,
      .frontFace = vk::FrontFace::eCounterClockwise,
      .depthBiasEnable = vk::False,
      .lineWidth = 1.0f};

  vk::PipelineMultisampleStateCreateInfo multisampling{
      .rasterizationSamples = vk::SampleCountFlagBits::e1,
      .sampleShadingEnable = vk::False};

  vk::PipelineColorBlendAttachmentState colorBlendAttachment{
      .blendEnable = vk::True,
      .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
      .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
      .colorBlendOp = vk::BlendOp::eAdd,
      .srcAlphaBlendFactor = vk::BlendFactor::eOne,
      .dstAlphaBlendFactor = vk::BlendFactor::eZero,
      .alphaBlendOp = vk::BlendOp::eAdd,
      .colorWriteMask =
          vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};

  vk::PipelineColorBlendStateCreateInfo colorBlending{
      .logicOpEnable = vk::False,
      .logicOp = vk::LogicOp::eCopy,
      .attachmentCount = 1,
      .pAttachments = &colorBlendAttachment};

  vk::PipelineDepthStencilStateCreateInfo depthStencil{
      .depthTestEnable = vk::True,
      .depthWriteEnable = vk::True,
      .depthCompareOp = vk::CompareOp::eLess,
      .depthBoundsTestEnable = vk::False,
      .stencilTestEnable = vk::False};

  vk::StructureChain<vk::GraphicsPipelineCreateInfo,
                     vk::PipelineRenderingCreateInfo>
      pipelineCreateInfoChain = {
          {.stageCount = 2,
           .pStages = shaderStages,
           .pVertexInputState = &vertexInputInfo,
           .pInputAssemblyState = &inputAssembly,
           .pViewportState = &viewportState,
           .pRasterizationState = &rasterizer,
           .pMultisampleState = &multisampling,
           .pDepthStencilState = &depthStencil,
           .pColorBlendState = &colorBlending,
           .pDynamicState = &dynamicState,
           .layout = pipelineLayout,
           .renderPass = nullptr},
          {
              .colorAttachmentCount = 1,
              .pColorAttachmentFormats = &swapChain.surfaceFormat.format,
              .depthAttachmentFormat = swapChain.depthFormat,
          }};

  return vk::raii::Pipeline(
      device, nullptr,
      pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
}


} // namespace GVK
