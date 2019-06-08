#version 460 core

// shader inputs
in GsOut
{
	vec4 fragment_position;
} fs_in;

// light position
uniform vec3 light_position;

// light frustum far plane
uniform float light_far_plane;

void main()
{
	// fragment distance to light
	float light_distance = length(fs_in.fragment_position.xyz - light_position);

	// map to [0, 1] range
	light_distance = light_distance / light_far_plane;

	// set light distance fragment depth
	gl_FragDepth = light_distance;
}