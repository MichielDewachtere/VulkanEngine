#ifndef STRUCTS_H
#define STRUCTS_H

#include <optional>
#include <string>
#include <vector>
#include <array>
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#pragma region Engine
struct VulkanContext
{
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkSurfaceKHR surface;
	VkRenderPass renderPass;
};

struct GameContext
{
	uint32_t windowWidth{ 800 }, windowHeight{ 600 };
	std::string windowTitle{ "Vulkan Tutorial" };
	float inputUpdateFrequency{ 0.016f };	// => one update every 16 milliseconds or 60 FPS
	VulkanContext vulkanContext;
	SDL_Window* pWindow;
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

	static VkVertexInputBindingDescription GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(PosCol2D);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static constexpr size_t attribute_count = 2; // Define the size of the attribute array

	static std::array<VkVertexInputAttributeDescription, attribute_count> GetAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, attribute_count> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(PosCol2D, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(PosCol2D, color);

		return attributeDescriptions;
	}
};

struct alignas(16) PosColNorm
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec3 normal;

	static VkVertexInputBindingDescription GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(PosColNorm);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static constexpr size_t attribute_count = 3; // Define the size of the attribute array

	static std::array<VkVertexInputAttributeDescription, attribute_count> GetAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, attribute_count> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(PosColNorm, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(PosColNorm, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(PosColNorm, normal);

		return attributeDescriptions;
	}
};

//struct TrianglePosCol
//{
//	PosCol2D vertex1;
//	PosCol2D vertex2;
//	PosCol2D vertex3;
//
//	TrianglePosCol(PosCol2D v1, PosCol2D v2, PosCol2D v3)
//		: vertex1(v1), vertex2(v2), vertex3(v3) {}
//};
//
//struct QuadPosCol
//{
//	PosCol2D vertex1;
//	PosCol2D vertex2;
//	PosCol2D vertex3;
//	PosCol2D vertex4;
//};
#pragma endregion Vertex Structures
#pragma region Uniform Buffer
struct alignas(16) UniformBufferObject
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};
#pragma endregion Uniform Buffer

#endif // STRUCTS_H