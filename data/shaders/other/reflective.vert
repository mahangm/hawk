#version 460 core

#include "../library/buffers/camera.glsl"
#include "../library/structs/transform.glsl"

// vertex attributes
layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;

// shader outputs
out VsOut
{
	vec3 vertex_normal;
	vec3 fragment_position;
} vs_out;

void main()
{
	// set shader outputs
	vs_out.vertex_normal = mat3(transform.normal) * normal;
	vs_out.fragment_position = vec3(transform.model * vec4(position, 1.0f));

	// shader output
	gl_Position = camera.projection * camera.view * transform.model * vec4(position, 1.0f);
}