#ifndef SCRIPTS_CAMERA_FPS_H
#define SCRIPTS_CAMERA_FPS_H
/**
 * This flag tells us that if camera should look with mouse movement or not.
 */
bool camera_lock = true;

/**
 * This flag tells us that if this is the first time camera is looking.
 */
bool camera_first_look = true;

/**
 * This flag tells us if camera should act like human head.
 */
bool camera_constrain = true;

/**
 * Last camera look position for mouse x.
 */
float camera_last_look_x = 0.0f;

/**
 * Last camera look position for mouse y.
 */
float camera_last_look_y = 0.0f;

/**
 * Camera movement speed.
 */
float camera_speed = 50.0f;

/**
 * Camera look sensitivity.
 */
float camera_sensitivity = 0.1f;

/**
 * Camera look logic.
 */
void cameraLook(float x, float y)
{
	if (!camera_lock)
	{
		// if first look
		if (camera_first_look)
		{
			camera_last_look_x = x;
			camera_last_look_y = y;

			camera_first_look = false;
		}

		// x offset
		float x_offset = x - camera_last_look_x;

		// y offset
		float y_offset = camera_last_look_y - y;

		// set last positions
		camera_last_look_x = x;
		camera_last_look_y = y;

		// hawk
		hawk::System::Core& app = hawk::System::Core::getInstance();

		// camera entity
		hawk::Objects::Entity* entity = app.video.camera->entity;

		// set yaw and pitch
		entity->transform->rotation.x += x_offset * camera_sensitivity;
		entity->transform->rotation.y += y_offset * camera_sensitivity;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (camera_constrain)
		{
			if (entity->transform->rotation.y > 89.0f) entity->transform->rotation.y = 89.0f;
			if (entity->transform->rotation.y < -89.0f) entity->transform->rotation.y = -89.0f;
		}
	}
}

/**
 * Camere movement logic.
 * @param dt Delta time.
 */
void cameraMove(float dt)
{
	// hawk
	hawk::System::Core& app = hawk::System::Core::getInstance();

	// disable cursor
	if (app.input.buttonDown(GLFW_MOUSE_BUTTON_2))
	{
		glfwSetInputMode(app.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		camera_lock = false;
		camera_first_look = true;
	}

	// enable cursor
	if (app.input.buttonUp(GLFW_MOUSE_BUTTON_2))
	{
		glfwSetInputMode(app.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		camera_lock = true;
	}

	// move
	if (!camera_lock)
	{
		float velocity = camera_speed * dt;

		// camera entity
		hawk::Objects::Entity* entity = app.video.camera->entity;

		if (app.input.keyPress(GLFW_KEY_W)) entity->transform->position += app.video.camera->front * velocity;
		if (app.input.keyPress(GLFW_KEY_S)) entity->transform->position -= app.video.camera->front * velocity;
		if (app.input.keyPress(GLFW_KEY_A)) entity->transform->position -= app.video.camera->right * velocity;
		if (app.input.keyPress(GLFW_KEY_D)) entity->transform->position += app.video.camera->right * velocity;
	}
}

/**
 * Camera zoom logic.
 * @param value Zoom value.
 */
void cameraZoom(float value)
{
	hawk::System::Core::getInstance().video.camera->fov += value;
}
#endif