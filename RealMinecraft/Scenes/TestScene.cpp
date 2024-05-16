#include "TestScene.h"

#include <utility>

#include <real_core/InputManager.h>

#include <RealEngine.h>
#include <Material/MaterialManager.h>
#include <Mesh/Mesh.h>

#include "Commands/MoveCommand.h"
#include "Commands/RotateCommand.h"
#include "Components/Player.h"
#include "Components/World.h"
#include "Util/BlockParser.h"
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

	{
		auto& world = CreateGameObject();
		world.GetTransform()->SetLocalPosition({ 0,0,0 });

		world.AddComponent<World>();
	}

	TransformInfo info;
	info.position = { 8,65,8 };
	auto& camera = CreateGameObject(info, "player");
	{
		camera.AddComponent<Player>();
		const auto comp = camera.AddComponent<Camera>();
		CameraManager::GetInstance().AddCamera(comp, true);
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
	}
}