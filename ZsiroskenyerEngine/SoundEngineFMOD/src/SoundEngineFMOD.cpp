#include "SoundEngineFMOD.h"

#include "../../Core/src/common.h"
#include "../../Core/src/StrUtil.h"
#include "../../Core/src/ILog.h"

#include "../../Core/src/Timer.h"

cSoundEngineFMOD::cSoundEngineFMOD()
:soundSystem(NULL), isMicConnected(false) {
	// Create sound system
	FMOD_RESULT result = FMOD::System_Create(&soundSystem);
	if (result != FMOD_OK)
		ILog::GetInstance()->MsgBox("[cSoundEngineFMOD] Can't create FMOD sound engine module");

	// 5.1 output
	soundSystem->setSpeakerMode(FMOD_SPEAKERMODE_5POINT1);

	// 32 software channel
	soundSystem->setSoftwareChannels(32);

	// 32 hardware channels
	soundSystem->setHardwareChannels(32);

	// Init with 16 virtual voice
	result = soundSystem->init(16, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
		ILog::GetInstance()->MsgBox("[cSoundEngineFMOD] Failed to init with 16 virtual voice");

	// Microphone detection
	int nDrivers = 0;
	result = soundSystem->getRecordNumDrivers(&nDrivers);
	if (result == FMOD_OK && nDrivers > 0)
		isMicConnected = true;
}

cSoundEngineFMOD::~cSoundEngineFMOD() {
	soundSystem->release();
	soundSystem = NULL;
}

void cSoundEngineFMOD::Release() {
	delete this;
}

bool cSoundEngineFMOD::GenWavFromMicrophone(const zsString& filePath, float stopAfterSeconds)  {
	ASSERT(stopAfterSeconds <= MAX_SOUND_RECORD_SEC_LENGTH);

	FMOD_RESULT result = soundSystem->setOutput(FMOD_OUTPUTTYPE_DSOUND);
	if (result != FMOD_OK) {
		ILog::GetInstance()->MsgBox("[cSoundEngineFMOD] Failed to set DirectSound Output for recording");
		return false;
	}

	int nDrivers;
	result = soundSystem->getNumDrivers(&nDrivers);
	if (result != FMOD_OK || nDrivers < 1) {
		ILog::GetInstance()->MsgBox("[cSoundEngineFMOD] There is no available sound Driver");
		return false;
	}

	result = soundSystem->setDriver(0);
	if (result != FMOD_OK) {
		ILog::GetInstance()->MsgBox("[cSoundEngineFMOD] Failed to set sound driver for the system");
		return false;
	}

	result = soundSystem->getRecordNumDrivers(&nDrivers);
	if (result != FMOD_OK || nDrivers < 1) {
		ILog::GetInstance()->MsgBox("[cSoundEngineFMOD] There is no available sound recording Driver");
		return false;
	}

	// Create sound that we record into
	FMOD::Sound *sound = NULL;
	FMOD_CREATESOUNDEXINFO exinfo; memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
		exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		exinfo.numchannels = 1;
		exinfo.format = FMOD_SOUND_FORMAT_PCM16;
		exinfo.defaultfrequency = 44100;
		exinfo.length = exinfo.defaultfrequency * sizeof(short)* exinfo.numchannels * MAX_SOUND_RECORD_SEC_LENGTH;
	result = soundSystem->createSound(0, FMOD_2D | FMOD_SOFTWARE | FMOD_OPENUSER, &exinfo, &sound);
	if (result != FMOD_OK) {
		ILog::GetInstance()->MsgBox("[cSoundEngineFMOD] Failed creating FMOD::sound for recording");
		return false;
	}


	// start recording...
	result = soundSystem->recordStart(0, sound, false);
	if (result != FMOD_OK) {
		ILog::GetInstance()->MsgBox("[cSoundEngineFMOD::GenWavFromMicrophone] Failed to start recording");
		return false;
	}

	cTimer t;
	static float timer = 0.0f;
	do {
		soundSystem->update();
		timer += t.GetDeltaSeconds();

	} while (timer < stopAfterSeconds);

	result = soundSystem->recordStop(0);
	if (result != FMOD_OK) {
		ILog::GetInstance()->MsgBox("Failed stop sound recording");
		return false;
	}
	SaveToWav(sound, filePath);
	return true;
}

void cSoundEngineFMOD::SaveToWav(FMOD::Sound *sound, const zsString& filePath) {
	FILE *fp;
	int             channels, bits;
	float           rate;
	void           *ptr1, *ptr2;
	unsigned int    lenbytes, len1, len2;

	if (!sound) {
		return;
	}

	sound->getFormat(0, 0, &channels, &bits);
	sound->getDefaults(&rate, 0, 0, 0);
	sound->getLength(&lenbytes, FMOD_TIMEUNIT_PCMBYTES);

	{
#if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
#pragma pack(1)
#endif

		// WAV Structures
		typedef struct {
			signed char id[4];
			int 		size;
		} RiffChunk;

		struct {
			RiffChunk       chunk;
			unsigned short	wFormatTag;			/* Format type  */
			unsigned short	nChannels;			/* Number of channels (i.e. mono, stereo...)  */
			unsigned int	nSamplesPerSec;		/* Sample rate  */
			unsigned int	nAvgBytesPerSec;    /* For buffer estimation  */
			unsigned short	nBlockAlign;		/* Block size of data  */
			unsigned short	wBitsPerSample;		/* Number of bits per sample of mono data */
		} FmtChunk = { { { 'f', 'm', 't', ' ' }, sizeof(FmtChunk)-sizeof(RiffChunk) }, 1, channels, (int)rate, (int)rate * channels * bits / 8, 1 * channels * bits / 8, bits };

		struct {
			RiffChunk   chunk;
		} DataChunk = { { { 'd', 'a', 't', 'a' }, lenbytes } };

		struct {
			RiffChunk   chunk;
			signed char rifftype[4];
		} WavHeader = { { { 'R', 'I', 'F', 'F' }, sizeof(FmtChunk)+sizeof(RiffChunk)+lenbytes }, { 'W', 'A', 'V', 'E' } };

#if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
#pragma pack()
#endif

		char ansiPath[256];
		cStrUtil::ToAnsi(filePath, ansiPath, 256);

		fp = fopen(ansiPath, "wb");

		// Write out the WAV header.
		fwrite(&WavHeader, sizeof(WavHeader), 1, fp);
		fwrite(&FmtChunk, sizeof(FmtChunk), 1, fp);
		fwrite(&DataChunk, sizeof(DataChunk), 1, fp);

		// Lock the sound to get access to the raw data.
		sound->lock(0, lenbytes, &ptr1, &ptr2, &len1, &len2);

		// Write it to disk.
		fwrite(ptr1, len1, 1, fp);

		//Unlock the sound to allow FMOD to use it again.
		sound->unlock(ptr1, ptr2, len1, len2);

		fclose(fp);
	}
}

bool cSoundEngineFMOD::isMicrophoneConnected() const {
	return isMicConnected;
}