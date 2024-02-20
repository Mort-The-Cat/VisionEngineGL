#ifndef VERTEX_BUFFER_DECLARATIONS_VISION
#define VERTEX_BUFFER_DECLARATIONS_VISION

#include "OpenGL_Declarations.h"

class Model_Vertex_Buffer
{
	unsigned int Buffer_ID;
	unsigned int Vertex_Array_ID;
public:
	std::vector<glm::vec2> Vertices;

	Model_Vertex_Buffer() {}

	Model_Vertex_Buffer(std::vector<glm::vec2> Vertices_P)
	{
		Vertices = Vertices_P;
	}

	void Create_Buffer()
	{
		glGenBuffers(1, &Buffer_ID);
		glGenVertexArrays(1, &Vertex_Array_ID);
	}

	void Bind_Buffer()
	{
		glBindBuffer(GL_ARRAY_BUFFER, Buffer_ID);
		glBindVertexArray(Vertex_Array_ID);
	}

	void Update_Buffer()
	{
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
		glEnableVertexAttribArray(0);
	}
};

#endif