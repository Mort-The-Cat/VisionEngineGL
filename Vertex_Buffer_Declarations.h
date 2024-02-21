#ifndef VERTEX_BUFFER_DECLARATIONS_VISION
#define VERTEX_BUFFER_DECLARATIONS_VISION

#include "OpenGL_Declarations.h"

class Model_Vertex_Buffer
{
	unsigned int Buffer_ID;
	unsigned int Vertex_Array_ID;

	unsigned int Index_Buffer_ID;

public:
	std::vector<glm::vec2> Vertices;
	std::vector<unsigned int> Indices;

	Model_Vertex_Buffer() {}

	Model_Vertex_Buffer(std::vector<glm::vec2> Vertices_P, std::vector<unsigned int> Indices_P)
	{
		Vertices = Vertices_P;
		Indices = Indices_P;
	}

	void Create_Buffer()
	{
		glGenBuffers(1, &Buffer_ID);
		glGenVertexArrays(1, &Vertex_Array_ID);

		glGenBuffers(1, &Index_Buffer_ID);
	}

	void Bind_Buffer()
	{
		glBindVertexArray(Vertex_Array_ID);

		// glBindBuffer(GL_ARRAY_BUFFER, Buffer_ID);

		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Index_Buffer_ID);
	}

	void Update_Buffer()
	{
		glBindVertexArray(Vertex_Array_ID);

		glBindBuffer(GL_ARRAY_BUFFER, Buffer_ID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0]) * Vertices.size(), Vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Index_Buffer_ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), Indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
		glEnableVertexAttribArray(0);
	}
};

#endif