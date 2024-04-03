#ifndef VULKANUTIL_H
#define VULKANUTIL_H

#define MAX_FRAMES_IN_FLIGHT 2

#include "structs.h"

// TODO: Delete this and rewrite it somewehre else
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#include <vector>
#include <fstream>

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

uint32_t FindMemoryType(const GameContext& context, uint32_t typeFilter, VkMemoryPropertyFlags properties);

void CreateImage(const GameContext& context, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
VkImageView CreateImageView(const GameContext& context, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

std::vector<char> readFile(const std::string& filename);

#endif // VULKANUTIL_H