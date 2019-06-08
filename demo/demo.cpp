#include "../hawk/hawk.hpp"
#include "scenes/default.hpp"

int main(int argc, char* argv[])
{
	// hawk
	hawk::System::Core& app = hawk::System::Core::getInstance();

	// initialize scene
	if (!initialize(app)) return 1;

	// start world
	app.start();

	// main loop
	while (app.isRunning())
	{
		// fixed update world
		app.fixedUpdate();

		// update world
		app.update();

		// late update world
		app.lateUpdate();

		// render world
		app.render();
		
		// destroy candidate objects
		app.destroy();

		// process per frame operations
		app.process();
	}

	// deinitialize and free resources
	app.deinitialize();

	return 0;
}