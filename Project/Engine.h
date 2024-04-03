#ifndef ENGINE_H
#define ENGINE_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <iostream>
#include <vulkan/vulkan_core.h>
#include "Util/Structs.h"
#include "Mesh/MeshIndexed.h"

class Renderer;
class SwapChain;
class Material;

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

	Material * m_pPosCol2D{ nullptr }, * m_pPosColNorm{ nullptr };
	Mesh<PosCol2D>* m_pTriangle{ nullptr };
	MeshIndexed<PosCol2D>* m_pRectangle{ nullptr };

	MeshIndexed<PosColNorm>* m_pCube1{ nullptr }, * m_pCube2{ nullptr };

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
	std::vector<const char*> GetRequiredExtensions();
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