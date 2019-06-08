#ifndef MATERIAL_BLINN_PHONG_TEXTURED_PARALLAX_STRUCT
#define MATERIAL_BLINN_PHONG_TEXTURED_PARALLAX_STRUCT
// material struct
uniform struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	sampler2D displacement;
	float shininess;
	float height;
} material;
#endif