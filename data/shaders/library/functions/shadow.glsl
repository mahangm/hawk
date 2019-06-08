#ifndef SHADOW_FUNCTIONS
#define SHADOW_FUNCTIONS
// array of offset direction for omnidirectional shadow sampling
const vec3 SHADOW_GRID_SAMPLING_DISK[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

// shadow bias
const float SHADOW_BIAS = 0.005f;

float calculateDirectionalShadow(uint light_id, vec4 fragment_light_position)
{
	// shadow map index
	uint shadow_map_id = scene.lights[light_id].shadow_map_id;

	// perspective divide
	vec3 projection_coords = fragment_light_position.xyz / fragment_light_position.w;

	// map to [0, 1] range
	projection_coords = projection_coords * 0.5f + 0.5f;
	
	// current fragment depth from light's perspective
	float current_depth = projection_coords.z;

	// shadow amount
	float shadow = 0.0f;

	// shadow map texel size
	vec2 texel_size = 1.0f / textureSize(shadow_maps[shadow_map_id].dsm, 0);

	// pcf
	for (int x = -2; x <= 2; ++x)
	{
		for (int y = -2; y <= 2; ++y)
		{
			// pcf depth
			float pcf_depth = texture(shadow_maps[shadow_map_id].dsm, projection_coords.xy + vec2(x, y) * texel_size).r;

			// add to shadow if fragment is in shadow
			shadow += current_depth > pcf_depth ? 1.0f : 0.0f;
		}
	}

	// average of surrounding texels
	shadow /= 25.0f;

	// solve over sampling
	if (projection_coords.z > 1.0f)
		shadow = 0.0f;

	return shadow;
}

float calculateOmnidirectionalShadow(uint light_id, vec3 fragment_light_position, vec3 fragment_position)
{
	// shadow map index
	uint shadow_map_id = scene.lights[light_id].shadow_map_id;

	// current fragment depth
	float current_depth = length(fragment_light_position);

	// view distance to fragment
	float view_distance = length(vec3(camera.position) - fragment_position);

	// disk 
	float disk_radius = (1.0f + (view_distance / scene.lights[light_id].far_plane)) / 25.0f;

	// shadow amount
	float shadow = 0.0f;

	// shadow samples
	int samples = 20;

	// pcf
	for (int i = 0; i < samples; i++)
	{
		// closest depth to light position
		float closest_depth = texture(shadow_maps[shadow_map_id].odsm, fragment_light_position + SHADOW_GRID_SAMPLING_DISK[i] * disk_radius).r;

		// map to [0, light's far plane] range
		closest_depth *= scene.lights[light_id].far_plane;

		// if fragment is in shadow
		if (current_depth - SHADOW_BIAS > closest_depth) shadow += 1.0f;
	}

	// average of samples
	shadow /= float(samples);

	return shadow;
}
#endif