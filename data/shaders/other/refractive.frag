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

// refraction ratio e. g. 1.00 (air) / 1.52 (glass), (air = 1.00, water = 1.33, ice = 1.309, glass = 1.52, diamond = 2.42) 
uniform float ratio;

void main()
{
	// view direction from camera to fragment
	vec3 view_direction = normalize(fs_in.fragment_position - vec3(camera.position));

	// refraction direction
	vec3 refract_direction = refract(view_direction, normalize(fs_in.vertex_normal), ratio);

	// shader output
	gl_FragColor = vec4(texture(material.cube, refract_direction).rgb, 1.0f) * material.color;
}