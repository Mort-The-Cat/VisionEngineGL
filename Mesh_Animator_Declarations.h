#ifndef MESH_ANIMATOR_DECLARATIONS
#define MESH_ANIMATOR_DECLARATIONS

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

struct Bones_Uniform_Buffer
{
	glm::mat4 Bone_Matrix[16]; // 256 'words' for the GPU
};

class Mesh_Animator
{
	Bones_Uniform_Buffer* Bone;
};

void Load_Mesh_Animator_Fbx(const char* File_Name, Mesh_Animator* Target_Animator)
{
	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(File_Name, (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices));

	if (Scene == nullptr)
		Throw_Error(" >> Failed to load FBX file!\n");

	std::unordered_map<std::string, size_t> Bone_Indices;
	
	for (size_t V = 0; V < Scene->mMeshes[0]->mNumBones; V++)
		Bone_Indices[std::string(Scene->mMeshes[0]->mBones[V]->mName.C_Str())] = V;

	for (size_t W = 0; W < Scene->mNumAnimations; W++)
	{
		for (size_t V = 0; V < Scene->mAnimations[W]->mNumChannels; V++)
			printf("a");
	}
}

#endif