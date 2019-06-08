#ifndef SCENE_BUFFER
#define SCENE_BUFFER
// scene buffer
layout (std140, binding = 2) uniform Scene
{
	Light lights[MAX_LIGHTS];
} scene;
#endif