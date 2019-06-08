#version 460 core

#include "../library/buffers/video.glsl"

// color buffers output
layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bloom_color;

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
	frag_color = texture(screen, fs_in.texture_coords);
	
	// bloom effect
	if (video.bloom)
	{
		// fragment brightness
		float brightness = dot(frag_color.rgb, vec3(0.2126f, 0.7152f, 0.0722f));

		// bloom color buffer output
		bloom_color = (brightness > 1.0f) ? vec4(frag_color.rgb, 1.0f) : vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}