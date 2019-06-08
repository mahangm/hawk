#ifndef SHADOW_MAP_STRUCT
#define SHADOW_MAP_STRUCT
// shadow map struct
uniform struct ShadowMap
{
	sampler2D dsm;
	samplerCube odsm;
} shadow_maps[MAX_SHADOWS];
#endif