#include "RealEngine.h"

#include <set>

#include <SDL2/SDL_vulkan.h>
#include <SDL_image.h>

#include <real_core/GameTime.h>

#include "Core/DepthBuffer/DepthBufferManager.h"
#include "Graphics/ShaderManager.h"
#include "Material/MaterialManager.h"
#include "Material/Pipelines/PosCol2DPipeline.h"
#include "Material/Pipelines/PosColNormPipeline.h"
#include "Material/Pipelines/PosTexNormPipeline.h"
#include "Mesh/MeshFactory.h"
#include "Misc/InputManager.h"
#include "Graphics/Renderer.h"

void RealEngine::Run()
{
	InitWindow();
	InitVulkan();
	InitRenderer();
	InitGame();

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
	auto& materialManager = MaterialManager::GetInstance();
	auto& renderer = Renderer::GetInstance();

	shaderManager.Init("resources/shaders");
	renderer.Init(m_GameContext);

	materialManager.AddMaterial<PosCol2DPipeline, PosCol2D>(m_GameContext);
	materialManager.AddMaterial<PosColNormPipeline, PosColNorm>(m_GameContext);
	materialManager.AddMaterial<PosTexNormPipeline, PosTexNorm>(m_GameContext);

	shaderManager.DestroyShaderModules(m_GameContext.vulkanContext.device);
}

void RealEngine::InitGame()
{
	const auto& materialManager = MaterialManager::GetInstance();

	{
		const std::vector triangleVertices{
			PosCol2D{ glm::vec2{0.5f + 0.0f, -0.25f},	glm::vec3{1.0f, 0.0f, 0.0f} },
			PosCol2D{ glm::vec2{0.5f + 0.25f,  0.25f},glm::vec3{0.0f, 1.0f, 0.0f} },
			PosCol2D{ glm::vec2{0.5f + -0.25f, 0.25f},glm::vec3{0.0f, 0.0f, 1.0f} },
		};

		MeshInfo info;
		info.vertexCapacity = static_cast<uint32_t>(triangleVertices.size());

		m_pTriangle = new Mesh<PosCol2D>(info);
		materialManager.GetMaterial<PosCol2DPipeline, PosCol2D>()->BindMesh(m_GameContext, m_pTriangle);
		m_pTriangle->AddVertices(triangleVertices);
		m_pTriangle->Init(m_GameContext);
	}
	{
		const std::vector vertices ={
			PosCol2D{ glm::vec2{-0.5 + -0.25, -0.25},	glm::vec3{1,0,0} },
			PosCol2D{ glm::vec2{-0.5 + 0.25,  -0.25},	glm::vec3{0,1,0} },
			PosCol2D{ glm::vec2{-0.5 + 0.25,   0.25},	glm::vec3{0,0,1} },
			PosCol2D{ glm::vec2{-0.5 + -0.25,  0.25},	glm::vec3{1,1,1} },
		};
		const std::vector<uint32_t> indices =
		{
			0, 1, 2, 2, 3, 0
		};

		MeshInfo info;
		info.indexCapacity = static_cast<uint32_t>(indices.size());
		info.vertexCapacity = static_cast<uint32_t>(vertices.size());

		m_pRectangle = new MeshIndexed<PosCol2D>(info);
		materialManager.GetMaterial<PosCol2DPipeline, PosCol2D>()->BindMesh(m_GameContext, m_pRectangle);
		m_pRectangle->AddVertices(vertices);
		m_pRectangle->AddIndices(indices);
		m_pRectangle->Init(m_GameContext);
	}
	{
		auto [indices, vertices] = MeshFactory::CreateCube({ 0,0,0 }, 1);
		MeshInfo info;
		info.indexCapacity = static_cast<uint32_t>(indices.size());
		info.vertexCapacity = static_cast<uint32_t>(vertices.size());
		info.usesUbo = true;

		m_pColoredCube = new MeshIndexed<PosColNorm>(info);
		materialManager.GetMaterial<PosColNormPipeline, PosColNorm>()->BindMesh(m_GameContext, m_pColoredCube);
		m_pColoredCube->AddVertices(vertices);
		m_pColoredCube->AddIndices(indices);
		m_pColoredCube->Init(m_GameContext);
	}
	{
		auto [indices, vertices] = MeshFactory::CreatePyramid({ -2,0,0 }, 1, 1);
		MeshInfo info;
		info.indexCapacity = static_cast<uint32_t>(indices.size());
		info.vertexCapacity = static_cast<uint32_t>(vertices.size());
		info.usesUbo = true;

		m_pColoredPyramid = new MeshIndexed<PosColNorm>(info);
		materialManager.GetMaterial<PosColNormPipeline, PosColNorm>()->BindMesh(m_GameContext, m_pColoredPyramid);
		m_pColoredPyramid->AddVertices(vertices);
		m_pColoredPyramid->AddIndices(indices);
		m_pColoredPyramid->Init(m_GameContext);
	}
	{
		auto [indices, vertices] = MeshFactory::CreateCubeMap({ 2,0,0 }, 1);
		MeshInfo info;
		info.indexCapacity = static_cast<uint32_t>(indices.size());
		info.vertexCapacity = static_cast<uint32_t>(vertices.size());
		info.usesUbo = true;
		info.texture = ContentManager::GetInstance().LoadTexture(m_GameContext, "Resources/textures/grass_side.png");

		m_pTexturedCube = new MeshIndexed<PosTexNorm>(info);
		materialManager.GetMaterial<PosTexNormPipeline, PosTexNorm>()->BindMesh(m_GameContext, m_pTexturedCube);
		m_pTexturedCube->AddVertices(vertices);
		m_pTexturedCube->AddIndices(indices);
		m_pTexturedCube->Init(m_GameContext);
	}
	{
		const auto model = ContentManager::GetInstance().LoadModel("Resources/Models/viking_room.obj", { 4,0,0 });
		const auto indices = model->GetIndices();
		const auto vertices = model->GetVertices();
		MeshInfo info;
		info.indexCapacity = static_cast<uint32_t>(indices.size());
		info.vertexCapacity = static_cast<uint32_t>(vertices.size());
		info.usesUbo = true;
		info.texture = ContentManager::GetInstance().LoadTexture(m_GameContext, "Resources/textures/viking_room.png");

		m_pModel = new MeshIndexed<PosTexNorm>(info);
		materialManager.GetMaterial<PosTexNormPipeline, PosTexNorm>()->BindMesh(m_GameContext, m_pModel);
		m_pModel->AddVertices(vertices);
		m_pModel->AddIndices(indices);
		m_pModel->Init(m_GameContext);
	}
}

