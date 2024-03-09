#ifndef STRUCTS_H
#define STRUCTS_H

#include <optional>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>


struct VulkanContext
{
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkSurfaceKHR surface;
};

struct GameContext
{
	uint32_t windowWidth{ 800 }, windowHeight{ 600 };
	std::string windowTitle{ "Vulkan Tutorial" };
	float inputUpdateFrequency{ 0.016f };	// => one update every 16 milliseconds or 60 FPS
	VulkanContext vulkanContext;
	GLFWwindow* pWindow;
};

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

#endif // STRUCTS_H