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
	//return glm::mat4(
	//	Matrix.a1, Matrix.b1, Matrix.c1, Matrix.d1,
	//	Matrix.a2, Matrix.b2, Matrix.c2, Matrix.d2,
	//	Matrix.a3, Matrix.b3, Matrix.c3, Matrix.d3,
	//	Matrix.a4, Matrix.b4, Matrix.c4, Matrix.d4);

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
};

struct Keyframe_Translation
{
	glm::vec3 Translation;
	float Time;
};

struct Keyframe_Rotation
{
	Quaternion::Quaternion Rotation;
	float Time;
};

struct Keyframe_Scaling
{
	glm::vec3 Scale;
	float Time;
};

struct Node_Data
{
	glm::mat4 Transformation;
	std::string Name;
	std::vector<Node_Data> Children;
};

class Mesh_Animator;

struct Bone_Info
{
	int Index;
	glm::mat4 Offset;

	std::string Name;
};

class Bone
{
public:
	std::vector<Keyframe_Translation> Translations;
	std::vector<Keyframe_Rotation> Rotations;
	std::vector<Keyframe_Scaling> Scalings;

	unsigned char Index;

	std::string Name;

	glm::mat4 Local_Matrix;

	void Update(float Time)
	{
		size_t Keyframe_Index = 0;
		while (Translations[Keyframe_Index].Time < Time)
			Keyframe_Index++;

		float Time_Length = Translations[Keyframe_Index].Time - Translations[Keyframe_Index - 1].Time;
		float Factor = Time - Translations[Keyframe_Index - 1].Time;
		Factor /= Time_Length;

		glm::vec3 Translation = Translations[Keyframe_Index].Translation * Factor + Translations[Keyframe_Index - 1].Translation * (1.0f - Factor);

		//

		Keyframe_Index = 0;
		while (Rotations[Keyframe_Index].Time < Time)
			Keyframe_Index++;

		Time_Length = Rotations[Keyframe_Index].Time - Rotations[Keyframe_Index - 1].Time;
		Factor = Time - Rotations[Keyframe_Index - 1].Time;
		Factor /= Time_Length;

		Quaternion::Quaternion Rotation = Quaternion::Sphere_Interpolate(Rotations[Keyframe_Index - 1].Rotation, Rotations[Keyframe_Index].Rotation, Factor);

		//

		Keyframe_Index = 0;
		while (Scalings[Keyframe_Index].Time < Time)
			Keyframe_Index++;

		Time_Length = Scalings[Keyframe_Index].Time - Scalings[Keyframe_Index - 1].Time;
		Factor = Time - Scalings[Keyframe_Index - 1].Time;
		Factor /= Time_Length;

		glm::vec3 Scale = Scalings[Keyframe_Index].Scale * Factor + Scalings[Keyframe_Index - 1].Scale * (1.0f - Factor);

		Local_Matrix = glm::translate(glm::mat4(1.0f), Translation) * Rotation.Get_Rotation_Matrix() * glm::scale(glm::mat4(1.0f), Scale);
	}

	Bone(const std::string& Namep, unsigned char Indexp)
	{
		Index = Indexp;
		Name = Namep;
		Local_Matrix = glm::mat4(1.0f);
	}

	Bone(const std::string& Namep, unsigned char Indexp, const aiNodeAnim* Channel)
	{
		Translations.resize(Channel->mNumPositionKeys);
		for (size_t W = 0; W < Translations.size(); W++)
		{
			Translations[W].Translation.x = Channel->mPositionKeys[W].mValue.x;
			Translations[W].Translation.y = Channel->mPositionKeys[W].mValue.y;
			Translations[W].Translation.z = Channel->mPositionKeys[W].mValue.z;
			Translations[W].Time = Channel->mPositionKeys[W].mTime;
		}

		Rotations.resize(Channel->mNumRotationKeys);
		for (size_t W = 0; W < Rotations.size(); W++)
		{
			Rotations[W].Rotation.W = Channel->mRotationKeys[W].mValue.w;
			Rotations[W].Rotation.X = Channel->mRotationKeys[W].mValue.x;
			Rotations[W].Rotation.Y = Channel->mRotationKeys[W].mValue.y;
			Rotations[W].Rotation.Z = Channel->mRotationKeys[W].mValue.z;

			Rotations[W].Time = Channel->mRotationKeys[W].mTime;
		}

		Scalings.resize(Channel->mNumScalingKeys);
		for (size_t W = 0; W < Scalings.size(); W++)
		{
			Scalings[W].Scale.x = Channel->mScalingKeys[W].mValue.x;
			Scalings[W].Scale.y = Channel->mScalingKeys[W].mValue.y;
			Scalings[W].Scale.z = Channel->mScalingKeys[W].mValue.z;

			Scalings[W].Time = Channel->mScalingKeys[W].mTime;
		}

		Index = Indexp;
		Name = Namep;
		Local_Matrix = glm::mat4(1.0f);
	}
};

