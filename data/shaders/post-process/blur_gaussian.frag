#version 460 core

// curve weights
const float WEIGHTS[5] = float[] (0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f);

// total passes
const int PASSES = 5;

// shader inputs
in VsOut
{
	vec2 texture_coords;
} fs_in;

// screen image
uniform sampler2D screen;

// horizontal pass
uniform bool horizontal;

void main()
{
	// texel size
	vec2 texture_offset = 1.0f / textureSize(screen, 0);

	// current fragment contribution
	vec3 result = texture(screen, fs_in.texture_coords).rgb * WEIGHTS[0];

	// check blur direction
	if (horizontal)
	{
		// horizontal blur pass
		for (int i = 1; i < PASSES; ++i)
		{
			result += texture(screen, fs_in.texture_coords + vec2(texture_offset.x * i, 0.0f)).rgb * WEIGHTS[i];
			result += texture(screen, fs_in.texture_coords - vec2(texture_offset.x * i, 0.0f)).rgb * WEIGHTS[i];
		}
	}
	else
	{
		// vertical blur pass
		for (int i = 1; i < PASSES; ++i)
		{
			result += texture(screen, fs_in.texture_coords + vec2(0.0f, texture_offset.y * i)).rgb * WEIGHTS[i];
			result += texture(screen, fs_in.texture_coords - vec2(0.0f, texture_offset.y * i)).rgb * WEIGHTS[i];
		}
	}

	// shader output
	gl_FragColor = vec4(result, 1.0f);
}