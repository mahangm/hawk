// Fast Approxmiation Anti Aliasing by Simon Rodriguez
// http://blog.simonrodriguez.fr/articles/30-07-2016_implementing_fxaa.html
#version 460 core

#include "../library/buffers/camera.glsl"

#define EDGE_THRESHOLD_MIN 0.0312
#define EDGE_THRESHOLD_MAX 0.125
#define EDGE_ITERATIONS 12
#define SUBPIXEL_QUALITY 0.75

// pixel quality for edge bounds
const float QUALITY[] = float[] (1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 2.0f, 2.0f, 2.0f, 2.0f, 4.0f, 8.0f);

// shader inputs
in VsOut
{
	vec2 texture_coords;
} fs_in;

// screen image
uniform sampler2D screen;

float rgbToLuma(vec3 rgb)
{
    return sqrt(dot(rgb, vec3(0.299f, 0.587f, 0.114f)));
}

void main()
{
	// inverse screen size
	vec2 inverse_screen_size = vec2(1.0f / camera.viewport.z, 1.0f / camera.viewport.w);

	// center fragment color
	vec3 color_center = texture(screen, fs_in.texture_coords).rgb;

	// luma at the current fragment
	float luma_center = rgbToLuma(color_center);

	// luma at the four direct neighbours of the current fragment
	float luma_down = rgbToLuma(textureOffset(screen, fs_in.texture_coords, ivec2(0, -1)).rgb);
	float luma_up = rgbToLuma(textureOffset(screen, fs_in.texture_coords, ivec2(0, 1)).rgb);
	float luma_left = rgbToLuma(textureOffset(screen, fs_in.texture_coords, ivec2(-1, 0)).rgb);
	float luma_right = rgbToLuma(textureOffset(screen, fs_in.texture_coords, ivec2(1, 0)).rgb);

	// find the maximum and minimum luma around the current fragment
	float luma_min = min(luma_center, min(min(luma_down, luma_up), min(luma_left, luma_right)));
	float luma_max = max(luma_center, max(max(luma_down, luma_up), max(luma_left, luma_right)));

	// compute the delta.
	float luma_range = luma_max - luma_min;

	// if the luma variation is lower that a threshold (or if we are in a really dark area), we are not on an edge, don't perform any AA
	if (luma_range < max(EDGE_THRESHOLD_MIN, luma_max * EDGE_THRESHOLD_MAX))
	{
		gl_FragColor = vec4(color_center, 1.0f);

		return;
	}

	// query the 4 remaining corners lumas
	float luma_down_left = rgbToLuma(textureOffset(screen, fs_in.texture_coords, ivec2(-1, -1)).rgb);
	float luma_up_right = rgbToLuma(textureOffset(screen, fs_in.texture_coords, ivec2(1, 1)).rgb);
	float luma_up_left = rgbToLuma(textureOffset(screen, fs_in.texture_coords, ivec2(-1, 1)).rgb);
	float luma_down_right = rgbToLuma(textureOffset(screen, fs_in.texture_coords, ivec2(1, -1)).rgb);

	// combine the four edges lumas (using intermediary variables for future computations with the same values)
	float luma_down_up = luma_down + luma_up;
	float luma_left_right = luma_left + luma_right;

	// same for corners
	float luma_left_corners = luma_down_left + luma_up_left;
	float luma_down_corners = luma_down_left + luma_down_right;
	float luma_right_corners = luma_down_right + luma_up_right;
	float luma_up_corners = luma_up_right + luma_up_left;

	// compute an estimation of the gradient along the horizontal and vertical axis
	float edge_horizontal =  abs(-2.0f * luma_left + luma_left_corners) + abs(-2.0f * luma_center + luma_down_up ) * 2.0f + abs(-2.0f * luma_right + luma_right_corners);
	float edge_vertical = abs(-2.0f * luma_up + luma_up_corners) + abs(-2.0f * luma_center + luma_left_right) * 2.0f + abs(-2.0f * luma_down + luma_down_corners);

	// is the local edge horizontal or vertical?
	bool is_horizontal = (edge_horizontal >= edge_vertical);

	// select the two neighboring texels lumas in the opposite direction to the local edge
	float luma1 = is_horizontal ? luma_down : luma_left;
	float luma2 = is_horizontal ? luma_up : luma_right;

	// compute gradients in this direction
	float gradient1 = luma1 - luma_center;
	float gradient2 = luma2 - luma_center;

	// which direction is the steepest?
	bool is_first_steepest = abs(gradient1) >= abs(gradient2);

	// gradient in the corresponding direction, normalized
	float gradient_scaled = 0.25f * max(abs(gradient1), abs(gradient2));

	// choose the step size (one pixel) according to the edge direction
	float step_length = is_horizontal ? inverse_screen_size.y : inverse_screen_size.x;

	// average luma in the correct direction
	float luma_local_average = 0.0f;

	if (is_first_steepest)
	{
		// switch the direction
		step_length = -step_length;
		luma_local_average = 0.5f * (luma1 + luma_center);
	} 
	else 
	{
		luma_local_average = 0.5f * (luma2 + luma_center);
	}

	// shift UV in the correct direction by half a pixel
	vec2 current_uv = fs_in.texture_coords;

	if (is_horizontal)
		current_uv.y += step_length * 0.5f;
	else
		current_uv.x += step_length * 0.5f;
	
	// compute offset (for each iteration step) in the right direction
	vec2 offset = is_horizontal ? vec2(inverse_screen_size.x, 0.0f) : vec2(0.0f, inverse_screen_size.y);

	// compute UVs to explore on each side of the edge, orthogonally. the QUALITY allows us to step faster
	vec2 uv1 = current_uv - offset;
	vec2 uv2 = current_uv + offset;

	// read the lumas at both current extremities of the exploration segment, and compute the delta wrt to the local average luma
	float luma_end1 = rgbToLuma(texture(screen, uv1).rgb);
	float luma_end2 = rgbToLuma(texture(screen, uv2).rgb);

	luma_end1 -= luma_local_average;
	luma_end2 -= luma_local_average;

	// if the luma deltas at the current extremities are larger than the local gradient, we have reached the side of the edge
	bool reached1 = abs(luma_end1) >= gradient_scaled;
	bool reached2 = abs(luma_end2) >= gradient_scaled;
	bool reachedBoth = reached1 && reached2;

	// if the side is not reached, we continue to explore in this direction
	if (!reached1) uv1 -= offset;
	if (!reached2) uv2 += offset;

	// if both sides have not been reached, continue to explore
	if (!reachedBoth)
	{
		for (int i = 2; i < EDGE_ITERATIONS; ++i)
		{
			// if needed, read luma in 1st direction, compute delta
			if (!reached1)
			{
				luma_end1 = rgbToLuma(texture(screen, uv1).rgb);
				luma_end1 = luma_end1 - luma_local_average;
			}

			// if needed, read luma in opposite direction, compute delta
			if (!reached2)
			{
				luma_end2 = rgbToLuma(texture(screen, uv2).rgb);
				luma_end2 = luma_end2 - luma_local_average;
			}

			// if the luma deltas at the current extremities is larger than the local gradient, we have reached the side of the edge
			reached1 = abs(luma_end1) >= gradient_scaled;
			reached2 = abs(luma_end2) >= gradient_scaled;
			reachedBoth = reached1 && reached2;

			// if the side is not reached, we continue to explore in this direction, with a variable quality
			if (!reached1) uv1 -= offset * QUALITY[i];
			if (!reached2) uv2 += offset * QUALITY[i];

			// if both sides have been reached, stop the exploration
			if (reachedBoth) break;
		}
	}

	// compute the distances to each extremity of the edge
	float distance1 = is_horizontal ? (fs_in.texture_coords.x - uv1.x) : (fs_in.texture_coords.y - uv1.y);
	float distance2 = is_horizontal ? (uv2.x - fs_in.texture_coords.x) : (uv2.y - fs_in.texture_coords.y);

	// in which direction is the extremity of the edge closer?
	bool is_direction1 = distance1 < distance2;
	float distance_final = min(distance1, distance2);

	// length of the edge
	float edge_thickness = distance1 + distance2;

	// uv offset: read in the direction of the closest side of the edge
	float pixel_offset = -distance_final / edge_thickness + 0.5f;

	// is the luma at center smaller than the local average?
	bool is_luma_center_smaller = luma_center < luma_local_average;

	// if the luma at center is smaller than at its neighbour, the delta luma at each end should be positive (same variation in the direction of the closer side of the edge)
	bool correct_variation = ((is_direction1 ? luma_end1 : luma_end2) < 0.0f) != is_luma_center_smaller;

	// if the luma variation is incorrect, do not offset
	float final_offset = correct_variation ? pixel_offset : 0.0f;

	// sub-pixel shifting
	// full weighted average of the luma over the 3x3 neighborhood.
	float luma_average = (1.0f / 12.0f) * (2.0f * (luma_down_up + luma_left_right) + luma_left_corners + luma_right_corners);

	// ratio of the delta between the global average and the center luma, over the luma range in the 3x3 neighborhood
	float sub_pixel_offset1 = clamp(abs(luma_average - luma_center) / luma_range, 0.0f, 1.0f);
	float sub_pixel_offset2 = (-2.0f * sub_pixel_offset1 + 3.0f) * sub_pixel_offset1 * sub_pixel_offset1;

	// compute a sub-pixel offset based on this delta
	float sub_pixel_offset_final = sub_pixel_offset2 * sub_pixel_offset2 * SUBPIXEL_QUALITY;

	// pick the biggest of the two offsets
	final_offset = max(final_offset, sub_pixel_offset_final);

	// compute the final UV coordinates
	vec2 final_uv = fs_in.texture_coords;

	if (is_horizontal)
		final_uv.y += final_offset * step_length;
	else
		final_uv.x += final_offset * step_length;

	gl_FragColor = texture(screen, final_uv);
}