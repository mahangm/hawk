#version 460 core

#include "../../../library/constants.glsl"
#include "../../../library/structs/light.glsl"
#include "../../../library/structs/shadow_map.glsl"
#include "../../../library/structs/renderer.glsl"
#include "../../../library/structs/material/blinn-phong/textured_normal.glsl"
#include "../../../library/buffers/video.glsl"
#include "../../../library/buffers/camera.glsl"
#include "../../../library/buffers/scene.glsl"
#include "../../../library/functions/shadow.glsl"

// shader inputs
in VsOut
{
	vec3 vertex_normal;
	vec2 texture_coords;
	vec3 fragment_position;
	vec3 tangent_view_position;
	vec3 tangent_fragment_position;
	mat3 tbn;
} fs_in;

// active lights
uniform uint lights_count;

void main()
{
	// camera view direction
	vec3 view_direction = normalize(vec3(camera.position) - fs_in.fragment_position);

	// texture coordination
	vec2 texture_coords = fs_in.texture_coords;
	
	// diffuse color
	vec4 diffuse_color = texture(material.diffuse, texture_coords);

	// specular color
	vec4 specular_color = texture(material.specular, texture_coords);

	// fragment normal vector
	vec3 fragment_normal = normalize((texture(material.normal, texture_coords).rgb * 2.0f - 1.0f) * fs_in.tbn);

	// lighting result
	vec4 lighting = vec4(0.0f);

	// calculate lights
	for (uint i = 0; i < lights_count; ++i)
	{
		// direction of light
		vec3 light_direction = (scene.lights[i].type == 1) ? normalize(-vec3(scene.lights[i].direction)) : normalize(vec3(scene.lights[i].position) - fs_in.fragment_position);

		// blin halfway vector
		vec3 halfway_direction = normalize(light_direction + view_direction);

		// diffuse
		float diffuse_impact = max(dot(fragment_normal, light_direction), 0.0f);
		vec4 diffuse = scene.lights[i].color * diffuse_impact * diffuse_color;

		// specular
		float specular_impact = pow(max(dot(fragment_normal, halfway_direction), 0.0f), material.shininess);
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
	vec4 ambient_color = video.ambient * diffuse_color;

	// default color buffer output
	gl_FragColor = ambient_color + lighting;
}