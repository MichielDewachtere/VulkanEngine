#include "Engine.h"

#include <set>

#include <SDL2/SDL_vulkan.h>

#include "Core/CommandPool.h"
#include "Core/DepthBuffer/DepthBufferManager.h"
#include "Mesh/Mesh.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Renderer.h"
#include "Material/Pipelines/PosCol2DPipeline.h"
#include "Material/Pipelines/PosColNormPipeline.h"
#include "Mesh/MeshIndexed.h"
#include "Util/GameTime.h"
#include "Material/Material.h"
#include "Mesh/BaseMesh.h"
#include "Mesh/MeshFactory.h"

#include "Misc/Camera.h"
#include "Misc/InputManager.h"

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

		++i;
	}

	return indices;
}

void Engine::InitWindow()
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

	m_pPosCol2D = new Material();
	m_pPosColNorm = new Material();
	m_pPosCol2D->AddPipeline<PosCol2DPipeline>(m_GameContext);
	m_pPosColNorm->AddPipeline<PosColNormPipeline>(m_GameContext);

	renderer.AddMaterial(m_pPosCol2D);
	renderer.AddMaterial(m_pPosColNorm);

	ShaderManager::GetInstance().DestroyShaderModules(m_GameContext.vulkanContext.device);
}

void Engine::InitGame()
{
	{
		const std::vector triangleVertices{
			PosCol2D{ glm::vec2{0.5f + 0.0f, -0.25f},	glm::vec3{1.0f, 0.0f, 0.0f} },
			PosCol2D{ glm::vec2{0.5f + 0.25f,  0.25f},glm::vec3{0.0f, 1.0f, 0.0f} },
			PosCol2D{ glm::vec2{0.5f + -0.25f, 0.25f},glm::vec3{0.0f, 0.0f, 1.0f} },
		};

		m_pTriangle = new Mesh<PosCol2D>(3, true);
		m_pPosCol2D->BindMesh(m_pTriangle);
		for (const auto& v : triangleVertices)
		{
			m_pTriangle->AddVertex(v);
		}
		m_pTriangle->Init(m_GameContext);
	}
	{
		const std::vector rectVertices =
	   {
			PosCol2D{ glm::vec2{-0.5 + -0.25, -0.25},	glm::vec3{1,0,0} },
			PosCol2D{ glm::vec2{-0.5 + 0.25,  -0.25},	glm::vec3{0,1,0} },
			PosCol2D{ glm::vec2{-0.5 + 0.25,   0.25},	glm::vec3{0,0,1} },
			PosCol2D{ glm::vec2{-0.5 + -0.25,  0.25},	glm::vec3{1,1,1} },
		};
		const std::vector<uint16_t> indices =
		{
			0, 1, 2, 2, 3, 0
		};

		m_pRectangle = new MeshIndexed<PosCol2D>(4, 6, true);
		m_pPosCol2D->BindMesh(m_pRectangle);
		for (const auto& v : rectVertices)
		{
			m_pRectangle->AddVertex(v);
		}
		for (const auto& i : indices)
		{
			m_pRectangle->AddIndex(i);
		}
		m_pRectangle->Init(m_GameContext);
	}
	{
		auto [indices, vertices] = MeshFactory::CreateCube({ 0,0,0 }, 1);
		m_pCube1 = new MeshIndexed<PosColNorm>(static_cast<uint32_t>(vertices.size()),
		                                      static_cast<uint32_t>(indices.size()), false);
		m_pPosColNorm->BindMesh(m_pCube1);
		for (const auto& v : vertices)
		{
			m_pCube1->AddVertex(v);
		}
		for (const auto& i : indices)
		{
			m_pCube1->AddIndex(i);
		}
		m_pCube1->Init(m_GameContext);
	}
	{
		auto [indices, vertices] = MeshFactory::CreateCube({ 2,0,0 }, 1);
		m_pCube2 = new MeshIndexed<PosColNorm>(static_cast<uint32_t>(vertices.size()),
		                                      static_cast<uint32_t>(indices.size()), false);
		m_pPosColNorm->BindMesh(m_pCube2);
		for (const auto& v : vertices)
		{
			m_pCube2->AddVertex(v);
		}
		for (const auto& i : indices)
		{
			m_pCube2->AddIndex(i);
		}
		m_pCube2->Init(m_GameContext);
	}
}

