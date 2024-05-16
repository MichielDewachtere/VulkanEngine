#include <RealEngine.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <real_core/SceneManager.h>

#include <Material/MaterialManager.h>
#include <Material/Pipelines/PosCol2DPipeline.h>
#include <Material/Pipelines/PosColNormPipeline.h>
#include <Material/Pipelines/PosTexNormPipeline.h>

#include "Scenes/TestScene.h"

void Load()
{
	using namespace real;

	const auto context = RealEngine::GetGameContext();

	auto& materialManager = MaterialManager::GetInstance();
	materialManager.AddMaterial<PosTexNormPipeline, PosTexNorm>(context);

	auto& sceneManager = real::SceneManager::GetInstance();

	sceneManager.CreateScene(new TestScene("test scene", "test input map"));
	sceneManager.SetSceneActive("test scene");
}

int main(int, char* [])
{
	real::RealEngine app;
	app.Run(Load);

	//try
	//{
	//	real::RealEngine app;
	//	app.Run(Load);
	//}
	//catch (const std::exception& e)
	//{
	//	std::cerr << e.what() << std::endl;
	//	return EXIT_FAILURE;
	//}

	return EXIT_SUCCESS;
}