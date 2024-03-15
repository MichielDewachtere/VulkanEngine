#include "Engine.h"

#include <set>

#include "Core/CommandPool.h"
#include "Core/SwapChain.h"
#include "Mesh/Mesh.h"
#include "Graphics/ShaderManager.h"

#include "Graphics/Renderer.h"
#include "Material/Pipelines/DefaultPipeline.h"
#include "Material/RenderPasses/DefaultRenderPass.h"
#include "Material/CommandBuffers/CommandBuffer.h"
#include "Util/GameTime.h"

void Engine::Run()
{
	InitWindow();
	InitVulkan();
	InitRenderer();
	InitGame();

	MainLoop();

	CleanUp();
}

QueueFamilyIndices Engine::FindQueueFamilies(const GameContext& context)
{
	return FindQueueFamilies(context.vulkanContext.physicalDevice, context.vulkanContext.surface);
}

QueueFamilyIndices Engine::FindQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport)
			indices.presentFamily = i;

		//if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
		//	indices.transferFamily = i;

		if (indices.isComplete())
			break;

		i++;
	}

	return indices;
}

void Engine::InitWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_GameContext.pWindow = glfwCreateWindow((int)m_GameContext.windowWidth, (int)m_GameContext.windowHeight, m_GameContext.windowTitle.c_str(), nullptr, nullptr);
}

void Engine::InitVulkan()
{
	// Create Instance
	CreateInstance();
	// Set up Debug Messenger
	SetupDebugMessenger();

	// Create Surface
	CreateSurface();

	// Pick PhysicalDevice
	PickPhysicalDevice();

	// Create Logical Device
	CreateLogicalDevice();

	// Create Command Pool
	CommandPool::GetInstance().Init(m_GameContext);
}

void Engine::InitRenderer()
{
	auto& renderer = Renderer::GetInstance();
	renderer.Init(m_GameContext);

	m_pMaterial = new Material();
	// RenderPasses
	m_pMaterial->AddRenderPass<DefaultRenderPass>(m_GameContext, *renderer.GetSwapChain());
	// Graphic Pipelines
	m_pMaterial->AddPipeline<DefaultPipeline>(m_GameContext);
	// Command Buffers
	m_pMaterial->AddCommandBuffer<CommandBuffer>(m_GameContext);

	renderer.AddMaterial(m_pMaterial);

	// Frame Buffers
	renderer.CreateFrameBuffers(m_GameContext);

	// Sync Objects
	renderer.CreateSyncObjects(m_GameContext);

	ShaderManager::GetInstance().DestroyShaderModules(m_GameContext.vulkanContext.device);
}

void Engine::InitGame()
{
	const std::vector triangleVertices =
	{
		PosCol2D{ glm::vec2{0.5f + 0.0f, -0.25f},	glm::vec3{1.0f, 0.0f, 0.0f} },
		PosCol2D{ glm::vec2{0.5f + 0.25f,  0.25f},	glm::vec3{0.0f, 1.0f, 0.0f} },
		PosCol2D{ glm::vec2{0.5f + -0.25f, 0.25f},	glm::vec3{0.0f, 0.0f, 1.0f} },
	};

	const std::vector rectVertices =
	{
		PosCol2D{ glm::vec2{-0.5 + -0.25, -0.25},	glm::vec3{1,0,0} },
		PosCol2D{ glm::vec2{-0.5 + 0.25,  -0.25},	glm::vec3{0,1,0} },
		PosCol2D{ glm::vec2{-0.5 + -0.25,  0.25},	glm::vec3{0,0,1} },
		PosCol2D{ glm::vec2{-0.5 + -0.25,  0.25},	glm::vec3{0,0,1} },
		PosCol2D{ glm::vec2{-0.5 + 0.25,  -0.25},	glm::vec3{0,1,0} },
		PosCol2D{ glm::vec2{-0.5 + 0.25,   0.25},	glm::vec3{1,1,1} },
	};

	m_pTriangle = new Mesh(3);
	for (const auto& v : triangleVertices)
	{
		m_pTriangle->AddVertex(v);
	}
	m_pTriangle->Init(m_GameContext);

	m_pRectangle = new Mesh(6);
	for (const auto& v : rectVertices)
	{
		m_pRectangle->AddVertex(v);
	}
	m_pRectangle->Init(m_GameContext);

	m_pMaterial->BindMesh(m_pTriangle);
	m_pMaterial->BindMesh(m_pRectangle);
}

void Engine::MainLoop()
{
	auto& time = GameTime::GetInstance();
	time.Init();

	float timer = 0;
	constexpr float fpsPrintTime = 1;

	while (!glfwWindowShouldClose(m_GameContext.pWindow))
	{
		glfwPollEvents();

		time.Update();

		Renderer::GetInstance().Draw(m_GameContext);

		timer += time.GetElapsed();
		if (timer >= fpsPrintTime)
		{
			timer = 0;
			std::cout << "FPS: " << time.GetFPS_Unsigned() << '\n';
		}
	}
	vkDeviceWaitIdle(m_GameContext.vulkanContext.device);
}

void Engine::CleanUp()
{
	Renderer::GetInstance().CleanUp(m_GameContext);
	m_pTriangle->CleanUp(m_GameContext);
	m_pRectangle->CleanUp(m_GameContext);

	CommandPool::GetInstance().CleanUp(m_GameContext);

	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
	}

	vkDestroyDevice(m_GameContext.vulkanContext.device, nullptr);

	vkDestroySurfaceKHR(m_Instance, m_GameContext.vulkanContext.surface, nullptr);
	vkDestroyInstance(m_Instance, nullptr);

	glfwDestroyWindow(m_GameContext.pWindow);
	glfwTerminate();
}

void Engine::CreateInstance()
{
	if (enableValidationLayers && !CheckValidationLayerSupport()) 
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = m_GameContext.windowTitle.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "TBD";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	const auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else 
	{
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create m_Instance!");
	}
}

std::vector<const char*> Engine::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void Engine::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
}

bool Engine::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : m_ValidationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

void Engine::SetupDebugMessenger()
{
	if (!enableValidationLayers) 
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	PopulateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void Engine::CreateSurface()
{
	if (glfwCreateWindowSurface(m_Instance, m_GameContext.pWindow, nullptr, &m_GameContext.vulkanContext.surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
}

void Engine::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices{ deviceCount };
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	for (const auto& device : devices) 
	{
		if (IsDeviceSuitable(device)) 
		{
			m_GameContext.vulkanContext.physicalDevice = device;
			break;
		}
	}

	if (m_GameContext.vulkanContext.physicalDevice == VK_NULL_HANDLE) 
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

bool Engine::IsDeviceSuitable(const VkPhysicalDevice& device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device, m_GameContext.vulkanContext.surface);
	const bool extensionsSupported = CheckDeviceExtensionSupport(device);
	return indices.isComplete() && extensionsSupported;
}

bool Engine::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

void Engine::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies(m_GameContext);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value()/*, indices.transferFamily.value()*/ };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

	if (enableValidationLayers) 
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
	}
	else 
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_GameContext.vulkanContext.physicalDevice, &createInfo, nullptr, &m_GameContext.vulkanContext.device) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create logical device!");
	}

	//vkGetDeviceQueue(m_GameContext.vulkanContext.device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
	//vkGetDeviceQueue(m_GameContext.vulkanContext.device, indices.presentFamily.value(), 0, &m_PresentQueue);
}