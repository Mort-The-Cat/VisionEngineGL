#version 440

out vec4 Out_Colour;

in vec2 UV;

uniform sampler2D Screen_Texture;
uniform sampler2D Position_Texture;
uniform sampler2D Normal_Texture;	// This is a quaternion, representing the TBN matrix of the surface normals
uniform sampler2D Material_Texture;

vec2 Final_UV = UV + texture(Material_Texture, UV).zw;

uniform samplerCube Cubemap;

const int Number_Of_Shadow_Maps = 1;

uniform samplerCubeShadow Shadow_Maps;

const uint Number_Of_Lights = 32;

uniform vec4 Light_Position[Number_Of_Lights];
uniform vec4 Light_Colour[Number_Of_Lights];
uniform vec4 Light_Direction[Number_Of_Lights];

uniform vec3 Camera_Position;
uniform vec3 Camera_Direction;

uniform mat4 Projection_Matrix;

vec3 Position = texture(Position_Texture, Final_UV).xyz;

vec3 Camera_To_Pixel = normalize(Camera_Position - Position);

mat3 TBN;

//

const uint Binary_Tree_Depth = 7;

uniform uint Leaf_Node_Indices[128];
uniform vec2 Partition_Nodes[63];
uniform float BVH_Conversion;
uniform float BVH_Inverse_Conversion;

uniform float Shader_Time;

uint Traverse_Partition_Nodes()
{
	uint Index = 0u;
	for(uint W = 0; W < 6; W++)
	{
		vec2 Node = Partition_Nodes[Index];

		uint Side = uint((Position.x > Node.x) || (Position.z > Node.y));

		Index = Index + Index;
		Index = Index + 1u + Side;
	}

	return Index;
}

//

vec3 Rotate_Vector(vec3 Vector, vec4 Quaternion)
{
	vec3 Temp = cross(Quaternion.xyz, Vector) + Quaternion.w * Vector;

	return Vector + 2 * cross(Quaternion.xyz, Temp);
}

vec3 Normal;

vec3 Sampler_Kernel[10] = vec3[](
	vec3(0.702116, 0.406985, 0.584291),
	//vec3(0.478921, 0.731482, -0.485354),
	//vec3(-0.348094, 0.811446, -0.469453),
	vec3(0.195132, 0.863031, 0.465941),
	//vec3(-0.251658, 0.477761, 0.841673),
	//vec3(0.734053, 0.560610, 0.383252),
	vec3(-0.531606, 0.450154, -0.717465),
	//vec3(-0.636180, 0.169777, 0.752630),
	//vec3(-0.631984, 0.369028, -0.681480),
	vec3(-0.563538, 0.719510, -0.405870),
	//vec3(0.029757, 0.965371, 0.259177),
	//vec3(-0.537857, 0.840600, 0.064044),
	vec3(-0.012134, 0.919739, -0.392342),
	//vec3(0.584067, 0.566880, 0.580959),
	//vec3(-0.197340, 0.881687, -0.428586),
	vec3(-0.245747, 0.672678, 0.697935),
	//vec3(-0.671037, 0.740315, 0.040544),
	//vec3(-0.909319, 0.211120, 0.358564),
	vec3(0.639245, 0.285763, -0.713937),
	//vec3(-0.697426, 0.332530, -0.634839),
	//vec3(-0.383475, 0.711091, 0.589319),
	vec3(0.929599, 0.353203, 0.105327),
	//vec3(-0.311763, 0.949643, 0.031316),
	//vec3(0.300734, 0.476594, -0.826085),
	vec3(-0.097804, 0.730708, 0.675648),
	//vec3(0.622650, 0.400573, 0.672196),
	//vec3(0.476705, 0.602865, 0.639771),
	vec3(-0.605368, 0.398809, 0.688825)

	//vec3(0.719899, 0.361192, -0.592694),
	//vec3(0.838148, 0.373252, 0.397733)
);

float Occlusion = 0.01f; //1f;

void Generate_TBN()
{
	vec3 Tangent = normalize(cross(Normal, vec3(-Normal.y, Normal.z, Normal.x)));
	vec3 Bitangent = cross(Normal, Tangent);

	TBN = mat3(Tangent, Normal, Bitangent);
}

void Check_Occlusion(vec3 Point)
{
	vec4 Screen_Point = Projection_Matrix * vec4(Point, 1);
	Screen_Point.xy /= Screen_Point.w;
	Screen_Point.xy = Screen_Point.xy * 0.5 + 0.5;

	Occlusion -= texture(Position_Texture, Screen_Point.xy).w + 0.01 < Screen_Point.w ? 0.05 : 0;

	// Occlusion -= 0.05 * Smoothing_Function(10 * (Screen_Point.w - texture(Position_Texture, Screen_Point.xy).w - 0.01));
}

void Ambient_Occlusion()
{
	for(int W = 0; W < 10; W += 1)
	{
		vec3 Sample_Point = TBN * Sampler_Kernel[W];

		Check_Occlusion(0.1 * Sample_Point + Position);
	}
}

