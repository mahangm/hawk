#version 460 core

#include "../../../library/constants.glsl"
#include "../../../library/structs/light.glsl"
#include "../../../library/structs/shadow_map.glsl"
#include "../../../library/buffers/video.glsl"
#include "../../../library/buffers/camera.glsl"
#include "../../../library/buffers/scene.glsl"
#include "../../../library/functions/shadow.glsl"

// shader inputs
in VsOut
{
	vec2 texture_coords;
} fs_in;

// active lights
uniform uint lights_count;

// fragment position buffer
layout (binding = 0) uniform sampler2D position;

// fragment normal buffer
layout (binding = 1) uniform sampler2D normal;

// fragment diffuse buffer
layout (binding = 2) uniform sampler2D diffuse;

// fragment specular buffer
layout (binding = 3) uniform sampler2D specular;

// fragment extra buffer
layout (binding = 4) uniform sampler2D extra;

// ssao texture
layout (binding = 5) uniform sampler2D ssao;

void main()
{
	// fragment normal
	vec3 fragment_normal = normalize(texture(normal, fs_in.texture_coords).xyz);

	// discard fragment if normal is empty
	if (fragment_normal == vec3(0.0f)) discard;

	// fragment position
	vec3 fragment_position = texture(position, fs_in.texture_coords).xyz;

	// diffuse color
	vec4 diffuse_color = texture(diffuse, fs_in.texture_coords);

	// specular color
	vec4 specular_color = texture(specular, fs_in.texture_coords);

	// extra amounts
	vec2 extra_amounts = texture(extra, fs_in.texture_coords).rg;

	// specular shininess
	float specular_shininess = extra_amounts.r;

	// fragment shadows
	int fragment_shadows = int(extra_amounts.g);

	// camera view direction
	vec3 view_direction = normalize(vec3(camera.position) - fragment_position);

	// lighting result
	vec4 lighting = vec4(0.0f);

	// calculate lights
	for (uint i = 0; i < lights_count; ++i)
	{
		// fragment position in light space
		vec4 fragment_light_position = scene.lights[i].matrix * vec4(fragment_position, 1.0f);

		// direction of light
		vec3 light_direction = (scene.lights[i].type == 1) ? normalize(-vec3(scene.lights[i].direction)) : normalize(vec3(scene.lights[i].position) - fragment_position);

		// blin halfway vector
		vec3 halfway_direction = normalize(light_direction + view_direction);

		// diffuse
		float diffuse_impact = max(dot(fragment_normal, light_direction), 0.0f);
		vec4 diffuse = scene.lights[i].color * diffuse_impact * diffuse_color;

		// specular
		float specular_impact = pow(max(dot(fragment_normal, halfway_direction), 0.0f), specular_shininess);
		vec4 specular = scene.lights[i].color * specular_impact * specular_color;

		// soft edge for spot light
		if (scene.lights[i].type == 3)
		{
			float theta = dot(light_direction, normalize(-vec3(scene.lights[i].direction)));
			float epsilon = scene.lights[i].cut_off - scene.lights[i].outer_cut_off;
			float intensity = clamp((theta - scene.lights[i].outer_cut_off) / epsilon, 0.0f, 1.0f);

			diffuse *= intensity;
			specular *= intensity;
		}

		// attenuation for point and spot light
		if (scene.lights[i].type != 1)
		{
			float distance = length(vec3(scene.lights[i].position) - fragment_position);
			float attenuation = pow(clamp(1.0 - pow(distance / scene.lights[i].range, 1.0), 0.0, 1.0), 2.0) / (distance * distance + 1.0);

			diffuse *= attenuation;
			specular *= attenuation;
		}

		// shadow
		float shadow = 0.0f;
		
		// if light casts shadows
		if (scene.lights[i].cast_shadows)
		{
			// if fragment recieves shadows
			if (fragment_shadows == 1)
			{
				// calculate shadow
				switch (scene.lights[i].type)
				{
				case 1:
				case 3:
					shadow = calculateDirectionalShadow(i, scene.lights[i].matrix * vec4(fragment_position, 1.0f));
					break;
				case 2:
					shadow = calculateOmnidirectionalShadow(i, fragment_position - vec3(scene.lights[i].position), fragment_position);
					break;
				}
			}
		}

		lighting += (1.0f - shadow) * (diffuse + specular);
	}

	// ambient occlusion factor
	float ambient_occlusion = 1.0f;

	// ssao effect
	if (video.ssao) ambient_occlusion = texture(ssao, fs_in.texture_coords).r;

	// ambient
	vec4 ambient_color = video.ambient * diffuse_color * ambient_occlusion;

	// default color buffer output
	gl_FragColor = ambient_color + lighting;
}