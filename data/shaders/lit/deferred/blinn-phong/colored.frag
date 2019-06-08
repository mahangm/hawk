#version 460 core

#include "../../../library/structs/renderer.glsl"
#include "../../../library/structs/material/blinn-phong/colored.glsl"

// color buffers output
layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec4 diffuse;
layout (location = 3) out vec4 specular;
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

	// fill diffuse buffer
	diffuse = material.diffuse;

	// fill specular buffer
	specular = material.specular;

	// fill specular shininess buffer
	extra.r = material.shininess;

	// fill shadows buffer
	extra.g = float(renderer.shadows);
}