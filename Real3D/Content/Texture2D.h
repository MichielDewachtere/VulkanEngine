#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <string>

#include <vulkan/vulkan_core.h>

#include "Util/structs.h"

namespace real
{
	class Texture2D final
	{
	public:
		explicit Texture2D(const std::string& path, const GameContext& context);
		~Texture2D() = default;

		Texture2D(const Texture2D&) = delete;
		Texture2D& operator=(const Texture2D&) = delete;
		Texture2D(Texture2D&&) = delete;
		Texture2D& operator=(Texture2D&&) = delete;

		void CleanUp(const GameContext& context) const;

		VkImage GetTextureImage() const { return m_TextureImage; }
		//VkDeviceMemory GetDeviceMemory() const { return m_TextureImageMemory; }
		VkImageView GetTextureImageView() const { return m_TextureImageView; }
		VkSampler GetTextureSampler() const { return m_TextureSampler; }

	private:
		VkImage m_TextureImage{};
		VmaAllocation m_TextureAllocation{};
		//VkDeviceMemory m_TextureImageMemory{};
		VkImageView m_TextureImageView{};
		VkSampler m_TextureSampler{};

		void CreateTextureImage(const GameContext& context, const std::string& path);
		void CreateTextureImageView(const GameContext& context);
		void CreateTextureSampler(const GameContext& context);

		static void TransitionImageLayout(const GameContext& context, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		static void CopyBufferToImage(const GameContext& context, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	};
}

#endif // TEXTURE2D_H