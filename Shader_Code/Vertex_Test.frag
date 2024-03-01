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

float Inverse_Material_W = texture(Material, UV).a;

mat3 TBN(vec3 P_Normal)
{

	vec3 Random_Vector = normalize(vec3(P_Normal.x + 3, P_Normal.y + 3, P_Normal.z + 3));

	vec3 Tangent = normalize(cross(Random_Vector, P_Normal));

	// Tangent = normalize(Tangent - dot(Tangent, P_Normal) * P_Normal);

	vec3 Bitangent = cross(P_Normal, Tangent);

	mat3 Matrix = (mat3(Tangent, P_Normal, Bitangent));

	return Matrix;
}

vec3 Normal_Map_Read()
{
	vec3 New_Values;

	New_Values.x = (texture(Material, UV).b * Inverse_Material_W * 2 - 1);
	New_Values.z = (texture(Material, UV).a * 2 - 1);

	New_Values.y = sqrt(1 - (New_Values.x * New_Values.x + New_Values.z * New_Values.z));

	// New_Values = vec3(0, 1, 0);

	return New_Values;
}

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
	//Final_Normal = normalize(TBN(Normal) * Normal_Map_Read());
	Final_Normal = Normal;
	// Final_Normal.y *= -1;

	Final_Normal = normalize(TBN(Final_Normal) * Normal_Map_Read());
	
	vec3 Light = Lighting();
	
	Out_Colour = vec4(Light, 1) * texture(Albedo, UV); // vec4(1, 1, 1, 1);

	// Out_Colour = vec4(Normal_Map_Read().xyz, 1);

	// Out_Colour = vec4(texture(Material, UV).z * Inverse_Material_W, 0, texture(Material, UV).w, 1);
}