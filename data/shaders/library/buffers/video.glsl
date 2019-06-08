#ifndef VIDEO_BUFFER
#define VIDEO_BUFFER
// video buffer
layout (std140, binding = 0) uniform Video
{
	vec4 ambient;
	bool ssao;
	bool bloom;
} video;
#endif