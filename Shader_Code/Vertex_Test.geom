#version 440

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec4 Position;
out vec3 Normal;
out vec3 UV_Tangent;
out vec3 UV_Bitangent;
out vec2 UV;

in DATA
{
	vec3 Normal;
	mat4 Projection_Matrix;
	vec2 UV;
	float Lighting_Transparency;
	
	float Vertex_Transparency;
} data_in[];

out float Lighting_Transparency;

out float Vertex_Transparency;

void main()
{
	vec2 UV_A = data_in[1].UV - data_in[0].UV;
	vec2 UV_B = data_in[2].UV - data_in[0].UV;

	vec3 Edge_A; // = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 Edge_B; // = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

	if((UV_A.x * UV_B.y - UV_B.x * UV_A.y) == 0)
	{
		UV_A = data_in[2].UV - data_in[0].UV;
		UV_B = data_in[1].UV - data_in[0].UV;

		Edge_A = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
		Edge_B = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	}
	else
	{
		Edge_A = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
		Edge_B = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	}

	/*
	float B;

	float A;
	
	if(UV_A.x == 0)
	{
		Edge_A = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
		Edge_B = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;

		UV_A = data_in[2].UV - data_in[0].UV;
		UV_B = data_in[1].UV - data_in[0].UV;
	}

	B = -abs(UV_A.y / (UV_B.y * UV_A.x - UV_B.x * UV_A.y));
	A = abs((1 - B * UV_B.x) / UV_A.x);

	vec3 Tangent = normalize(Edge_A * A + Edge_B * B);*/

	float Inv = 1.0f / (UV_A.x * UV_B.y - UV_B.x * UV_A.y);

	vec3 Tangent = -normalize(Inv * (UV_B.y * Edge_A - UV_A.y * Edge_B));

	//

	for(int W = 0; W < 3; W++)
	{
		vec4 Calculated_Position = (data_in[0].Projection_Matrix * gl_in[W].gl_Position);
		gl_Position = Calculated_Position;
		Position = vec4(gl_in[W].gl_Position.xyz, Calculated_Position.z);
		Normal = data_in[W].Normal;
		UV = data_in[W].UV;
		UV_Tangent = Tangent;
		UV_Bitangent = cross(Tangent, Normal);

		Lighting_Transparency = data_in[W].Lighting_Transparency;
		
		Vertex_Transparency = data_in[W].Vertex_Transparency;

		EmitVertex();
	}

	EndPrimitive();
}