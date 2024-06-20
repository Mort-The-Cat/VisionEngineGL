#ifndef LIGHTING_HANDLER_OPENGL
#define LIGHTING_HANDLER_OPENGL

#include "OpenGL_Declarations.h"

#include "Shadow_Map_Renderer_Declarations.h"

struct Light_Uniform_Location_Object
{
	int Position;
	int Colour;
	int Direction;

	int Cubemap;

} Light_Uniform_Location;

Light_Uniform_Location_Object Initialise_Light_Uniform_Locations_Object(Shader Shader)
{
	Light_Uniform_Location_Object Location;
	Location.Position = glGetUniformLocation(Shader.Program_ID, "Light_Position");
	Location.Colour = glGetUniformLocation(Shader.Program_ID, "Light_Colour");
	Location.Direction = glGetUniformLocation(Shader.Program_ID, "Light_Direction");

	Location.Cubemap = glGetUniformLocation(Shader.Program_ID, "Cubemap");

	return Location;
}

#define NUMBER_OF_LIGHTS 20

class Light_Uniform_Buffer // We'll update this and implement any BVH code later on. Until then, we don't need any more
{
public:
	std::array<glm::vec4, NUMBER_OF_LIGHTS> Position;
	std::array<glm::vec4, NUMBER_OF_LIGHTS> Colour;
	std::array<glm::vec4, NUMBER_OF_LIGHTS> Direction;

	void Update_Buffer(Light_Uniform_Location_Object Location)
	{
		glUniform4fv(Location.Position, NUMBER_OF_LIGHTS, glm::value_ptr(Position[0]));
		glUniform4fv(Location.Colour, NUMBER_OF_LIGHTS, glm::value_ptr(Colour[0]));
		glUniform4fv(Location.Direction, NUMBER_OF_LIGHTS, glm::value_ptr(Direction[0]));
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

	float Attenuation; // This is important for things like fire vs things like ordinary lights- it'd be strange if your viewmodel's colour was overexposed the whole game usw.

	bool Flags[1] = { false };
	Lightsource() {}

	Lightsource(glm::vec3 Positionp, glm::vec3 Colourp, glm::vec3 Directionp, float FOVp = 360, float Blurp = 1.0f, float Attenuationp = 0.6f)
	{
		Position = Positionp;
		Colour = Colourp;
		Direction = Directionp;
		FOV = FOVp;
		Blur = Blurp;
		Attenuation = Attenuationp;
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
		Light_Uniforms.Colour[W].w = Scene_Lights[W]->Attenuation;			// Attenuation value

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

	if (Shadow_Mapper::Shadow_Mapping)
	{
		Max_Index = std::min((unsigned int)Scene_Lights.size(), NUMBER_OF_SHADOW_MAPS);
		for (size_t W = 0; W < Max_Index; W++)
			Shadow_Mapper::Shadow_Maps[W].Update_View_Matrices(Scene_Lights[W]->Position);
	}

	Light_Uniforms.Update_Buffer(Light_Uniform_Location);
}

#endif