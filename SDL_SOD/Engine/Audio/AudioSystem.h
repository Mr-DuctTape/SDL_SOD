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

	void PrintError()
	{
		std::cout << SDL_GetError() << "\n";
	}
public:

	struct AudioClip
	{
		SDL_AudioStream* stream = nullptr;

		Uint8* data = nullptr;
		Uint32 length = 0;
	};

	std::unordered_map<std::string, AudioClip> m_audioClips;
	SDL_AudioDeviceID m_playBackDevice;

	AudioManager()
	{
		if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
		{
			std::cout << "(CRITICAL ERROR) Unable to intialize SDL_Audio: ";
			std::cout << SDL_GetError();
		}

		m_playBackDevice = GetAudioDevice();
		m_playBackDevice = SDL_OpenAudioDevice(m_playBackDevice, NULL);
	}

	SDL_AudioDeviceID GetAudioDevice()
	{
		int count;
		SDL_AudioDeviceID* array = SDL_GetAudioPlaybackDevices(&count);
		if (!array)
		{
			std::cout << "(ERROR) Unable to find any PlaybackDevices: ";
			std::cout << SDL_GetError() << "\n";
			return 0;
		}
		for (int i = 0; i < count; i++)
		{
			if (SDL_IsAudioDevicePhysical(array[i]) && SDL_IsAudioDevicePlayback(array[i]))
			{
				return array[i];
			}
		}

		std::cout << "(ERROR) Unable to find any PlayBackDevices: ";
		return 0;
	}

	AudioClip* CreateAudioClip(const std::string& name, const std::string& filePath)
	{
		std::filesystem::path working_dir = std::filesystem::current_path();
		std::filesystem::path wav_path = working_dir.string() + "/" + filePath;
		if (!std::filesystem::exists(wav_path))
		{
			std::cout << "(ERROR) Path doesn't exist! : " << wav_path.string() << "\n";
			return nullptr;
		}

		SDL_AudioSpec deviceFormat;
		if (!SDL_GetAudioDeviceFormat(m_playBackDevice, &deviceFormat, nullptr))
		{
			PrintError();
			return nullptr;
		}

		AudioClip clip;
		SDL_AudioSpec wavFormat;
		if (!SDL_LoadWAV(wav_path.string().c_str(), &wavFormat, &clip.data, &clip.length))
		{
			PrintError();
			return nullptr;
		}

		clip.stream = SDL_CreateAudioStream(&wavFormat, &deviceFormat);
		if (!clip.stream) {
			PrintError();
			return nullptr;
		}
		if (!SDL_PutAudioStreamData(clip.stream, clip.data, clip.length))
		{
			PrintError();
			return nullptr;
		}

		m_audioClips.emplace(name, clip);
		return &m_audioClips.find(name)->second;
	}

	void PlayAudioClip(const std::string& name)
	{
		AudioClip& audioClip = m_audioClips.find(name)->second;

		static std::random_device rd;
		static std::mt19937 rng(rd()); // Random number generator

		std::uniform_real_distribution<float> dist(1.0f, 1.5f);

		float randomFloat = dist(rng);
		ChangePitch(audioClip, randomFloat);

		if (!SDL_ClearAudioStream(audioClip.stream) && DEBUGPRINT)
		{
			PrintError();
		}

		if (!SDL_PutAudioStreamData(audioClip.stream, audioClip.data, audioClip.length) && DEBUGPRINT)
		{
			PrintError();
		}

		if (!SDL_BindAudioStream(m_playBackDevice, audioClip.stream) && DEBUGPRINT)
		{
			PrintError();
		}
		if (!SDL_ResumeAudioDevice(m_playBackDevice) && DEBUGPRINT)
		{
			PrintError();
		}
	}

	void PlayAudioClip(AudioClip& audioClip)
	{
		static std::random_device rd;
		static std::mt19937 rng(rd()); // Random number generator

		std::uniform_real_distribution<float> dist(1.0f, 1.5f);

		float randomFloat = dist(rng);
		ChangePitch(audioClip, randomFloat);
		if (!SDL_ClearAudioStream(audioClip.stream) && DEBUGPRINT)
		{
			PrintError();
		}

		if (!SDL_PutAudioStreamData(audioClip.stream, audioClip.data, audioClip.length) && DEBUGPRINT)
		{
			PrintError();
		}

		if (!SDL_BindAudioStream(m_playBackDevice, audioClip.stream) && DEBUGPRINT)
		{
			PrintError();
		}
		/*if (!SDL_ResumeAudioDevice(m_playBackDevice) && DEBUGPRINT)
		{
			PrintError();
		}*/
	}

	void ChangePitch(AudioClip& clip, float pitch)
	{
		SDL_SetAudioStreamFrequencyRatio(clip.stream, pitch);
	}

	~AudioManager()
	{

	}
};