#define ANIMF_TO_BE_DELETED 0u
#define ANIMF_LOOP 1u

aiBone* Get_Bone(aiBone** Bones, size_t Length, std::string Name)
{
	for (size_t W = 0; W < Length; W++)
		if (std::string(Bones[W]->mName.C_Str()) == Name)
			return Bones[W];
	return nullptr;
}

class Mesh_Animator
{
public:
	Bones_Uniform_Buffer* Skeleton_Uniforms;

	std::vector<Bone> Bones;

	Node_Data Root_Node;

	std::map<std::string, Bone_Info> Bone_Info_Map;

	float Duration = 0;

	float Time = 0;

	Bone* Find_Bone(const std::string& Name)
	{
		for (size_t W = 0; W < Bones.size(); W++)
			if (strcmp(Name.c_str(), Bones[W].Name.c_str()) == 0)
				return &Bones[W];

		return nullptr;
	}

	void Calculate_Bone_Matrix(const Node_Data* Node, glm::mat4 Parent_Matrix)
	{
		Bone* Bone = Find_Bone(Node->Name);

		glm::mat4 Node_Matrix;

		if (Bone)
		{
			Bone->Update(Time);
			Node_Matrix = Bone->Local_Matrix;
		}
		else
			Node_Matrix = glm::mat4(1.0f);

		glm::mat4 Global_Matrix = Parent_Matrix * Node_Matrix;

		if (Bone_Info_Map.find(Node->Name) != Bone_Info_Map.end())
			Skeleton_Uniforms->Bone_Matrix[Bone_Info_Map[Node->Name].Index] = Global_Matrix * Bone_Info_Map[Node->Name].Offset;

		for (size_t W = 0; W < Node->Children.size(); W++)
			Calculate_Bone_Matrix(&Node->Children[W], Global_Matrix);
	}

	void Update_Skeleton()
	{
		Time += Tick;

		if (Time > Duration)
			Time = 0;

		Calculate_Bone_Matrix(&Root_Node, glm::mat4(1.0f));
	}
};

// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

// https://learnopengl.com/code_viewer_gh.php?code=src/8.guest/2020/skeletal_animation/skeletal_animation.cpp

void Animator_Read_Hierarchy_Data(Node_Data* Target_Node_Data, const aiNode* Source_Node)
{
	Target_Node_Data->Name = Source_Node->mName.C_Str();
	Target_Node_Data->Transformation = Assimp_Matrix_To_Mat4(Source_Node->mTransformation);
	Target_Node_Data->Children.resize(Source_Node->mNumChildren);

	for (size_t W = 0; W < Source_Node->mNumChildren; W++)
	{
		Node_Data Data;
		Animator_Read_Hierarchy_Data(&Data, Source_Node->mChildren[W]);
		Target_Node_Data->Children[W] = Data;
	}
}

void Load_Mesh_Animator_Fbx(const char* File_Name, Mesh_Animator* Target_Animator)
{
	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(File_Name, (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_PopulateArmatureData));
	
	if (Scene == nullptr)
		Throw_Error(" >> Failed to load FBX file!\n");

	{
		auto Animation = Scene->mAnimations[0];
		Target_Animator->Duration = Animation->mDuration / Animation->mTicksPerSecond;
		Animator_Read_Hierarchy_Data(&Target_Animator->Root_Node, Scene->mRootNode);

		size_t Bone_Count = 0;

		for (size_t W = 0; W < Scene->mMeshes[0]->mNumBones; W++)
		{
			auto Channel = Animation->mChannels[W];
			std::string Name = Channel->mNodeName.C_Str();

			Target_Animator->Bone_Info_Map[Name].Index = Bone_Count;
			Target_Animator->Bone_Info_Map[Name].Offset = Assimp_Matrix_To_Mat4(Scene->mMeshes[0]->mBones[W]->mOffsetMatrix);
			Bone_Count++;
		}

		for (size_t W = 0; W < Animation->mNumChannels; W++)
		{
			auto Channel = Animation->mChannels[W];
			std::string Name = Channel->mNodeName.C_Str();

			if (Target_Animator->Bone_Info_Map.find(Name) != Target_Animator->Bone_Info_Map.end()) // This adds all of the bones that weren't defined in the scene mesh such as the root node
			{
				Target_Animator->Bone_Info_Map[Name].Index = Bone_Count;
				Bone_Count++;
			}

			Target_Animator->Bones.push_back(Bone(Name, Bone_Count, Channel)); // Only the bones in the animation channels have any animations
		}
	}

	//

	Target_Animator->Duration = Scene->mAnimations[0]->mDuration / Scene->mAnimations[0]->mTicksPerSecond;

	

	Importer.FreeScene();
}

#endif