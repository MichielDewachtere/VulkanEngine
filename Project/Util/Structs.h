#ifndef STRUCTS_H
#define STRUCTS_H

#include <optional>
#include <string>
#include <vector>
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
};

struct alignas(16) PosColNorm
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec3 normal;
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