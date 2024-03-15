#ifndef ENGINE_H
#define ENGINE_H

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <iostream>
#include <vulkan/vulkan_core.h>
#include "Util/Structs.h"
class Mesh;
class Material;
class Renderer;
class SwapChain;

class Engine final
{
public:
	explicit Engine() = default;
	~Engine() = default;

	Engine(const Engine&) = delete;
	Engine operator=(const Engine&) = delete;
	Engine(Engine&&) = delete;
	Engine operator=(Engine&&) = delete;

	void Run();

	static QueueFamilyIndices FindQueueFamilies(const GameContext& context);
	static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

private:
	GameContext m_GameContext{};

	VkInstance m_Instance{ nullptr };
	VkDebugUtilsMessengerEXT m_DebugMessenger{ nullptr };
	//Renderer* m_pRenderer;

	Material* m_pMaterial;
	Mesh* m_pTriangle, *m_pRectangle;

	const std::vector<const char*> m_ValidationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};

	const std::vector<const char*> m_DeviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	void InitWindow();
	void InitVulkan();
	void InitRenderer();
	//void InitImGui();
	void InitGame();

	void MainLoop();
	void CleanUp();

	void CreateInstance();
	static std::vector<const char*> GetRequiredExtensions();
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	bool CheckValidationLayerSupport();

	void SetupDebugMessenger();
	void CreateSurface();

	void PickPhysicalDevice();
	bool IsDeviceSuitable(const VkPhysicalDevice& device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

	void CreateLogicalDevice();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}

};

#endif // ENGINE_H