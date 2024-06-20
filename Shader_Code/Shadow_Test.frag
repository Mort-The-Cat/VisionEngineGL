#version 440

layout(location = 0) out float Out_Depth;

in vec4 Fragment_Position;

uniform vec3 Light_Position;
uniform float Inverse_Far_Plane;

void main()
{
	vec3 To_Vector = Light_Position - Fragment_Position.xyz;

	float Distance = sqrt(dot(To_Vector, To_Vector));

	Out_Depth = Distance;
}