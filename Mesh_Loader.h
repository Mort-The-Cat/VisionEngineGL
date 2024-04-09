#ifndef MESH_LOADER_OPENGL
#define MESH_LOADER_OPENGL

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"

#include "assimp/Importer.hpp"
// #include "assimp/code/Importer.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

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

void Load_Mesh_Fbx(const char* File_Name, Model_Mesh* Target_Mesh)
{
	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(File_Name, (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices));

	if (Scene == nullptr)
	{
		Throw_Error(" >> Failed to load fbx with assimp!");
	}

	// Then, we handle all of the meshes

	std::unordered_map<Model_Vertex, uint32_t> Unique_Vertices{};

	for (size_t W = 0; W < Scene->mNumMeshes; W++)
	{
		const aiMesh* Mesh = Scene->mMeshes[W];
		for (size_t V = 0; V < Mesh->mNumFaces; V++)
			for (size_t U = 0; U < 3; U++)
			{
				Model_Vertex Vertex;

				size_t Index = Mesh->mFaces[V].mIndices[U];

				Vertex.Position = glm::vec3(
					Mesh->mVertices[Index].x, 
					-Mesh->mVertices[Index].z, 
					-Mesh->mVertices[Index].y);

				Vertex.Normal = glm::vec3(
					Mesh->mNormals[Index].x,
					-Mesh->mNormals[Index].z,
					-Mesh->mNormals[Index].y);

				Vertex.UV = glm::vec2(
					Mesh->mTextureCoords[0][Index].x,
					1.0f - Mesh->mTextureCoords[0][Index].y);

				if (Unique_Vertices.count(Vertex) == 0) // If we don't have it already
				{
					Unique_Vertices[Vertex] = Target_Mesh->Vertices.size();
					Target_Mesh->Vertices.push_back(Vertex);
				}

				Target_Mesh->Indices.push_back(Unique_Vertices[Vertex]);
			}
	}
}

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