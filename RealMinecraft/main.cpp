#include <RealEngine.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <real_core/SceneManager.h>

#include <Material/MaterialManager.h>

#include "Materials/DiffuseMaterial.h"
#include "Materials/GuiMaterial.h"
#include "Materials/OutlineMaterial.h"
#include "Materials/TransparentMaterial.h"
#include "Materials/TranspriteMaterial.h"
#include "Materials/WaterMaterial.h"
#include "Scenes/TestScene.h"
#include "Util/GameInfo.h"

void Load()
{
	//constexpr int seed = 1; // Set seed to a constant value
	//srand(seed); // Seed random number generator

	using namespace real;

	const auto context = RealEngine::GetGameContext();

	auto& materialManager = MaterialManager::GetInstance();
	Materials::diffuseMaterial = materialManager.AddMaterial<DiffuseMaterial>(context).first;
	Materials::waterMaterial = materialManager.AddMaterial<WaterMaterial>(context).first;
	Materials::transparentMaterial = materialManager.AddMaterial<TransparentMaterial>(context).first;
	Materials::transpriteMaterial = materialManager.AddMaterial<TranspriteMaterial>(context).first;
	Materials::outlineMaterial = materialManager.AddMaterial<OutlineMaterial>(context).first;
	Materials::outlineMaterial = materialManager.AddMaterial<GuiMaterial>(context).first;

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