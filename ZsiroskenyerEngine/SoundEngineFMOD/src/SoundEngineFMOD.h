/// SoundEngineFMOD By Zsíroskenyér Team 2013.11.23 8:55
// Sound engine implementation based on FMOD sound library
#pragma once

#include "../../Core/src/ISoundEngine.h"
#include "../../Core/src/zsString.h"
#include "FMOD/include/fmod.hpp"

#define MAX_SOUND_RECORD_SEC_LENGTH 60

class cSoundEngineFMOD : public ISoundEngine {
public:
	cSoundEngineFMOD();
	~cSoundEngineFMOD();
	void Release() override;

	bool GenWavFromMicrophone(const zsString& filePath, float stopAfterSeconds) override;

	bool isMicrophoneConnected() const override;

protected:
	void SaveToWav(FMOD::Sound *sound, const zsString& filePath);

	// Sound engine
	FMOD::System *soundSystem;

	bool isMicConnected;
};

