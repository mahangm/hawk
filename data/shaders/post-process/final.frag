#version 460 core

#include "../library/buffers/video.glsl"

// shader inputs
in VsOut
{
	vec2 texture_coords;
} fs_in;

// screen image
layout (binding = 0) uniform sampler2D screen;

// bloom image
layout (binding = 1) uniform sampler2D bloom;

// gamma value
uniform float gamma = 2.2f;

// hdr exposure
uniform float exposure = 1.0f;

void main()
{
	// hdr color sample
	vec4 result = texture(screen, fs_in.texture_coords);

	// bloom effect
	if (video.bloom)
	{
		// bloom color sample
		vec4 bloom_color = texture(bloom, fs_in.texture_coords);

		// additive blend bloom
		result += bloom_color;
	}

	// reinhard tone mapping
	//result = result / (result + vec4(1.0f));

	// exposure tone mapping
	result = vec4(1.0f) - exp(-result * exposure);

	// gamma correction
	result = pow(result, vec4(1.0f / gamma));

	// shader output
	gl_FragColor = result;
}