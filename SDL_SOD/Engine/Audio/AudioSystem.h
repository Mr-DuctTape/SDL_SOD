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

	inline AudioClip& GetAudio(const std::string& name)
	{
		return m_audioClips[name];
	}

	AudioManager()
	{
		if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
		{
			std::cout << "(CRITICAL ERROR) Unable to intialize SDL_Audio: ";
			std::cout << SDL_GetError();
			std::cout << "\n";
		}

		m_playBackDevice = GetAudioDevice();
		m_playBackDevice = SDL_OpenAudioDevice(m_playBackDevice, NULL);

		if (!SDL_GetAudioDeviceFormat(m_playBackDevice, &m_deviceFormat, nullptr))
		{
			PrintError();
			return;
		}

		m_audioSources.resize(32);
		for (auto& source : m_audioSources)
		{
			source.stream = SDL_CreateAudioStream(&m_deviceFormat, &m_deviceFormat);
			
			if (!source.stream)
			{
				PrintError();
				continue;
			}

			SDL_BindAudioStream(m_playBackDevice, source.stream);
		}
		if constexpr (DEBUGPRINT)
			std::cout << "[" << "\033[39m" << "AUDIOSYSTEM" << "\033[37m" << "] " << " Initialized: " << this << "\n";
	}

	void RebindAudioDevice()
	{
		m_playBackDevice = SDL_OpenAudioDevice(m_playBackDevice, NULL);

		if (!SDL_GetAudioDeviceFormat(m_playBackDevice, &m_deviceFormat, nullptr))
		{
			PrintError();
			return;
		}

		m_audioSources.resize(32);
		for (auto& source : m_audioSources)
		{
			SDL_DestroyAudioStream(source.stream);
			source.stream = SDL_CreateAudioStream(&m_deviceFormat, &m_deviceFormat);

			if (!source.stream)
			{
				PrintError();
				continue;
			}

			SDL_BindAudioStream(m_playBackDevice, source.stream);
		}
	}
	AudioClip* CreateAudioClip(const std::string& name, const std::string& filePath);
	void Update();
	void Play(const std::string& audioClip, float volume);
	void Play(AudioManager::AudioClip& audioClip, float volume);
	void ChangeVolume(AudioSource& source, float volume);
	void ChangePitch(AudioSource& source, float pitch);
	void RandomizePitch(AudioSource& source, float pitch);

	std::vector<uint32_t> GetAvailableDevices()
	{
		int count;
		SDL_AudioDeviceID* array = SDL_GetAudioPlaybackDevices(&count);
		std::vector<uint32_t> devices;
		for (int i = 0; i < count; i++)
		{
			if (SDL_IsAudioDevicePhysical(array[i]) && SDL_IsAudioDevicePlayback(array[i]))
			{
				devices.push_back(array[i]);
			}
		}
		return devices;
	}
	const char* GetDeviceName(uint32_t id)
	{
		return SDL_GetAudioDeviceName(id);
	}

private:
	void PlayerHelper(AudioClip& audioClip, float volume);
	SDL_AudioDeviceID GetAudioDevice();
};