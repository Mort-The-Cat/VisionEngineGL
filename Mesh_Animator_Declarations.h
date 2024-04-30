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

		// Current_Rotation = Quaternion::Quaternion(1.0f, 0.0f, 0.0f, 0.0f);

		Current_Rotation.Normalise();

		// This interpolates the rotation

		glm::vec3 Current_Position = Transformation[Keyframe_Index] * (Scalar)+Transformation[Keyframe_Index - 1] * (1.0f - Scalar);

		Animator->Skeleton_Uniforms->Bone_Matrix[Index] = (glm::rotate(Current_Rotation.Get_Rotation_Matrix(), 0.0f, glm::vec3(0, 1, 0)) * Offset_Matrix);
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

	//

	Target_Animator->Duration = Scene->mAnimations[0]->mDuration / Scene->mAnimations[0]->mTicksPerSecond;

	for (size_t W = 1; W < Scene->mAnimations[0]->mNumChannels; W++)
	{
		aiNodeAnim* Node = Scene->mAnimations[0]->mChannels[W];

		aiBone* Bone_Info = Get_Bone(Scene->mMeshes[0]->mBones, Scene->mMeshes[0]->mNumBones, Node->mNodeName.C_Str());

		//if (Bone_Info != nullptr)
		//	continue;

		Bone* Bone = &Target_Animator->Bones[Bone_Indices[Node->mNodeName.C_Str()]];

		Bone->Durations.resize(Node->mNumRotationKeys);
		Bone->Rotations.resize(Node->mNumRotationKeys);
		Bone->Transformation.resize(Node->mNumRotationKeys);

		Bone->Offset_Matrix = Assimp_Matrix_To_Mat4(Bone_Info->mOffsetMatrix);

		aiVector3D Matrix_Translation, Matrix_Scaling;
		aiQuaternion Matrix_Rotation;
		Bone_Info->mOffsetMatrix.Inverse().Decompose(Matrix_Scaling, Matrix_Rotation, Matrix_Translation);

		glm::vec3 Scaling = glm::vec3(
			Matrix_Scaling.x,
			Matrix_Scaling.y,
			Matrix_Scaling.z
		);

		//

		aiVector3D Transform_Translation, Transform_Scaling;
		aiQuaternion Transform_Rotation;
		Bone_Info->mArmature->mTransformation.Decompose(Transform_Scaling, Transform_Rotation, Transform_Translation);

		// Matrix_Rotation.w *= -1;

		Matrix_Translation = Matrix_Rotation.Rotate(Matrix_Translation);

		// Matrix_Translation.x /= Scaling.x;
		// Matrix_Translation.y /= Scaling.y;
		// Matrix_Translation.z /= Scaling.z;

		// Transform_Translation = Transform_Rotation.Rotate(Transform_Translation);

		Transform_Translation = Transform_Rotation.Rotate(Transform_Translation);

		glm::vec3 Translation = glm::vec3(
			Transform_Translation.x / Transform_Scaling.x,
			-Transform_Translation.y / Transform_Scaling.y,
			-Transform_Translation.z / Transform_Scaling.z);

		//

		Bone->Offset_Matrix = glm::scale(Bone->Offset_Matrix, Scaling); // We reverse all of the scaling because we don't want the scaling to affect the model at all

		Translation *= Scaling;

		//Target_Animator->Skeleton_Uniforms->Bone_Origins[Bone->Index] = glm::vec3(0, 0, 0); // I also won't worry about these rn

		Target_Animator->Skeleton_Uniforms->Bone_Origins[Bone->Index] = glm::vec4(Node->mPositionKeys[0].mValue.x, Node->mPositionKeys[0].mValue.y, -Node->mPositionKeys[0].mValue.z, 1.0f) * Bone->Offset_Matrix;

		Target_Animator->Skeleton_Uniforms->Bone_Origins[Bone->Index] *= Scaling;

		Target_Animator->Skeleton_Uniforms->Bone_Origins[Bone->Index] += glm::vec3(Matrix_Translation.x, Matrix_Translation.y, Matrix_Translation.z);

		Target_Animator->Skeleton_Uniforms->Bone_Origins[Bone->Index] += Translation;

		// Matrix_Translation = Matrix_Rotation.Rotate()


		for (size_t V = 0; V < Node->mNumRotationKeys; V++)
		{
			Bone->Transformation[V] = glm::vec4(Node->mPositionKeys[V].mValue.x, Node->mPositionKeys[V].mValue.y, -Node->mPositionKeys[V].mValue.z, 1.0f) * Bone->Offset_Matrix;
			
			Bone->Transformation[V] *= Scaling; // We don't really want the scaling to affect anything

			//Bone->Transformation[V] += glm::vec3(Matrix_Translation.x, Matrix_Translation.y, Matrix_Translation.z);

			//Bone->Transformation[V] += glm::vec3(glm::vec4(Translation, 1.0f)); // *Bone->Offset_Matrix); //Scaling * Translation;

			Bone->Rotations[V] = Quaternion::Quaternion(1, 0, 0, 0); // I won't worry about this rn

			Bone->Rotations[V] = Quaternion::Quaternion(-Node->mRotationKeys[V].mValue.w, Node->mRotationKeys[V].mValue.x, Node->mRotationKeys[V].mValue.y, Node->mRotationKeys[V].mValue.z);

			Bone->Durations[V] = Node->mRotationKeys[V].mTime / Scene->mAnimations[0]->mTicksPerSecond;
		}

		Bone->Offset_Matrix[3] = glm::vec4(0, 0, 0, 1);
	}

	Importer.FreeScene();
}











