#include <GVKRender/image.hpp>

namespace GVK {

std::pair<vk::raii::Image, vk::raii::DeviceMemory>
createImage(const vk::raii::Device &device,
            const vk::raii::PhysicalDevice &physicalDevice, uint32_t width,
            uint32_t height, vk::Format format, vk::ImageTiling tiling,
            vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties) {
  vk::ImageCreateInfo imageInfo{.imageType = vk::ImageType::e2D,
                                .format = format,
                                .extent = {width, height, 1},
                                .mipLevels = 1,
                                .arrayLayers = 1,
                                .samples = vk::SampleCountFlagBits::e1,
                                .tiling = tiling,
                                .usage = usage,
                                .sharingMode = vk::SharingMode::eExclusive};

  vk::raii::Image image = vk::raii::Image(device, imageInfo);

  vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
  vk::MemoryAllocateInfo allocInfo{
      .allocationSize = memRequirements.size,
      .memoryTypeIndex = GVK::findMemoryType(
          physicalDevice, memRequirements.memoryTypeBits, properties)};
  vk::raii::DeviceMemory imageMemory =
      vk::raii::DeviceMemory(device, allocInfo);
  image.bindMemory(imageMemory, 0);

  return {std::move(image), std::move(imageMemory)};
}

void transitionImageLayout(const vk::raii::CommandBuffer &commandBuffer,
                           vk::Image image, vk::ImageLayout oldLayout,
                           vk::ImageLayout newLayout) {
  vk::ImageMemoryBarrier barrier{
      .oldLayout = oldLayout,
      .newLayout = newLayout,
      .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
      .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
      .image = image,
      .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor,
                           .levelCount = 1,
                           .layerCount = 1}};

  vk::PipelineStageFlags sourceStage;
  vk::PipelineStageFlags destinationStage;

  if (oldLayout == vk::ImageLayout::eUndefined &&
      newLayout == vk::ImageLayout::eTransferDstOptimal) {
    barrier.srcAccessMask = {};
    barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

    sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
    destinationStage = vk::PipelineStageFlagBits::eTransfer;
  } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal &&
             newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    sourceStage = vk::PipelineStageFlagBits::eTransfer;
    destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
  } else if (oldLayout == vk::ImageLayout::eUndefined &&
             newLayout == vk::ImageLayout::eColorAttachmentOptimal) {
    barrier.srcAccessMask = {};
    barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    sourceStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    destinationStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
  } else if (oldLayout == vk::ImageLayout::eColorAttachmentOptimal &&
             newLayout == vk::ImageLayout::ePresentSrcKHR) {
    barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    barrier.dstAccessMask = {};

    sourceStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    destinationStage = vk::PipelineStageFlagBits::eBottomOfPipe;
  } else {
    throw std::invalid_argument("unsupported layout transition!");
  }

  commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, nullptr,
                                barrier);
}

void copyBufferToImage(vk::raii::CommandBuffer &commandBuffer,
                       const vk::raii::Buffer &buffer, vk::raii::Image &image,
                       uint32_t width, uint32_t height) {
  vk::BufferImageCopy region{
      .bufferOffset = 0,
      .bufferRowLength = 0,
      .bufferImageHeight = 0,
      .imageSubresource = {.aspectMask = vk::ImageAspectFlagBits::eColor,
                           .mipLevel = 0,
                           .baseArrayLayer = 0,
                           .layerCount = 1},
      .imageOffset = {0, 0, 0},
      .imageExtent = {width, height, 1}};

  commandBuffer.copyBufferToImage(buffer, image,
                                  vk::ImageLayout::eTransferDstOptimal, region);
}

vk::raii::ImageView createImageView(const vk::raii::Device &device,
                                    const vk::Image &image, vk::Format format) {
  vk::ImageViewCreateInfo viewInfo{
      .image = image,
      .viewType = vk::ImageViewType::e2D,
      .format = format,
      .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor,
                           .baseMipLevel = 0,
                           .levelCount = 1,
                           .baseArrayLayer = 0,
                           .layerCount = 1}};
  return vk::raii::ImageView(device, viewInfo);
}

} // namespace GVK
