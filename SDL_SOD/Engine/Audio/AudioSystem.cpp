#include "AudioSystem.h"

SDL_AudioSpec AudioManager::m_deviceFormat = {};
SDL_AudioSpec AudioManager::m_wavFormat = {};

void AudioManager::Update()
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

void AudioManager::Play(const std::string& audioClip, float volume)
{
	auto it = m_audioClips.find(audioClip);
	if (it == m_audioClips.end())
		return;

	PlayerHelper(it->second, volume);
}
void AudioManager::Play(AudioManager::AudioClip& audioClip, float volume)
{
	PlayerHelper(audioClip, volume);
}

void AudioManager::PlayerHelper(AudioClip& audioClip, float volume)
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

SDL_AudioDeviceID AudioManager::GetAudioDevice()
{
	std::vector<uint32_t> devices = GetAvailableDevices();

	return devices[0];
}

AudioManager::AudioClip* AudioManager::CreateAudioClip(const std::string& name, const std::string& filePath)
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

void AudioManager::ChangeVolume(AudioSource& source, float volume)
{
	source.volume = volume;
	SDL_SetAudioStreamGain(source.stream, volume);
}
void AudioManager::ChangePitch(AudioSource& source, float pitch)
{
	source.pitch = pitch;
	SDL_SetAudioStreamFrequencyRatio(source.stream, pitch);
}
void AudioManager::RandomizePitch(AudioSource& source, float pitch)
{
	static std::random_device rd;
	static std::mt19937 rng(rd()); // Random number generator

	std::uniform_real_distribution<float> dist(1.0f, 1.1f);
	float randomFloat = dist(rng);
	SDL_SetAudioStreamFrequencyRatio(source.stream, randomFloat);
}