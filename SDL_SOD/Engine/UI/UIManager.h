#pragma once
#include <SDL3/SDL.h>
#include "../Math/Vector2.h"
#include <vector>
#include <unordered_map>

class InputSystem;
class AudioManager;
class RenderingSystem;
class Camera;

struct UIElement
{
	SDL_Texture* texture = nullptr;
	bool pressed = false;

	std::string text{};
	Vec2f screenPos{};

	float height = 0.0f;
	float width = 0.0f;

	SDL_Color currentColor{ 255, 255, 255, 255 };
	SDL_Color stationaryColor{ 155, 156, 155, 255 };
	SDL_Color highlightedColor{ 125, 156, 125, 240 };
	SDL_Color pressedColor{ 100, 156, 100, 255 };

	bool MouseHoverOver(InputSystem& inputSystem);
	bool IsButtonPressed();
	static void RenderButtonText(SDL_Renderer* renderer, UIElement& element);
	static void RenderButton(SDL_Renderer* renderer, UIElement& element);
};

struct UIDropDown : UIElement
{
	std::vector<UIElement> elements;;
};

struct UIToggle : UIElement
{
};

struct UIButton : UIElement
{

};

struct UIWindow
{
	std::unordered_map<std::string, size_t> buttonIndex;
	std::vector<UIButton> buttons;

	std::unordered_map<std::string, size_t> dropdownIndex;
	std::vector<UIDropDown> dropdownButtons;

	std::unordered_map<std::string, size_t> toggleIndex;
	std::vector<UIToggle> toggles;

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
	void RenderToggles(std::vector<UIToggle>& toggles, SDL_Renderer* renderer) {};
	void RenderDropdowns(std::vector<UIDropDown>& dropDowns, SDL_Renderer* renderer);

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
		if (m_windowIndex.find(buttonName) == m_windowIndex.end())
			throw std::runtime_error("Could not find UIWindow: " + buttonName);

		return window.buttons[window.buttonIndex[buttonName]];
	}

	void WindowAddDropdown(UIWindow& window, const UIDropDown& dropDown, const std::string& dropDownName)
	{
		window.dropdownButtons.emplace_back(dropDown);
		window.dropdownIndex.emplace(dropDownName, window.dropdownButtons.size() - 1);
	}
	void WindowAddToggle(UIWindow& window, const UIToggle& toggle, const std::string& toggleName)
	{
		window.toggles.emplace_back(toggle);
		window.toggleIndex.emplace(toggleName, window.dropdownButtons.size() - 1);
	}

	void WindowAddButton(UIWindow& window, const UIButton& button, const std::string& buttonName)
	{
		window.buttons.emplace_back(button);
		window.buttonIndex.emplace(buttonName, window.buttons.size() - 1);
		std::cout << "Added button: " << buttonName << "\n";
	}
	void WindowDestroyButton(UIWindow& window, const std::string& buttonName);
	void WindowClearButtons(UIWindow& window)
	{
		window.buttonIndex.clear();
		window.buttons.clear();
	}


	void Update();
};