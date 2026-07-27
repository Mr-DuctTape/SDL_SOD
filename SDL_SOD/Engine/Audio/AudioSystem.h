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
	}

	void Update()
	{
		for (auto& source : m_audioSources)
		{
			if (!source.isPlaying)
				continue;

			int queued = SDL_GetAudioStreamQueued(source.stream);

			if (queued <= 0)
			{
				source.isPlaying = false;
				SDL_ClearAudioStream(source.stream);
			}
		}
	}

	void Play(const std::string& audioClip, float volume)
	{
		auto it = m_audioClips.find(audioClip);
		if (it == m_audioClips.end())
			return;

		PlayerHelper(it->second, volume);
	}
private:
	void PlayerHelper(AudioClip& audioClip, float volume)
	{
		AudioSource* audioSource = nullptr;
		for (auto& source : m_audioSources)
		{
			if (!source.isPlaying)
			{
				audioSource = &source;
				break;
			}
		}
		if (!audioSource)
			return;

		RandomizePitch(*audioSource, audioSource->pitch);
		ChangeVolume(*audioSource, volume);

		SDL_ClearAudioStream(audioSource->stream);
		if (!SDL_PutAudioStreamData(audioSource->stream, audioClip.data.data(), audioClip.length) && DEBUGPRINT)
		{
			PrintError();
		}
		SDL_FlushAudioStream(audioSource->stream);
		if (!SDL_ResumeAudioDevice(m_playBackDevice) && DEBUGPRINT)
		{
			PrintError();
		}
		audioSource->isPlaying = true;
	}
public:
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
		std::vector<uint32_t> devices;
		for (int i = 0; i < count; i++)
		{
			if (SDL_IsAudioDevicePhysical(array[i]) && SDL_IsAudioDevicePlayback(array[i]))
			{
				devices.push_back(array[i]);
			}
		}

		std::cout << "--- Available audio devices ---\n";
		int i = 0;
		if (devices.size() <= 1)
			return devices[0];

		for (uint32_t device : devices)
		{
			std::cout << "Device " << device << " :" << SDL_GetAudioDeviceName(device) << "\n";
		}

		uint32_t device = 0;
		std::cout << "Choose device: ";
		std::cin >> device;
		std::cout << "\n";

		if (!device)
			std::cout << "(ERROR) Unable to find any PlayBackDevices: ";
		return device;
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

		AudioClip clip;
		Uint8* wavData = nullptr;
		Uint32 wavLength = 0;

		if (!SDL_LoadWAV(wav_path.string().c_str(), &clip.wavFormat, &wavData, &wavLength))
		{
			PrintError();
			return nullptr;
		}
		SDL_AudioStream* convertStream =
			SDL_CreateAudioStream(&clip.wavFormat, &m_deviceFormat);

		if (!convertStream)
		{
			PrintError();
			SDL_free(wavData);
			return nullptr;
		}
		SDL_PutAudioStreamData(convertStream, wavData, wavLength);
		SDL_FlushAudioStream(convertStream);
		int convertedLength = SDL_GetAudioStreamAvailable(convertStream);
		clip.data.resize(convertedLength);
		SDL_GetAudioStreamData(
			convertStream,
			clip.data.data(),
			convertedLength
		);
		clip.length = convertedLength;
		SDL_DestroyAudioStream(convertStream);
		SDL_free(wavData);
		m_audioClips.emplace(name, std::move(clip));
		return &m_audioClips.find(name)->second;
	}

	void ChangeVolume(AudioSource& source, float volume)
	{
		source.volume = volume;
		SDL_SetAudioStreamGain(source.stream, volume);
	}
	void ChangePitch(AudioSource& source, float pitch)
	{
		source.pitch = pitch;
		SDL_SetAudioStreamFrequencyRatio(source.stream, pitch);
	}
	void RandomizePitch(AudioSource& source, float pitch)
	{
		static std::random_device rd;
		static std::mt19937 rng(rd()); // Random number generator

		std::uniform_real_distribution<float> dist(1.0f, 1.1f);
		float randomFloat = dist(rng);
		SDL_SetAudioStreamFrequencyRatio(source.stream, randomFloat);
	}
};