#ifndef MESH_LOADER_OPENGL
#define MESH_LOADER_OPENGL

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"

glm::vec3 Calculate_UV_Tangent(Model_Vertex A, Model_Vertex B, Model_Vertex Origin)
{
	//Model_Vertex Origin = Target_Mesh->Vertices[Target_Mesh->Indices[W]];
	Model_Vertex Delta_A = A, Delta_B = B;
	//Model_Vertex Delta_A = Target_Mesh->Vertices[Target_Mesh->Indices[W + 1]], Delta_B = Target_Mesh->Vertices[Target_Mesh->Indices[W + 2]];
	Delta_A -= Origin;
	Delta_B -= Origin;

	float B_Scale, A_Scale;

	B_Scale = -Delta_A.UV.y / (Delta_B.UV.y * Delta_A.UV.x - Delta_B.UV.x * Delta_A.UV.y);

	A_Scale = (1 - B_Scale * Delta_B.UV.x) / Delta_A.UV.x;

	float U_Value = A_Scale * Delta_A.UV.x + B_Scale * Delta_B.UV.x;
	float V_Value = A_Scale * Delta_A.UV.y + B_Scale * Delta_B.UV.y;

	return glm::normalize(A_Scale * Delta_A.Position + B_Scale * Delta_B.Position);
}

/*void Calculate_UV_Tangents(Model_Mesh* Target_Mesh) // This only works if every face has unique vertices, not sharing any...
{
	// This *will* cause us to calculate the UV tangent for each vertex more than once, but it doesn't really matter that much.

	// If I think it does matter and I have a good idea for it, I'll loop through them in a slightly different manner

	for (size_t W = 0; W < Target_Mesh->Indices.size(); W += 3) // every tri is determined by every 3rd index
	{
		Target_Mesh->Vertices[Target_Mesh->Indices[W]].UV_Tangent = Calculate_UV_Tangent(
			Target_Mesh->Vertices[Target_Mesh->Indices[W + 1]], 
			Target_Mesh->Vertices[Target_Mesh->Indices[W + 2]], 
			Target_Mesh->Vertices[Target_Mesh->Indices[W]]);

		Target_Mesh->Vertices[Target_Mesh->Indices[W + 1]].UV_Tangent = Calculate_UV_Tangent(
			Target_Mesh->Vertices[Target_Mesh->Indices[W + 2]],
			Target_Mesh->Vertices[Target_Mesh->Indices[W]],
			Target_Mesh->Vertices[Target_Mesh->Indices[W + 1]]);

		Target_Mesh->Vertices[Target_Mesh->Indices[W + 2]].UV_Tangent = Calculate_UV_Tangent(
			Target_Mesh->Vertices[Target_Mesh->Indices[W]],
			Target_Mesh->Vertices[Target_Mesh->Indices[W + 1]],
			Target_Mesh->Vertices[Target_Mesh->Indices[W + 2]]);

		// This gives 
	}
}*/

void Load_Mesh_Obj(const char* File_Name, Model_Mesh* Target_Mesh)
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
				-Attributes.vertices[3 * Index.vertex_index + 1],
				Attributes.vertices[3 * Index.vertex_index + 2]
			};

			Vertex.Normal = {
				Attributes.normals[3 * Index.normal_index],
				-Attributes.normals[3 * Index.normal_index + 1],
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

	// Calculate_UV_Tangents(Target_Mesh);
}

#endif