void RealEngine::MainLoop()
{
	auto& time = real::GameTime::GetInstance();
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
	"-=- INFO -=-\nPress I to print info\n"
		"Press C to clear the console\n"
		"Press F1 to toggle fps\n"
		"Press F3 to toggle the 2d pipeline on / off\n"
		"Press F4 to toggle the 3d colored pipeline on / off\n"
		"Press F5 to toggle the 3d textured pipeline on / off\n"
		"-=- END INFO -=-\n"
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
			const auto material = MaterialManager::GetInstance().GetMaterial<PosCol2DPipeline, PosCol2D>();
			material->SetIsActive(!material->IsActive());
			auto s = material->IsActive() ? "\033[1;32mON\033[0m" : "\033[1;31mOFF\033[0m";
			std::cout << "2D pipeline turned " << s << "\n";
		}
		if (InputManager::GetInstance().IsKeyboardKey(InputState::pressed, SDL_SCANCODE_F4))
		{
			const auto material = MaterialManager::GetInstance().GetMaterial<PosColNormPipeline, PosColNorm>();
			material->SetIsActive(!material->IsActive());
			auto s = material->IsActive() ? "\033[1;32mON\033[0m" : "\033[1;31mOFF\033[0m";
			std::cout << "3D colored pipeline turned " << s << "\n";
		}
		if (InputManager::GetInstance().IsKeyboardKey(InputState::pressed, SDL_SCANCODE_F5))
		{
			const auto material = MaterialManager::GetInstance().GetMaterial<PosTexNormPipeline, PosTexNorm>();
			material->SetIsActive(!material->IsActive());
			auto s = material->IsActive() ? "\033[1;32mON\033[0m" : "\033[1;31mOFF\033[0m";
			std::cout << "3D textured pipeline turned " << s << "\n";
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

void RealEngine::CleanUp()
{
	Renderer::GetInstance().CleanUp(m_GameContext);
	DepthBufferManager::GetInstance().CleanUp(m_GameContext);
	MaterialManager::GetInstance().RemoveMaterials(m_GameContext);
	ContentManager::GetInstance().CleanUp(m_GameContext);

	vkDestroyRenderPass(m_GameContext.vulkanContext.device, m_GameContext.vulkanContext.renderPass, nullptr);

	m_pTriangle->CleanUp(m_GameContext);
	m_pRectangle->CleanUp(m_GameContext);
	m_pColoredCube->CleanUp(m_GameContext);
	m_pColoredPyramid->CleanUp(m_GameContext);
	m_pTexturedCube->CleanUp(m_GameContext);
	m_pModel->CleanUp(m_GameContext);

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