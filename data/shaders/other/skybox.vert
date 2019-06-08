#version 460 core

#include "../library/buffers/camera.glsl"

// vertex attributes
layout (location = 0) in vec3 position;

// shader outputs
out VsOut
{
	vec3 texture_coords;
} vs_out;

void main()
{
	// set shader outputs
	vs_out.texture_coords = position;

	// shader output (set maximum depth (z) as (w) which is 1.0f)
	gl_Position = vec4(camera.projection * mat4(mat3(camera.view)) * vec4(position, 1.0f)).xyww;
}