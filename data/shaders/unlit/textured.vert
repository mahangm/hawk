#version 460 core	

#include "../library/buffers/camera.glsl"
#include "../library/structs/transform.glsl"

// vertex attributes
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

// shader outputs
out VsOut
{
	vec2 texture_coords;
} vs_out;

void main()
{
	// set shader outputs
	vs_out.texture_coords = uv;

	// shader output
	gl_Position = camera.projection * camera.view * transform.model * vec4(position, 1.0f);
}