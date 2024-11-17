#version 440

layout(location = 0) in vec2 In_Position;
layout(location = 1) in vec2 In_UV;

out vec2 UV;

void main()
{
	gl_Position = vec4(In_Position, 1.f, 1.f); // This should work? Hope so

	UV = In_UV;
}