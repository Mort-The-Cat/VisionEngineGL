#ifndef MESH_ANIMATOR_DECLARATIONS
#define MESH_ANIMATOR_DECLARATIONS

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Quaternion.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#define NUMBER_OF_ANIMATOR_BONES 16

glm::mat4 Assimp_Matrix_To_Mat4(aiMatrix4x4 Matrix)
{
	return glm::mat4(
		Matrix.a1, Matrix.b1, Matrix.c1, Matrix.d1,
		Matrix.a2, Matrix.b2, Matrix.c2, Matrix.d2,
		Matrix.a3, Matrix.b3, Matrix.c3, Matrix.d3,
		Matrix.a4, Matrix.b4, Matrix.c4, Matrix.d4);

	return glm::mat4(
		Matrix.a1, Matrix.c1, Matrix.b1, Matrix.d1,
		Matrix.a3, Matrix.c3, Matrix.b3, Matrix.d3,
		Matrix.a2, Matrix.c2, Matrix.b2, Matrix.d2,
		Matrix.a4, Matrix.c4, Matrix.b4, Matrix.d4);

	//return glm::mat4(
	//	Matrix.d4, Matrix.c4, Matrix.b4, Matrix.a4,
	//	Matrix.d3, Matrix.c3, Matrix.b3, Matrix.a3,
	//	Matrix.d2, Matrix.c2, Matrix.b2, Matrix.a2,
	//	Matrix.d1, Matrix.c1, Matrix.b1, Matrix.a1);

	/*return glm::mat4(
		Matrix.a1, Matrix.a2, Matrix.a3, Matrix.a4,
		Matrix.b1, Matrix.b2, Matrix.b3, Matrix.b4,
		Matrix.c1, Matrix.c2, Matrix.c3, Matrix.c4,
		Matrix.d1, Matrix.d2, Matrix.d3, Matrix.d4
	);*/
}

struct Bones_Uniform_Buffer
{
	glm::mat4 Bone_Matrix[NUMBER_OF_ANIMATOR_BONES]; // 256 'words' for the GPU
	glm::vec3 Bone_Origins[NUMBER_OF_ANIMATOR_BONES]; // 48 'words' for the GPU
};

class Mesh_Animator;

class Bone
{
public:
	std::vector <Quaternion::Quaternion> Rotations;
	std::vector<glm::vec3> Transformation;

	// std::vector<glm::vec3> Scaling; (This'll probably remain unused but I can implement it properly later.)

	std::vector<float> Durations; // Iterate through these when applying animation transformations to find the transformation in question and apply it accordingly

	std::vector<unsigned char> Child_Indices;

	unsigned char Index;

	glm::mat4 Offset_Matrix;

	void Calculate_Transformations(Mesh_Animator* Animator, unsigned char Parent_Index);

	void Calculate_Transformation(Mesh_Animator* Animator);
};

#define ANIMF_TO_BE_DELETED 0u
#define ANIMF_LOOP 1u

class Mesh_Animator
{
public:
	Bones_Uniform_Buffer* Skeleton_Uniforms;

	std::vector<Bone> Bones;

	float Duration = 0;

	float Time = 0;

	void Update_Skeleton()
	{
		Time += Tick;

		if (Time > Duration)
			Time = 0;

		for (size_t W = 0; W < Bones.size(); W++)
			Bones[W].Calculate_Transformation(this);
	}
};

void Bone::Calculate_Transformation(Mesh_Animator* Animator)
{
	uint16_t Keyframe_Index = 0;
	float Time = 0;

	while (Animator->Time >= Durations[Keyframe_Index] && Keyframe_Index < Durations.size() - 1)
		Keyframe_Index++;

	Time = Animator->Time - Durations[Keyframe_Index];

	{
		float Scalar = Time / (Durations[Keyframe_Index] - Durations[Keyframe_Index - 1]); // This normalises the time for this current keyframe between 0-1

		Quaternion::Quaternion Current_Rotation = Quaternion::Sphere_Interpolate(Rotations[Keyframe_Index - 1], Rotations[Keyframe_Index], Scalar);

		Current_Rotation.Normalise();

		// This interpolates the rotation

		glm::vec3 Current_Position = Transformation[Keyframe_Index] * (Scalar)+Transformation[Keyframe_Index - 1] * (1.0f - Scalar);

		Animator->Skeleton_Uniforms->Bone_Matrix[Index] = (glm::rotate(Current_Rotation.Get_Rotation_Matrix(), 0.0f * 3.14159f, glm::vec3(0, 1, 0)) * Offset_Matrix);
		Animator->Skeleton_Uniforms->Bone_Matrix[Index][3] = glm::vec4(Current_Position, 1.0f);

		// This sets our bone's transformation matrix
	}
}

