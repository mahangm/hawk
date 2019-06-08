#version 460 core

// shader inputs
in VsOut
{
	vec2 texture_coords;
} fs_in;

// screen image
uniform sampler2D screen;

// depth near plane
uniform float near_plane;

// depth far plane
uniform float far_plane;

float linearizeDepth(float depth)
{
	// back to NDC 
    float z = depth * 2.0 - 1.0;

    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main()
{             
    float depth_value = texture(screen, fs_in.texture_coords).r;

	// orthographic
    gl_FragColor = vec4(vec3(depth_value), 1.0);

	// perspective
	//gl_FragColor = vec4(vec3(linearizeDepth(depth_value) / far_plane), 1.0);
}