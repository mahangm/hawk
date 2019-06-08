#version 460 core

// shader inputs
in VsOut
{
	vec2 texture_coords;
} fs_in;

// ssao image
uniform sampler2D ssao;

void main()
{
	// ssao image texel size
	vec2 texel_size = 1.0f / vec2(textureSize(ssao, 0));

	// result
	float result = 0.0f;

	// sample fragments
	for (int x = -2; x < 2; ++x)
	{
		for (int y = -2; y < 2; ++y)
		{
			// sample offset
			vec2 offset = vec2(float(x), float(y)) * texel_size;

			result += texture(ssao, fs_in.texture_coords + offset).r;
		}
	}

	gl_FragColor.r = result / (4.0f * 4.0f);
}