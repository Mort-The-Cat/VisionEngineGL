#ifndef VERTEX_BUFFER_DECLARATIONS_VISION
#define VERTEX_BUFFER_DECLARATIONS_VISION

#include "OpenGL_Declarations.h"

//

class Base_Vertex_Buffer
{
public:
	unsigned int Buffer_ID = Unassigned_Bit_Mask;
	unsigned int Vertex_Array_ID = Unassigned_Bit_Mask;

	uint16_t Buffer_Storage_Hint = GL_STATIC_DRAW;

	// Don't necessarily need index buffers

	Base_Vertex_Buffer() {}

	virtual void Create_Buffer() { ; }

	virtual void Bind_Buffer() { ; }

	virtual void Update_Vertices() { ; } // This will only update the vertices in the vertex buffer - ideal for vertex animation

	virtual void Update_Buffer() { ; }

	virtual void Delete_Buffer() { ; }
};

//

struct Billboard_Vertex
{
	glm::vec2 Screen_Position;
	glm::vec2 UV;

	Billboard_Vertex() {}

	Billboard_Vertex(glm::vec2 Screen_Positionp, glm::vec2 UVp)
	{
		Screen_Position = Screen_Positionp;
		UV = UVp;
	}
};

struct Model_Vertex
{
	glm::vec3 Position; // We need a 3d position vector
	glm::vec3 Normal;

	// glm::vec3 UV_Tangent; // This is the tangent of the surface normal, aligned to the UV plane

	glm::vec2 UV;

	// float Occlusion = 1;

	Model_Vertex() {}

	void operator -=(const Model_Vertex& Other) // This doesn't subtract *everything*
	{
		Position -= Other.Position;
		UV -= Other.UV;
	}

	bool operator== (const Model_Vertex& Other) const
	{
		return 
			Other.Position == Position && 
			Other.Normal == Normal &&
			Other.UV == UV;
	}
};

//

struct Model_Mesh
{
	std::vector<Model_Vertex> Vertices;
	std::vector<unsigned int> Indices;
};

struct Billboard_Mesh
{
	std::vector<Billboard_Vertex> Vertices;
	std::vector<unsigned int> Indices;
};

class Billboard_Vertex_Buffer : public Base_Vertex_Buffer
{
	unsigned int Index_Buffer_ID = Unassigned_Bit_Mask;
public:
	unsigned int Indices_Count = 0;

	Billboard_Mesh Mesh; // Caching and sharing this data would be more effort than it's worth I think

	Billboard_Vertex_Buffer(float X1, float Y1, float X2, float Y2)
	{
		Mesh.Vertices.resize(4);

		Mesh.Vertices[0] = { glm::vec2(X1, Y1), glm::vec2(0, 0) };
		Mesh.Vertices[1] = { glm::vec2(X2, Y1), glm::vec2(1, 0) };
		Mesh.Vertices[2] = { glm::vec2(X2, Y2), glm::vec2(1, 1) };
		Mesh.Vertices[3] = { glm::vec2(X1, Y2), glm::vec2(0, 1) };

		Mesh.Indices = 
		{
			0, 1, 2,
			2, 3, 0
		};

		Create_Buffer();
		Bind_Buffer();
		Update_Buffer();
	}

	Billboard_Vertex_Buffer() {}

	void Delete_Buffer() override
	{
		if (Buffer_ID != Unassigned_Bit_Mask)
		{
			glDeleteBuffers(1, &Buffer_ID);
			glDeleteBuffers(1, &Index_Buffer_ID);

			glDeleteVertexArrays(1, &Vertex_Array_ID);
		}
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh.Vertices[0]) * Mesh.Vertices.size(), Mesh.Vertices.data(), GL_STATIC_DRAW);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Mesh.Indices[0]) * Mesh.Indices.size(), Mesh.Indices.data(), GL_STATIC_DRAW);

		Indices_Count = Mesh.Indices.size();

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Billboard_Vertex), (void*)0); // Position
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Billboard_Vertex), (void*)(sizeof(float) * 2)); // UV
		glEnableVertexAttribArray(1);
	}
};

class Model_Vertex_Buffer : public Base_Vertex_Buffer
{
	unsigned int Index_Buffer_ID = Unassigned_Bit_Mask; // Model vertex buffers need an index buffer
public:

	unsigned int Indices_Count = 0;

	Model_Mesh* Mesh;

	Model_Vertex_Buffer() {}

	//Model_Vertex_Buffer(std::vector<Model_Vertex> Vertices_P, std::vector<unsigned int> Indices_P)

	Model_Vertex_Buffer(Model_Mesh* Meshp)
	{
		Mesh = Meshp;
		//Vertices = Vertices_P;
		//Indices = Indices_P;
	}

	void Delete_Buffer() override
	{
		if (Buffer_ID != Unassigned_Bit_Mask)
		{
			glDeleteBuffers(1, &Buffer_ID);
			glDeleteBuffers(1, &Index_Buffer_ID);

			glDeleteVertexArrays(1, &Vertex_Array_ID);
		}
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

	void Update_Vertices() override
	{
		glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh->Vertices[0]) * Mesh->Vertices.size(), Mesh->Vertices.data(), Buffer_Storage_Hint);
	}

	void Update_Buffer() override
	{
		Update_Vertices();

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Mesh->Indices[0]) * Mesh->Indices.size(), Mesh->Indices.data(), GL_STATIC_DRAW);

		Indices_Count = Mesh->Indices.size();

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Model_Vertex), (void*)0); // Position
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Model_Vertex), (void*)(sizeof(float) * 3)); // Normal
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Model_Vertex), (void*)(sizeof(float) * 6)); // UV
		glEnableVertexAttribArray(2);

		//glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Model_Vertex), (void*)(sizeof(float) * 8)); // Bone rigging weight
		//glEnableVertexAttribArray(3);

		//glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, sizeof(Model_Vertex), (void*)(sizeof(float) * 9));
		//glEnableVertexAttribArray(4);

	}
};

template<> struct std::hash<Model_Vertex>
{
	size_t operator()(Model_Vertex const& Vertex) const
	{
		return
			((std::hash<glm::vec3>()(Vertex.Position) ^
				((std::hash<glm::vec2>()(Vertex.UV) << 1) >> 1) ^
				(std::hash<glm::vec3>()(Vertex.Normal) << 2) >> 2));
	}
};

#endif