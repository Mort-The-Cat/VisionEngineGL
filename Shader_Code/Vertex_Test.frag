#version 440

out vec4 Out_Colour;

uniform vec4 Light_Position[20];
uniform vec4 Light_Colour[20];
uniform vec4 Light_Direction[20];

uniform sampler2D Albedo;
uniform sampler2D Material;

uniform samplerCube Cubemap;

uniform vec3 Camera_Position;
uniform vec3 Camera_Direction;

in vec3 Position;
in vec3 Normal;
in vec2 UV;

vec3 Final_Normal;

vec3 Specular_Lighting = vec3(0, 0, 0);

float Inverse_Material_W = texture(Material, UV).a;

vec3 Camera_To_Pixel = normalize(Camera_Position - Position);

mat3 TBN(vec3 P_Normal)
{

	vec3 Random_Vector = Camera_To_Pixel;

	// vec3 Random_Vector = normalize(P_Normal - Camera_To_Pixel * 0.25);

	vec3 Tangent = normalize(cross(Random_Vector, P_Normal));

	// Tangent = normalize(Tangent - dot(Tangent, P_Normal) * P_Normal);

	vec3 Bitangent = cross(P_Normal, Tangent);

	mat3 Matrix = transpose(mat3(Tangent, P_Normal, Bitangent));

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

	float Opacity = texture(Material, UV).g * Inverse_Material_W;

	Out_Colour = vec4(Light, 1) * texture(Albedo, UV);
	
	// Out_Colour = vec4(Opacity, Opacity, Opacity, 1) * texture(Cubemap, reflect(Camera_To_Pixel, Final_Normal)) + vec4(Light * Opacity, 1) * texture(Albedo, UV); // vec4(1, 1, 1, 1);

	// Out_Colour = vec4(Final_Normal, 1);

	//Out_Colour = vec4(Normal_Map_Read().xyz, 1);

	// Out_Colour = vec4(texture(Material, UV).z * Inverse_Material_W, 0, texture(Material, UV).w, 1);
}