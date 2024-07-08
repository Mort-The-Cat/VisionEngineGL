#version 440

layout(location = 0) in vec2 In_Position;
layout(location = 1) in vec2 In_UV;

uniform mat4 Projection_Matrix;

/*out DATA
{
	vec3 Normal;
	mat4 Projection_Matrix;
	vec2 UV;
	float Lighting_Transparency;

	float Vertex_Transparency;
} data_out;*/

out vec2 UV;
out float Vertex_Transparency;

uniform vec3 Camera_Position;
uniform vec3 Camera_Up_Direction;
uniform vec3 Camera_Direction;

vec3 Camera_Right_Direction = cross(Camera_Direction, Camera_Up_Direction);

uniform float Particle_Data[2400];

// Number of particles per orbit = 32

uint Particle_Index = gl_InstanceID * 3u;

float Radius = Particle_Data[Particle_Index];

float Age = Particle_Data[Particle_Index + 1u] * 0.5f;

float Index = Particle_Data[Particle_Index + 2u];

vec2 Rotated_Position()
{
	float Sin = sin(Age + Index + Radius);
	float Cos = cos(Age + Index + Radius);

	vec2 Rotated;

	float Temp = In_Position.x;

	Rotated.x = In_Position.x * Cos + In_Position.y * Sin;
	Rotated.y = In_Position.y * Cos - Temp * Sin;

	return Rotated;
}

void main()
{
	vec3 Particle_Position = vec3(sin(Index + Age / Radius) * 0.75f, cos(Index * Index + Radius) * 0.05f, cos(Index + Age / Radius));

	float Orbital_Offset_Sin = sin(Radius / 3) * Radius * 0.75f;

	float Orbital_Offset_Cos = cos(Radius / 3) * Radius * 0.75f;

	float Temp_X = Particle_Position.x;

	Particle_Position.x = Particle_Position.x * Orbital_Offset_Cos + Particle_Position.z * Orbital_Offset_Sin;
	Particle_Position.z = Particle_Position.z * Orbital_Offset_Cos - Temp_X * Orbital_Offset_Sin;

	vec4 Transformed_Position = vec4(Particle_Position, 1);

	vec2 Rotated = Rotated_Position();

	Transformed_Position.xyz -= Camera_Right_Direction * Rotated.x * (Index > 6.28318f ? 0.025f : 1.0f);
	Transformed_Position.xyz += Camera_Up_Direction * Rotated.y * (Index > 6.28318f ? 0.025f : 1.0f);

	Transformed_Position.xyz *= 0.5f;

	Transformed_Position.z += 10;

	gl_Position = Projection_Matrix * Transformed_Position;

	UV = In_UV;

	Vertex_Transparency = Index > 6.28318f ? 3.0f : 0.1f;

	/*data_out.Normal = normalize(Camera_Position - Transformed_Position.xyz);
	data_out.UV = In_UV;
	data_out.Projection_Matrix = Projection_Matrix;

	data_out.Lighting_Transparency = 0.5;
	
	data_out.Vertex_Transparency = 1.0; // / (Particle_Age * 10 + 1);*/
}