#include "RealEngine.h"

#include <set>

#include <SDL2/SDL_vulkan.h>
#include <SDL_image.h>

#include <real_core/GameTime.h>
#include <real_core/InputManager.h>
#include <real_core/imgui_impl_sdl2.h>
#include <real_core/SceneManager.h>

#include "Core/DepthBuffer/DepthBufferManager.h"
#include "Graphics/ShaderManager.h"
#include "Material/MaterialManager.h"
#include "Misc/InputManager.h"
#include "Graphics/Renderer.h"
#include "ImGui/imgui_impl_vulkan.h"

RealEngine::RealEngine()
{
	InitWindow();
	InitVulkan();
	InitRenderer();
	InitImGui();
	InitGame();
}

void RealEngine::Run(const std::function<void(const GameContext&)>& load)
{
	load(m_GameContext);
	MainLoop();
	CleanUp();
}

void RealEngine::InitSDL()
{
	InitWindow();
	InitSDLImage();
}

void RealEngine::InitWindow()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	SDL_Vulkan_LoadLibrary(nullptr);

	const auto pWindow = SDL_CreateWindow(
		m_GameContext.windowTitle.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		static_cast<int>(m_GameContext.windowWidth),
		static_cast<int>(m_GameContext.windowHeight),
		SDL_WINDOW_VULKAN
	);

	if (pWindow == nullptr)
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	m_GameContext.pWindow = pWindow;
}

void RealEngine::InitSDLImage()
{
	// Initialize SDL_image
	const int imgFlags = IMG_INIT_PNG; // You can specify other image formats here
	if (!(IMG_Init(imgFlags) & imgFlags)) 
	{
		throw std::runtime_error(std::string("SDL_image could not initialize! SDL_image Error: ") + IMG_GetError());
	}
}

void RealEngine::InitVulkan()
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

void RealEngine::InitRenderer()
{
	auto& shaderManager = ShaderManager::GetInstance();
	auto& renderer = Renderer::GetInstance();

	shaderManager.Init("resources/shaders");
	renderer.Init(m_GameContext);
}

void RealEngine::InitImGui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForVulkan(m_GameContext.pWindow);
	ImGui_ImplVulkan_InitInfo initInfo = {};
	initInfo.Instance = m_Instance;
	initInfo.PhysicalDevice = m_GameContext.vulkanContext.physicalDevice;
	initInfo.Device = m_GameContext.vulkanContext.device;
	initInfo.RenderPass = m_GameContext.vulkanContext.renderPass;
	initInfo.QueueFamily = FindQueueFamilies(m_GameContext.vulkanContext.physicalDevice,
	                                         m_GameContext.vulkanContext.surface).graphicsFamily.value();
	initInfo.Queue = Renderer::GetInstance().GetGraphicsQueue();
	{
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
		};
		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets = 1;
		poolInfo.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		poolInfo.pPoolSizes = pool_sizes;
		vkCreateDescriptorPool(m_GameContext.vulkanContext.device, &poolInfo, nullptr, &m_ImGuiDescriptorPool);
	}
	initInfo.DescriptorPool = m_ImGuiDescriptorPool;
	initInfo.MinImageCount = 2;
	initInfo.ImageCount = 2;
	initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	ImGui_ImplVulkan_Init(&initInfo);
}

void RealEngine::MainLoop()
{
	auto& time = real::GameTime::GetInstance();
	auto& input = real::InputManager::GetInstance();
	auto& cameraInput = InputManager::GetInstance();
	auto& renderer = Renderer::GetInstance();
	auto& sceneManager = real::SceneManager::GetInstance();

	time.Init();
	cameraInput.Initialize(m_GameContext);

	float timer = 0;
	bool doContinue = true;

	while (doContinue)
	{
		time.Update();

		doContinue = input.ProcessInput();

		sceneManager.Update();
		renderer.Draw(m_GameContext);

		cameraInput.UpdateInputStates(m_GameContext, true);

		timer += time.GetElapsed();
		if (constexpr float fpsPrintTime = 1; 
			timer >= fpsPrintTime)
		{
			timer = 0;
			std::cout << "\033[1;90mFPS: " << time.GetFPS_Unsigned() << "\033[0m\n";
		}
	}

	vkDeviceWaitIdle(m_GameContext.vulkanContext.device);
}

