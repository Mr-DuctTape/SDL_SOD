#pragma once
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_init.h>
#include <unordered_map>
#include <string>
#include <iostream>
#include <filesystem>
#include "../Macros/DEBUGPRINT.h"
#include <random>

class AudioManager
{
private:
	static void PrintError()
	{
		std::cout << SDL_GetError() << "\n";
	}
	static SDL_AudioSpec m_deviceFormat;
	static SDL_AudioSpec m_wavFormat;

public:
	struct AudioClip
	{
		SDL_AudioSpec wavFormat{};

		std::vector<Uint8> data;
		Uint32 length = 0;
	};

	struct AudioSource
	{
		bool isPlaying = false;
		SDL_AudioStream* stream = nullptr;

		float pitch = 1.0f;
		float volume = 1.0f;
	};

	std::vector<AudioSource> m_audioSources;
	std::unordered_map<std::string, AudioClip> m_audioClips;
	SDL_AudioDeviceID m_playBackDevice;

	AudioManager();

	inline AudioClip& GetAudio(const std::string& name)
	{
		return m_audioClips[name];
	}

	AudioClip* CreateAudioClip(const std::string& name, const std::string& filePath);
	void Update();
	void Play(const std::string& audioClip, float volume);
	void Play(AudioManager::AudioClip& audioClip, float volume);
	void ChangeVolume(AudioSource& source, float volume);
	void ChangePitch(AudioSource& source, float pitch);
	void RandomizePitch(AudioSource& source, float pitch);

	void RebindAudioDevice();

	std::vector<uint32_t> GetAvailableDevices();
	const char* GetDeviceName(uint32_t id)
	{
		return SDL_GetAudioDeviceName(id);
	}

private:
	void PlayerHelper(AudioClip& audioClip, float volume);
	SDL_AudioDeviceID GetAudioDevice();
};