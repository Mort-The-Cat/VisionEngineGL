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
		glm::vec3 Position = glm::vec3(0, 0, 0);
		float Volume = 0;
		std::mutex Sounds_Mutex;
		std::vector<irrklang::ISound*> Sounds;
		bool Flags[1] = { false };

		~Audio_Source()
		{
			// Just delete all sounds
			Sounds_Mutex.lock();
			for (size_t W = 0; W < Sounds.size(); W++)
			{
				Sounds[W]->stop();
				Sounds[W]->drop();
				//delete Sounds[W];
			}
			Sounds.clear();
			Sounds_Mutex.unlock();
		}

		void Play_Sound(irrklang::ISoundSource* Sound_Source)
		{
			irrklang::ISound* Sound = nullptr;
			Sounds_Mutex.lock();
			while(Sound == nullptr)
				Sound = Sound_Engine->play2D(Sound_Source, false, true, false, true);
			//Sound_Engine->play3D(Sound_Source, irrklang::vec3df(Position.x, Position.y, Position.z), false, false, false, false);
			Sounds.push_back(Sound);
			Sounds_Mutex.unlock();
		}

		void Update(const Camera& Camera)
		{
			glm::vec3 Delta_Vector = Position - Camera.Position;

			float Delta_Inverse_Length = (Fast::Sqrt(glm::dot(Delta_Vector, Delta_Vector)));

			float Panning = std::fminf(1, std::fmaxf(-1, glm::dot(Current_Listener_Right_Ear_Vector, Delta_Vector * glm::vec3(1.0f / Delta_Inverse_Length))));

			Panning *= Panning * Panning;
			
			float Perceived_Volume = Volume / (1 + 0.5 * Delta_Inverse_Length);

			Sounds_Mutex.lock();
			for (size_t W = 0; W < Sounds.size(); W++)
			{
				if(!Flags[ASF_TO_BE_DELETED]) // Some error handling is highly adviced if you're going to use multithreading with this
					if (Sounds[W]->isFinished()) // If the sound has completed.
					{
						Sounds[W]->stop();
						Sounds[W]->drop();
						//delete Sounds[W];
						Sounds[W] = nullptr;
					}
					else
					{
						Sounds[W]->setPan(Panning);
						Sounds[W]->setVolume(Perceived_Volume);

						if(Sounds[W]->getIsPaused())
							Sounds[W]->setIsPaused(false);
					}
			}

			auto Completed_Sounds = std::remove_if(Sounds.begin(), Sounds.end(), Is_Deleted);
			Sounds.erase(Completed_Sounds, Sounds.end());

			Sounds_Mutex.unlock();
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
		Sound_Engine->update();

		 for (size_t W = 0; W < Audio_Sources.size(); W++)
		 	Audio_Sources[W]->Update(Listener);

		for (size_t W = 0; W < NUMBER_OF_WORKERS; W++)
			Job_System::Submit_Job(Job_System::Job(Job_Handle_Audio_Task, new Job_Handle_Audio_Parameters(W, &Listener)));
	}
}

#endif