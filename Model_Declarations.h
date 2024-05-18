#ifndef MODEL_DECLARATIONS_VISIONGL
#define MODEL_DECLARATIONS_VISIONGL

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Uniform_Buffer_Declarations.h"
#include "Texture_Declarations.h"
#include "Job_System_Declarations.h"

#include "Mesh_Animator_Declarations.h"

#include "Hitdetection.h"

#define MF_TO_BE_DELETED 0
#define MF_ACTIVE 1
#define MF_SOLID 2
#define MF_PHYSICS_TEST 3

class Model
{
public:
	Model_Vertex_Buffer Mesh;
	Model_Uniform_Buffer Uniforms;

	Texture Albedo;
	Texture Material;

	glm::vec3 Position;
	glm::vec3 Orientation;

	Controller* Control;

	Hitbox* Hitbox;

	bool Flags[4] = { false, false, false, false }; // Doesn't really matter how many bits we use for this

	Model() {}

	Model(std::vector<size_t> Flagsp) 
	{ 
		for (size_t W = 0; W < Flagsp.size(); W++)
			Flags[Flagsp[W]] = true;
	}

	~Model()
	{
		delete Control;
	}

	void Render(Shader Shader)
	{
		Mesh.Bind_Buffer();
		if (Mesh.Buffer_Storage_Hint == GL_DYNAMIC_DRAW)
			Mesh.Update_Vertices();

		Uniforms.Model_Matrix = glm::translate(glm::mat4(1.0f), Position);
		Uniforms.Model_Matrix = glm::rotate(Uniforms.Model_Matrix, Orientation.z, glm::vec3(0, 0, 1));
		Uniforms.Model_Matrix = glm::rotate(Uniforms.Model_Matrix, Orientation.y, glm::vec3(1, 0, 0));
		Uniforms.Model_Matrix = glm::rotate(Uniforms.Model_Matrix, -Orientation.x, glm::vec3(0, 1, 0));

		Uniforms.Model_Position = Position;

		Uniforms.Update_Buffer(Model_Uniform_Location);

		Albedo.Parse_Texture(Shader, "Albedo", 0);
		Albedo.Bind_Texture();
		Material.Parse_Texture(Shader, "Material", 1);
		Material.Bind_Texture();

		glDrawElements(GL_TRIANGLES, Mesh.Indices_Count, GL_UNSIGNED_INT, 0);
	}
};

void Create_Model(Model_Vertex_Buffer Mesh, Texture Albedo, Texture Material, Model* Target_Model, Controller* Controlp, Hitbox* Hitboxp);

std::vector<Model*> Scene_Models;

#endif