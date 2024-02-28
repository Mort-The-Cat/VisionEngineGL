#ifndef UNIFORM_BUFFER_DECLARATIONS_VISION
#define UNIFORM_BUFFER_DECLARATIONS_VISION

#include "OpenGL_Declarations.h"

//

struct Model_Uniform_Location_Object
{
	int Test_Colour;

	int Model_Matrix;

	int Projection_Matrix;
} Model_Uniform_Location;

void Initialise_Model_Uniform_Locations_Object(Shader Shader)
{
	Model_Uniform_Location.Test_Colour = glGetUniformLocation(Shader.Program_ID, "Test_Colour");
	Model_Uniform_Location.Model_Matrix = glGetUniformLocation(Shader.Program_ID, "Model_Matrix");
	Model_Uniform_Location.Projection_Matrix = glGetUniformLocation(Shader.Program_ID, "Projection_Matrix");
}

class Model_Uniform_Buffer
{
public:
	glm::vec4 Colour;

	glm::mat4 Model_Matrix;
	Model_Uniform_Buffer() {}

	Model_Uniform_Buffer(glm::vec4 Colourp)
	{
		Colour = Colourp;
	}

	void Update_Buffer()
	{
		glUniform4f(Model_Uniform_Location.Test_Colour, Colour.x, Colour.y, Colour.z, Colour.w);

		glUniformMatrix4fv(Model_Uniform_Location.Model_Matrix, 1, GL_FALSE, glm::value_ptr(Model_Matrix));

		glUniformMatrix4fv(Model_Uniform_Location.Projection_Matrix, 1, GL_FALSE, glm::value_ptr(Projection_Matrix));
	}
};

#endif
