#version 440

out vec4 Out_Colour;

in vec2 UV;

uniform sampler2D Screen_Texture;
uniform sampler2D Position_Texture;
uniform sampler2D Normal_Texture;	// This is a quaternion, representing the TBN matrix of the surface normals
uniform sampler2D Material_Texture;

uniform samplerCube Cubemap;

uniform vec4 Light_Position[20];
uniform vec4 Light_Colour[20];
uniform vec4 Light_Direction[20];

uniform vec3 Camera_Position;
uniform vec3 Camera_Direction;

vec3 Position = texture(Position_Texture, UV).xyz;

vec3 Camera_To_Pixel = normalize(Camera_Position - Position);

vec3 Rotate_Vector(vec3 Vector, vec4 Quaternion)
{
	vec3 Temp = cross(Quaternion.xyz, Vector) + Quaternion.w * Vector;

	return Vector + 2 * cross(Quaternion.xyz, Temp);
}

vec3 Normal;

//

float Specular_Texture = texture(Material_Texture, UV).x;

vec3 Specular_Lighting = vec3(0, 0, 0);

void Handle_Specular(float In_FOV, vec3 Light_To_Pixel, int Light_Index)
{
	float Specular_Value = In_FOV * pow(max(0, dot(reflect(-Light_To_Pixel, Normal), Camera_To_Pixel)), 1 + 124 * Specular_Texture);

	Specular_Value *= Specular_Texture * 1.5;

	Specular_Lighting += Light_Colour[Light_Index].xyz * Specular_Value;
}

vec3 Lighting()
{
	vec3 Sum_Of_Light = vec3(0.1, 0.1, 0.1);

	for(int W = 0; W < 8; W++)
	{
		vec3 Light_To_Pixel = Light_Position[W].xyz - Position;

		float Squared_Distance = dot(Light_To_Pixel, Light_To_Pixel);

		float Inverse_Length = inversesqrt(Squared_Distance);
		Light_To_Pixel *= Inverse_Length;

		float Dot_Normal_Light = max(0.0f, dot(Light_To_Pixel, Normal));

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

//

void main()
{
	Normal = texture(Normal_Texture, UV).xyz; //Rotate_Vector(vec3(1, 0, 0), texture(Normal_Texture, UV));

	vec3 Reflection_Vector = normalize(reflect(Camera_To_Pixel, Normal));

	// vec3 Light = Lighting();

	float Reflectivity = texture(Material_Texture, UV).y;

	// Out_Colour = vec4(Specular_Lighting, 0) + vec4(vec3(Reflectivity), 1) * texture(Cubemap, Reflection_Vector) + vec4(Light * (1.0f - Reflectivity), 1) * texture(Screen_Texture, UV);

	Out_Colour = vec4(1.0f) - texture(Screen_Texture, UV);
}