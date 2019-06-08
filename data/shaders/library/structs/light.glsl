#ifndef LIGHT_STRUCT
#define LIGHT_STRUCT
// light struct
struct Light
{
	int id;
	int type;
	bool cast_shadows;
	uint shadow_map_id;
	vec4 color;
	vec4 position;
	vec4 direction;
	float range;
	float cut_off;
	float outer_cut_off;
	float far_plane;
	mat4 matrix;
};
#endif