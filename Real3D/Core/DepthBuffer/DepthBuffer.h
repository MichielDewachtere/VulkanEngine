#ifndef DEPTHBUFFER_H
#define DEPTHBUFFER_H

#include <vulkan/vulkan_core.h>

#include "Util/Structs.h"

namespace real
{
	class DepthBuffer
	{
	public:
		explicit DepthBuffer(const GameContext& context);
		~DepthBuffer() = default;

		DepthBuffer(const DepthBuffer&) = delete;
		DepthBuffer& operator=(const DepthBuffer&) = delete;
		DepthBuffer(DepthBuffer&&) = delete;
		DepthBuffer& operator=(DepthBuffer&&) = delete;

		void Create(const GameContext& context);
		void CleanUp(const GameContext& context);

		VkImage GetImage() const { return m_DepthImage; }
		VkImageView GetImageView() const { return m_DepthImageView; }

		static VkFormat FindSupportedFormat(const GameContext& context, const std::vector<VkFormat>& candidates,
											VkImageTiling tiling, VkFormatFeatureFlags features);
		static VkFormat FindDepthFormat(const GameContext& context);

	private:
		VkImage m_DepthImage{};
		VkDeviceMemory m_DepthImageMemory{};
		VkImageView m_DepthImageView{};

		static bool HasStencilComponent(VkFormat format);
	};
}

#endif // DEPTHBUFFER_H