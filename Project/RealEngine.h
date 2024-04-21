#ifndef REALENGINE3D_H
#define REALENGINE3D_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <iostream>
#include <vulkan/vulkan_core.h>

#include "Mesh/MeshIndexed.h"
#include "Util/Structs.h"

class Renderer;
class SwapChain;

class RealEngine final
{
public:
	explicit RealEngine() = default;
	~RealEngine() = default;

	RealEngine(const RealEngine&) = delete;
	RealEngine operator=(const RealEngine&) = delete;
	RealEngine(RealEngine&&) = delete;
	RealEngine operator=(RealEngine&&) = delete;

	void Run();

private:
	GameContext m_GameContext{};

	VkInstance m_Instance{ nullptr };
	VkDebugUtilsMessengerEXT m_DebugMessenger{ nullptr };

	uint8_t m_pPosCol2D, m_pPosColNorm;
	Mesh<PosCol2D>* m_pTriangle{ nullptr };
	MeshIndexed<PosCol2D>* m_pRectangle{ nullptr };

	MeshIndexed<PosColNorm>* m_pColoredCube{ nullptr };
	MeshIndexed<PosColNorm>* m_pColoredPyramid{ nullptr };
	MeshIndexed<PosTexNorm>* m_pTexturedCube{ nullptr };
	MeshIndexed<PosTexNorm>* m_pModel{ nullptr };

	const std::vector<const char*> m_ValidationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};

	const std::vector<const char*> m_DeviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	void InitSDL();
	void InitWindow();
	void InitSDLImage();
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

#endif // REALENGINE3D_H