//

float Specular_Texture = texture(Material_Texture, Final_UV).x;

vec3 Specular_Lighting = vec3(0, 0, 0);

void Handle_Specular(float In_FOV, vec3 Light_To_Pixel, uint Light_Index)
{
	float Specular_Value = In_FOV * pow(max(0, dot(reflect(-Light_To_Pixel, Normal), Camera_To_Pixel)), 1 + 124 * Specular_Texture);

	Specular_Value *= Specular_Texture; // * 1.5;

	Specular_Lighting += Light_Colour[Light_Index].xyz * Specular_Value;
}

float Vector_To_Depth (vec3 Vec, float Length)
{
    vec3 Abs_Vec = abs(Vec);
    float Local_Z = Length * max(Abs_Vec.x, max(Abs_Vec.y, Abs_Vec.z));

    const float Far = 25.0f;
    const float Near = 0.01f;

    float Norm_Z = (Far + Near) / (Far - Near) - (2 * Far * Near)/(Far - Near)/Local_Z;
    return (Norm_Z + 1.0) * 0.5;
}

float Shadow_Check(vec3 Light_To_Pixel, float Distance, uint Light_Index)
{
	// float Closest_Depth = 25.0f * texture(Shadow_Maps, vec4(Light_To_Pixel, 0.0f));

	// return Distance - 0.00f > Closest_Depth ? 0.0f : 1.0f;

	return texture(Shadow_Maps, vec4(-Light_To_Pixel, Vector_To_Depth(Light_To_Pixel, Distance - 0.00f)));
}

uint Get_Leaf_Node_Index(uint Node, uint Index)
{
	uint Value = Leaf_Node_Indices[Node + (Index >> 2u)];

	Value >>= (Index & 3u) << 3u;

	Value &= 255u;

	return Value;
}

vec3 Lighting()
{
	vec3 Sum_Of_Light = vec3(Occlusion);

	uint Leaf_Node_Index = (Traverse_Partition_Nodes() - 63u) << 1u;

	uint W = 1;

	vec3 Light_To_Pixel = Light_Position[0].xyz - Position;

	float Shadow_Check_Result = Shadow_Check(normalize(Light_To_Pixel), sqrt(dot(Light_To_Pixel, Light_To_Pixel)), 0u);

	for(uint Index = 0; Index < 8; Index++)
	{
		W = Get_Leaf_Node_Index(Leaf_Node_Index, Index);

		// W = Index;

		vec3 Light_To_Pixel = Light_Position[W].xyz - Position;

		float Squared_Distance = dot(Light_To_Pixel, Light_To_Pixel);

		float Inverse_Length = inversesqrt(Squared_Distance);
		Light_To_Pixel *= Inverse_Length;

		float Dot_Normal_Light = max(0.0f, dot(Light_To_Pixel, Normal));

		float Angle = 57 * acos(dot(Light_To_Pixel, -Light_Direction[W].xyz));

		float In_FOV = min(1, Light_Position[W].w * max(0, 1.0f - (Angle - Light_Direction[W].w))) * max(Shadow_Check_Result, float(W > 0u));

		Dot_Normal_Light *= In_FOV;

		Handle_Specular(In_FOV, Light_To_Pixel, W);

		float Attenuation_Value = inversesqrt(Light_Colour[W].w + Squared_Distance);

		// Light_Colour[W].w refers to the attenuation value

		Sum_Of_Light += Dot_Normal_Light * Attenuation_Value * Light_Colour[W].xyz;
	}

	return Sum_Of_Light;
}

//

vec4 Quaternion;

float Add_Screen_Grain()
{
	float X = (UV.x + 4.1) * (UV.y + 4) * (Shader_Time * 10 + 2);

	return mod((mod(X, 13) + 1) * (mod(X, 123) + 1), 0.01) - 0.005f;
}

void main()
{
	Normal = texture(Normal_Texture, Final_UV).xyz; //

	// Quaternion = texture(Normal_Texture, UV);

	// Quaternion *= inversesqrt(dot(Quaternion, Quaternion));
	
	// Normal = Rotate_Vector(vec3(0, 0, 1), Quaternion);

	Generate_TBN();

	Ambient_Occlusion();

	// Out_Colour = texture(Screen_Texture, UV);

	// return;

	vec3 Reflection_Vector = normalize(reflect(Camera_To_Pixel, Normal));

	vec3 Light = Lighting();

	float Reflectivity = texture(Material_Texture, Final_UV).y;

	vec4 Final_Colour = (vec4(Specular_Lighting, 0) + vec4(vec3(Reflectivity), 1) * texture(Cubemap, Reflection_Vector) + vec4(Light, 1) * texture(Screen_Texture, Final_UV));

	Out_Colour = Final_Colour + vec4(vec3(Add_Screen_Grain() * 17.0f), 0.0f);
}