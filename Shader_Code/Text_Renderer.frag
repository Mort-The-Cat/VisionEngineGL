#version 440

in vec2 UV;

flat in uint Index;

uniform sampler2DArray Letters;

uniform uint Character_Indices[128];

uniform vec4 Colour;

void main()
{
	gl_FragColor = Colour * texture(Letters, vec3(UV, float(Character_Indices[Index]))); // Very sweet little function

	// gl_FragColor = vec4(UV, 1, 1);
}