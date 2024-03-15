#ifndef STRUCTS_H
#define STRUCTS_H

#include <optional>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>
#include <iostream>

#pragma region Engine
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

	// TODO: https://vulkan-tutorial.com/Vertex_buffers/Staging_buffer
	//std::optional<uint32_t> transferFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value()/* && transferFamily.has_value()*/;
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};
#pragma endregion Engine
#pragma region Vertex Structurs
struct PosCol2D
{
	glm::vec2 pos;
	glm::vec3 color;

	//static VkVertexInputBindingDescription GetBindingDescription()
	//{
	//	VkVertexInputBindingDescription bindingDescription{};
	//	bindingDescription.binding = 0;
	//	bindingDescription.stride = sizeof(PosCol2D);
	//	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	//	return bindingDescription;
	//}

	//static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
	//{
	//	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

	//	attributeDescriptions[0].binding = 0;
	//	attributeDescriptions[0].location = 0;
	//	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	//	attributeDescriptions[0].offset = 0;

	//	attributeDescriptions[1].binding = 0;
	//	attributeDescriptions[1].location = 1;
	//	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	//	attributeDescriptions[1].offset = 8;

	//	return attributeDescriptions;
	//}
};

struct TrianglePosCol
{
	PosCol2D vertex1;
	PosCol2D vertex2;
	PosCol2D vertex3;

	TrianglePosCol(PosCol2D v1, PosCol2D v2, PosCol2D v3)
		: vertex1(v1), vertex2(v2), vertex3(v3) {}
};

struct QuadPosCol
{
	PosCol2D vertex1;
	PosCol2D vertex2;
	PosCol2D vertex3;
	PosCol2D vertex4;
};
#pragma endregion Vertex Structures

#endif // STRUCTS_H