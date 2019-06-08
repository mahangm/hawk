#version 460 core

#include "../../../library/buffers/camera.glsl"
#include "../../../library/structs/renderer.glsl"
#include "../../../library/structs/material/pbr/textured_parallax.glsl"
#include "../../../library/functions/parallax.glsl"

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
	vec2 texture_coords = calculateParallaxMapping(fs_in.texture_coords, normalize(fs_in.tangent_view_position - fs_in.tangent_fragment_position));

	// albedo color
	vec4 albedo_color = texture(material.albedo, texture_coords);

	// metallic amount
	float metallic_amount = texture(material.metallic, texture_coords).r;

	// roughness amount
	float roughness_amount = texture(material.roughness, texture_coords).r;

	// ambient amount
	float ambient_amount = texture(material.ambient, texture_coords).r;

	// fragment normal vector
	vec3 fragment_normal = (texture(material.normal, texture_coords).rgb * 2.0f - 1.0f) * fs_in.tbn;

	// fill position buffer
	position = fs_in.fragment_position;

	// fill normal buffer
	normal = fragment_normal;

	// fill albedo buffer
	albedo = albedo_color;

	// fill metallic buffer
	surface.r = metallic_amount;

	// fill roughness buffer
	surface.g = roughness_amount;

	// fill ambient buffer
	surface.b = ambient_amount;

	// fill shadows buffer
	extra.r = float(renderer.shadows);
}