#version 460 core

#include "../../../library/constants.glsl"
#include "../../../library/structs/light.glsl"
#include "../../../library/structs/shadow_map.glsl"
#include "../../../library/structs/renderer.glsl"
#include "../../../library/structs/material/pbr/textured.glsl"
#include "../../../library/buffers/video.glsl"
#include "../../../library/buffers/camera.glsl"
#include "../../../library/buffers/scene.glsl"
#include "../../../library/functions/shadow.glsl"
#include "../../../library/functions/pbr.glsl"

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

// irradiance map
layout (binding = 24) uniform samplerCube irradiance;

// radiance map
layout (binding = 25) uniform samplerCube radiance;

// brdf integration map
layout (binding = 26) uniform sampler2D brdf;

void main()
{
	// camera view direction
	vec3 view_direction = normalize(vec3(camera.position) - fs_in.fragment_position);

	// texture coordination
	vec2 texture_coords = fs_in.texture_coords;

	// albedo color
	vec4 albedo_color = texture(material.albedo, texture_coords);
	
	// metallic amount
	float metallic_amount = texture(material.metallic, texture_coords).r;

	// roughness amount
	float roughness_amount = texture(material.roughness, texture_coords).r;

	// ambient amount
	float ambient_amount = texture(material.ambient, texture_coords).r;

	// fragment normal vector
	vec3 fragment_normal = fs_in.vertex_normal;

	// reflection direction
	vec3 reflect_direction = reflect(-view_direction, fragment_normal);

	// reflectance at normal incidence, if die-electric use f0 of 0.04 otherwise use albedo for metallic workflow
	vec4 f0 = mix(vec4(0.04f), albedo_color, metallic_amount);

	// lighting result
	vec4 lighting = vec4(0.0f);

	// calculate lights (reflectance equation)
	for (uint i = 0; i < lights_count; ++i)
	{	
		// direction of light
		vec3 light_direction = (scene.lights[i].type == 1) ? normalize(-vec3(scene.lights[i].direction)) : normalize(vec3(scene.lights[i].position) - fs_in.fragment_position);

		// blinn halfway vector
		vec3 halfway_direction = normalize(view_direction + light_direction);

		// radiance color
		vec4 radiance = scene.lights[i].color;

		// soft edge for spot light
		if (scene.lights[i].type == 3)
		{
			float theta = dot(light_direction, normalize(-vec3(scene.lights[i].direction)));
			float epsilon = scene.lights[i].cut_off - scene.lights[i].outer_cut_off;
			float intensity = clamp((theta - scene.lights[i].outer_cut_off) / epsilon, 0.0f, 1.0f);

			radiance *= intensity;
		}

		// attenuation for point and spot light
		if (scene.lights[i].type != 1)
		{
			float distance = length(vec3(scene.lights[i].position) - fs_in.fragment_position);
			float attenuation = pow(clamp(1.0 - pow(distance / scene.lights[i].range, 1.0), 0.0, 1.0), 2.0) / (distance * distance + 1.0);

			radiance *= attenuation;
		}

		// cook-torrance brdf
		float ndf = distributionGGX(fragment_normal, halfway_direction, roughness_amount);
		float g = geometrySmith(fragment_normal, view_direction, light_direction, roughness_amount);
		vec4 f = fresnelSchlick(clamp(dot(halfway_direction, view_direction), 0.0f, 1.0f), f0);
		vec4 nom = ndf * g * f;
		float denom = 4.0f * max(dot(fragment_normal, view_direction), 0.0f) * max(dot(fragment_normal, light_direction), 0.0f);

		// specular color (prevent division by zero for n_dot_v 0.0 or n_dot_l 0.0)
		vec4 specular = nom / max(denom, 0.001f);

		// incoming light equal to fresnel
		vec4 ks = f;

		// outgoing light with energy conservation
		vec4 kd = vec4(1.0f) - ks;

		// kill diffuse lighting for pure metallic surfaces
		kd *= 1.0f - metallic_amount;

		// scale light by n_dot_l
		float n_dot_l = max(dot(fragment_normal, light_direction), 0.0f);

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

		// add to outgoing radiance (we already effected ks in BRDF so no need here)
		lighting += (1.0f - shadow) * (kd * albedo_color / PI + specular) * radiance * n_dot_l;
	}

	// ambient light
	vec4 ambient_color = vec4(1.0f);

	// take ambient light from environment
	if (camera.ibl)
	{
		// fresnel
		vec4 f = fresnelSchlickRoughness(max(dot(fragment_normal, view_direction), 0.0f), f0, roughness_amount);

		// incoming ambient light
		vec4 ks = f;

		// outgoing ambient light
		vec4 kd = vec4(1.0f) - ks;

		// kill outgoing light for pure metallic surfaces
		kd *= 1.0f - metallic_amount;

		// indirect diffuse color
		vec4 irradiance_color = texture(irradiance, fragment_normal);

		// indirect prefiltered sepcular color
		vec4 radiance_prefiltered_color = textureLod(radiance, reflect_direction, roughness_amount * MAX_REFLECTION_LOD);

		// environment brdf mappping
		vec2 brdf_integration = texture(brdf, vec2(max(dot(fragment_normal, view_direction), 0.0f), roughness_amount)).xy;

		// indirect specular color
		vec4 radiance_color = radiance_prefiltered_color * (f * brdf_integration.x + brdf_integration.y);

		// calculate ambient
		ambient_color = (kd * (irradiance_color * albedo_color) + radiance_color) * ambient_amount;
	}
	else
	{
		// calculate ambient
		ambient_color = video.ambient * albedo_color * ambient_amount;
	}

	// discard full transparent fragments
	if (albedo_color.a <= 0.01f) discard;
	
	// default color buffer output
	gl_FragColor = ambient_color + lighting;
}