#version 460 core

#include "../library/buffers/camera.glsl"

// shader inputs
in VsOut
{
	vec3 vertex_normal;
	vec3 fragment_position;
} fs_in;

// material struct
uniform struct Material
{
	vec4 color;
	samplerCube cube;
} material;

void main()
{
	// view direction from camera to fragment
	vec3 view_direction = normalize(fs_in.fragment_position - vec3(camera.position));

	// reflection direction
	vec3 reflect_direction = reflect(view_direction, normalize(fs_in.vertex_normal));

	// shader output
	gl_FragColor = vec4(texture(material.cube, reflect_direction).rgb, 1.0f) * material.color;
}