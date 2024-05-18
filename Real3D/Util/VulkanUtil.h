#ifndef VULKANUTIL_H
#define VULKANUTIL_H

#define MAX_FRAMES_IN_FLIGHT 2

#include <vector>
#include <fstream>
#include <vulkan/vulkan_core.h>

#include "Structs.h"
#include "Concepts.h"

namespace real
{
#ifdef NDEBUG
	const bool enableValidationLayers = true;
#else
	const bool enableValidationLayers = true;
#endif


	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	uint32_t FindMemoryType(const GameContext& context, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

	void CreateImage(const real::GameContext& context, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
		VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VmaAllocation& imageAllocation);
	VkImageView CreateImageView(const GameContext& context, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	VkImageView CreateImageView(const GameContext& context, VkImage image, VkFormat format);

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

	void CreateBuffer(const real::GameContext& context, VkDeviceSize size, VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties, VkBuffer& buffer, VmaAllocation& bufferAllocation);


	std::vector<char> ReadFile(const std::string& filename);

	template <typename T>
	std::vector<T> fillUntilSize(std::vector<T>& src, std::vector<T>& dest, std::size_t targetSize)
	{
		// Determine the number of elements to copy
		std::size_t numToCopy = std::min(targetSize - dest.size(), src.size());

		// Copy elements to the destination vector
		std::copy_n(src.begin(), numToCopy, std::back_inserter(dest));

		// Remove the copied elements from the source vector
		std::vector<T> leftOver(src.begin() + numToCopy, src.end());
		src = leftOver;

		return leftOver;
	}
}

#endif // VULKANUTIL_H