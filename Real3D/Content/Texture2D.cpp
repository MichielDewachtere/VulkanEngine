#include "Texture2D.h"

#include <stdexcept>

#include <SDL_image.h>

#include "Core/CommandBuffers/CommandBuffer.h"
#include "Util/VulkanUtil.h"

real::Texture2D::Texture2D(const std::string& path, const GameContext& context)
{
    CreateTextureImage(context, path);
    CreateTextureImageView(context);
    CreateTextureSampler(context);
}

void real::Texture2D::CleanUp(const GameContext& context) const
{
    vkDestroySampler(context.vulkanContext.device, m_TextureSampler, nullptr);
    vkDestroyImageView(context.vulkanContext.device, m_TextureImageView, nullptr);

    vmaDestroyImage(context.vulkanContext.allocator, m_TextureImage, m_TextureAllocation);

    //vkDestroyImage(context.vulkanContext.device, m_TextureImage, nullptr);
    //vkFreeMemory(context.vulkanContext.device, m_TextureImageMemory, nullptr);
}

void real::Texture2D::CreateTextureImage(const GameContext& context, const std::string& path)
{
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        throw std::runtime_error("failed to load texture image!");
    }

    // Convert surface to RGBA format
    SDL_Surface* convertedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface); // Free the original surface

    if (!convertedSurface) {
        throw std::runtime_error("failed to convert surface to RGBA format!");
    }

    const int texWidth = convertedSurface->w;
    const int texHeight = convertedSurface->h;
    const int texChannels = convertedSurface->format->BytesPerPixel;
    const VkDeviceSize imageSize = texWidth * texHeight * texChannels;

    VkBuffer stagingBuffer;
    VmaAllocation stagingBufferAllocation;
    //VkDeviceMemory stagingBufferMemory;

    CreateBuffer(context, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
        stagingBufferAllocation);

    void* data;
    vmaMapMemory(context.vulkanContext.allocator, stagingBufferAllocation, &data);
    memcpy(data, convertedSurface->pixels, imageSize);
    vmaUnmapMemory(context.vulkanContext.allocator, stagingBufferAllocation);

    //void* data;
    //vkMapMemory(context.vulkanContext.device, stagingBufferMemory, 0, imageSize, 0, &data);
    //memcpy(data, convertedSurface->pixels, imageSize);
    //vkUnmapMemory(context.vulkanContext.device, stagingBufferMemory);

    SDL_FreeSurface(convertedSurface);

    CreateImage(context, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_TextureImage, m_TextureAllocation);

    TransitionImageLayout(context, m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(context, stagingBuffer, m_TextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    TransitionImageLayout(context, m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vmaDestroyBuffer(context.vulkanContext.allocator, stagingBuffer, stagingBufferAllocation);
    //vkDestroyBuffer(context.vulkanContext.device, stagingBuffer, nullptr);
    //vkFreeMemory(context.vulkanContext.device, stagingBufferMemory, nullptr);
}

void real::Texture2D::CreateTextureImageView(const GameContext& context)
{
    m_TextureImageView = CreateImageView(context, m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB);
}

void real::Texture2D::CreateTextureSampler(const GameContext& context)
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_NEAREST;
    samplerInfo.minFilter = VK_FILTER_NEAREST;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(context.vulkanContext.physicalDevice, &properties);
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(context.vulkanContext.device, &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void real::Texture2D::TransitionImageLayout(const GameContext& context, VkImage image, VkFormat format,
	VkImageLayout oldLayout, VkImageLayout newLayout)
{
    const VkCommandBuffer commandBuffer = CommandBuffer::StartSingleTimeCommands(context);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    CommandBuffer::StopSingleTimeCommands(context, commandBuffer);
}

void real::Texture2D::CopyBufferToImage(const GameContext& context, VkBuffer buffer, VkImage image, uint32_t width,
	uint32_t height)
{
    const VkCommandBuffer commandBuffer = CommandBuffer::StartSingleTimeCommands(context);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    CommandBuffer::StopSingleTimeCommands(context, commandBuffer);
}
