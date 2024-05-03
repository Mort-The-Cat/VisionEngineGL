#ifndef UNIFORM_BUFFER_DECLARATIONS_VISION
#define UNIFORM_BUFFER_DECLARATIONS_VISION

#include "OpenGL_Declarations.h"
#include "Mesh_Animator_Declarations.h"

//

void Camera::Bind_Buffers(Camera_Uniform_Location_Object Location)
{
	glUniform3f(Location.Camera_Position, Position.x, Position.y, Position.z);
	glUniformMatrix4fv(Location.Projection_Matrix, 1, GL_FALSE, glm::value_ptr(Projection_Matrix));

	glUniform3f(Location.Camera_Up_Direction, Camera_Up_Direction.x, Camera_Up_Direction.y, Camera_Up_Direction.z);

	glUniform3f(Location.Camera_Direction, Camera_Direction.x, Camera_Direction.y, Camera_Direction.z);
}

Camera_Uniform_Location_Object Initialise_Camera_Uniform_Locations_Object(Shader Shader)
{
	Camera_Uniform_Location_Object Locations;
	Locations.Projection_Matrix = glGetUniformLocation(Shader.Program_ID, "Projection_Matrix");
	Locations.Camera_Position = glGetUniformLocation(Shader.Program_ID, "Camera_Position");
	Locations.Camera_Up_Direction = glGetUniformLocation(Shader.Program_ID, "Camera_Up_Direction");
	Locations.Camera_Direction = glGetUniformLocation(Shader.Program_ID, "Camera_Direction");

	return Locations;
}

struct Model_Uniform_Location_Object
{
	int Model_Matrix;

	int Model_Position;

	int Model_Bones;
} Model_Uniform_Location;

Model_Uniform_Location_Object Initialise_Model_Uniform_Locations_Object(Shader Shader)
{
	Model_Uniform_Location_Object Location;
	Location.Model_Matrix = glGetUniformLocation(Shader.Program_ID, "Model_Matrix");
	// Model_Uniform_Location.Projection_Matrix = glGetUniformLocation(Shader.Program_ID, "Projection_Matrix");
	Location.Model_Position = glGetUniformLocation(Shader.Program_ID, "Model_Position");

	Location.Model_Bones = glGetUniformLocation(Shader.Program_ID, "Model_Bones");
	return Location;
}

class Model_Uniform_Buffer
{
public:
	glm::mat4 Model_Matrix;
	glm::vec3 Model_Position;
	Bones_Uniform_Buffer Model_Bones;
	Model_Uniform_Buffer()
	{
		for (size_t W = 0; W < NUMBER_OF_ANIMATOR_BONES; W++)
		{
			Model_Bones.Bone_Matrix[W] = glm::mat4(1.0f);// glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.1f * W, 0)), DTR * W * 5, glm::vec3(0, 1, 0));		// This just sets all of the matrices to a nice default value
		}
		//Model_Bones.Bone_Matrix[2] = glm::rotate(glm::mat4(1.0f), DTR * 90.0f, glm::vec3(1, 0, 0));
	}

	void Update_Buffer(Model_Uniform_Location_Object Model_Uniform_Location)
	{
		glUniform3f(Model_Uniform_Location.Model_Position, Model_Position.x, Model_Position.y, Model_Position.z);

		glUniformMatrix4fv(Model_Uniform_Location.Model_Matrix, 1, GL_FALSE, glm::value_ptr(Model_Matrix));

		glUniformMatrix4fv(Model_Uniform_Location.Model_Bones, NUMBER_OF_ANIMATOR_BONES, GL_FALSE, glm::value_ptr(Model_Bones.Bone_Matrix[0]));
		//glUniformMatrix4fv(Model_Uniform_Location.Projection_Matrix, 1, GL_FALSE, glm::value_ptr(Projection_Matrix));
	}
};

#endif
