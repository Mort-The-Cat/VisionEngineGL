#version 440

in vec2 UV;

uniform sampler2D Albedo;

uniform vec4 Colour;

float Multisample_Texture()
{
	float Sum = 0;
	Sum += 0.444444444 * texture(Albedo, UV).r;

	Sum += 0.111111111 * texture(Albedo, UV + vec2(0, 0.001)).r;
	Sum += 0.111111111 * texture(Albedo, UV + vec2(0.001, 0)).r;
	Sum += 0.111111111 * texture(Albedo, UV - vec2(0, 0.001)).r;
	Sum += 0.111111111 * texture(Albedo, UV - vec2(0.001, 0)).r;

	Sum += 0.0277777778 * texture(Albedo, UV + vec2(0.001, 0.001)).r;
	Sum += 0.0277777778 * texture(Albedo, UV + vec2(0.001, -0.001)).r;
	Sum += 0.0277777778 * texture(Albedo, UV + vec2(-0.001, 0.001)).r;
	Sum += 0.0277777778 * texture(Albedo, UV + vec2(-0.001, -0.001)).r;

	return Sum;
}

void main()
{
	gl_FragColor = Colour * texture(Albedo, UV).r; // Very sweet little function

	// gl_FragColor = vec4(1, 1, 1, texture(Albedo, UV).r);

	// gl_FragColor = vec4(1, 1, 1, 1);
}