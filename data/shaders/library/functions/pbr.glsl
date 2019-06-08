#ifndef PBR_FUNCTIONS
#define PBR_FUNCTIONS
float distributionGGX(vec3 n, vec3 h, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float n_dot_h = max(dot(n, h), 0.0f);
	float n_dot_h2 = n_dot_h * n_dot_h;

	float nom = a2;
	float denom = (n_dot_h2 * (a2 - 1.0f) + 1.0f);
	denom = PI * denom * denom;

	return nom / denom;
}

float geometrySchlickGGX(float n_dot_v, float roughness)
{
	float r = (roughness + 1.0f);
	float k = (r * r) / 8.0f;

	float nom = n_dot_v;
	float denom = n_dot_v * (1.0f - k) + k;

	return nom / denom;
}

float geometrySmith(vec3 n, vec3 v, vec3 l, float roughness)
{
	float n_dot_v = max(dot(n, v), 0.0f);
	float n_dot_l = max(dot(n, l), 0.0f);
	float ggx2 = geometrySchlickGGX(n_dot_v, roughness);
	float ggx1 = geometrySchlickGGX(n_dot_l, roughness);

	return ggx1 * ggx2;
}

vec4 fresnelSchlick(float cos_theta, vec4 f0)
{
	return f0 + (1.0f - f0) * pow(1.0f - cos_theta, 5.0f);
}

vec4 fresnelSchlickRoughness(float cos_theta, vec4 f0, float roughness)
{
    return f0 + (max(vec4(1.0f - roughness), f0) - f0) * pow(1.0f - cos_theta, 5.0f);
}
#endif