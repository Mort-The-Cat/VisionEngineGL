#version 440 

layout(location = 0) in vec2 In_Position;
layout(location = 1) in vec2 In_UV;

out vec2 UV;
flat out uint Index;

uniform float Size_Of_Letter;
uniform float Height_Of_Letter;
uniform uint Line_Length;

void main()
{
	uint Horizontal_Offset = gl_InstanceID % Line_Length;
	uint Vertical_Offset = gl_InstanceID / Line_Length;

	gl_Position = vec4(In_Position + vec2(Size_Of_Letter * Horizontal_Offset, Vertical_Offset * -Height_Of_Letter), 1.0f, 1.0f);
	UV = In_UV;
	Index = gl_InstanceID;
}