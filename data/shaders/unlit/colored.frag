#version 460 core

// material struct
uniform struct Material
{
	vec4 color;
} material;

void main()
{
	// default color buffer output
	gl_FragColor = material.color;
}