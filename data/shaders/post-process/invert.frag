#version 460 core

// shader inputs
in VsOut
{
	vec2 texture_coords;
} fs_in;

// screen image
uniform sampler2D screen;

void main()
{
	// shader output
	gl_FragColor = vec4(vec3(1.0f - texture(screen, fs_in.texture_coords)), 1.0f);
}