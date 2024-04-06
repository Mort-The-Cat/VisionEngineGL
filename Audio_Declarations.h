#ifndef VISION_AUDIO_DECLARATIONS
#define VISION_AUDIO_DECLARATIONS

#include "irrKlang.h"
#include "ik_vec3d.h"

#include "glm/glm.hpp"

#pragma comment(lib, "irrKlang.lib")

irrklang::ISoundEngine* Sound_Engine;

irrklang::ISoundSource* Sound_Effect_Source;

irrklang::ISound* Fire_Sound;

irrklang::ISoundSource* Bump_Sound_Effect_Source;

const glm::vec3 Audio_Position_Multiplier = glm::vec3(0.25);

glm::vec3 Current_Listener_Right_Ear_Vector;

irrklang::vec3df Get_Klang_Vector(glm::vec3 Vector)
{
	return { Vector.x, Vector.y, Vector.z };
}

void Initialise_Sound_Engine()
{
	Sound_Engine = irrklang::createIrrKlangDevice();

	Sound_Engine->setSoundVolume(0.5);

	Sound_Engine->play2D("Assets/Audio/PC.wav", false);

	Sound_Effect_Source = Sound_Engine->addSoundSourceFromFile("Assets/Audio/Makarov.wav", irrklang::ESM_AUTO_DETECT, true);

	Bump_Sound_Effect_Source = Sound_Engine->addSoundSourceFromFile("Assets/Audio/Step.wav", irrklang::ESM_AUTO_DETECT, true);

	Fire_Sound = Sound_Engine->play2D("Assets/Audio/Burner2.wav", true, true);
	Fire_Sound->setVolume(0);
	Fire_Sound->setIsPaused(false);
}

void Destroy_Sound_Engine()
{
	Sound_Engine->drop();
	Sound_Engine = nullptr;
}

#endif
