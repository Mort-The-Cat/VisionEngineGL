#version 440

layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_UV;

uniform mat4 Projection_Matrix;

out DATA
{
	vec3 Normal;
	mat4 Projection_Matrix;
	vec2 UV;
	float Lighting_Transparency;
	
	float Vertex_Transparency;
} data_out;

uniform float Particle_Data[2400]; // We know that the smoke particle has 8 floats in it total

int Particle_Index = gl_InstanceID * 8;

vec3 Particle_Position = vec3(Particle_Data[Particle_Index], Particle_Data[Particle_Index + 1], Particle_Data[Particle_Index + 2]);
vec3 Particle_Velocity = vec3(Particle_Data[Particle_Index + 4], Particle_Data[Particle_Index + 5], Particle_Data[Particle_Index + 6]);

float Particle_Age = Particle_Data[Particle_Index + 3];
float Particle_Gravity = -0.2; //Particle_Data[Particle_Index + 7];

void main()
{
	vec4 Transformed_Position = vec4(In_Position + Particle_Position, 1);

	Transformed_Position.xyz += Particle_Velocity * log(1 + Particle_Age);

	Transformed_Position.y += Particle_Gravity * Particle_Age;

	gl_Position = Transformed_Position;

	data_out.Normal = In_Normal; // We don't have any rotation on this yet lol
	data_out.UV = In_UV;
	data_out.Projection_Matrix = Projection_Matrix;

	data_out.Lighting_Transparency = 0.5;
	
	data_out.Vertex_Transparency = 1.0; // / (Particle_Age * sqrt(Particle_Age) + 1);
}