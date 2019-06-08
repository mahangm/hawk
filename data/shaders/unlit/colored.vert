#version 460 core

#include "../library/buffers/camera.glsl"
#include "../library/structs/transform.glsl"

// vertex attributes
layout (location = 0) in vec3 position;

void main()
{
	// shader output
	gl_Position = camera.projection * camera.view * transform.model * vec4(position, 1.0f);
}