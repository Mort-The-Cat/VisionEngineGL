#version 440

layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_UV;

uniform mat4 Model_Matrix;
uniform mat4 Projection_Matrix;

out vec3 Normal;
out vec2 UV;

void main()
{
	gl_Position = Projection_Matrix * Model_Matrix * vec4(In_Position, 1);
	Normal = In_Normal;
	UV = In_UV;
}