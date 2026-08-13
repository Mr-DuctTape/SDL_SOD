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

	struct Dialog
	{
		bool activated = false;

		float charTimer = 0.0f;
		float timePerChar = 0.1f;

		float dialogTimer = 0.0f;
		float timeBetweenDialogs = 2.0f;

		AudioManager::AudioClip audioClip;

		std::vector<std::string> entireDialog;
		std::string currentString;
		size_t currentCharPos = 0;
		size_t dialogCount = 0;

		Vec2f position;
	};

	std::unordered_map<std::string, size_t> m_dialogIndex;

	void UpdateDialogs(float deltaTime)
	{
		for (auto& dialog : m_dialogs)
		{
			if (!dialog.activated)
				return;

			for (auto& string : dialog.entireDialog)
			{
				if (dialog.dialogCount >= dialog.entireDialog.size())
				{
					continue;
				}

				if (string != dialog.entireDialog[dialog.dialogCount])
				{
					continue;
				}

				dialog.charTimer += deltaTime;

				if (dialog.charTimer >= dialog.timePerChar)
				{
					if (dialog.currentString.size() == string.size()) 
					{
						dialog.dialogTimer += deltaTime;
						if (dialog.dialogTimer >= dialog.timeBetweenDialogs)
						{
							dialog.dialogCount++;
							dialog.currentString.clear();
							dialog.currentCharPos = 0;
							dialog.charTimer = 0;
							dialog.dialogTimer = 0;
						}
						continue;
					}

					if (string[dialog.currentCharPos] != 0)
					{
						dialog.currentString += string[dialog.currentCharPos];

						if (string[dialog.currentCharPos] != ' ')
							m_audioManager->Play(dialog.audioClip, 1.0f);
					}

					dialog.currentCharPos++;
					dialog.charTimer = 0;
				}
			}
		}
	}

public:
	std::vector<Dialog> m_dialogs;

	void RenderDialogs(SDL_Renderer* renderer)
	{
		for (const auto& dialog : m_dialogs)
		{
			SDL_RenderDebugText(renderer, dialog.position.x, dialog.position.y, dialog.currentString.c_str());
		}
	}

	void Initialize(AudioManager& audioManager)
	{
		m_audioManager = &audioManager;
		m_dialogs.reserve(16);
		m_dialogIndex.reserve(16);
	}

	Dialog& GetDialog(const std::string& name)
	{
		return m_dialogs[m_dialogIndex[name]];
	}

	[[nodiscard]] Dialog& CreateDialog(const std::string& name)
	{
		Dialog& dialog = m_dialogs.emplace_back();
		m_dialogIndex.emplace(name, m_dialogs.size() - 1);
		return dialog;
	}

	void SetDialogSound(const std::string& dialog, AudioManager::AudioClip& audioClip)
	{
		GetDialog(dialog).audioClip = audioClip;
	}

	void ActivateDialog(const std::string& dialog)
	{
		GetDialog(dialog).activated = true;
	}

	void Update(float deltaTime)
	{
		for (auto& dialog : m_dialogs)
		{
			UpdateDialogs(deltaTime);
		}
	}
};