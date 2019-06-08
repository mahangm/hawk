#ifndef MATERIAL_PBR_COLORED_STRUCT
#define MATERIAL_PBR_COLORED_STRUCT
// material struct
uniform struct Material
{
	vec4 albedo;
	float metallic;
	float roughness;
	float ambient;
} material;
#endif