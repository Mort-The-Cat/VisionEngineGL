#ifndef UNIFORM_BUFFER_DECLARATIONS_VISION
#define UNIFORM_BUFFER_DECLARATIONS_VISION

#include "OpenGL_Declarations.h"

//

struct Camera_Uniform_Location_Object
{
	int Projection_Matrix;
	int Camera_Position;
	int Camera_Direction;
} Camera_Uniform_Location;

void Camera::Bind_Buffers()
{
	glUniform3f(Camera_Uniform_Location.Camera_Position, Position.x, Position.y, Position.z);
	glUniformMatrix4fv(Camera_Uniform_Location.Projection_Matrix, 1, GL_FALSE, glm::value_ptr(Projection_Matrix));

	glUniform3f(Camera_Uniform_Location.Camera_Direction, Camera_Direction.x, Camera_Direction.y, Camera_Direction.z);
}

void Initialise_Camera_Uniform_Locations_Object(Shader Shader)
{
	Camera_Uniform_Location.Projection_Matrix = glGetUniformLocation(Shader.Program_ID, "Projection_Matrix");
	Camera_Uniform_Location.Camera_Position = glGetUniformLocation(Shader.Program_ID, "Camera_Position");
	Camera_Uniform_Location.Camera_Direction = glGetUniformLocation(Shader.Program_ID, "Camera_Direction");
}

struct Model_Uniform_Location_Object
{
	int Test_Colour;

	int Model_Matrix;

	int Model_Position;
} Model_Uniform_Location;

void Initialise_Model_Uniform_Locations_Object(Shader Shader)
{
	Model_Uniform_Location.Test_Colour = glGetUniformLocation(Shader.Program_ID, "Test_Colour");
	Model_Uniform_Location.Model_Matrix = glGetUniformLocation(Shader.Program_ID, "Model_Matrix");
	// Model_Uniform_Location.Projection_Matrix = glGetUniformLocation(Shader.Program_ID, "Projection_Matrix");
	Model_Uniform_Location.Model_Position = glGetUniformLocation(Shader.Program_ID, "Model_Position");
}

class Model_Uniform_Buffer
{
public:
	glm::mat4 Model_Matrix;
	glm::vec3 Model_Position;
	Model_Uniform_Buffer() {}

	void Update_Buffer()
	{
		glUniform3f(Model_Uniform_Location.Model_Position, Model_Position.x, Model_Position.y, Model_Position.z);

		glUniformMatrix4fv(Model_Uniform_Location.Model_Matrix, 1, GL_FALSE, glm::value_ptr(Model_Matrix));

		//glUniformMatrix4fv(Model_Uniform_Location.Projection_Matrix, 1, GL_FALSE, glm::value_ptr(Projection_Matrix));
	}
};

#endif
