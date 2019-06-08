#version 460 core

// shader inputs
in VsOut
{
	vec2 texture_coords;
} fs_in;

// material struct
uniform struct Material
{
	vec4 color;
	sampler2D texture;
} material;

void main()
{
	// default color buffer output
	gl_FragColor = texture(material.texture, fs_in.texture_coords) * material.color;
}