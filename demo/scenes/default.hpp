#ifndef SCENES_DEFAULT_H
#define SCENES_DEFAULT_H

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

bool initialize(hawk::System::Core& app)
{
	// initialize
	if (!app.initialize(1280, 720, "Default", hawk::Managers::Video::Rendering::Deferred, hawk::Managers::Video::Lighting::PBR)) return 1;

	// set input callbacks
	app.input.onMouseMove = &onMouseMove;
	app.input.onMouseScroll = &onMouseScroll;

	// set ambient light
	app.video.ambient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

	// load assets
	if (!app.assets.addMaterial("White", app.assets.shaders["LitColored"], hawk::Assets::Material::Type::Lit)) return false;
	if (!app.assets.addMaterial("Red", app.assets.shaders["LitColored"], hawk::Assets::Material::Type::Lit)) return false;
	if (!app.assets.addMesh("Floor", hawk::Assets::Mesh::Primitive::Quad, 5.0f, 5.0f)) return false;
	if (!app.assets.addMesh("Box", hawk::Assets::Mesh::Primitive::Cube)) return false;

	// set white material parameters
	app.assets.materials["White"]->setFloatVec4("material.albedo", glm::vec4(1.0f));
	app.assets.materials["White"]->setFloat("material.metallic", 0.2f);
	app.assets.materials["White"]->setFloat("material.roughness", 0.8f);
	app.assets.materials["White"]->setFloat("material.ambient", 1.0f);
	
	// set red material parameters
	app.assets.materials["Red"]->setFloatVec4("material.albedo", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	app.assets.materials["Red"]->setFloat("material.metallic", 0.2f);
	app.assets.materials["Red"]->setFloat("material.roughness", 0.8f);
	app.assets.materials["Red"]->setFloat("material.ambient", 1.0f);

	// main camera
	hawk::Objects::Entity* main_camera = app.scene.world->addEntity("MainCamera", new hawk::Objects::Entity());
	main_camera->addComponent("Transform", new hawk::Components::Transform(glm::vec3(-10.0f, 10.0f, 10.0f), glm::vec3(45.0f, -40.0f, 0.0f)));
	main_camera->addComponent("MainCamera", new hawk::Components::Camera());
	main_camera->onUpdate = &onUpdateMainCamera;

	// main light
	hawk::Objects::Entity* main_light = app.scene.world->addEntity("MainLight", new hawk::Objects::Entity());
	main_light->addComponent("Transform", new hawk::Components::Transform(glm::vec3(0.0f, 3.0f, 0.0f)));
	main_light->addComponent("Light", new hawk::Components::Light(hawk::Components::Light::Type::Point, glm::vec4(1.0f), 10.0f, 20.0f));

	// white floor
	hawk::Objects::Entity* floor = app.scene.world->addEntity("Floor", new hawk::Objects::Entity());
	floor->addComponent("Transform", new hawk::Components::Transform(glm::vec3(0.0f), glm::vec3(-90.0f, 0.0f, 0.0f)));
	floor->addComponent("MeshRenderer", new hawk::Components::MeshRenderer(app.assets.materials["White"], app.assets.meshes["Floor"]));

	// red box
	hawk::Objects::Entity* box = app.scene.world->addEntity("Box", new hawk::Objects::Entity());
	box->addComponent("Transform", new hawk::Components::Transform(glm::vec3(0.0f, 1.0f, 0.0f)));
	box->addComponent("MeshRenderer", new hawk::Components::MeshRenderer(app.assets.materials["Red"], app.assets.meshes["Box"]));

	return true;
}
#endif