aiBone* Get_Bone(aiBone** Bones, size_t Length, std::string Name)
{
	for (size_t W = 0; W < Length; W++)
		if (std::string(Bones[W]->mName.C_Str()) == Name)
			return Bones[W];
	return nullptr;
}

void Load_Mesh_Animator_Fbx(const char* File_Name, Mesh_Animator* Target_Animator)
{
	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(File_Name, (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_PopulateArmatureData));

	if (Scene == nullptr)
		Throw_Error(" >> Failed to load FBX file!\n");

	std::unordered_map<std::string, size_t> Bone_Indices;

	for (size_t W = 0; W < Scene->mMeshes[0]->mNumBones; W++)
	{
		Bone_Indices[Scene->mMeshes[0]->mBones[W]->mName.C_Str()] = W;

		Target_Animator->Bones.push_back(Bone());
		Target_Animator->Bones[W].Index = Target_Animator->Bones.size() - 1;
	}

	for (size_t W = 0; W < 1; W++)
	{
		Target_Animator->Duration = Scene->mAnimations[W]->mDuration / Scene->mAnimations[W]->mTicksPerSecond;

		for (size_t V = 1; V < Scene->mAnimations[W]->mNumChannels; V++)
		{
			aiNodeAnim* Node = Scene->mAnimations[W]->mChannels[V];

			Bone* Bone = &Target_Animator->Bones[Bone_Indices[Node->mNodeName.C_Str()]];

			aiBone* Bone_Assimp = Get_Bone(Scene->mMeshes[0]->mBones, Scene->mMeshes[0]->mNumBones, Node->mNodeName.C_Str());

			Bone->Durations.resize(Node->mNumRotationKeys);
			Bone->Rotations.resize(Node->mNumRotationKeys);
			Bone->Transformation.resize(Node->mNumRotationKeys);

			Bone->Offset_Matrix = glm::inverse(Assimp_Matrix_To_Mat4(Bone_Assimp->mOffsetMatrix));

			aiVector3D Translation;
			aiQuaternion Rotation;
			Bone_Assimp->mOffsetMatrix.DecomposeNoScaling(Rotation, Translation);

			Bone->Offset_Matrix[3] = glm::vec4(0, 0, 0, 1);

			Target_Animator->Skeleton_Uniforms->Bone_Origins[Bone->Index] = glm::vec4(Translation.x, Translation.y, Translation.z, 0);



			Bone->Offset_Matrix = Bone->Offset_Matrix * Quaternion::Quaternion(Node->mRotationKeys[0].mValue.w, Node->mRotationKeys[0].mValue.x, Node->mRotationKeys[0].mValue.y, Node->mRotationKeys[0].mValue.z).Get_Rotation_Matrix();


			Target_Animator->Skeleton_Uniforms->Bone_Origins[Bone->Index] += glm::vec3(glm::vec4(Node->mPositionKeys[0].mValue.x, Node->mPositionKeys[0].mValue.y, Node->mPositionKeys[0].mValue.z, 1.0f) * Bone->Offset_Matrix);


			for (size_t U = 0; U < Scene->mAnimations[W]->mChannels[V]->mNumRotationKeys; U++)
			{
				Bone->Durations[U] = Node->mPositionKeys[U].mTime / Scene->mAnimations[0]->mTicksPerSecond;;

				Bone->Transformation[U] = glm::vec4(Node->mPositionKeys[U].mValue.x, Node->mPositionKeys[U].mValue.y, Node->mPositionKeys[U].mValue.z, 1.0f) * Bone->Offset_Matrix;

				Bone->Rotations[U] = Quaternion::Quaternion(Node->mRotationKeys[U].mValue.w, Node->mRotationKeys[U].mValue.x, Node->mRotationKeys[U].mValue.z, Node->mRotationKeys[U].mValue.y);
			}
		}
	}

	Importer.FreeScene();
}














// Old code

