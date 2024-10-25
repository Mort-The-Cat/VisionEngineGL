#version 440

layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_UV;

uniform mat4 Projection_Matrix;

uniform float Particle_Data[2400]; // We know that the smoke particle has 8 floats in it total

int Particle_Index = gl_InstanceID * 8;

vec3 Particle_Position = vec3(Particle_Data[Particle_Index], Particle_Data[Particle_Index + 1], Particle_Data[Particle_Index + 2]);
vec3 Particle_Velocity = vec3(Particle_Data[Particle_Index + 4], Particle_Data[Particle_Index + 5], Particle_Data[Particle_Index + 6]);

float Particle_Age = Particle_Data[Particle_Index + 3];
float Particle_Gravity = -0.15; //Particle_Data[Particle_Index + 7];

out vec2 Refracted_Vector;

void main()
{
	vec4 Transformed_Position = vec4(In_Position * 3 + Particle_Position, 1.0f);

	Transformed_Position.xyz += Particle_Velocity * log(Particle_Age + 1);

	Transformed_Position.y += Particle_Gravity * Particle_Age;

	gl_Position = Projection_Matrix * Transformed_Position;

	Transformed_Position.xyz -= In_Normal * length(In_Position) * 0.75f;

	Transformed_Position = Projection_Matrix * Transformed_Position;

	Refracted_Vector = Transformed_Position.xy / Transformed_Position.w;
}