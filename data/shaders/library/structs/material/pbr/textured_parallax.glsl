#ifndef MATERIAL_PBR_TEXTURED_PARALLAX_STRUCT
#define MATERIAL_PBR_TEXTURED_PARALLAX_STRUCT
// material struct
uniform struct Material
{
	sampler2D albedo;
	sampler2D metallic;
	sampler2D roughness;
	sampler2D ambient;
	sampler2D normal;
	sampler2D displacement;
	float height;
} material;
#endif