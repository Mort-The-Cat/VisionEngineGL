#ifndef MESH_ANIMATOR_DECLARATIONS
#define MESH_ANIMATOR_DECLARATIONS

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Quaternion.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

glm::mat4 Assimp_Matrix_To_Mat4(aiMatrix4x4 Matrix)
{
	return glm::mat4(
		Matrix.a1, Matrix.b1, Matrix.c1, Matrix.d1,
		Matrix.a2, Matrix.b2, Matrix.c2, Matrix.d2,
		Matrix.a3, Matrix.b3, Matrix.c3, Matrix.d3,
		Matrix.a4, Matrix.b4, Matrix.c4, Matrix.d4);

	//return glm::mat4(
	//	Matrix.a1, Matrix.c1, Matrix.b1, Matrix.d1,
	//	Matrix.a3, Matrix.c3, Matrix.b3, Matrix.d3,
	//	Matrix.a2, Matrix.c2, Matrix.b2, Matrix.d2,
	//	Matrix.a4, Matrix.c4, Matrix.b4, Matrix.d4);

	//return glm::mat4(
	//	Matrix.d4, Matrix.c4, Matrix.b4, Matrix.a4,
	//	Matrix.d3, Matrix.c3, Matrix.b3, Matrix.a3,
	//	Matrix.d2, Matrix.c2, Matrix.b2, Matrix.a2,
	//	Matrix.d1, Matrix.c1, Matrix.b1, Matrix.a1);

	return glm::mat4(
		Matrix.a1, Matrix.a2, Matrix.a3, Matrix.a4,
		Matrix.b1, Matrix.b2, Matrix.b3, Matrix.b4,
		Matrix.c1, Matrix.c2, Matrix.c3, Matrix.c4,
		Matrix.d1, Matrix.d2, Matrix.d3, Matrix.d4
	);
}

//

struct Mesh_Animation
{
	float Duration, Tickrate;
	std::vector<std::vector<Model_Vertex>> Keyframes;
	std::vector<size_t> Indices;
};

#define ANIMF_TO_BE_DELETED 0u
#define ANIMF_LOOP_BIT 1u

class Mesh_Animator
{
public:
	float Time;

	bool Flags[2] = { false, true };

	const Mesh_Animation* Animation; // This will certainly be shared between multiple mesh_animators, so storing it in a cache for all to use is wise

	void Handle_Update(Model_Vertex_Buffer* Mesh)
	{
		size_t Keyframe_Index = Time * Animation->Tickrate; // Automatically rounds down during integre conversion

		float Time_Scalar = Time * Animation->Tickrate - ((float)Keyframe_Index);

		// Time_Scalar /= Animation->Tickrate;

		for (size_t W = 0; W < Animation->Keyframes[Animation->Indices[Keyframe_Index]].size(); W++)
		{
			Model_Vertex A, B;
			A = Animation->Keyframes[Animation->Indices[Keyframe_Index]][W];
			B = Animation->Keyframes[Animation->Indices[Keyframe_Index + 1]][W];

			Mesh->Mesh->Vertices[W].Position = A.Position * (1.0f - Time_Scalar) + B.Position * Time_Scalar;
			Mesh->Mesh->Vertices[W].Normal = glm::normalize(A.Normal * (1.0f - Time_Scalar) + B.Normal * Time_Scalar);
		}
	}

	void Update_Mesh(Model_Vertex_Buffer* Mesh)
	{
		// Since it gives a keyframe every tick, we can use a kind of array indexing to get the keyframe indices quickly

		Time += Tick;

		if (Time * Animation->Tickrate >= (Animation->Duration - 2) && Flags[ANIMF_LOOP_BIT])
			Time = 0;

		Handle_Update(Mesh);
	}
};

//

void Load_Animation_File(const char* Directory, Mesh_Animation* Animation)
{
	std::ifstream File(Directory);

	std::string Line;
	std::stringstream Stream;

	if (File.is_open())
	{
		std::getline(File, Line);
		Animation->Tickrate = std::stoi(Line);

		std::getline(File, Line);
		Animation->Duration = std::stoi(Line);

		for (size_t W = 0; W < Animation->Duration; W++)
		{
			std::getline(File, Line);
			Animation->Indices.push_back(std::stoi(Line));
		}

		while (std::getline(File, Line))
		{
			if (Line.length() == 0)
			{
				Animation->Keyframes.push_back(std::vector<Model_Vertex>(0));
				continue;
			}

			Stream.str(Line);

			Model_Vertex Vertex;

			Stream >> Vertex.Position.x >> Vertex.Position.y >> Vertex.Position.z >> Vertex.Normal.x >> Vertex.Normal.y >> Vertex.Normal.z;

			Animation->Keyframes.back().push_back(Vertex);

		}

		File.close();
	}
	else
		Throw_Error(" >> Unable to load animation file!\n");
}

#endif