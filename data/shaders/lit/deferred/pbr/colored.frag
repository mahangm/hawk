#version 460 core

#include "../../../library/structs/renderer.glsl"
#include "../../../library/structs/material/pbr/colored.glsl"

// color buffers output
layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec4 albedo;
layout (location = 3) out vec4 surface;
layout (location = 4) out vec2 extra;

// shader inputs
in VsOut
{
	vec3 vertex_normal;
	vec3 fragment_position;
} fs_in;

void main()
{
	// fill position buffer
	position = fs_in.fragment_position;

	// fill normal buffer
	normal = fs_in.vertex_normal;

	// fill albedo buffer
	albedo = material.albedo;

	// fill metallic buffer
	surface.r = material.metallic;

	// fill roughness buffer
	surface.g = material.roughness;

	// fill ambient buffer
	surface.b = material.ambient;

	// fill shadows buffer
	extra.r = float(renderer.shadows);
}