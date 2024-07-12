#version 440 

layout(location = 0) in vec2 In_Position;
layout(location = 1) in vec2 In_UV;

out vec2 UV;
flat out uint Index;

uniform float Size_Of_Letter;

void main()
{
	gl_Position = vec4(In_Position + vec2(Size_Of_Letter * gl_InstanceID, 0.0f), 1.0f, 1.0f);
	UV = In_UV;
	Index = gl_InstanceID;
}