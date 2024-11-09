#version 440

layout(location = 0) out vec4 Colour;

in vec3 Position;

uniform sampler2D Screen_Texture;
uniform sampler2D Position_Texture;

uniform vec2 Screen_Dimensions;

uniform vec3 Camera_Position;

flat in float Volume_Density_Out;
flat in float C_Out;
flat in float Cosine_Theta_Squared_Out;
flat in float Dot_Source_Direction_Out;

flat in vec3 Cone_Origin_Out;
flat in vec3 Cone_Direction_Out;
flat in vec3 Colour_Out;

void main()
{
	vec3 To_Fragment = normalize(Camera_Position - Position);

	float Dot_A_Delta = dot(To_Fragment, Cone_Direction_Out);
	float Dot_A_A = dot(To_Fragment, To_Fragment);
	float Dot_A_Source = dot(To_Fragment, (Camera_Position - Cone_Origin_Out));

	float A = Dot_A_Delta * Dot_A_Delta - Cosine_Theta_Squared_Out * Dot_A_A;
	float B = 2 * (Dot_A_Delta * Dot_Source_Direction_Out - Cosine_Theta_Squared_Out * Dot_A_Source);

	float Omega = 0.5f / A;

	float Discriminant = B * B - 4 * A * C_Out;

	float Delta = Omega * sqrt(Discriminant);

	Omega *= -B;

	float Distance_From_Camera = length(texture(Position_Texture, gl_FragCoord.xy * Screen_Dimensions).xyz - Camera_Position.xyz);

	float Lambda_Near = min(0, Omega - Delta);

	// Lambda_Near = -length(Camera_Position - Position);

	float Lambda_Far = max(-Distance_From_Camera, Omega + Delta);

	if(Lambda_Far > 0)
		Lambda_Far = -Distance_From_Camera;

	float Transparency = 0.95f - exp(0.6f * (-abs(Lambda_Far - Lambda_Near)));

	Colour = vec4(Colour_Out, Transparency);
}