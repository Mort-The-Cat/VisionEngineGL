#version 440

layout(location = 0) out vec4 Out_Colour;

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

in float Vertex_Transparency;

void main()
{
	// Out_Colour = vec4(0.4, 0.25, 0.8, (1.0f - 2.0f * length(UV - vec2(0.5f))) );
	
	Out_Colour = texture(Albedo, UV) * vec4(vec3(Vertex_Transparency), 1.0f);
}