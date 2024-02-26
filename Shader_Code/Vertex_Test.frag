#version 440

out vec4 Out_Colour;

uniform vec4 Test_Colour;

uniform sampler2D Texture;

in vec3 Normal;
in vec2 UV;

void main()
{
	Out_Colour = Test_Colour * texture(Texture, UV); // vec4(1, 1, 1, 1);
}