void Engine::MainLoop()
{
	auto& time = GameTime::GetInstance();
	auto& input = InputManager::GetInstance();
	auto& camera = Camera::GetInstance();

	time.Init();
	input.Initialize(m_GameContext);
	camera.Init(m_GameContext);

	float timer = 0;
	constexpr float fpsPrintTime = 1;

	bool doContinue = true;
	bool printFps = true;

	const std::string text{
	"-=- INFO -=-\nPress I to print info\nPress C to clear the console\nPress F1 to toggle fps\nPress F3 to toggle the 2d pipeline on/off\nPress F4 to toggle the 3d pipeline on/off\n-=- END INFO -=-\n"
	};

	std::cout << text;

	while (doContinue)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				doContinue = false;
			}
		}

		time.Update();
		input.UpdateInputStates(m_GameContext, false);

		Renderer::GetInstance().Draw(m_GameContext);

		camera.Update(m_GameContext);

#pragma region DEBUG STUFF
		if (InputManager::GetInstance().IsKeyboardKey(InputState::pressed, SDL_SCANCODE_C))
		{
			system("cls");
		}
		if (InputManager::GetInstance().IsKeyboardKey(InputState::pressed, SDL_SCANCODE_I))
		{
			std::cout << text;
		}
		if (InputManager::GetInstance().IsKeyboardKey(InputState::pressed, SDL_SCANCODE_F1))
		{
			printFps = !printFps;
			auto s = printFps ? "\033[1;32mON\033[0m" : "\033[1;31mOFF\033[0m";
			std::cout << "FPS turned " << s << '\n';
		}
		if (InputManager::GetInstance().IsKeyboardKey(InputState::pressed, SDL_SCANCODE_F3))
		{
			m_pPosCol2D->SetIsActive(!m_pPosCol2D->IsActive());
			auto s = m_pPosCol2D->IsActive() ? "\033[1;32mON\033[0m" : "\033[1;31mOFF\033[0m";
			std::cout << "2D pipeline turned " << s << "\n";
		}
		if (InputManager::GetInstance().IsKeyboardKey(InputState::pressed, SDL_SCANCODE_F4))
		{
			m_pPosColNorm->SetIsActive(!m_pPosColNorm->IsActive());
			auto s = m_pPosColNorm->IsActive() ? "\033[1;32mON\033[0m" : "\033[1;31mOFF\033[0m";
			std::cout << "3D pipeline turned " << s << "\n";
		}
#pragma endregion DEBUG STUFF

		timer += time.GetElapsed();
		if (printFps && timer >= fpsPrintTime)
		{
			timer = 0;
			std::cout << "\033[1;90mFPS: " << time.GetFPS_Unsigned() << "\033[0m\n";
		}

	}
	vkDeviceWaitIdle(m_GameContext.vulkanContext.device);
}

void Engine::CleanUp()
{
	Renderer::GetInstance().CleanUp(m_GameContext);
	DepthBufferManager::GetInstance().CleanUp(m_GameContext);

	vkDestroyRenderPass(m_GameContext.vulkanContext.device, m_GameContext.vulkanContext.renderPass, nullptr);

	m_pTriangle->CleanUp(m_GameContext);
	m_pRectangle->CleanUp(m_GameContext);
	m_pCube1->CleanUp(m_GameContext);
	m_pCube2->CleanUp(m_GameContext);

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
	const auto result = SDL_Vulkan_CreateSurface(m_GameContext.pWindow, m_Instance, &m_GameContext.vulkanContext.surface);
	if (result == SDL_FALSE) {
		throw std::runtime_error("Failed to create Vulkan surface from SDL window: " + std::string(SDL_GetError()));
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