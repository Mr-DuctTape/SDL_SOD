#include "Dialog.h"

float DialogSystem::dialogVolume = 0.5f;

void DialogSystem::Dialog::Update(AudioManager& audioManager, float deltaTime)
{
	auto IncreaseTimer = [deltaTime](float& timer)
		{
			timer += deltaTime;
		};

	// Reserve memory to avoid unessecarry memory allocations
	constexpr int reserved = 128;
	if(displayedLine.capacity() != reserved)
	{
		displayedLine.reserve(reserved);
	}

	for (const std::string& dialogLine : entireDialog)
	{
		if (dialogLine != entireDialog[lineIndex])
		{
			continue;
		}

		if (displayedLine.size() == dialogLine.size())
		{
			IncreaseTimer(lineTimer);
			if (lineTimer >= timeBetweenLines)
			{
				displayedLine.clear();
				charIndex = 0;
				charTimer = 0.0f;
				lineTimer = 0.0f;
				lineIndex++;
			}
		}
		else
		{
			IncreaseTimer(charTimer);
			if (charTimer >= timePerChar)
			{
				char currentChar = dialogLine[charIndex];

				if (currentChar != 0)
				{
					displayedLine += currentChar;

					if (currentChar != ' ') {
						audioManager.Play(audioClip, DialogSystem::dialogVolume);
					}
				}

				charIndex++;
				charTimer = 0;
			}
		}

	}
}

void DialogSystem::UpdateDialogs(float deltaTime)
{
	for (auto& dialog : m_dialogs)
	{
		if (!dialog.activated)
		{
			continue;
		}

		if (dialog.lineIndex >= dialog.entireDialog.size())
		{
			dialog.finished = true;
			continue;
		}

		dialog.Update(*m_audioManager, deltaTime);
	}
}

void DialogSystem::RenderDialogs(SDL_Renderer* renderer)
{
	for (const auto& dialog : m_dialogs)
	{
		SDL_RenderDebugText(renderer, dialog.position.x, dialog.position.y, dialog.displayedLine.c_str());
	}
}

void DialogSystem::Initialize(AudioManager& audioManager)
{
	m_audioManager = &audioManager;
	m_dialogs.reserve(32);
	m_dialogIndex.reserve(32);
	if constexpr (DEBUGPRINT) {
		std::cout << "[" << "\033[34m" << "DIALOGSYSTEM" << "\033[37m" << "] " << " Initialized: " << this << "\n";
	}
}

size_t DialogSystem::GetDialogIndex(const std::string& name)
{
	return m_dialogIndex[name];
}

DialogSystem::Dialog& DialogSystem::GetDialog(size_t index)
{
	return m_dialogs[index];
}
DialogSystem::Dialog& DialogSystem::GetDialog(const std::string& name)
{
	return m_dialogs[m_dialogIndex[name]];
}

bool DialogSystem::IsActive(size_t index)
{
	return m_dialogs[index].activated;
}
bool DialogSystem::IsActive(const std::string& name)
{
	return m_dialogs[m_dialogIndex[name]].activated;
}

bool DialogSystem::IsFinished(const std::string& name)
{
	return m_dialogs[m_dialogIndex[name]].finished;
}
bool DialogSystem::IsFinished(size_t index)
{
	return m_dialogs[index].finished;
}

DialogSystem::Dialog& DialogSystem::CreateDialog(const std::string& name)
{
	Dialog& dialog = m_dialogs.emplace_back();
	m_dialogIndex.emplace(name, m_dialogs.size() - 1);
	return dialog;
}

void DialogSystem::SetDialogSound(const std::string& dialog, AudioManager::AudioClip& audioClip)
{
	GetDialog(dialog).audioClip = audioClip;
}

void DialogSystem::ActivateDialog(Dialog& dialog) // O(N)
{
	for (auto& dialogs : m_dialogs)
	{
		if (&dialog == &dialogs)
		{
			dialogs.activated = true;
		}
	}
}
void DialogSystem::ActivateDialog(size_t index) // O(1)
{
	GetDialog(index).activated = true;
}
void DialogSystem::ActivateDialog(const std::string& dialog) // O(1)
{
	GetDialog(dialog).activated = true;
}


bool DialogSystem::SetFinished(size_t index)
{
	return m_dialogs[index].finished = true;
}

bool DialogSystem::SetFinished(const std::string& name)
{
	return m_dialogs[m_dialogIndex[name]].finished = true;
}

void DialogSystem::Update(float deltaTime)
{
	for (auto& dialog : m_dialogs)
	{
		UpdateDialogs(deltaTime);
	}
}