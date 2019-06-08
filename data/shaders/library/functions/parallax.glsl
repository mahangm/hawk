#ifndef PARALLAX_FUNCTIONS
#define PARALLAX_FUNCTIONS
vec2 calculateParallaxMapping(vec2 texture_coords, vec3 view_direction)
{
	// depth layers limit
	const float min_layers = 8, max_layers = 32;

	// depth layers count based on view direction
	float layers = mix(max_layers, min_layers, abs(dot(vec3(0.0f, 0.0f, 1.0f), view_direction)));

	// depth of each layer
	float layer_depth = 1.0f / layers;

	// depth of current layer
	float current_layer_depth = 0.0f;

	// the amount to shift the texture coordinates per layer
	vec2 p = view_direction.xy / view_direction.z * material.height;

	// shifted texture coordinates
	vec2 delta_texture_coords = p / layers;

	// current texture coordinates
	vec2 current_texture_coords = texture_coords;

	// current displacement depth
	float current_depth = texture(material.displacement, current_texture_coords).r;

	// iterate layers top to bottom
	while (current_layer_depth < current_depth)
	{
		// shift texture coordinates along side p
		current_texture_coords -= delta_texture_coords;

		// get current shifted displacement depth
		current_depth = texture(material.displacement, current_texture_coords).r;

		// get next layer depth
		current_layer_depth += layer_depth;
	}

	// texture coordinate before collision
	vec2 prev_texture_coordinates = current_texture_coords + delta_texture_coords;

	// displacement depth before collision
	float before_depth = texture(material.displacement, prev_texture_coordinates).r - current_layer_depth + layer_depth;
	
	// depth after collision
	float after_depth = current_depth - current_layer_depth;

	// weight of interpolation
	float weight = after_depth / (after_depth - before_depth);

	// interpolate texture coordinates based on closest depths
	vec2 final_texture_coords = prev_texture_coordinates * weight + current_texture_coords * (1.0f - weight);

	return final_texture_coords;
}
#endif