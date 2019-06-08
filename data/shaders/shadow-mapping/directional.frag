#version 460 core

void main()
{
	// set fragment depth
	gl_FragDepth = gl_FragCoord.z;
}