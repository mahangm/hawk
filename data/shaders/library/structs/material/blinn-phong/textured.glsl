#ifndef MATERIAL_BLINN_PHONG_TEXTURED_STRUCT
#define MATERIAL_BLINN_PHONG_TEXTURED_STRUCT
// material struct
uniform struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
} material;
#endif