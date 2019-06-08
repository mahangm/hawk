#version 460 core

#include "../../../library/buffers/camera.glsl"
#include "../../../library/structs/renderer.glsl"
#include "../../../library/structs/material/blinn-phong/textured_normal.glsl"

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
	vec2 texture_coords;
	vec3 fragment_position;
	vec3 tangent_view_position;
	vec3 tangent_fragment_position;
	mat3 tbn;
} fs_in;

void main()
{
	// camera view direction
	vec3 view_direction = normalize(vec3(camera.position) - fs_in.fragment_position);

	// texture coordination
	vec2 texture_coords = fs_in.texture_coords;

	// diffuse color
	vec4 diffuse_color = texture(material.diffuse, texture_coords);

	// specular color
	vec4 specular_color = texture(material.specular, texture_coords);

	// fragment normal vector
	vec3 fragment_normal = (texture(material.normal, texture_coords).rgb * 2.0f - 1.0f) * fs_in.tbn;

	// fill position buffer
	position = fs_in.fragment_position;

	// fill normal buffer
	normal = fragment_normal;

	// fill diffuse buffer
	diffuse = diffuse_color;

	// fill specular buffer
	specular = specular_color;

	// fill specular shininess buffer
	extra.r = material.shininess;

	// fill shadows buffer
	extra.g = float(renderer.shadows);
}