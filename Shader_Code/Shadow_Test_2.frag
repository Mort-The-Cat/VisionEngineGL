#version 440

// layout(location = 0) out vec4 Out_Colour;

out float gl_FragDepth;

in vec3 Position;

void main()
{
	// Out_Colour = vec4(0, 0, 0, 1.0);
	// gl_FragDepth = 0.0f; // gl_FragCoord.z;

	gl_FragDepth = sqrt(dot(Position.xyz, Position.xyz)) / 25.0f;
}