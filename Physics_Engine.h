#ifndef PHYSICS_ENGINE_VISIONGL
#define PHYSICS_ENGINE_VISIONGL

#include "OpenGL_Declarations.h"
#include "Job_System_Declarations.h"
#include "Model_Declarations.h"
#include "Hitdetection.h"

void Wait_On_Physics();

namespace Physics
{
	class Physics_Object;

	class Impulse_Object
	{
	public:
		glm::vec3 A_Position, B_Position;
		Physics_Object* A = nullptr;
		Physics_Object* B = nullptr;
		Collision_Info Collision;
	};

#define PF_TO_BE_DELETED 0u

	class Physics_Object
	{
	public:
		Model* Object;

		glm::vec3 Forces = glm::vec3(0, 0, 0);
		glm::vec3 Velocity = glm::vec3(0, 0, 0);
		
		glm::vec3 Centre_Of_Mass = glm::vec3(0, 0, 0);

		float Mass, Inv_Mass;

		float Elasticity; // Bounciness

		bool Flags[1] = { false };

		virtual void Resolve_Collision(Impulse_Object* Collision)
		{
			glm::vec3 A_Velocity = Velocity;
			glm::vec3 B_Velocity = Collision->B != nullptr ? Collision->B->Velocity : glm::vec3(0, 0, 0);

			glm::vec3 Relative_Velocity = B_Velocity - A_Velocity;

			float Normal_Speed = glm::dot(Relative_Velocity, -Collision->Collision.Collision_Normal);

			glm::vec3 Impulse(0, 0, 0);

			if (Normal_Speed > 0)
			{
				float A_Inv_Mass = Inv_Mass;
				float B_Inv_Mass = Collision->B != nullptr ? Collision->B->Inv_Mass : 0.0f;

				float E = Elasticity * (Collision->B != nullptr ? Collision->B->Elasticity : 1.0f);

				float J = -(1.0f + E) * Normal_Speed / (A_Inv_Mass + B_Inv_Mass);

				Impulse = J * Collision->Collision.Collision_Normal;

				Forces += Impulse;
			}

			float B_Active = Collision->B != nullptr;

			glm::vec3 Delta = Collision->Collision.Collision_Normal * (Fast::Add_Epsilon(Collision->Collision.Overlap, 2) / (1.0f + B_Active));

			Object->Position += Delta;

			if (Collision->B != nullptr) // Apply the forces/deltas to B accordingly (but only if B is a physics object itself) 
			{
				Collision->B->Forces -= Impulse;

				Collision->B->Object->Position -= Delta;
			}
		}

		virtual void Step()
		{
			float Gravity = Tick * 4.9;

			Forces *= Inv_Mass * 0.5;

			Velocity += Forces;
			Velocity.y += Gravity;

			Object->Position += Velocity * abs(Tick);

			Velocity += Forces;
			Velocity.y += Gravity;

			Forces.x = 0;
			Forces.y = 0;
			Forces.z = 0;
		}
	};

	std::vector<Physics_Object*> Scene_Physics_Objects;

	std::mutex Recorded_Impulses_Mutex;
	std::vector<Impulse_Object> Recorded_Impulses;

	std::mutex Threads_Working_Count_Mutex;
	char Threads_Working_On_Physics = 0;

