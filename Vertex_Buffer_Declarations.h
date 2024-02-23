#ifndef VERTEX_BUFFER_DECLARATIONS_VISION
#define VERTEX_BUFFER_DECLARATIONS_VISION

#include "OpenGL_Declarations.h"

//

class Base_Vertex_Buffer
{
public:
	unsigned int Buffer_ID = 0xFFFFFFFF;
	unsigned int Vertex_Array_ID = 0xFFFFFFFF;

	// Don't necessarily need index buffers

	Base_Vertex_Buffer() {}

	virtual void Create_Buffer() { ; }

	virtual void Bind_Buffer() { ; }

	virtual void Update_Buffer() { ; }
};

//

struct Model_Vertex
{
	glm::vec3 Position; // We need a 3d position vector
	glm::vec3 Normal;
	glm::vec2 UV;
	Model_Vertex() {}
	Model_Vertex(float X, float Y, float R, float G, float B, float U, float V)
	{
		Position.x = X;
		Position.y = Y;
		Position.z = 0;
		//Colour.r = R;
		//Colour.g = G;
		//Colour.b = B;
		UV.x = U;
		UV.y = V;
	}
};

//

class Model_Vertex_Buffer : public Base_Vertex_Buffer
{
	unsigned int Index_Buffer_ID = 0xFFFFFFFF; // Model vertex buffers need an index buffer
public:
	std::vector<Model_Vertex> Vertices;
	std::vector<unsigned int> Indices;

	Model_Vertex_Buffer() {}

	Model_Vertex_Buffer(std::vector<Model_Vertex> Vertices_P, std::vector<unsigned int> Indices_P)
	{
		Vertices = Vertices_P;
		Indices = Indices_P;
	}

	void Create_Buffer() override
	{
		glGenBuffers(1, &Buffer_ID);
		glGenVertexArrays(1, &Vertex_Array_ID);

		glGenBuffers(1, &Index_Buffer_ID);
	}

	void Bind_Buffer() override
	{
		glBindVertexArray(Vertex_Array_ID);
		glBindBuffer(GL_ARRAY_BUFFER, Buffer_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Index_Buffer_ID);
	}

	void Update_Buffer() override
	{
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0]) * Vertices.size(), Vertices.data(), GL_STATIC_DRAW);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), Indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Model_Vertex), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Model_Vertex), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Model_Vertex), (void*)(sizeof(float) * 6));
		glEnableVertexAttribArray(2);
	}
};

#endif