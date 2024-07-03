#version 440

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 Projection_Matrix[6];

out vec3 Position;

void main()
{
	for(int Face = 0; Face < 6; Face++)
	{
		gl_Layer = Face;

		for(int Vertex = 0; Vertex < 3; Vertex++)
		{
			gl_Position = Projection_Matrix[Face] * gl_in[Vertex].gl_Position;

			EmitVertex();
		}

		EndPrimitive();

	}
}