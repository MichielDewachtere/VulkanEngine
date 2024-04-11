#ifndef VULKANUTIL_H
#define VULKANUTIL_H

#define MAX_FRAMES_IN_FLIGHT 2

#include "structs.h"
#include "Concepts.h"

// TODO: Delete this and rewrite it somewehre else
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#include <vector>
#include <fstream>
#include <vulkan/vulkan_core.h>

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

uint32_t FindMemoryType(const GameContext& context, uint32_t typeFilter, VkMemoryPropertyFlags properties);

void CreateImage(const GameContext& context, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
VkImageView CreateImageView(const GameContext& context, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

template<vertex_type V>
VkPipelineVertexInputStateCreateInfo GetVertexInputInfo()
{
	VkPipelineVertexInputStateCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	info.pVertexAttributeDescriptions = V::GetAttributeDescriptions().data();
	info.vertexAttributeDescriptionCount = static_cast<uint32_t>(V::attribute_count);

	const VkVertexInputBindingDescription bindingDescription = V::GetBindingDescription();
	info.pVertexBindingDescriptions = &bindingDescription;
	info.vertexBindingDescriptionCount = 1;

	// ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
	return info;
}
VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo(VkPrimitiveTopology topology);

void CreateBuffer(const GameContext& context, VkDeviceSize size, VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

VkImageView CreateImageView(const GameContext& context, VkImage image, VkFormat format);

std::vector<char> readFile(const std::string& filename);

#endif // VULKANUTIL_H