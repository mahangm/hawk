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
	// fragment color
	vec4 frag_color = texture(screen, fs_in.texture_coords);

	// average of fragment color components
	float average = 0.2126f * frag_color.r + 0.7152f * frag_color.g + 0.0722f * frag_color.b;

	// shader output
	gl_FragColor = vec4(average, average, average, 1.0f);
}