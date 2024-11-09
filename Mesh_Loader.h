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

aiVertexWeight Get_Optimal_Bone(const aiMesh* Mesh, size_t Vertex_Index)
{
	aiVertexWeight Optimal_Bone;
	Optimal_Bone.mWeight = 0;
	for(size_t W = 0; W < Mesh->mNumBones; W++)
		for (size_t V = 0; V < Mesh->mBones[W]->mNumWeights; V++)
			if (Mesh->mBones[W]->mWeights[V].mVertexId == Vertex_Index)
			{
				if (Mesh->mBones[W]->mWeights[V].mWeight >= Optimal_Bone.mWeight)
				{
					Optimal_Bone = Mesh->mBones[W]->mWeights[V];
					Optimal_Bone.mVertexId = W;
				}

				break;
			}

	return Optimal_Bone;
}

void Load_Mesh_Fbx(const char* File_Name, Model_Mesh* Target_Mesh)
{
	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(File_Name, (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_PopulateArmatureData));

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
					//aiVertexWeight Weight = Get_Optimal_Bone(Mesh, Index);
					//Vertex.Bone_Rigging_Index = Weight.mVertexId;
					//V/ertex.Bone_Rigging_Weight = Weight.mWeight;
					Target_Mesh->Vertices.push_back(Vertex);
				}

				Target_Mesh->Indices.push_back(Unique_Vertices[Vertex]);
			}
	}

	Importer.FreeScene();
}

struct Vertex_Indices
{
	size_t Positions_Index, Normals_Index, UV_Index;

	bool operator==(const Vertex_Indices Other)
	{
		return Positions_Index == Other.Positions_Index && Normals_Index == Other.Normals_Index && UV_Index == Other.UV_Index;
	}
};

bool Identify_Existing_Vertex(std::vector<Vertex_Indices>& Unique_Vertices, Vertex_Indices Vertex, size_t* Index)
{
	for(size_t W = 0; W < Unique_Vertices.size(); W++)
		if (Unique_Vertices[W] == Vertex)
		{
			*Index = W;
			return true;
		}
	return false;
}

void Load_Hitbox_Mesh_Obj(const char* File_Name, Model_Mesh* Target_Mesh)
{
	std::ifstream File(File_Name);

	if (File.is_open())
	{
		std::string Line;
		while (std::getline(File, Line))
		{
			std::stringstream Buffer;
			std::string Prefix;
			Buffer << Line;
			Buffer >> Prefix;

			if (Prefix == "v")
			{
				Model_Vertex Vertex;

				Buffer >> Vertex.Position.x >> Vertex.Position.y >> Vertex.Position.z;

				Vertex.Position.y *= -1;

				Target_Mesh->Vertices.push_back(Vertex);
			}
		}
	}
	else
	{
		Throw_Error("Unable to open file!\n");
	}

	File.close();
}

void Load_Mesh_Obj_Old(const char* File_Name, Model_Mesh* Target_Mesh)
{
	std::vector<glm::vec3> Positions;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec2> UVs;

	std::vector<Vertex_Indices> Unique_Vertices;

	std::ifstream File(File_Name);

	if (File.is_open())
	{
		std::string Line;
		while (std::getline(File, Line))
		{
			std::stringstream Buffer;
			std::string Prefix;
			Buffer << Line;
			Buffer >> Prefix;
			
			if (Prefix[0] != '#')
			{

				if (Prefix == "v")
				{
					Positions.push_back(glm::vec3());
					Buffer >> Positions.back().x >> Positions.back().y >> Positions.back().z;

					Positions.back().y *= -1;
				}
				else if (Prefix == "vn")
				{
					Normals.push_back(glm::vec3());
					Buffer >> Normals.back().x >> Normals.back().y >> Normals.back().z;

					Normals.back().y *= -1;
				}
				else if (Prefix == "vt")
				{
					UVs.push_back(glm::vec2());
					Buffer >> UVs.back().x >> UVs.back().y;

					UVs.back().y = 1.0f - UVs.back().y;
				}
				else if (Prefix == "f")
				{
					for (size_t W = 0; W < Line.length(); W++)
						if (Line[W] == '/')
							Line[W] = ' ';

					Buffer = std::stringstream();
					Buffer << Line;
					Buffer >> Prefix;

					for (size_t W = 0; W < 3; W++)
					{
						size_t Position_Index, Normal_Index, UV_Index;
						Buffer >> Position_Index >> UV_Index >> Normal_Index;

						Vertex_Indices Indices;
						Indices.Positions_Index = Position_Index;
						Indices.Normals_Index = Normal_Index;
						Indices.UV_Index = UV_Index;

						size_t Index;

						if (!Identify_Existing_Vertex(Unique_Vertices, Indices, &Index))
						{
							Index = Target_Mesh->Vertices.size();

							Model_Vertex Vertex;
							Vertex.Position = Positions[Position_Index - 1];
							Vertex.Normal = Normals[Normal_Index - 1];
							Vertex.UV = UVs[UV_Index - 1];

							Target_Mesh->Vertices.push_back(Vertex);
						}

						Target_Mesh->Indices.push_back(Index);
					}
				}

			}
		}
	}
	else
		Throw_Error("Unable to open file!\n");

	File.close();
}

void Load_Mesh_Obj(const char* File_Name, Model_Mesh* Target_Mesh, bool Anim_Mesh = false)
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

			if (Unique_Vertices.count(Vertex) == 0 || Anim_Mesh)
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