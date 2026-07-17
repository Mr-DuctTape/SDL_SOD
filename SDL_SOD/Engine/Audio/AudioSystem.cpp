#include "AudioSystem.h"


void OpenDevice() {
	
	SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL, NULL, NULL);
}