#ifndef MATERIAL_BLINN_PHONG_TEXTURED_NORMAL_STRUCT
#define MATERIAL_BLINN_PHONG_TEXTURED_NORMAL_STRUCT
// material struct
uniform struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	float shininess;
} material;
#endif