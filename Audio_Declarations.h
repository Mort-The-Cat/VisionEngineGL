#ifndef VISION_AUDIO_DECLARATIONS
#define VISION_AUDIO_DECLARATIONS

#include "irrKlang.h"

#pragma comment(lib, "irrKlang.lib")

irrklang::ISoundEngine* Sound_Engine;

irrklang::ISoundSource* Sound_Effect_Source;

irrklang::ISound* Fire_Sound;

void Initialise_Sound_Engine()
{
	Sound_Engine = irrklang::createIrrKlangDevice();

	Sound_Engine->play2D("Assets/Audio/PC.wav", false);

	Sound_Effect_Source = Sound_Engine->addSoundSourceFromFile("Assets/Audio/Makarov.wav", irrklang::ESM_AUTO_DETECT, true);

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
