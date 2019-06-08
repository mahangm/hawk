#ifndef CAMERA_BUFFER
#define CAMERA_BUFFER
// camera buffer
layout (std140, binding = 1) uniform Camera
{
	mat4 view;
	mat4 projection;
	vec4 position;
	vec4 viewport;
	bool ibl;
} camera;
#endif