#version 440

layout(location = 0) in vec2 In_Position;

void main()
{
	gl_Position = vec4(In_Position, 1, 1);
}