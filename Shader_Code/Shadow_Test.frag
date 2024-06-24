#version 440

// out float Out_Depth;

in vec4 Fragment_Position;

uniform vec3 Light_Position;
uniform float Inverse_Far_Plane;

void main()
{
	vec3 To_Vector = Light_Position - Fragment_Position.xyz;

	float Distance = sqrt(dot(To_Vector, To_Vector));

	gl_FragDepth = 0.5f; //Distance / 200;
}