void Load_Mesh_Animator_Fbx_Old(const char* File_Name, Mesh_Animator* Target_Animator)
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

			Bone->Offset_Matrix = (Assimp_Matrix_To_Mat4(Bone_Assimp->mOffsetMatrix));
			Bone->Offset_Matrix[3] = glm::vec4(0, 0, 0, 1);

			aiVector3D Translation;
			aiVector3D Scaling;
			aiQuaternion Rotation;
			
			//Bone_Assimp->mOffsetMatrix.Decompose(Scaling, Rotation, Translation);

			// Bone_Assimp->mArmature->mTransformation.Decompose(Scaling, Rotation, Translation);

			glm::vec3 Scaling_Correction;

			// Target_Animator->Skeleton_Uniforms->Bone_Origins[Bone->Index] = glm::vec4(Translation.x, Translation.y, Translation.z, 1.0f);

			Bone_Assimp->mArmature->mTransformation.Decompose(Scaling, Rotation, Translation);

			// Translation = Rotation.Rotate(Translation);

			Scaling_Correction = glm::vec3(
				1.0f / Scaling.x,
				1.0f / Scaling.y,
				1.0f / Scaling.z);

			Target_Animator->Skeleton_Uniforms->Bone_Origins[Bone->Index] -= Scaling_Correction * glm::vec3(Translation.x, Translation.y, Translation.z);

			Target_Animator->Skeleton_Uniforms->Bone_Origins[Bone->Index] += glm::vec3(glm::vec4(Node->mPositionKeys[0].mValue.x, Node->mPositionKeys[0].mValue.y, Node->mPositionKeys[0].mValue.z, 1.0f));// *Bone->Offset_Matrix);




			for (size_t U = 0; U < Scene->mAnimations[W]->mChannels[V]->mNumRotationKeys; U++)
			{
				Bone->Durations[U] = Node->mPositionKeys[U].mTime / Scene->mAnimations[0]->mTicksPerSecond;

				Bone->Transformation[U] = glm::vec4(Node->mPositionKeys[U].mValue.x, Node->mPositionKeys[U].mValue.y, Node->mPositionKeys[U].mValue.z, 1.0f);// *Bone->Offset_Matrix;
				// Bone->Transformation[U] *= Scaling_Correction;

				Bone->Rotations[U] = Quaternion::Quaternion(-Node->mRotationKeys[U].mValue.w, Node->mRotationKeys[U].mValue.x, Node->mRotationKeys[U].mValue.y, Node->mRotationKeys[U].mValue.z);
			}

			Bone_Assimp->mOffsetMatrix.Decompose(Scaling, Rotation, Translation);
			Bone->Offset_Matrix = (Assimp_Matrix_To_Mat4(aiMatrix4x4(Rotation.Normalize().GetMatrix())));

			Bone->Offset_Matrix[3] = glm::vec4(0, 0, 0, 1);

			// Bone->Offset_Matrix = Bone->Offset_Matrix * Quaternion::Quaternion(Node->mRotationKeys[0].mValue.w, Node->mRotationKeys[0].mValue.x, Node->mRotationKeys[0].mValue.y, Node->mRotationKeys[0].mValue.z).Get_Rotation_Matrix();
		}
	}

	Importer.FreeScene();
}

#endif