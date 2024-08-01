#version 440

layout(location = 0) in vec2 In_Position;
layout(location = 1) in vec2 In_UV;

uniform mat4 Projection_Matrix;

out vec3 Normal;
out vec4 Position;

out vec3 UV_Tangent;
out vec3 UV_Bitangent;

out vec2 UV;
out float Lighting_Transparency;
out float Vertex_Transparency;

uniform float Particle_Data[2400]; // We know that the smoke particle has 8 floats in it total

uniform vec3 Camera_Position;
uniform vec3 Camera_Up_Direction;
uniform vec3 Camera_Direction;

vec3 Camera_Right_Direction = cross(Camera_Direction, Camera_Up_Direction);

int Particle_Index = gl_InstanceID * 8;

vec3 Particle_Position = vec3(Particle_Data[Particle_Index], Particle_Data[Particle_Index + 1], Particle_Data[Particle_Index + 2]);
vec3 Particle_Velocity = vec3(Particle_Data[Particle_Index + 4], Particle_Data[Particle_Index + 5], Particle_Data[Particle_Index + 6]);

float Particle_Age = Particle_Data[Particle_Index + 3];
float Particle_Gravity = -0.2; // Particle_Data[Particle_Index + 7];

float Natural_Delta_Time = log(1 + Particle_Age * 5) * 0.2;

float Particle_Angle = 2 * Natural_Delta_Time * Particle_Data[Particle_Index + 7];

vec2 Rotated_Position()
{
	float Sin = sin(Particle_Angle);
	float Cos = cos(Particle_Angle);

	vec2 Rotated;

	float Temp = In_Position.x;

	Rotated.x = In_Position.x * Cos + In_Position.y * Sin;
	Rotated.y = In_Position.y * Cos - Temp * Sin;

	return Rotated;
}

void main()
{
	vec4 Transformed_Position = vec4(Particle_Position, 1);

	Transformed_Position.xyz += Particle_Velocity * Natural_Delta_Time;

	Transformed_Position.y += Particle_Gravity * Particle_Age;

	vec2 Rotated = Rotated_Position(); // * vec2(log(Particle_Age * 0.5 + 1) + 1);

	Transformed_Position.xyz -= Camera_Right_Direction * Rotated.x;
	Transformed_Position.xyz += Camera_Up_Direction * Rotated.y;

	Position = Transformed_Position;

	gl_Position = Projection_Matrix * Transformed_Position;

	Normal = normalize(Camera_Position - Transformed_Position.xyz);
	UV = In_UV;

	Lighting_Transparency = 0.5;

	
	
	Vertex_Transparency = 1.0 / (Particle_Age * 2.0f + length(Position.xyz - Particle_Position));
}