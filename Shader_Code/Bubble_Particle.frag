#version 440

layout(location = 0) out vec4 Out_Colour;
layout(location = 3) out vec4 Material_Out;

in vec2 Refracted_Vector;

uniform vec2 Inv_Window_Dimensions;

void main()
{
	vec2 Fragment_Vector = gl_FragCoord.xy * Inv_Window_Dimensions * vec2(2.0f) - vec2(1.0f);

	vec2 Tangential_Difference = Refracted_Vector.xy - Fragment_Vector.xy;

	Tangential_Difference /= (vec2(1.0f) + Refracted_Vector.xy * Fragment_Vector.xy);

	Material_Out.zw = Tangential_Difference;
}