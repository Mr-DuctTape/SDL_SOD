#pragma once
#include <SDL3/SDL.h>
#include "../Math/Vector2.h"
#include <vector>
#include <unordered_map>
#include "../Audio/AudioSystem.h"

class InputSystem;
class RenderingSystem;
class Camera;

class UIElement
{
private:
	AudioManager::AudioClip m_audioClip;

public:
	SDL_Texture* texture = nullptr;

	bool playedAudio = false;
	bool pressed = false;
	bool displayName = false;

	std::string name{};
	std::string displayText{};

	Vec2f screenPos{};
	float height = 0.0f;
	float width = 0.0f;

	SDL_Color currentColor{ 255, 255, 255, 255 };
	SDL_Color stationaryColor{ 155, 156, 155, 0 };
	SDL_Color highlightedColor{ 125, 156, 125, 100 };
	SDL_Color pressedColor{ 100, 156, 100, 255 };


	bool MouseHoverOver(InputSystem& inputSystem);
	bool IsButtonPressed();
	void SetAudioClip(AudioManager::AudioClip& audioClip)
	{
		m_audioClip = audioClip;
	}
	void PlayAudio(AudioManager& audioManager, float volume)
	{
		audioManager.Play(m_audioClip, volume);
	}
	void PlayAudio(AudioManager& audioManager, const std::string& audioName, float volume)
	{
		audioManager.Play(audioName, volume);
	}
	static void RenderButtonText(SDL_Renderer* renderer, UIElement& element);
	static void RenderButton(SDL_Renderer* renderer, UIElement& element);
};

struct UIButton : UIElement
{

};

struct UIWindow
{
	std::unordered_map<std::string, size_t> buttonIndex;
	std::vector<UIButton> buttons;

	bool visible = false;
	bool destroyed = false;
};

class UIManager
{
private:
	InputSystem* inputSystem = nullptr;
	RenderingSystem* renderingSystem = nullptr;
	AudioManager* audioManager = nullptr;

private:
	std::unordered_map<std::string, size_t> m_windowIndex;
	std::vector<UIWindow> m_windows;

	void RenderButtons(std::vector<UIButton>& buttons, SDL_Renderer* renderer);

public:
	void Initialize(InputSystem& inputSystem, RenderingSystem& renderingSystem, AudioManager& audioManager);

	size_t CreateWindow(const std::string& name) // Returns index to window
	{
		m_windows.emplace_back();
		size_t index = m_windows.size() - 1;
		m_windowIndex.emplace(name, index);
		std::cout << "Created window: " << name << "\n";
		return index;
	}
	UIWindow& GetWindow(size_t index)
	{
		return m_windows[index];
	}
	UIWindow& GetWindow(const std::string& name)
	{
		if (m_windowIndex.find(name) == m_windowIndex.end())
			throw std::runtime_error("Could not find UIWindow: " + name);

		return m_windows[m_windowIndex[name]];
	}

	void SetWindowVisible(const std::string& name)
	{
		GetWindow(name).visible = true;
	}
	void DestroyWindow(const std::string& name)
	{
		GetWindow(name).destroyed = true;
	}

	void RenderWindows(SDL_Renderer* renderer);

	UIButton& WindowGetButton(UIWindow& window, const std::string& buttonName)
	{
		return window.buttons[window.buttonIndex[buttonName]];
	}

	void WindowAddButton(UIWindow& window, const UIButton& button, const std::string& buttonName)
	{
		window.buttons.emplace_back(button);
		window.buttonIndex.emplace(buttonName, window.buttons.size() - 1);
	}
	void WindowClearButtons(UIWindow& window)
	{
		window.buttonIndex.clear();
		window.buttons.clear();
	}

	void Update();
};