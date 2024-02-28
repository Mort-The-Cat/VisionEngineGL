#ifndef MODEL_DECLARATIONS_VISIONGL
#define MODEL_DECLARATIONS_VISIONGL

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Uniform_Buffer_Declarations.h"
#include "Texture_Declarations.h"

#define MF_TO_BE_DELETED 0

class Model
{
	Model_Vertex_Buffer Mesh;
	Model_Uniform_Buffer Uniforms;

	glm::vec3 Position;
	glm::vec3 Orientation;

	bool Flags[1]; // Doesn't really matter how many bits we use for this

	Model() {}

	void Render()
	{
		Mesh.Bind_Buffer();
		Uniforms.Update_Buffer();

		glDrawElements(GL_TRIANGLES, Mesh.Indices_Count, GL_UNSIGNED_INT, 0);
	}
};

void Create_Model(Model_Vertex_Buffer Mesh, Texture Albedo)
{

}

std::vector<Model> Scene_Models;

#endif