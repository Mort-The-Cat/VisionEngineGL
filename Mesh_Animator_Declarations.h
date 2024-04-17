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
public:
	std::vector<Quaternion::Quaternion> Rotations;
	std::vector<glm::vec3> Transformation;
	
	// std::vector<glm::vec3> Scaling; (This'll probably remain unused but I can implement it properly later.)

	std::vector<float> Durations; // Iterate through these when applying animation transformations to find the transformation in question and apply it accordingly

	std::vector<unsigned char> Child_Indices;

	unsigned char Index;

	void Calculate_Transformations(Mesh_Animator* Animator, unsigned char Parent_Index);
}; 

#define ANIMF_TO_BE_DELETED 0u
#define ANIMF_LOOP 1u

class Mesh_Animator
{
public:
	Bones_Uniform_Buffer* Skeleton_Uniforms;

	std::vector<Bone> Bones;

	float Time = 0;

	void Update_Skeleton()
	{
		//Time += Tick;
		
		Skeleton_Uniforms->Bone_Matrix[0] = glm::mat4(1.0f);
		Bones[0].Calculate_Transformations(this, 0);
	}
};

void Bone::Calculate_Transformations(Mesh_Animator* Animator, unsigned char Parent_Index)
{
	unsigned char Keyframe_Index = 0;
	float Time = 0;
	if (Durations.size() == 0)
	{
		Animator->Skeleton_Uniforms->Bone_Matrix[Index] = Animator->Skeleton_Uniforms->Bone_Matrix[Parent_Index];
		return;
	}

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

		Animator->Skeleton_Uniforms->Bone_Matrix[Index] =
			glm::translate(Current_Rotation.Get_Rotation_Matrix(), Current_Position) *
			Animator->Skeleton_Uniforms->Bone_Matrix[Parent_Index];

		// This sets our bone's transformation matrix
	}

	for (size_t W = 0; W < Child_Indices.size(); W++)
		Animator->Bones[Child_Indices[W]].Calculate_Transformations(Animator, Index);

	// This recursively handles all of the bone transformations in a way that's still nice on the heap's memory
}

void Recursively_Add_Bones(aiNode* Node, Mesh_Animator* Target_Animator, std::unordered_map<std::string, size_t>& Bone_Indices)
{
	size_t Bone_Index = Target_Animator->Bones.size();
	
	//if (Bone_Indices.find(Node->mName.C_Str()) == Bone_Indices.end())	// This node is no good..
	//	return;
	
	Bone_Indices[Node->mName.C_Str()] = Bone_Index;
	Target_Animator->Bones.push_back(Bone());
	Target_Animator->Bones[Bone_Index].Index = Bone_Index;
	Target_Animator->Bones[Bone_Index].Child_Indices.resize(Node->mNumChildren);

	for (size_t W = 0; W < Node->mNumChildren; W++)
	{
		Target_Animator->Bones[Bone_Index].Child_Indices[W] = Target_Animator->Bones.size();
		Recursively_Add_Bones(Node->mChildren[W], Target_Animator, Bone_Indices);
	}
}

void Load_Mesh_Animator_Fbx(const char* File_Name, Mesh_Animator* Target_Animator)
{
	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(File_Name, (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_PopulateArmatureData));

	if (Scene == nullptr)
		Throw_Error(" >> Failed to load FBX file!\n");

	std::unordered_map<std::string, size_t> Bone_Indices;

	Recursively_Add_Bones(Scene->mMeshes[0]->mBones[0]->mArmature->mChildren[0], Target_Animator, Bone_Indices);

	for (size_t W = 0; W < Scene->mNumAnimations; W++)
	{
		for (size_t V = 0; V < Scene->mAnimations[W]->mNumChannels; V++)
		{
			aiNodeAnim* Node = Scene->mAnimations[W]->mChannels[V];

			Bone* Bone = &Target_Animator->Bones[Bone_Indices[Node->mNodeName.C_Str()]];
			Bone->Durations.resize(Node->mNumRotationKeys);
			Bone->Transformation.resize(Node->mNumRotationKeys);
			Bone->Rotations.resize(Node->mNumRotationKeys);
			for (size_t U = 0; U < Bone->Durations.size(); U++)
			{
				Bone->Durations[U] = Node->mPositionKeys[U].mTime;
				Bone->Transformation[U] = glm::vec3(Node->mPositionKeys[U].mValue.x, Node->mPositionKeys[U].mValue.y, Node->mPositionKeys[U].mValue.z);

				Bone->Rotations[U] = Quaternion::Quaternion(Node->mRotationKeys[U].mValue.w, Node->mRotationKeys[U].mValue.x, Node->mRotationKeys[U].mValue.y, Node->mRotationKeys[U].mValue.z);
			}
			// The rotation keys are some of the most important for skeletal animation,
			// but position keys are way more versatile

			// However I may need different code to handle this
		}
	}
	
	//for (size_t V = 0; V < Scene->mMeshes[0]->mNumBones; V++)
		//Bone_Indices[std::string(Scene->mMeshes[0]->mBones[V]->mName.C_Str())] = V;
}

#endif