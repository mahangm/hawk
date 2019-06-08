#version 460 core

#include "../../../library/buffers/camera.glsl"
#include "../../../library/structs/transform.glsl"

// vertex attributes
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

// shader outputs
out VsOut
{
	vec3 vertex_normal;
	vec2 texture_coords;
	vec3 fragment_position;
	vec3 tangent_view_position;
	vec3 tangent_fragment_position;
	mat3 tbn;
} vs_out;

void main()
{
	// tbn vectors
	vec3 t = normalize(mat3(transform.normal) * tangent);
	vec3 b = normalize(mat3(transform.normal) * bitangent);
	vec3 n = normalize(mat3(transform.normal) * normal);

	// right handedness fix for models with mirrored uvs
	if (dot(cross(n, t), b) < 0.0f) t *= -1.0f;

	// set tbn matrix
	vs_out.tbn = transpose(mat3(t, b, n));

	// set shader outputs
	vs_out.vertex_normal = mat3(transform.normal) * normal;
	vs_out.texture_coords = uv;
	vs_out.fragment_position = vec3(transform.model * vec4(position, 1.0f));
	vs_out.tangent_view_position = vs_out.tbn * vec3(camera.position);
	vs_out.tangent_fragment_position = vs_out.tbn * vs_out.fragment_position;

	// shader output
	gl_Position = camera.projection * camera.view * transform.model * vec4(position, 1.0f);
}