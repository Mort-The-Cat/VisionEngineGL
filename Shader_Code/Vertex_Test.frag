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
in vec3 UV_Tangent;
in vec3 UV_Bitangent;
in vec2 UV;

vec3 Final_Normal;

vec3 Specular_Lighting = vec3(0, 0, 0);

float Inverse_Material_W = 1.0f; //texture(Material, UV).a;

vec3 Camera_To_Pixel = normalize(Camera_Position - Position);

vec3 Reflection_Vector;

// https://github.com/VictorGordan/opengl-tutorials/blob/main/YoutubeOpenGL%2027%20-%20Normal%20Maps/default.geom

// I'm going to need to get tangent space aligned with UV space to get the normal map texture actually working. 

// I'll just precompute this during model loading and parse the tangent to the vertex shader

mat3 TBN(vec3 P_Normal)
{
	vec3 Tangent = UV_Tangent;

	vec3 Bitangent = UV_Bitangent;

	mat3 Matrix = (mat3(Tangent, Normal, Bitangent));

	return Matrix;
}

vec3 Normal_Map_Read()
{
	vec3 New_Values;

	New_Values.z = -(texture(Material, UV).z * Inverse_Material_W * 2 - 1);
	New_Values.x = -(texture(Material, UV).w * 2 - 1);

	New_Values.y = sqrt(1 - (New_Values.x * New_Values.x + New_Values.z * New_Values.z));

	return New_Values;
}

float Specular_Texture = texture(Material, UV).x;

void Handle_Specular(float In_FOV, vec3 Light_To_Pixel, int Light_Index)
{
	float Specular_Value = In_FOV * pow(max(0, dot(reflect(-Light_To_Pixel, Final_Normal), Camera_To_Pixel)), 1 + 124 * Specular_Texture);

	Specular_Value *= Specular_Texture * 1.5;

	Specular_Lighting += Light_Colour[Light_Index].xyz * Specular_Value;
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

		float Angle = 57 * acos(dot(Light_To_Pixel, -Light_Direction[W].xyz));

		float In_FOV = min(1, Light_Position[W].w * max(0, 1.0f - (Angle - Light_Direction[W].w)));

		Dot_Normal_Light *= In_FOV;

		Handle_Specular(In_FOV, Light_To_Pixel, W);

		Sum_Of_Light += Dot_Normal_Light * Inverse_Length * Light_Colour[W].xyz;
	}

	return Sum_Of_Light;
}

void main()
{
	Final_Normal = normalize(Normal);
	
	// Final_Normal.y *= -1;

	Final_Normal = normalize(TBN(Final_Normal) * Normal_Map_Read());

	Reflection_Vector = normalize(reflect(Camera_To_Pixel, Final_Normal));
	
	vec3 Light = Lighting();

	float Reflectivity = texture(Material, UV).g;

	float Opacity = texture(Albedo, UV).a;

	//Out_Colour = vec4(Final_Normal, 0.5); //texture(Material, UV);
	
	Out_Colour = vec4(Specular_Lighting, 0) + vec4(Reflectivity, Reflectivity, Reflectivity, Opacity) * texture(Cubemap, Reflection_Vector) + vec4(Light * (1 - Reflectivity), Opacity) * texture(Albedo, UV); // vec4(1, 1, 1, 1);
}