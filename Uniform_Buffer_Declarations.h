#ifndef UNIFORM_BUFFER_DECLARATIONS_VISION
#define UNIFORM_BUFFER_DECLARATIONS_VISION

#include "OpenGL_Declarations.h"

//

void Camera::Bind_Buffers(Camera_Uniform_Location_Object Location)
{
	glUniform3f(Location.Camera_Position, Position.x, Position.y, Position.z);
	glUniformMatrix4fv(Location.Projection_Matrix, 1, GL_FALSE, glm::value_ptr(Projection_Matrix));

	glUniform3f(Location.Camera_Direction, Camera_Direction.x, Camera_Direction.y, Camera_Direction.z);
}

Camera_Uniform_Location_Object Initialise_Camera_Uniform_Locations_Object(Shader Shader)
{
	Camera_Uniform_Location_Object Locations;
	Locations.Projection_Matrix = glGetUniformLocation(Shader.Program_ID, "Projection_Matrix");
	Locations.Camera_Position = glGetUniformLocation(Shader.Program_ID, "Camera_Position");
	Locations.Camera_Direction = glGetUniformLocation(Shader.Program_ID, "Camera_Direction");

	return Locations;
}

struct Model_Uniform_Location_Object
{
	int Test_Colour;

	int Model_Matrix;

	int Model_Position;
} Model_Uniform_Location;

Model_Uniform_Location_Object Initialise_Model_Uniform_Locations_Object(Shader Shader)
{
	Model_Uniform_Location_Object Location;
	Location.Test_Colour = glGetUniformLocation(Shader.Program_ID, "Test_Colour");
	Location.Model_Matrix = glGetUniformLocation(Shader.Program_ID, "Model_Matrix");
	// Model_Uniform_Location.Projection_Matrix = glGetUniformLocation(Shader.Program_ID, "Projection_Matrix");
	Location.Model_Position = glGetUniformLocation(Shader.Program_ID, "Model_Position");
	return Location;
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
