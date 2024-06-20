#version 440

layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_UV;

out vec4 Fragment_Position;

uniform mat4 Model_Matrix;

uniform mat4 Shadow_Matrix;

void main()
{
	Fragment_Position = Model_Matrix * vec4(In_Position, 0.0f);

	gl_Position = Shadow_Matrix * Fragment_Position;
}