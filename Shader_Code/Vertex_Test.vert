#version 440

layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_UV;

uniform mat4 Model_Matrix;
uniform mat4 Projection_Matrix;
uniform vec3 Model_Position;

out vec3 Position;
out vec3 Normal;
out vec2 UV;

void main()
{
	vec4 Transformed_Position = Model_Matrix * vec4(In_Position, 1);
	gl_Position = Projection_Matrix * Transformed_Position;
	Position = Transformed_Position.xyz;
	Normal = (Model_Matrix * vec4(In_Normal, 1)).xyz - Model_Position;
	UV = In_UV;
}