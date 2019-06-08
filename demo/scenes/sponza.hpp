#ifndef SCENES_SPONZA_H
#define SCENES_SPONZA_H

// scripts
#include "../scripts/camera/fps.hpp"

void onMouseMove(double x, double y)
{
	// camera look
	cameraLook((float)x, (float)y);
}

void onMouseScroll(double x, double y)
{
	// camera zoom
	cameraZoom(-(float)y);
}

void onUpdateMainCamera(hawk::Objects::Entity* entity)
{
	// hawk
	hawk::System::Core& app = hawk::System::Core::getInstance();

	// quit
	if (app.input.keyDown(GLFW_KEY_ESCAPE)) app.stop();

	// camera movement
	cameraMove(app.time.getDeltaTime());
}

std::vector<std::string> getCubePaths(const std::string& path, const std::string& type)
{
	std::vector<std::string> paths;

	paths.push_back(std::string(path).append("/").append(type).append("_posx.dds"));
	paths.push_back(std::string(path).append("/").append(type).append("_negx.dds"));
	paths.push_back(std::string(path).append("/").append(type).append("_posy.dds"));
	paths.push_back(std::string(path).append("/").append(type).append("_negy.dds"));
	paths.push_back(std::string(path).append("/").append(type).append("_posz.dds"));
	paths.push_back(std::string(path).append("/").append(type).append("_negz.dds"));

	return paths;
}

bool initialize(hawk::System::Core& app)
{
	// initialize
	if (!app.initialize(1280, 720, "Sponza", hawk::Managers::Video::Rendering::Deferred, hawk::Managers::Video::Lighting::PBR)) return 1;

	// set input callbacks
	app.input.onMouseMove = &onMouseMove;
	app.input.onMouseScroll = &onMouseScroll;

	// set video effects
	app.video.fxaa = true;
	app.video.bloom = true;

	// load cube maps
	std::vector<std::string> skybox_paths = getCubePaths("../data/skybox/champagne-castle", "skybox");
	std::vector<std::string> irradiance_paths = getCubePaths("../data/skybox/champagne-castle", "irradiance");
	std::vector<std::string> radiance_paths = getCubePaths("../data/skybox/champagne-castle", "radiance");

	// load assets
	if (!app.assets.addTexture("Skybox", skybox_paths, false, true, -1, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR)) return false;
	if (!app.assets.addTexture("Irradiance", irradiance_paths, false, true, -1, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR)) return false;
	if (!app.assets.addTexture("Radiance", radiance_paths, false, true, 5, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR)) return false;
	if (!app.assets.addShader("Skybox", std::vector<std::string> { "../data/shaders/other/skybox.vert", "../data/shaders/other/skybox.frag" })) return false;
	if (!app.assets.addShader("Vignette", std::vector<std::string> { "../data/shaders/post-process/screen.vert", "../data/shaders/post-process/vignette.frag" })) return false;
	if (!app.assets.addMaterial("Skybox", app.assets.shaders["Skybox"])) return false;
	if (!app.assets.addMaterial("Vignette", app.assets.shaders["Vignette"])) return false;
	if (!app.assets.addSound("Ambient", "../data/sounds/ambient.wav", FMOD_2D | FMOD_CREATESTREAM)) return false;
	if (!app.assets.addModel("Sponza", "../data/models/sponza/sponza.obj")) return false;

	// set skybox texture
	app.assets.materials["Skybox"]->setTexture("material.cube", app.assets.textures["Skybox"]);

	// main camera
	hawk::Objects::Entity* main_camera = app.scene.world->addEntity("MainCamera", new hawk::Objects::Entity());
	main_camera->addComponent("Transform", new hawk::Components::Transform(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(-90.0f, 0.0f, 0.0f)));
	auto camera = main_camera->addComponent("MainCamera", new hawk::Components::Camera());
	camera->far_plane = 300.0f;
	camera->materials.push_back(app.assets.materials["Vignette"]);
	camera->skybox = app.assets.materials["Skybox"];
	camera->irradiance = app.assets.textures["Irradiance"];
	camera->radiance = app.assets.textures["Radiance"];
	main_camera->addComponent("AudioListener", new hawk::Components::AudioListener());
	main_camera->addComponent("AudioPlayer", new hawk::Components::AudioPlayer(app.assets.sounds["Ambient"], true, true));
	main_camera->onUpdate = &onUpdateMainCamera;

	// main light
	hawk::Objects::Entity* main_light = app.scene.world->addEntity("MainLight", new hawk::Objects::Entity());
	main_light->addComponent("Transform", new hawk::Components::Transform(glm::vec3(0.0f), glm::vec3(0.0f, -24.0f, 70.0f)));
	auto light = main_light->addComponent("Light", new hawk::Components::Light(hawk::Components::Light::Type::Directional, glm::vec4(1.0f), 10.0f, 20.0f, true, 1.0f, 300.0f));
	light->frustum = glm::vec4(-100.0f, 100.0f, -100.0f, 100.0f);

	// sponza
	hawk::Objects::Entity* sponza = app.scene.world->addEntity("Sponza", app.assets.models["Sponza"]);
	sponza->transform->scale = glm::vec3(0.1f);
	sponza->setStatic(true, true);

	return true;
}
#endif