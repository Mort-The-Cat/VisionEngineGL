#ifndef MODEL_DECLARATIONS_VISIONGL
#define MODEL_DECLARATIONS_VISIONGL

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Uniform_Buffer_Declarations.h"
#include "Texture_Declarations.h"

#define MF_TO_BE_DELETED 0

class Model
{
public:
	Model_Vertex_Buffer Mesh;
	Model_Uniform_Buffer Uniforms;

	Texture Albedo;

	glm::vec3 Position;
	glm::vec3 Orientation;

	bool Flags[1]; // Doesn't really matter how many bits we use for this

	Model() {}

	void Render()
	{
		Mesh.Bind_Buffer();

		Uniforms.Model_Matrix = glm::translate(glm::mat4(1.0f), Position);
		Uniforms.Model_Matrix = glm::rotate(Uniforms.Model_Matrix, Orientation.z, glm::vec3(0, 0, 1));
		Uniforms.Model_Matrix = glm::rotate(Uniforms.Model_Matrix, Orientation.y, glm::vec3(1, 0, 0));
		Uniforms.Model_Matrix = glm::rotate(Uniforms.Model_Matrix, -Orientation.x, glm::vec3(0, 1, 0));

		Uniforms.Update_Buffer();

		glDrawElements(GL_TRIANGLES, Mesh.Indices_Count, GL_UNSIGNED_INT, 0);
	}
};

void Create_Model(Model_Vertex_Buffer Mesh, Texture Albedo, Model* Target_Model)
{
	Target_Model->Mesh = Mesh;
	Target_Model->Albedo = Albedo;
}

std::vector<Model> Scene_Models;

#endif