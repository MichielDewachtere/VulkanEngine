#include "RealEngine.h"

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <real_core/SceneManager.h>

#include "Scenes/TestScene.h"
#include "Material/MaterialManager.h"
#include "Material/Pipelines/PosCol2DPipeline.h"
#include "Material/Pipelines/PosColNormPipeline.h"
#include "Material/Pipelines/PosTexNormPipeline.h"

void Load(const GameContext& context)
{
	auto& materialManager = MaterialManager::GetInstance();
	materialManager.AddMaterial<PosCol2DPipeline, PosCol2D>(context);
	materialManager.AddMaterial<PosColNormPipeline, PosColNorm>(context);
	materialManager.AddMaterial<PosTexNormPipeline, PosTexNorm>(context);

	auto& sceneManager = real::SceneManager::GetInstance();

	sceneManager.CreateScene(new TestScene("test scene", "test input map"));
	sceneManager.SetSceneActive("test scene");
}

int main(int, char* [])
{
	// DISABLE_LAYER_AMD_SWITCHABLE_GRAPHICS_1 = 1
	//DISABLE_LAYER_NV_OPTIMUS_1 = 1
	//_putenv_s("DISABLE_LAYER_AMD_SWITCHABLE_GRAPHICS_1", "1");
	//_putenv_s("DISABLE_LAYER_NV_OPTIMUS_1", "1");

	try
	{
		RealEngine app;
		app.Run(Load);
	}
	catch (const std::exception& e) 
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}