// Natural vignetting by ApoorvaJ
// https://www.shadertoy.com/view/4lSXDm
#version 460 core

#include "../library/buffers/camera.glsl"

// shader inputs
in VsOut
{
	vec2 texture_coords;
} fs_in;

// screen image
uniform sampler2D screen;

// fall off
float fall_off = 0.25f;

void main()
{
	// screen resolution
	vec2 resolution = camera.viewport.zw;

	// vignette
	vec2 uv = gl_FragCoord.xy / resolution.xy;
    vec2 coord = (uv - 0.5) * (resolution.x / resolution.y) * 2.0;
    float rf = sqrt(dot(coord, coord)) * fall_off;
    float rf2_1 = rf * rf + 1.0;
    float e = 1.0 / (rf2_1 * rf2_1);

	// source color
	vec3 src = texture(screen, fs_in.texture_coords).rgb;

	// shader output
	gl_FragColor = vec4(src * e, 1.0);
}