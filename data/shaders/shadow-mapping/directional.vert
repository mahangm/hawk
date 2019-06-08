#version 460 core

#include "../library/structs/transform.glsl"

// vertex attributes
layout (location = 0) in vec3 position;

// light space matrix
uniform mat4 light_matrix;

void main()
{
	// shader output
	gl_Position = light_matrix * transform.model * vec4(position, 1.0f);
}