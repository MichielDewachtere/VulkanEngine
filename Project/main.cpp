#include "Engine.h"

#define SDL_MAIN_HANDLED
#include <SDL.h>

int main(int, char* [])
{
	// DISABLE_LAYER_AMD_SWITCHABLE_GRAPHICS_1 = 1
	//DISABLE_LAYER_NV_OPTIMUS_1 = 1
	//_putenv_s("DISABLE_LAYER_AMD_SWITCHABLE_GRAPHICS_1", "1");
	//_putenv_s("DISABLE_LAYER_NV_OPTIMUS_1", "1");

	Engine app;
	app.Run();

	//try
	//{
	//	Engine app;
	//	app.Run();
	//}
	//catch (const std::exception& e) 
	//{
	//	std::cerr << e.what() << std::endl;
	//	return EXIT_FAILURE;
	//}

	return EXIT_SUCCESS;
}