void RealEngine::CleanUp()
{
	real::SceneManager::GetInstance().Destroy();

	Renderer::GetInstance().CleanUp(m_GameContext);
	DepthBufferManager::GetInstance().CleanUp(m_GameContext);
	MaterialManager::GetInstance().RemoveMaterials(m_GameContext);
	ContentManager::GetInstance().CleanUp(m_GameContext);
	ShaderManager::GetInstance().DestroyShaderModules(m_GameContext.vulkanContext.device);

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	vkDestroyDescriptorPool(m_GameContext.vulkanContext.device, m_ImGuiDescriptorPool, nullptr);
	vkDestroyRenderPass(m_GameContext.vulkanContext.device, m_GameContext.vulkanContext.renderPass, nullptr);

	CommandPool::GetInstance().CleanUp(m_GameContext);

	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
	}

	vkDestroyDevice(m_GameContext.vulkanContext.device, nullptr);

	vkDestroySurfaceKHR(m_Instance, m_GameContext.vulkanContext.surface, nullptr);
	vkDestroyInstance(m_Instance, nullptr);

	SDL_DestroyWindow(m_GameContext.pWindow);
	m_GameContext.pWindow = nullptr;
	SDL_Quit();
}

void RealEngine::CreateInstance()
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

std::vector<const char*> RealEngine::GetRequiredExtensions()
{
	std::vector<const char*> extensions;

	// SDL Vulkan extensions
	unsigned int sdlExtensionCount = 0;
	if (!SDL_Vulkan_GetInstanceExtensions(m_GameContext.pWindow, &sdlExtensionCount, nullptr)) {
		throw std::runtime_error("Failed to get SDL Vulkan instance extensions: " + std::string(SDL_GetError()));
	}

	// Allocate space for extension names
	std::vector<const char*> sdlExtensionNames(sdlExtensionCount);
	if (!SDL_Vulkan_GetInstanceExtensions(m_GameContext.pWindow, &sdlExtensionCount, sdlExtensionNames.data())) {
		throw std::runtime_error("Failed to get SDL Vulkan instance extensions: " + std::string(SDL_GetError()));
	}

	// Add SDL Vulkan extensions to the list
	extensions.insert(extensions.end(), sdlExtensionNames.begin(), sdlExtensionNames.end());

	// Additional extensions
	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void RealEngine::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
}

bool RealEngine::CheckValidationLayerSupport()
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

void RealEngine::SetupDebugMessenger()
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

void RealEngine::CreateSurface()
{
	const auto result = SDL_Vulkan_CreateSurface(m_GameContext.pWindow, m_Instance, &m_GameContext.vulkanContext.surface);
	if (result == SDL_FALSE) {
		throw std::runtime_error("Failed to create Vulkan surface from SDL window: " + std::string(SDL_GetError()));
	}
}

void RealEngine::PickPhysicalDevice()
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

bool RealEngine::IsDeviceSuitable(const VkPhysicalDevice& device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device, m_GameContext.vulkanContext.surface);
	const bool extensionsSupported = CheckDeviceExtensionSupport(device);

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() && extensionsSupported /*&& swapChainAdequate */&& supportedFeatures.samplerAnisotropy;
}

bool RealEngine::CheckDeviceExtensionSupport(VkPhysicalDevice device)
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

void RealEngine::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies(m_GameContext.vulkanContext.physicalDevice, m_GameContext.vulkanContext.surface);

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
	deviceFeatures.samplerAnisotropy = VK_TRUE;

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
}