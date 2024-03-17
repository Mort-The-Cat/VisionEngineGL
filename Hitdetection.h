#ifndef HITDETECTION_VISIONGL
#define HITDETECTION_VISIONGL

#include "Hitdetection_Declarations.h"

namespace Collision_Test
{
	Collision_Info Find_Collision(Hitbox* A, bool (*Should_Compare)(Hitbox*, Hitbox*), Hitbox** Target_Pointer)
	{
		Collision_Info Info;

		for (size_t W = 0; W < Scene_Hitboxes.size(); W++)
		{
			if (Should_Compare(A, Scene_Hitboxes[W])) // If we should compare these two hitboxes
			{
				Info = A->Hitdetection(Scene_Hitboxes[W]); // Compare them

				if (Info.Overlap != 0) // If there is an intersection of any kind
				{
					*Target_Pointer = Scene_Hitboxes[W]; // Set the pointer of the hitbox in question
					return Info;						// and return the collision info that we got
				}
			}
		}

		*Target_Pointer = nullptr;

		return Info;
	}

	bool Always_Compare(Hitbox* A, Hitbox* B)
	{
		return true;
	}

	Collision_Info Raycast(glm::vec3 Origin, glm::vec3 Velocity, size_t Max_Step, bool (*Should_Compare)(Hitbox*, Hitbox*), Hitbox** Target_Pointer) // A raycast only receives 1 hitbox
	{
		Sphere_Hitbox Particle;
		Particle.Radius = 0.1;
		Particle.Position = &Origin;

		size_t Step = 0;

		Collision_Info Info;

		do
		{
			*Particle.Position += Velocity;
			Step++;
			Info = Find_Collision(&Particle, Should_Compare, Target_Pointer);
		} while (Step < Max_Step && Info.Overlap == 0);

		return Info; // If we never found a collision, this will have already been initialised to zero anyways so its fine
	}

	Collision_Info Sphere_Against_Sphere(Sphere_Hitbox& Sphere, Sphere_Hitbox& Other)
	{
		glm::vec3 Overlap = *Sphere.Position - *Other.Position;

		float Length = sqrtf(glm::dot(Overlap, Overlap));

		if (Sphere.Radius + Other.Radius > Length)
		{
			float Inv_Length = 1.0f / Length;
			Overlap *= Inv_Length;

			return Collision_Info(*Other.Position + Overlap * Other.Radius, Overlap, Length - (Sphere.Radius + Other.Radius));
		}
		else
			return Collision_Info();
	}

	Collision_Info AABB_Against_AABB(AABB_Hitbox& AABB, AABB_Hitbox& Other)
	{
		float Positions[6][2] =
		{
			{ AABB.A.x + AABB.Position->x, Other.B.x + Other.Position->x },
			{ AABB.A.y + AABB.Position->y, Other.B.y + Other.Position->y },
			{ AABB.A.z + AABB.Position->z, Other.B.z + Other.Position->z },

			{ Other.A.x + Other.Position->x, AABB.B.x + AABB.Position->x },
			{ Other.A.y + Other.Position->y, AABB.B.y + AABB.Position->y },
			{ Other.A.z + Other.Position->z, AABB.B.z + AABB.Position->z }
		};

		float Delta_Overlaps[6] =
		{
			Positions[0][0] - Positions[0][1], // positive x (towards the initial caller of the "hitdetection" function)
			Positions[1][0] - Positions[1][1], // positive y
			Positions[2][0] - Positions[2][1], // positive z

			Positions[3][0] - Positions[3][1], // negative x
			Positions[4][0] - Positions[4][1], // negative y
			Positions[5][0] - Positions[5][1], // negative z
		};

		glm::vec3 Normals[6] =
		{
			glm::vec3(1, 0, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 0, 1),

			glm::vec3(-1, 0, 0),
			glm::vec3(0, -1, 0),
			glm::vec3(0, 0, -1)
		};

		uint8_t X_Index = Delta_Overlaps[0] > Delta_Overlaps[3] ? 0 : 3;
		uint8_t Y_Index = Delta_Overlaps[1] > Delta_Overlaps[4] ? 1 : 4;
		uint8_t Z_Index = Delta_Overlaps[2] > Delta_Overlaps[5] ? 2 : 5;

		bool X_Clip, Y_Clip, Z_Clip;

		X_Clip = Delta_Overlaps[X_Index] < 0; // < 0 && Delta_Overlaps[3] < 0;
		Y_Clip = Delta_Overlaps[Y_Index] < 0; // < 0 && Delta_Overlaps[4] < 0;
		Z_Clip = Delta_Overlaps[Z_Index] < 0; // < 0 && Delta_Overlaps[5] < 0;

		if ((!(X_Clip && Y_Clip && Z_Clip)) || &Other == &AABB)
			return Collision_Info();
		else
		{
			glm::vec3 Average_Position;

			Average_Position.x = (Positions[X_Index][0] + Positions[X_Index][1]) * 0.5f;
			Average_Position.y = (Positions[Y_Index][0] + Positions[Y_Index][1]) * 0.5f;
			Average_Position.z = (Positions[Z_Index][0] + Positions[Z_Index][1]) * 0.5f;

			uint8_t Index = Delta_Overlaps[X_Index] > Delta_Overlaps[Y_Index] ? X_Index : Y_Index;
			Index = Delta_Overlaps[Index] > Delta_Overlaps[Z_Index] ? Index : Z_Index;

			return Collision_Info(Average_Position, Normals[Index], Delta_Overlaps[Index]);
		}
	}

