#pragma once
#include "../Engine/Audio/AudioSystem.h"
#include "../Engine/Math/Vector2.h"
#include <SDL3/SDL.h>
#include <vector>
#include <unordered_map>
#include <string>

class DialogSystem
{
private:
	// Dependencies
	AudioManager* m_audioManager = nullptr;

	void UpdateDialogs(float deltaTime);

	std::unordered_map<std::string, size_t> m_dialogIndex;
public:
	struct Dialog
	{
		bool activated = false;
		bool finished = false;

		float charTimer = 0.0f;
		float timePerChar = 0.8f;

		float lineTimer = 0.0f;
		float timeBetweenLines = 18.0f;

		AudioManager::AudioClip audioClip;

		std::vector<std::string> entireDialog;
		std::string displayedLine;

		size_t charIndex = 0;
		size_t lineIndex = 0;

		Vec2f position;

		void Update(AudioManager& audioManager, float deltaTime);
	};
	std::vector<Dialog> m_dialogs;

	static float dialogVolume;

	void RenderDialogs(SDL_Renderer* renderer);
	void Initialize(AudioManager& audioManager);

	size_t GetDialogIndex(const std::string& name);

	Dialog& GetDialog(size_t index);
	Dialog& GetDialog(const std::string& name);

	bool IsActive(size_t index);
	bool IsFinished(size_t index);
	bool SetFinished(size_t index);

	bool IsActive(const std::string& name);
	bool IsFinished(const std::string& name);
	bool SetFinished(const std::string& name);

	[[nodiscard]] Dialog& CreateDialog(const std::string& name);

	void SetDialogSound(const std::string& dialog, AudioManager::AudioClip& audioClip);

	void ActivateDialog(size_t index); // O(1)
	void ActivateDialog(Dialog& dialog); // O(N)
	void ActivateDialog(const std::string& dialog); // O(1)

	void Update(float deltaTime);

	~DialogSystem()
	{
		if constexpr (DEBUGPRINT)
		{
			for (auto& dialog : m_dialogs)
			{
				std::cout << "[" << "\033[34m" << "DIALOGSYSTEM" << "\033[37m" << "] " << " Destroying Dialog: " << &dialog << "\n";
			}
		}
	}
};