#include "DepthBuffer.h"

#include <stdexcept>

#include "Graphics/Renderer.h"
#include "Core/SwapChain.h"
#include "Util/VulkanUtil.h"

DepthBuffer::DepthBuffer(const GameContext& context)
{
	Create(context);
}

void DepthBuffer::Create(const GameContext& context)
{
	const VkFormat depthFormat = FindDepthFormat(context);

	const auto [width, height] = Renderer::GetInstance().GetSwapChain()->GetExtent();

	CreateImage(context, width, height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthImage, m_DepthImageMemory);
	m_DepthImageView = CreateImageView(context, m_DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void DepthBuffer::CleanUp(const GameContext& context)
{
	vkDestroyImageView(context.vulkanContext.device, m_DepthImageView, nullptr);
	vkDestroyImage(context.vulkanContext.device, m_DepthImage, nullptr);
	vkFreeMemory(context.vulkanContext.device, m_DepthImageMemory, nullptr);
}

VkFormat DepthBuffer::FindSupportedFormat(const GameContext& context, const std::vector<VkFormat>& candidates,
                                          const VkImageTiling tiling, const VkFormatFeatureFlags features)
{
	for (const VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(context.vulkanContext.physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			return format;

		if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			return format;
	}

	throw std::runtime_error("failed to find supported format!");
}

VkFormat DepthBuffer::FindDepthFormat(const GameContext& context)
{
	return FindSupportedFormat(
		context,
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

bool DepthBuffer::HasStencilComponent(const VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}
