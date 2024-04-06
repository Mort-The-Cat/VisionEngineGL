#ifndef AUDIO_HANDLER_DECLARATIONS_VISION
#define AUDIO_HANDLER_DECLARATIONS_VISION

// We also want to be able to load sound effects from the cache, probably returning an irrklang::ISoundSource

#include "irrKlang.h"
#include "glm/glm.hpp"
#include <vector>

#include "OpenGL_Declarations.h"

#define ASF_TO_BE_DELETED 0

// ASF stands for audio-source-flags lol

// We can modify this later if we really want to

namespace Audio
{
	class Audio_Source
	{
	public:
		glm::vec3 Position;
		float Volume;
		std::vector<irrklang::ISound*> Sounds;
		bool Flags[1];

		~Audio_Source()
		{
			// Just delete all sounds
			for (size_t W = 0; W < Sounds.size(); W++)
				Sounds[W]->drop();
			Sounds.clear();
		}

		void Play_Sound(irrklang::ISoundSource* Sound_Source)
		{
			irrklang::ISound* Sound = Sound_Engine->play2D(Sound_Source, false, true, false, true);

			Sounds.push_back(Sound);
		}

		void Update(const Camera& Camera)
		{
			glm::vec3 Delta_Vector = -Position + Camera.Position;

			float Delta_Inverse_Length = (Fast::Sqrt(glm::dot(Delta_Vector, Delta_Vector)));

			float Panning = std::fminf(1, std::fmaxf(-1, glm::dot(Current_Listener_Right_Ear_Vector, Delta_Vector * glm::vec3(1.0f / Delta_Inverse_Length))));

			Panning *= Panning * Panning;
			
			float Perceived_Volume = Volume / (1 + Delta_Inverse_Length);

			for (size_t W = 0; W < Sounds.size(); W++)
			{
				if(Sounds[W] != nullptr) // Some error handling is highly adviced if you're going to use multithreading with this
					if (Sounds[W]->isFinished()) // If the sound has completed.
					{
						Sounds[W]->drop();
						Sounds[W] = nullptr;
					}
					else
					{
						Sounds[W]->setPan(Panning);
						Sounds[W]->setVolume(Perceived_Volume);

						Sounds[W]->setIsPaused(false);
					}
			}

			auto Completed_Sounds = std::remove_if(Sounds.begin(), Sounds.end(), Is_Deleted);
			Sounds.erase(Completed_Sounds, Sounds.end());
		}
	};

	std::vector<Audio_Source*> Audio_Sources;

	void Handle_Audio_Deletions()
	{
		for(size_t W = 0; W < Audio_Sources.size(); W++)
			if (Audio_Sources[W]->Flags[ASF_TO_BE_DELETED])
			{
				delete Audio_Sources[W];
				Audio_Sources[W] = nullptr;
			}

		auto Deleted_Sound_Sources = std::remove_if(Audio_Sources.begin(), Audio_Sources.end(), Is_Deleted);
		Audio_Sources.erase(Deleted_Sound_Sources, Audio_Sources.end());
	}

	Audio_Source* Create_Audio_Source(glm::vec3 Position, float Volume)
	{
		Audio_Source* Source = new Audio_Source();

		Source->Position = Position;
		Source->Volume = Volume;
		Audio_Sources.push_back(Source);

		return Source;
	}

	struct Job_Handle_Audio_Parameters
	{
		size_t Offset;
		const Camera* Listener;
		Job_Handle_Audio_Parameters(size_t Offsetp, const Camera* Camerap)
		{
			Offset = Offsetp;
			Listener = Camerap;
		}
	};

	void Job_Handle_Audio_Task(void* Data)
	{
		Job_Handle_Audio_Parameters* Params = static_cast<Job_Handle_Audio_Parameters*>(Data);

		size_t W = Params->Offset;
		const Camera& Camera = *Params->Listener;
		
		delete Params; // Frees up the memory that was parsed to the worker

		for (; W < Audio_Sources.size(); W += NUMBER_OF_WORKERS)
			Audio_Sources[W]->Update(Camera);
	}

	void Handle_Audio(Camera& Listener)
	{
		for (size_t W = 0; W < NUMBER_OF_WORKERS; W++)
			Job_System::Submit_Job(Job_System::Job(Job_Handle_Audio_Task, new Job_Handle_Audio_Parameters(W, &Listener)));
	}
}

#endif