void Bone::Calculate_Transformations(Mesh_Animator* Animator, unsigned char Parent_Index)
{
	unsigned char Keyframe_Index = 0;
	float Time = 0;
	if (Durations.size() == 0)
	{
		Animator->Skeleton_Uniforms->Bone_Matrix[Index] = glm::mat4(1.0f); // Offset_Matrix; // (Animator->Skeleton_Uniforms->Bone_Matrix[Parent_Index]);// *Offset_Matrix;
		return;
	}

	while (Animator->Time >= Durations[Keyframe_Index] && Keyframe_Index < Durations.size())
		Keyframe_Index++;

	Time = Durations[Keyframe_Index] - Animator->Time;

	{
		float Scalar = Time / (Durations[Keyframe_Index] - Durations[Keyframe_Index - 1]); // This normalises the time for this current keyframe between 0-1

		Quaternion::Quaternion Current_Rotation = Quaternion::Sphere_Interpolate(Rotations[Keyframe_Index - 1], Rotations[Keyframe_Index], Scalar);

		Current_Rotation.Normalise();

		// This interpolates the rotation

		glm::vec3 Current_Position = Transformation[Keyframe_Index - 1] * (Scalar)+Transformation[Keyframe_Index] * (1.0f - Scalar);

		Animator->Skeleton_Uniforms->Bone_Matrix[Index] = (glm::rotate(Current_Rotation.Get_Rotation_Matrix(), 3.14159f, glm::vec3(0, 1, 0)) * Offset_Matrix);
		Animator->Skeleton_Uniforms->Bone_Matrix[Index][3] = glm::vec4(Current_Position, 1.0f);

		// This sets our bone's transformation matrix
	}
	// This recursively handles all of the bone transformations in a way that's still nice on the heap's memory
}


void Recursively_Add_Bones(aiNode* Node, Mesh_Animator* Target_Animator, std::unordered_map<std::string, size_t>& Bone_Indices, size_t Parent_Index)
{
	size_t Bone_Index = Target_Animator->Bones.size();

	Bone_Indices[Node->mName.C_Str()] = Bone_Index;
	Target_Animator->Bones.push_back(Bone());
	Target_Animator->Bones[Bone_Index].Index = Bone_Index;
	Target_Animator->Bones[Bone_Index].Child_Indices.resize(Node->mNumChildren);

	for (size_t W = 0; W < Node->mNumChildren; W++)
	{
		Target_Animator->Bones[Bone_Index].Child_Indices[W] = Target_Animator->Bones.size();
		Recursively_Add_Bones(Node->mChildren[W], Target_Animator, Bone_Indices, Bone_Index);
	}
}

void Load_Mesh_Animator_Fbx_Old(const char* File_Name, Mesh_Animator* Target_Animator)
{
	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(File_Name, (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_PopulateArmatureData));

	if (Scene == nullptr)
		Throw_Error(" >> Failed to load FBX file!\n");

	std::unordered_map<std::string, size_t> Bone_Indices;

	Target_Animator->Skeleton_Uniforms->Bone_Origins[0] = glm::vec3(0.0f);

	Recursively_Add_Bones(Scene->mMeshes[0]->mBones[0]->mArmature->mChildren[0], Target_Animator, Bone_Indices, 0);

	for (size_t W = 0; W < Scene->mNumAnimations; W++)
	{
		for (size_t V = 1; V < Scene->mAnimations[W]->mNumChannels; V++)
		{
			aiNodeAnim* Node = Scene->mAnimations[W]->mChannels[V];

			Bone* Bone = &Target_Animator->Bones[Bone_Indices[Node->mNodeName.C_Str()]];

			aiBone* Bone_Assimp = Get_Bone(Scene->mMeshes[0]->mBones, Scene->mMeshes[0]->mNumBones, Node->mNodeName.C_Str());

			Bone->Durations.resize(Node->mNumRotationKeys);
			Bone->Transformation.resize(Node->mNumRotationKeys);
			Bone->Rotations.resize(Node->mNumRotationKeys);

			Bone->Offset_Matrix = Assimp_Matrix_To_Mat4(Bone_Assimp->mOffsetMatrix);

			aiVector3D Translation;
			aiQuaternion Rotation;
			Bone_Assimp->mOffsetMatrix.DecomposeNoScaling(Rotation, Translation);

			Bone->Offset_Matrix[3] = glm::vec4(0, 0, 0, 1);

			Target_Animator->Skeleton_Uniforms->Bone_Origins[Bone->Index] = glm::vec4(Translation.x, Translation.y, Translation.z, 0.0f); // *Bone->Offset_Matrix;

			for (size_t U = 0; U < Node->mNumRotationKeys; U++)
			{
				Bone->Durations[U] = Node->mPositionKeys[U].mTime / Scene->mAnimations[0]->mTicksPerSecond;
				Bone->Transformation[U] = glm::vec4(Node->mPositionKeys[U].mValue.x, Node->mPositionKeys[U].mValue.y, Node->mPositionKeys[U].mValue.z, 1.0f) * Bone->Offset_Matrix;

				Bone->Rotations[U] = Quaternion::Quaternion(-Node->mRotationKeys[U].mValue.w, Node->mRotationKeys[U].mValue.x, Node->mRotationKeys[U].mValue.z, Node->mRotationKeys[U].mValue.y);
			}
			// The rotation keys are some of the most important for skeletal animation,
			// but position keys are way more versatile

			// However I may need different code to handle this
		}
	}

	Importer.FreeScene();
}

#endif