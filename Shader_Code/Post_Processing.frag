#version 440

out vec4 Out_Colour;

in vec2 UV;

uniform sampler2D Screen_Texture;

void main()
{
	Out_Colour = vec4(1.0f) - texture(Screen_Texture, UV);
}