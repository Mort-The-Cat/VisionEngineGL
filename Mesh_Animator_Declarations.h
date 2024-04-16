#ifndef MESH_ANIMATOR_DECLARATIONS
#define MESH_ANIMATOR_DECLARATIONS

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Quaternion.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#define NUMBER_OF_ANIMATOR_BONES 16

struct Bones_Uniform_Buffer
{
	glm::mat4 Bone_Matrix[NUMBER_OF_ANIMATOR_BONES]; // 256 'words' for the GPU
};

class Mesh_Animator;

class Bone
{
	std::vector<Quaternion::Quaternion> Rotations;
	std::vector<glm::vec3> Transformation;
	
	// std::vector<glm::vec3> Scaling; (This'll probably remain unused but I can implement it properly later.)

	std::vector<float> Durations; // Iterate through these when applying animation transformations to find the transformation in question and apply it accordingly

	std::vector<unsigned char> Child_Indices;

	unsigned char Index;

	void Calculate_Transformations(Mesh_Animator* Animator, unsigned char Parent_Index);
}; 

class Mesh_Animator
{
public:
	Bones_Uniform_Buffer* Skeleton_Uniforms;

	std::vector<Bone> Bones;

	float Time;
};

void Bone::Calculate_Transformations(Mesh_Animator* Animator, unsigned char Parent_Index)
{
	unsigned char Keyframe_Index = 0;
	float Time = 0;
	while (Time > Durations[Keyframe_Index] && Keyframe_Index < Durations.size())
	{
		Time -= Durations[Keyframe_Index];
		Keyframe_Index++;
	}

	if (Keyframe_Index == Durations.size())
	{
		Time = 1;	// This just caps the time so we don't need to do any different kinda interpolation code
		Keyframe_Index--;
	}

	{
		float Scalar = Time / Durations[Keyframe_Index]; // This normalises the time for this current keyframe between 0-1

		Quaternion::Quaternion Current_Rotation = Quaternion::Sphere_Interpolate(Rotations[Keyframe_Index], Rotations[Keyframe_Index + 1], Scalar);

		// This interpolates the rotation

		glm::vec3 Current_Position = Transformation[Keyframe_Index] * (1.0f - Scalar) + Transformation[Keyframe_Index + 1] * Scalar;

		Animator->Skeleton_Uniforms->Bone_Matrix[Index] = Animator->Skeleton_Uniforms->Bone_Matrix[Parent_Index] * glm::translate(Current_Rotation.Get_Rotation_Matrix(), Current_Position);

		// This sets our bone's transformation matrix
	}

	for (size_t W = 0; W < Child_Indices.size(); W++)
		Animator->Bones[Child_Indices[W]].Calculate_Transformations(Animator, Index);

	// This recursively handles all of the bone transformations in a way that's still nice on the heap's memory
}

void Recursively_Add_Bones(aiNode* Node, Mesh_Animator* Target_Animator)
{

}

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
		{
			Recursively_Add_Bones(Scene->mRootNode, Target_Animator);
		}
	}
}

#endif