#version 440

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

out vec4 Fragment_Position;

uniform mat4 Shadow_Matrix[1];

void main()
{
	for(int Face = 0; Face < 6; Face++)
	{
		for(int Vertices = 0; Vertices < 3; Vertices++)
		{
			gl_Layer = Face;

			gl_Position = Shadow_Matrix[Face] * gl_in[Vertices].gl_Position;
			Fragment_Position = gl_in[Vertices].gl_Position;
			EmitVertex();
		}

		EndPrimitive();
	}
}