#version 460 core

// input shape
layout (triangles) in;

// output shape
layout (triangle_strip, max_vertices = 18) out;

// shader outputs
out GsOut
{
	vec4 fragment_position;
} gs_out;

// shadow map matrices
uniform mat4 light_matrices[6];

void main()
{
	// iterate shadow cube map faces
	for (int face = 0; face < 6; ++face)
	{
		// set cube map face
		gl_Layer = face;

		// iterate triangle vertices
		for (int i = 0; i < 3; ++i)
		{
			// set shader outputs
			gs_out.fragment_position = gl_in[i].gl_Position;

			// set vertex position
			gl_Position = light_matrices[face] * gs_out.fragment_position;

			// make vertex
			EmitVertex();
		}

		// make shape
		EndPrimitive();
	}
}