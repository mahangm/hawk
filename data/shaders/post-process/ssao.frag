#version 460 core

#include "../library/buffers/camera.glsl"

// shader inputs
in VsOut
{
	vec2 texture_coords;
} fs_in;

// fragment position buffer
layout (binding = 0) uniform sampler2D position;

// fragment normal buffer
layout (binding = 1) uniform sampler2D normal;

// ssao noise texture
layout (binding = 2) uniform sampler2D noise;

// ssao sample kernel
uniform vec3 kernel[64];

// sample radius
float radius = 0.5f;

// sample bias
float bias = 0.025f;

// occlusion strength
float strength = 2.0f;

void main()
{
	// fragment normal
	vec3 fragment_normal = texture(normal, fs_in.texture_coords).xyz;

	// discard fragment if normal is empty
	if (fragment_normal == vec3(0.0f)) discard;

	// fragment position in view space
	vec3 fragment_position = vec3(camera.view * vec4(texture(position, fs_in.texture_coords).xyz, 1.0f));

	// fragment normal in view space
	fragment_normal = normalize(mat3(camera.view) * fragment_normal);

	// ssao noise texture scale
	vec2 noise_scale = vec2(camera.viewport.z / 4.0f, camera.viewport.w / 4.0f);

	// random sampling vector
	vec3 sample_vector = normalize(texture(noise, fs_in.texture_coords * noise_scale).xyz);

	// fragment tangent
	vec3 fragment_tangent = normalize(sample_vector - fragment_normal * dot(sample_vector, fragment_normal));
	
	// fragment bitangent
	vec3 fragment_bitangent = cross(fragment_normal, fragment_tangent);
	
	// tbn matrix
	mat3 tbn = mat3(fragment_tangent, fragment_bitangent, fragment_normal);

	// fragment occlusion factor
	float occlusion = 0.0f;

	// calculate occlusion factor
	for (int i = 0; i < 64; ++i)
	{
		// sample position in view space
		vec3 sample_position = fragment_position + (tbn * kernel[i]) * radius;

		// position texture offset in clip space
		vec4 offset = camera.projection * vec4(sample_position, 1.0f);

		// perspective divide
		offset.xyz /= offset.w;

		// transform to (0, 1) range
		offset.xyz = offset.xyz * 0.5f + 0.5f;

		// sample depth
		float sample_depth = vec3(camera.view * texture(position, offset.xy)).z;

		// fragment radius range check
		float range_check = smoothstep(0.0f, 1.0f, radius / abs(fragment_position.z - sample_depth));

		// calculate
		occlusion += (sample_depth >= sample_position.z + bias ? 1.0f : 0.0f) * range_check;
	}

	gl_FragColor.r = pow(1.0f - (occlusion / 64.0f), strength);
}