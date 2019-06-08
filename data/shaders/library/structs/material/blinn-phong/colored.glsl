#ifndef MATERIAL_BLINN_PHONG_COLORED_STRUCT
#define MATERIAL_BLINN_PHONG_COLORED_STRUCT
// material struct
uniform struct Material
{
	vec4 diffuse;
	vec4 specular;
	float shininess;
} material;
#endif