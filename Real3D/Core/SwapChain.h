#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan/vulkan_core.h>
#include "util/structs.h"

namespace real
{
	class SwapChain final
	{
	public:
		SwapChain() = default;
		~SwapChain() = default;

		SwapChain(const SwapChain&) = delete;
		SwapChain operator=(const SwapChain&) = delete;
		SwapChain(SwapChain&&) = delete;
		SwapChain operator=(SwapChain&&) = delete;

		void CreateSwapChain(const GameContext& context);
		void CreateSwapChainImages(const GameContext& context);

		void CleanUp(const GameContext& context) const;

		VkSwapchainKHR GetSwapChain() const { return m_SwapChain; }
		const std::vector<VkImage>& GetImages() const { return m_SwapChainImages; }
		VkFormat GetFormat() const { return m_SwapChainImageFormat; }
		VkExtent2D GetExtent() const { return m_SwapChainExtent; }
		const std::vector<VkImageView>& GetImageViews() const { return m_SwapChainImageViews; }

	private:
		VkSwapchainKHR m_SwapChain;
		std::vector<VkImage> m_SwapChainImages;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;
		std::vector<VkImageView> m_SwapChainImageViews;

		static SwapChainSupportDetails QuerySwapChainSupport(const GameContext& context);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(SDL_Window* pWindow, const VkSurfaceCapabilitiesKHR& capabilities) const;
		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	};
}

#endif // SWAPCHAIN_H