	void Job_Record_Collisions(void* Data)
	{
		size_t* Offset = (size_t*)Data;

		for (size_t W = 0; W < Scene_Physics_Objects.size(); W++)
		{
			for (size_t V = W + 1 + *Offset; V < Scene_Physics_Objects.size(); V += NUMBER_OF_WORKERS)
			{
				Impulse_Object Impulse;

				Impulse.Collision = Scene_Physics_Objects[W]->Object->Hitbox->Hitdetection(Scene_Physics_Objects[V]->Object->Hitbox);

				Impulse.A = Scene_Physics_Objects[W];
				Impulse.B = Scene_Physics_Objects[V];

				Impulse.A_Position = Scene_Physics_Objects[W]->Object->Position;
				Impulse.B_Position = Scene_Physics_Objects[V]->Object->Position;

				if (Impulse.Collision.Overlap != 0)
				{
					Recorded_Impulses_Mutex.lock();
					Recorded_Impulses.push_back(Impulse);
					Recorded_Impulses_Mutex.unlock();
				}
			}

			for (size_t V = *Offset + Scene_Physics_Objects.size(); V < Scene_Hitboxes.size(); V += NUMBER_OF_WORKERS)
			{
				Impulse_Object Impulse;

				Impulse.Collision = Scene_Physics_Objects[W]->Object->Hitbox->Hitdetection(Scene_Hitboxes[V]);

				Impulse.A = Scene_Physics_Objects[W];
				Impulse.B = nullptr;

				Impulse.A_Position = Scene_Physics_Objects[W]->Object->Position;
				Impulse.B_Position = *Scene_Hitboxes[V]->Position;
				
				if (Impulse.Collision.Overlap != 0)
				{
					Recorded_Impulses_Mutex.lock();
					Recorded_Impulses.push_back(Impulse);
					Recorded_Impulses_Mutex.unlock();
				}
			}
		}

		Threads_Working_Count_Mutex.lock();
		Threads_Working_On_Physics--;
		Threads_Working_Count_Mutex.unlock();

		delete Offset;
	}

	void Job_Resolve_Collisions(void* Data)
	{
		size_t* Offset = (size_t*)Data;

		for (size_t W = *Offset; W < Recorded_Impulses.size(); W += NUMBER_OF_WORKERS)
			Recorded_Impulses[W].A->Resolve_Collision(&Recorded_Impulses[W]);
		
		for (size_t W = *Offset; W < Scene_Physics_Objects.size(); W += NUMBER_OF_WORKERS)
			Scene_Physics_Objects[W]->Step();

		Threads_Working_Count_Mutex.lock();
		Threads_Working_On_Physics--;
		Threads_Working_Count_Mutex.unlock();

		delete Offset;
	}

	void Record_Collisions()
	{
		Wait_On_Physics();

		Threads_Working_Count_Mutex.lock();
		Threads_Working_On_Physics = NUMBER_OF_WORKERS;
		Threads_Working_Count_Mutex.unlock();

		Recorded_Impulses.clear(); // I'm so dumb XD I forgot to include this!

		for (size_t W = 0; W < NUMBER_OF_WORKERS; W++)
			Job_System::Submit_Job(Job_System::Job(Job_Record_Collisions, new size_t(W)));
	}

	void Resolve_Collisions()
	{
		Wait_On_Physics();

		Threads_Working_Count_Mutex.lock();
		Threads_Working_On_Physics = NUMBER_OF_WORKERS;
		Threads_Working_Count_Mutex.unlock();

		for (size_t W = 0; W < NUMBER_OF_WORKERS; W++)
			Job_System::Submit_Job(Job_System::Job(Job_Resolve_Collisions, new size_t(W)));
	}
}

class Physics_Object_Controller : public Controller
{
public:
	Physics::Physics_Object* Physics_Info;

	// Lightsource* Light;

	float Time = 10;

	virtual void Initialise_Control(Model* Objectp) override
	{
		Object = Objectp;

		Physics_Info = new Physics::Physics_Object();

		Physics_Info->Mass = 1.0f;
		Physics_Info->Inv_Mass = 1.0f;
		Physics_Info->Elasticity = 0.9f;

		Physics::Scene_Physics_Objects.push_back(Physics_Info);

		Physics_Info->Object = Object;
	}

	virtual void Control_Function() override
	{
		Time -= Tick;

		bool Should_Delete = Object->Position.y > 5 || Time < 0;

		Object->Flags[MF_TO_BE_DELETED] = Should_Delete;
		Physics_Info->Flags[PF_TO_BE_DELETED] = Should_Delete;
		Object->Hitbox->Flags[HF_TO_BE_DELETED] = Should_Delete;
	}
};

void Wait_On_Physics()
{
	bool Still_Working = false;
	do
	{
		Physics::Threads_Working_Count_Mutex.lock();
		Still_Working = Physics::Threads_Working_On_Physics;
		Physics::Threads_Working_Count_Mutex.unlock();
	} while (Still_Working);
}

#endif
