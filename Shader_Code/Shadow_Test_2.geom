#version 440

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 Projection_Matrix;

out vec3 Position;

void main()
{
	for(int Vertex = 0; Vertex < 3; Vertex++)
	{
		gl_Position = Projection_Matrix * gl_in[Vertex].gl_Position;

		EmitVertex();
	}

	EndPrimitive();
}