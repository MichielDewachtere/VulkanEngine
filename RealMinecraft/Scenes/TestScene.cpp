#include "TestScene.h"

#include <utility>

#include <real_core/InputManager.h>

#include <RealEngine.h>
#include <Material/MaterialManager.h>
#include <Mesh/Mesh.h>

#include "Commands/MoveCommand.h"
#include "Commands/RotateCommand.h"
#include "Components/Chunk.h"
#include "Util/BlockParser.h"

TestScene::TestScene(std::string name, std::string inputMap)
	: Scene(std::move(name), std::move(inputMap))
{
}

void TestScene::Load()
{
	using namespace real;

	//const auto& materialManager = MaterialManager::GetInstance();
	const auto context = RealEngine::GetGameContext();

	//std::vector<PosTexNorm> vertices;
	//std::vector<uint32_t> indices;

	//auto v = BlockParser::GetInstance().GetFaceData(EDirection::north, EBlock::debug);
	//vertices.insert(vertices.end(), v.begin(), v.end());
	//indices.insert(indices.end(), { 0, 1, 2, 2, 3, 0 });

	//v = BlockParser::GetInstance().GetFaceData(EDirection::east, EBlock::debug);
	//vertices.insert(vertices.end(), v.begin(), v.end());
	//indices.insert(indices.end(), { 4, 5, 6, 6, 7, 4 });

	//v = BlockParser::GetInstance().GetFaceData(EDirection::south, EBlock::debug);
	//vertices.insert(vertices.end(), v.begin(), v.end());
	//indices.insert(indices.end(), { 8, 9, 10, 10, 11, 8 });

	//v = BlockParser::GetInstance().GetFaceData(EDirection::west, EBlock::debug);
	//vertices.insert(vertices.end(), v.begin(), v.end());
	//indices.insert(indices.end(), { 12, 13, 14, 14, 15, 12 });

	//v = BlockParser::GetInstance().GetFaceData(EDirection::up, EBlock::debug);
	//vertices.insert(vertices.end(), v.begin(), v.end());
	//indices.insert(indices.end(), { 16, 17, 18, 18, 19, 16 });

	//v = BlockParser::GetInstance().GetFaceData(EDirection::down, EBlock::debug);
	//vertices.insert(vertices.end(), v.begin(), v.end());
	//indices.insert(indices.end(), { 20, 21, 22, 22, 23, 20 });

	{
		auto& chunk = CreateGameObject();
		chunk.GetTransform()->SetLocalPosition({ 0,0,0 });

		chunk.AddComponent<Chunk>();
	}
	//{
	//	auto& chunk = CreateGameObject();
	//	chunk.GetTransform()->SetLocalPosition({ 8,0,0 });

	//	chunk.AddComponent<Chunk>();
	//	//mesh->AddVertices(vertices);
	//	//mesh->AddIndices(indices);
	//	//mesh->Init(context);
	//}

	TransformInfo info;
	info.position = { 8,65,8 };
	auto& camera = CreateGameObject(info);
	{
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