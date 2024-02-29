#ifndef LIGHTING_HANDLER_OPENGL
#define LIGHTING_HANDLER_OPENGL

#include "OpenGL_Declarations.h"

struct Light_Uniform_Location_Object
{
	int Position;
	int Colour;
	int Direction;
} Light_Uniform_Location;

void Initialise_Light_Uniform_Locations_Object(Shader Shader)
{
	Light_Uniform_Location.Position = glGetUniformLocation(Shader.Program_ID, "Light_Position");
	Light_Uniform_Location.Colour = glGetUniformLocation(Shader.Program_ID, "Light_Colour");
	Light_Uniform_Location.Direction = glGetUniformLocation(Shader.Program_ID, "Light_Direction");

}

#define NUMBER_OF_LIGHTS 20

class Light_Uniform_Buffer // We'll update this and implement any BVH code later on. Until then, we don't need any more
{
public:
	std::array<glm::vec4, NUMBER_OF_LIGHTS> Position;
	std::array<glm::vec4, NUMBER_OF_LIGHTS> Colour;
	std::array<glm::vec4, NUMBER_OF_LIGHTS> Direction;

	void Update_Buffer()
	{
		glUniform4fv(Light_Uniform_Location.Position, NUMBER_OF_LIGHTS, glm::value_ptr(Position[0]));
		glUniform4fv(Light_Uniform_Location.Colour, NUMBER_OF_LIGHTS, glm::value_ptr(Colour[0]));
		glUniform4fv(Light_Uniform_Location.Direction, NUMBER_OF_LIGHTS, glm::value_ptr(Direction[0]));
	}
} Light_Uniforms;

#define LF_TO_BE_DELETED 0

class Lightsource
{
public:
	glm::vec3 Position;
	glm::vec3 Colour;
	glm::vec3 Direction; 

	float FOV;
	float Blur;

	bool Flags[1] = { false };
	Lightsource() {}

	Lightsource(glm::vec3 Positionp, glm::vec3 Colourp, glm::vec3 Directionp, float FOVp, float Blurp)
	{
		Position = Positionp;
		Colour = Colourp;
		Direction = Directionp;
		FOV = FOVp;
		Blur = Blurp;
	}
};

std::vector<Lightsource*> Scene_Lights;

void Update_Lighting_Buffer()
{
	size_t Max_Index = std::min((int)Scene_Lights.size(), NUMBER_OF_LIGHTS);
	for (size_t W = 0; W < Max_Index; W++)
	{
		Light_Uniforms.Position[W].x = Scene_Lights[W]->Position.x;
		Light_Uniforms.Position[W].y = Scene_Lights[W]->Position.y;
		Light_Uniforms.Position[W].z = Scene_Lights[W]->Position.z;
		Light_Uniforms.Position[W].w = 1.0f / Scene_Lights[W]->Blur;		// Blur value

		Light_Uniforms.Colour[W].r = Scene_Lights[W]->Colour.r;
		Light_Uniforms.Colour[W].g = Scene_Lights[W]->Colour.g;
		Light_Uniforms.Colour[W].b = Scene_Lights[W]->Colour.b;
		Light_Uniforms.Colour[W].w = 1;								// Attenuation value

		Light_Uniforms.Direction[W].x = Scene_Lights[W]->Direction.x;
		Light_Uniforms.Direction[W].y = Scene_Lights[W]->Direction.y;
		Light_Uniforms.Direction[W].z = Scene_Lights[W]->Direction.z;
		Light_Uniforms.Direction[W].w = Scene_Lights[W]->FOV;
	}

	for (size_t W = Max_Index; W < NUMBER_OF_LIGHTS; W++)
	{
		Light_Uniforms.Position[W] = glm::vec4(0, 0, 0, 0);
		Light_Uniforms.Colour[W] = glm::vec4(0, 0, 0, 0);
		Light_Uniforms.Direction[W] = glm::vec4(0, 0, 0, 0);
	}

	Light_Uniforms.Update_Buffer();
}

#endif