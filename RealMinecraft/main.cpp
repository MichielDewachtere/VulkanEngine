#include <RealEngine.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <real_core/SceneManager.h>

#include <Material/MaterialManager.h>
#include <Material/Pipelines/PosTexNormPipeline.h>

#include "Pipelines/GlassPipeline.h"
#include "Pipelines/WaterPipeline.h"
#include "Scenes/TestScene.h"

void Load()
{
	using namespace real;

	const auto context = RealEngine::GetGameContext();

	auto& materialManager = MaterialManager::GetInstance();
	materialManager.AddMaterial<PosTexNormPipeline, PosTexNorm>(context);
	materialManager.AddMaterial<WaterPipeline, PosTexNorm>(context);
	materialManager.AddMaterial<GlassPipeline, PosTexNorm>(context);

	auto& sceneManager = SceneManager::GetInstance();

	sceneManager.CreateScene(new TestScene("test scene", "test input map"));
	sceneManager.SetSceneActive("test scene");
}

int main(int, char* [])
{
#ifdef NDEBUG
	try
	{
		real::RealEngine app;
		app.Run(Load);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
#else
	real::RealEngine app;
	app.Run(Load);
#endif // NDEBUG

	return EXIT_SUCCESS;
}