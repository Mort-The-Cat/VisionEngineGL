#version 440

layout(location = 0) out vec4 Out_Colour;
layout(location = 1) out vec4 Position_Out;
layout(location = 2) out vec4 Normal_Out;
layout(location = 3) out vec4 Material_Out;

uniform vec4 Light_Position[20];
uniform vec4 Light_Colour[20];
uniform vec4 Light_Direction[20];

uniform sampler2D Albedo;
uniform sampler2D Material;

uniform samplerCube Cubemap;

uniform vec3 Camera_Position;
uniform vec3 Camera_Direction;

in vec4 Position;
in vec3 Normal;
in vec3 UV_Tangent;
in vec3 UV_Bitangent;
in vec2 UV;

in float Lighting_Transparency;

in float Vertex_Transparency;

vec3 Final_Normal;

vec3 Specular_Lighting = vec3(0, 0, 0);

// float Inverse_Material_W = 1.0f; //texture(Material, UV).a;

vec3 Camera_To_Pixel = normalize(Camera_Position - Position.xyz);

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

	New_Values.z = -(texture(Material, UV).z * 2 - 1);
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

	for(int W = 0; W < 8; W++)
	{
		vec3 Light_To_Pixel = Light_Position[W].xyz - Position.xyz;

		float Squared_Distance = dot(Light_To_Pixel, Light_To_Pixel);

		float Inverse_Length = inversesqrt(Squared_Distance);
		Light_To_Pixel *= Inverse_Length;

		float Dot_Normal_Light = max(Lighting_Transparency, dot(Light_To_Pixel, Final_Normal));

		float Angle = 57 * acos(dot(Light_To_Pixel, -Light_Direction[W].xyz));

		float In_FOV = min(1, Light_Position[W].w * max(0, 1.0f - (Angle - Light_Direction[W].w)));

		Dot_Normal_Light *= In_FOV;

		Handle_Specular(In_FOV, Light_To_Pixel, W);

		float Attenuation_Value = inversesqrt(Light_Colour[W].w + Squared_Distance);

		// Light_Colour[W].w refers to the attenuation value

		Sum_Of_Light += Dot_Normal_Light * Attenuation_Value * Light_Colour[W].xyz;
	}

	return Sum_Of_Light;
}

// https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/

vec4 TBN_To_Quaternion(mat3 Matrix)
{
	vec4 Quaternion;

	Quaternion.w = 0.5f * sqrt(1 + Matrix[0][0] + Matrix[1][1] + Matrix[2][2]);

	float Inverse = 0.25f / Quaternion.w;

	Quaternion.x = (Matrix[2][1] - Matrix[1][2]) * Inverse;
	Quaternion.y = (Matrix[0][2] - Matrix[2][0]) * Inverse;
	Quaternion.z = (Matrix[1][0] - Matrix[0][1]) * Inverse;

	return Quaternion;
}

void Output_Normal_Quaternion()
{
	vec3 New_Tangent = -normalize(cross(Final_Normal, UV_Bitangent));
	vec3 New_Bitangent = -normalize(cross(New_Tangent, Final_Normal));

	vec4 Quaternion = TBN_To_Quaternion(mat3(New_Tangent, Final_Normal, New_Bitangent));

	Normal_Out = Quaternion;
}

void main()
{
	Final_Normal = normalize(Normal);

	mat3 TBN = TBN(Final_Normal);
	
	Final_Normal = normalize(TBN * Normal_Map_Read());

	Reflection_Vector = normalize(reflect(Camera_To_Pixel, Final_Normal));
	
	// vec3 Light = Lighting();

	float Reflectivity = texture(Material, UV).g;

	float Opacity = texture(Albedo, UV).a * Vertex_Transparency;

	Out_Colour = texture(Albedo, UV);
	
	Position_Out = Position;
	Material_Out = vec4(texture(Material, UV).xy, 0, 0);
	
	// Output_Normal_Quaternion();

	Normal_Out = vec4(Final_Normal, 0); // TBN_To_Quaternion(TBN(Final_Normal));
	
	// Out_Colour = vec4(Specular_Lighting, 0) + vec4(Reflectivity, Reflectivity, Reflectivity, Opacity) * texture(Cubemap, Reflection_Vector) + vec4(Light * (1 - Reflectivity), Opacity) * texture(Albedo, UV); // vec4(1, 1, 1, 1);

	// NOTE: TBN MATRICES ARE PURELY ROTATIONAL

	// ORTHOGONAL ROTATION MATRICES ALWAYS HAVE A POSITION DETERMINANT 

	// WE CAN USE THE SIMPLEST METHOD FOR CALCULATING A QUATERNION
}