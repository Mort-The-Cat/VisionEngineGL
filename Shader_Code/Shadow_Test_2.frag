#version 440

// layout(location = 0) out vec4 Out_Test;

uniform vec3 Light_Position;

void main()
{
	// gl_FragColor = vec4(0, 0, 0, 0);
	gl_FragDepth = gl_FragCoord.z;
	// Out_Test = vec4(0.5f, 0.5f, 0.5f, 1.0f);
}