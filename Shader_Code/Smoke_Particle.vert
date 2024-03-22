#version 440

layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_UV;

uniform float Particle_Data[2400];

uniform mat4 Projection_Matrix;

out DATA
{
	vec3 Normal;
	mat4 Projection_Matrix;
	vec2 UV;
} data_out;

vec3 Particle_Position = vec3(0, 0, 0);
vec3 Particle_Velocity = vec3(0, 0, 0);

float Particle_Age = 0;
float Particle_Gravity = 0;

void main()
{
	vec4 Transformed_Position = vec4(In_Position + Particle_Position, 1);

	Transformed_Position.xyz += Particle_Velocity * Particle_Age;

	Transformed_Position.y += Particle_Gravity * Particle_Age * Particle_Age;

	gl_Position = Transformed_Position;

	data_out.Normal = In_Normal; // We don't have any rotation on this yet lol
	data_out.UV = In_UV;
	data_out.Projection_Matrix = Projection_Matrix;
}