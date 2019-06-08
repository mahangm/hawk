#version 460 core

#include "../../../library/constants.glsl"
#include "../../../library/structs/light.glsl"
#include "../../../library/structs/shadow_map.glsl"
#include "../../../library/structs/renderer.glsl"
#include "../../../library/structs/material/blinn-phong/colored.glsl"
#include "../../../library/buffers/video.glsl"
#include "../../../library/buffers/camera.glsl"
#include "../../../library/buffers/scene.glsl"
#include "../../../library/functions/shadow.glsl"

// shader inputs
in VsOut
{
	vec3 vertex_normal;
	vec3 fragment_position;
} fs_in;

// active lights
uniform uint lights_count;

void main()
{
	// fragment normal vector
	vec3 fragment_normal = normalize(fs_in.vertex_normal);

	// camera view direction
	vec3 view_direction = normalize(vec3(camera.position) - fs_in.fragment_position);

	// lighting result
	vec4 lighting = vec4(0.0f);

	// calculate lights
	for (uint i = 0; i < lights_count; ++i)
	{
		// direction of light
		vec3 light_direction = (scene.lights[i].type == 1) ? normalize(-vec3(scene.lights[i].direction)) : normalize(vec3(scene.lights[i].position) - fs_in.fragment_position);

		// blinn halfway vector
		vec3 halfway_direction = normalize(light_direction + view_direction);

		// diffuse
		float diffuse_impact = max(dot(fragment_normal, light_direction), 0.0f);
		vec4 diffuse = scene.lights[i].color * diffuse_impact * material.diffuse;

		// specular
		float specular_impact = pow(max(dot(fragment_normal, halfway_direction), 0.0f), material.shininess);
		vec4 specular = scene.lights[i].color * specular_impact * material.specular;

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
			float distance = length(vec3(scene.lights[i].position) - fs_in.fragment_position);
			float attenuation = pow(clamp(1.0 - pow(distance / scene.lights[i].range, 1.0), 0.0, 1.0), 2.0) / (distance * distance + 1.0);

			diffuse *= attenuation;
			specular *= attenuation;
		}

		// shadow
		float shadow = 0.0f;
		
		// if light casts shadows
		if (scene.lights[i].cast_shadows)
		{
			// if renderer recieves shadows
			if (renderer.shadows == 1)
			{
				// calculate shadow
				switch (scene.lights[i].type)
				{
				case 1:
				case 3:
					shadow = calculateDirectionalShadow(i, scene.lights[i].matrix * vec4(fs_in.fragment_position, 1.0f));
					break;
				case 2:
					shadow = calculateOmnidirectionalShadow(i, fs_in.fragment_position - vec3(scene.lights[i].position), fs_in.fragment_position);
					break;
				}
			}
		}

		lighting += (1.0f - shadow) * (diffuse + specular);	
	}

	// ambient
	vec4 ambient_color = video.ambient * material.diffuse;

	// default color buffer output
	gl_FragColor = ambient_color + lighting;
}