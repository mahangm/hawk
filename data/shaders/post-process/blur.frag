#version 460 core

// uv offset
const float OFFSET = 1.0f / 300.0f;

// shader inputs
in VsOut
{
	vec2 texture_coords;
} fs_in;

// screen image
uniform sampler2D screen;

void main()
{
	// surrounding fragments coordiantes
    vec2 offsets[9] = vec2[](
        vec2(-OFFSET,  OFFSET), // top-left
        vec2( 0.0f,    OFFSET), // top-center
        vec2( OFFSET,  OFFSET), // top-right
        vec2(-OFFSET,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( OFFSET,  0.0f),   // center-right
        vec2(-OFFSET, -OFFSET), // bottom-left
        vec2( 0.0f,   -OFFSET), // bottom-center
        vec2( OFFSET, -OFFSET)  // bottom-right    
    );

	// blur kernel
    float kernel[9] = float[](
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
        1.0f / 16.0f, 4.0f / 16.0f, 1.0f / 16.0f,
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
    );
    
	// texture samples
    vec3 samples[9];

	// fill texture samples
    for (int i = 0; i < 9; ++i)
        samples[i] = vec3(texture(screen, fs_in.texture_coords.st + offsets[i]));

	// frag color
    vec3 frag_color = vec3(0.0);

	// make frag color
    for (int i = 0; i < 9; ++i)
        frag_color += samples[i] * kernel[i];

	// shader output
	gl_FragColor = vec4(frag_color, 1.0f);
}