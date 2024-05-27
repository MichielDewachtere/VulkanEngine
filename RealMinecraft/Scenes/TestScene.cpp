#include "TestScene.h"

#include <glm/vec3.hpp>

#include <utility>

#include <real_core/InputManager.h>

#include <RealEngine.h>
#include <Material/MaterialManager.h>
#include <Mesh/Mesh.h>

#include "Commands/InteractCommand.h"
#include "Commands/MoveCommand.h"
#include "Commands/RotateCommand.h"
#include "Components/Gui.h"
#include "Components/OutlineBlock.h"
#include "Components/Player.h"
#include "Components/World.h"
#include "Materials/GuiMaterial.h"
#include "Materials/OutlineMaterial.h"
#include "Mesh/MeshFactory.h"
#include "Mesh/MeshIndexed.h"
#include "Misc/CameraManager.h"
#include "Misc/Camera.h"
#include "Util/BlockParser.h"
#include "Util/GameStructs.h"
#include "Util/NoiseManager.h"

TestScene::TestScene(std::string name, std::string inputMap)
	: Scene(std::move(name), std::move(inputMap))
{
}

void TestScene::Load()
{
	using namespace real;
	const auto context = RealEngine::GetGameContext();

	NoiseManager::GetInstance().Initialize(0);


	auto& world = CreateGameObject();
	world.GetTransform()->SetLocalPosition({ 0,0,0 });

	const auto worldComponent = world.AddComponent<World>();

	constexpr int reach = 10;

	TransformInfo transformInfo;
	transformInfo.position = { 8,75,8 };
	auto& camera = CreateGameObject(transformInfo, "player");
	{
		camera.AddComponent<Player>();
		const auto comp = camera.AddComponent<Camera>();
		CameraManager::GetInstance().AddCamera(comp, true);

	}

	transformInfo = {};
	transformInfo.position = { 0,0,-4 };
	auto& block = CreateGameObject(transformInfo);
	{
		const auto [indices, vertices] = MeshFactory::CreateCube(glm::ivec3{ -0.001,-0.001,-0.001 }, 1.002, glm::u8vec3{ 0, 0, 0 });
		MeshInfo meshInfo{};
		meshInfo.indexCapacity = static_cast<uint32_t>(indices.size());
		meshInfo.vertexCapacity = static_cast<uint32_t>(vertices.size());
		meshInfo.usesUbo = true;
		const auto mesh = block.AddComponent<MeshIndexed<PosColNorm, UniformBufferObject>>(meshInfo);

		mesh->AddIndices(indices);
		mesh->AddVertices(vertices);
		mesh->Init(context);

		mesh->SetMaterial(MaterialManager::GetInstance().GetMaterial<OutlineMaterial>());
		mesh->Disable();

		block.AddComponent<OutlineBlock>(&camera, worldComponent, reach);
	}

	transformInfo = {};
	transformInfo.position = { 0,0,0 };
	auto& gui = CreateGameObject(transformInfo);
	{
		MeshInfo meshInfo{};
		meshInfo.indexCapacity = static_cast<uint32_t>(600);
		meshInfo.vertexCapacity = static_cast<uint32_t>(400);
		meshInfo.usesUbo = true;
		const auto mesh = gui.AddComponent<MeshIndexed<PosTex, WorldMatrix>>(meshInfo);
		mesh->Init(context);
		mesh->SetMaterial(MaterialManager::GetInstance().GetMaterial<GuiMaterial>());

		gui.AddComponent<Gui>();
	}

	{
		auto& input = real::InputManager::GetInstance();
		const auto map = input.AddInputMap("test", true);
		map->AddKeyboardAction<MoveCommand>(0, KeyState::keyPressed, SDL_SCANCODE_A, &camera, glm::ivec3{ -1,0,0 });
		map->AddKeyboardAction<MoveCommand>(1, KeyState::keyPressed, SDL_SCANCODE_D, &camera, glm::ivec3{ 1,0,0 });
		map->AddKeyboardAction<MoveCommand>(2, KeyState::keyPressed, SDL_SCANCODE_LSHIFT, &camera, glm::ivec3{ 0,-1,0 });
		map->AddKeyboardAction<MoveCommand>(3, KeyState::keyPressed, SDL_SCANCODE_SPACE, &camera, glm::ivec3{ 0,1,0 });
		map->AddKeyboardAction<MoveCommand>(4, KeyState::keyPressed, SDL_SCANCODE_S, &camera, glm::ivec3{ 0,0,1 });
		map->AddKeyboardAction<MoveCommand>(5, KeyState::keyPressed, SDL_SCANCODE_W, &camera, glm::ivec3{ 0,0,-1 });

		map->AddMouseAction<RotateCommand>(6, KeyState::keyPressed, MouseButton::left, &camera);

		map->AddMouseAction<InteractCommand>(7, KeyState::keyDown, MouseButton::left, &block, worldComponent, false);
		map->AddMouseAction<InteractCommand>(8, KeyState::keyDown, MouseButton::right, &block, worldComponent, true);
	}
}