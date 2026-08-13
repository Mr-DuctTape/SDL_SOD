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

	std::unordered_map<std::string, size_t> m_dialogIndex;

	void UpdateDialogs(float deltaTime)
	{
		for (auto& dialog : m_dialogs)
		{
			if (!dialog.activated)
			{
				continue;
			}

			if (dialog.currentDialogCount >= dialog.entireDialog.size())
			{
				dialog.finished = true;
				continue;
			}

			for (const std::string& text : dialog.entireDialog)
			{
				if (text != dialog.entireDialog[dialog.currentDialogCount])
				{
					continue;
				}

				dialog.charTimer += deltaTime;

				if (dialog.currentString.size() == text.size())
				{
					dialog.dialogTimer += deltaTime;
					if (dialog.dialogTimer >= dialog.timeBetweenDialogs)
					{
						dialog.currentDialogCount++;
						dialog.currentString.clear();
						dialog.currentCharPos = 0;
						dialog.charTimer = 0;
						dialog.dialogTimer = 0;
						continue;
					}
				}
				else if (dialog.charTimer >= dialog.timePerChar)
				{
					char currentChar = text[dialog.currentCharPos];

					if (currentChar != '\0')
					{
						dialog.currentString += currentChar;

						if (currentChar != ' ')
							m_audioManager->Play(dialog.audioClip, 1.0f);
					}

					dialog.currentCharPos++;
					dialog.charTimer = 0;
				}
			}
		}
	}

public:
	struct Dialog
	{
		bool activated = false;
		bool finished = false;

		float charTimer = 0.0f;
		float timePerChar = 0.3f;

		float dialogTimer = 0.0f;
		float timeBetweenDialogs = 10.0f;

		AudioManager::AudioClip audioClip;

		std::vector<std::string> entireDialog;
		std::string currentString;
		size_t currentCharPos = 0;
		size_t currentDialogCount = 0;

		Vec2f position;
	};
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
		m_dialogs.reserve(32);
		m_dialogIndex.reserve(32);
	}

	size_t GetDialogIndex(const std::string& name)
	{
		return m_dialogIndex[name];
	}
	Dialog& GetDialog(Dialog& dialog)
	{

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

	void ActivateDialog(Dialog& dialog) // O(N)
	{
		for (auto& dialogs : m_dialogs)
		{
			if (&dialog == &dialogs)
			{
				dialogs.activated = true;
			}
		}
	}
	void ActivateDialog(const std::string& dialog) // O(1)
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