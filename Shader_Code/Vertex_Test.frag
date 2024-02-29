#version 440

out vec4 Out_Colour;

uniform vec4 Light_Position[20];
uniform vec4 Light_Colour[20];
uniform vec4 Light_Direction[20];

uniform sampler2D Albedo;
uniform sampler2D Material;

in vec3 Position;
in vec3 Normal;
in vec2 UV;

vec3 Final_Normal;

vec3 Specular_Lighting = vec3(0, 0, 0);

vec3 Lighting()
{
	vec3 Sum_Of_Light = vec3(0.1, 0.1, 0.1);

	for(int W = 0; W < 20; W++)
	{
		vec3 Light_To_Pixel = Light_Position[W].xyz - Position;
		float Inverse_Length = inversesqrt(dot(Light_To_Pixel, Light_To_Pixel));
		Light_To_Pixel *= Inverse_Length;

		float Dot_Normal_Light = max(0, dot(Light_To_Pixel, Final_Normal));

		//

		float Angle = 57 * acos(dot(Light_To_Pixel, -Light_Direction[W].xyz));

		float In_FOV = min(1, Light_Position[W].w * max(0, 1.0f - (Angle - Light_Direction[W].w)));

		//

		Sum_Of_Light += In_FOV * Dot_Normal_Light * Inverse_Length * Light_Colour[W].xyz;
	}

	return Sum_Of_Light;
}

void main()
{
	Final_Normal = Normal;
	Final_Normal.y *= -1;
	
	vec3 Light = Lighting();
	
	Out_Colour = vec4(Light, 1) * texture(Albedo, UV); // vec4(1, 1, 1, 1);
}