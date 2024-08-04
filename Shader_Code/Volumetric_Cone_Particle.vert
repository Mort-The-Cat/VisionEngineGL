#version 440

layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_UV;

//

uniform float Particle_Data[1100];

uint Particle_ID = gl_InstanceID * 22;

float Volume_Density = Particle_Data[Particle_ID];
float C = Particle_Data[Particle_ID + 1];
float Cosine_Theta_Squared = Particle_Data[Particle_ID + 2];
float Dot_Source_Direction = Particle_Data[Particle_ID + 3];

mat3 Scaling_Rotation = mat3(
	Particle_Data[Particle_ID + 4], Particle_Data[Particle_ID + 5], Particle_Data[Particle_ID + 6],
	Particle_Data[Particle_ID + 7], Particle_Data[Particle_ID + 8], Particle_Data[Particle_ID + 9],
	Particle_Data[Particle_ID + 10], Particle_Data[Particle_ID + 11], Particle_Data[Particle_ID + 12]
);

vec3 Cone_Origin = vec3(Particle_Data[Particle_ID + 13], Particle_Data[Particle_ID + 14], Particle_Data[Particle_ID + 15]);
vec3 Cone_Direction = vec3(Particle_Data[Particle_ID + 16], Particle_Data[Particle_ID + 17], Particle_Data[Particle_ID + 18]);
vec3 Colour = vec3(Particle_Data[Particle_ID + 19], Particle_Data[Particle_ID + 20], Particle_Data[Particle_ID + 21]);

uniform mat4 Projection_Matrix;
uniform vec3 Camera_Position;

flat out float Volume_Out;
flat out float C_Out;
flat out float Cosine_Theta_Squared_Out;
flat out float Dot_Source_Direction_Out;
flat out vec3 Cone_Origin_Out;
flat out vec3 Cone_Direction_Out;
flat out vec3 Colour_Out;

out vec3 Position;

void Particle_Outputs()
{
	Volume_Out = Volume_Density;
	C_Out = C;
	Cosine_Theta_Squared_Out = Cosine_Theta_Squared;
	Dot_Source_Direction_Out = Dot_Source_Direction;
	Cone_Origin_Out = Cone_Origin;
	Cone_Direction_Out = Cone_Direction;
	Colour_Out = Colour;
}

void main()
{
	Particle_Outputs();

	float Tan = 4 * sqrt((1.0f - Cosine_Theta_Squared) / Cosine_Theta_Squared);
	vec3 Transformed_Cone_Positions = In_Position * vec3(Tan, 4.0f, Tan);

	Position = Scaling_Rotation * Transformed_Cone_Positions + Cone_Origin;

	vec4 Transformed_Position = Projection_Matrix * vec4(Position, 1.0f);

	// Transformed_Position.z = max(0.01, Transformed_Position.z);

	gl_Position = Transformed_Position;
}