	Collision_Info AABB_Against_Sphere(AABB_Hitbox& AABB, Sphere_Hitbox& Sphere)
	{
		glm::vec3 AABB_Position;
		AABB_Position.x = std::fminf(std::fmaxf(Sphere.Position->x, AABB.A.x + AABB.Position->x), AABB.B.x + AABB.Position->x);
		AABB_Position.y = std::fminf(std::fmaxf(Sphere.Position->y, AABB.A.y + AABB.Position->y), AABB.B.y + AABB.Position->y);
		AABB_Position.z = std::fminf(std::fmaxf(Sphere.Position->z, AABB.A.z + AABB.Position->z), AABB.B.z + AABB.Position->z);

		glm::vec3 Overlap = AABB_Position - *Sphere.Position;

		if (Overlap == glm::vec3(0, 0, 0))
			//Overlap = *AABB.Position - *Sphere.Position;

		{
			float Positions[6][2] =
			{
				{ AABB.A.x + AABB.Position->x, Sphere.Position->x },
				{ AABB.A.y + AABB.Position->y, Sphere.Position->y },
				{ AABB.A.z + AABB.Position->z, Sphere.Position->z },

				{ Sphere.Position->x, AABB.B.x + AABB.Position->x },
				{ Sphere.Position->y, AABB.B.y + AABB.Position->y },
				{ Sphere.Position->z, AABB.B.z + AABB.Position->z }
			};

			float Delta_Overlaps[6] =
			{
				Positions[0][0] - Positions[0][1] - Sphere.Radius, // positive x (towards the initial caller of the "hitdetection" function)
				Positions[1][0] - Positions[1][1] - Sphere.Radius, // positive y
				Positions[2][0] - Positions[2][1] - Sphere.Radius, // positive z

				Positions[3][0] - Positions[3][1] - Sphere.Radius, // negative x
				Positions[4][0] - Positions[4][1] - Sphere.Radius, // negative y
				Positions[5][0] - Positions[5][1] - Sphere.Radius, // negative z
			};

			glm::vec3 Normals[6] =
			{
				glm::vec3(1, 0, 0),
				glm::vec3(0, 1, 0),
				glm::vec3(0, 0, 1),

				glm::vec3(-1, 0, 0),
				glm::vec3(0, -1, 0),
				glm::vec3(0, 0, -1)
			};

			uint8_t X_Index = Delta_Overlaps[0] > Delta_Overlaps[3] ? 0 : 3;
			uint8_t Y_Index = Delta_Overlaps[1] > Delta_Overlaps[4] ? 1 : 4;
			uint8_t Z_Index = Delta_Overlaps[2] > Delta_Overlaps[5] ? 2 : 5;

			glm::vec3 Average_Position;

			Average_Position.x = (Positions[X_Index][0] + Positions[X_Index][1]) * 0.5f;
			Average_Position.y = (Positions[Y_Index][0] + Positions[Y_Index][1]) * 0.5f;
			Average_Position.z = (Positions[Z_Index][0] + Positions[Z_Index][1]) * 0.5f;

			uint8_t Index = Delta_Overlaps[X_Index] > Delta_Overlaps[Y_Index] ? X_Index : Y_Index;
			Index = Delta_Overlaps[Index] > Delta_Overlaps[Z_Index] ? Index : Z_Index;

			return Collision_Info(Average_Position, Normals[Index], Delta_Overlaps[Index]);
		}

		float Length = sqrtf(glm::dot(Overlap, Overlap));

		if (Sphere.Radius > Length)
		{
			float Inv_Length = 1.0f / Length;
			Overlap *= Inv_Length;

			return Collision_Info(AABB_Position, Overlap, Length - Sphere.Radius);
		}
		else
			return Collision_Info();
	}
};

Collision_Info AABB_Hitbox::AABB_Hitdetection(AABB_Hitbox* Other)
{
	return Collision_Test::AABB_Against_AABB(*this, *Other);
}

Collision_Info AABB_Hitbox::Sphere_Hitdetection(Sphere_Hitbox* Other)
{
	return Collision_Test::AABB_Against_Sphere(*this, *Other);
}

//

Collision_Info Sphere_Hitbox::AABB_Hitdetection(AABB_Hitbox* Other)
{
	return Inverse_Collision(Collision_Test::AABB_Against_Sphere(*Other, *this));
}

Collision_Info Sphere_Hitbox::Sphere_Hitdetection(Sphere_Hitbox* Other)
{
	return Collision_Test::Sphere_Against_Sphere(*this, *Other);
}

#endif
