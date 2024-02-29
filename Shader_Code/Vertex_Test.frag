#version 440

out vec4 Out_Colour;

uniform vec4 Light_Position[20];
uniform vec4 Light_Colour[20];
uniform vec4 Light_Direction[20];

uniform vec4 Test_Colour;

uniform sampler2D Texture;

in vec3 Position;
in vec3 Normal;
in vec2 UV;

void main()
{
	Out_Colour = inversesqrt(dot(Light_Position[0].xyz - Position, Light_Position[0].xyz - Position)) * Test_Colour * texture(Texture, UV); // vec4(1, 1, 1, 1);
}