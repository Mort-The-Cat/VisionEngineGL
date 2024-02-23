#ifndef MESH_LOADER_OPENGL
#define MESH_LOADER_OPENGL

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"

void Load_Mesh_Obj(const char* File_Name, Model_Vertex_Buffer* Target_Mesh)
{
#define Obj tinyobj // This means we can avoid using this god-awful namespace 

	Obj::attrib_t Attributes;
	std::vector<Obj::shape_t> Shapes;
	std::vector<Obj::material_t> Materials;

	std::string Warning, Error;

	if (!Obj::LoadObj(&Attributes, &Shapes, &Materials, &Warning, &Error, File_Name))
	{
		printf(" >> Failed to load object!\n%s\n%s", Warning.c_str(), Error.c_str());
		std::cin.get();
	}

	std::unordered_map<Model_Vertex, uint32_t> Unique_Vertices{};

	for (const auto& Shape : Shapes)
	{
		for (const auto& Index : Shape.mesh.indices)
		{
			Model_Vertex Vertex;

			Vertex.Position = {
				Attributes.vertices[3 * Index.vertex_index],
				Attributes.vertices[3 * Index.vertex_index + 1],
				Attributes.vertices[3 * Index.vertex_index + 2]
			};

			Vertex.Normal = {
				Attributes.normals[3 * Index.normal_index],
				Attributes.normals[3 * Index.normal_index + 1],
				Attributes.normals[3 * Index.normal_index + 2]
			};

			Vertex.UV = {
				Attributes.texcoords[2 * Index.texcoord_index],
				1.0 - Attributes.texcoords[2 * Index.texcoord_index + 1]
			};

			//Vertex.Occlusion = 1;

			//Vertices.push_back(Vertex);
			//Indices.push_back(Indices.size()); // We'll adjust this later

			if (Unique_Vertices.count(Vertex) == 0)
			{
				Unique_Vertices[Vertex] = Target_Mesh->Vertices.size();
				Target_Mesh->Vertices.push_back(Vertex);
			}

			Target_Mesh->Indices.push_back(Unique_Vertices[Vertex]);
		}
	}
}

#endif