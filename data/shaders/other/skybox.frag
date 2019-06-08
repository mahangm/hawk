#version 460 core

// shader inputs
in VsOut
{
	vec3 texture_coords;
} fs_in;

// material struct
uniform struct Material
{
	samplerCube cube;
} material;

void main()
{
	// shader output
	gl_FragColor = texture(material.cube, fs_in.texture_coords);
}