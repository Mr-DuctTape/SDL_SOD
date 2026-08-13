#include "Game.h"
#include "GameUI.h"

// --- GAME UI ---

void GameUI::CreateMainMenuButtons()
{
	constexpr float buttonHeight = 100.0f;
	constexpr float buttonWidth = 500.0f;
	constexpr float offset = 200.0f;

	Vec2f startPos{ renderingSystem.renderResX * 0.5f - buttonWidth * 0.5f, 100.0f };

	UIButton button;
	button.height = buttonHeight;
	button.width = buttonWidth;
	button.displayText = "[ Start ]";
	button.screenPos = startPos;
	button.SetHoverAudio(audioManager.GetAudio("Hover"));
	button.SetClickAudio(audioManager.GetAudio("Click"));

	UIWindow& window = uiManager.GetWindow(m_mainMenu);
	uiManager.WindowAddButton(window, button, "Start");

	button.height = buttonHeight;
	button.width = buttonWidth;
	button.displayText = "[ Settings ]";
	button.screenPos = { startPos.x, startPos.y += offset };

	uiManager.WindowAddButton(window, button, "Settings");

	button.height = buttonHeight;
	button.width = buttonWidth;
	button.displayText = "[ Quit ]";
	button.screenPos = { startPos.x, startPos.y += offset };
	uiManager.WindowAddButton(window, button, "Quit");

	window.visible = true;
}

void GameUI::CreateSettingsButtons()
{
	constexpr float buttonHeight = 50.0f;
	constexpr float buttonWidth = 400.0f;
	constexpr float yOffset = 75.0f;

	Vec2f startPos{ renderingSystem.renderResX * 0.5f - buttonWidth * 0.5f - 100.0f, 100.0f };

	UIWindow& window = uiManager.GetWindow(m_settingsMenu);

	UIButton button;
	button.displayName = true;
	button.height = buttonHeight;
	button.width = buttonWidth;
	button.name = "Display Mode:";
	button.displayText = "[Windowed]";
	button.screenPos = { 100.0f, startPos.y };
	button.SetClickAudio(audioManager.GetAudio("Click"));
	button.SetHoverAudio(audioManager.GetAudio("Hover"));

	uiManager.WindowAddButton(window, button, "DisplayMode");

	button.height = buttonHeight;
	button.width = buttonWidth;
	button.name = "V-Sync:";
	button.displayText = "[ON]";
	button.screenPos = { 100.0f, startPos.y += yOffset };

	uiManager.WindowAddButton(window, button, "VSync");

	button.height = buttonHeight;
	button.width = buttonWidth;
	button.name = "Debug Mode:";
	button.displayText = "[OFF]";
	button.screenPos = { 100.0f, startPos.y += yOffset };
	uiManager.WindowAddButton(window, button, "Debug");

	button.height = buttonHeight;
	button.width = buttonWidth;
	button.name = "Frame Limit:";
	button.displayText = "[60]";
	button.screenPos = { 100.0f, startPos.y += yOffset };

	uiManager.WindowAddButton(window, button, "FrameLimit");

	button.height = buttonHeight;
	button.width = buttonWidth;
	button.name = "Main Volume:";
	button.displayText = "[";

	for (int i = 0; i < 12; i++)
	{
		button.displayText += '-';
	}

	button.displayText += "]";
	button.screenPos = { 100.0f, startPos.y += yOffset };

	uiManager.WindowAddButton(window, button, "MainSound");

	button.height = buttonHeight + 50;
	button.width = buttonWidth;
	button.name = "Back";
	button.displayText = "[Back]";
	button.screenPos = { renderingSystem.renderResX * 0.5f - buttonWidth * 0.5f, startPos.y + 150};
	button.displayName = false;

	uiManager.WindowAddButton(window, button, "Back");
	window.visible = false;
}

void GameUI::CreatePauseMenuButtons()
{
	constexpr float buttonHeight = 100.0f;
	constexpr float buttonWidth = 500.0f;
	constexpr float offset = 200.0f;

	UIWindow& window = uiManager.GetWindow(m_pausedMenu);

	Vec2f startPos{ renderingSystem.renderResX * 0.5f - buttonWidth * 0.5f, 100.0f };

	UIButton button;
	button.height = buttonHeight;
	button.width = buttonWidth;
	button.displayText = "[ Start ]";
	button.screenPos = startPos;
	button.SetHoverAudio(audioManager.GetAudio("Hover"));
	button.SetClickAudio(audioManager.GetAudio("Click"));

	uiManager.WindowAddButton(window, button, "Start");

	button.height = buttonHeight;
	button.width = buttonWidth;
	button.displayText = "[ Settings ]";
	button.screenPos = { startPos.x, startPos.y += offset };

	uiManager.WindowAddButton(window, button, "Settings");

	button.height = buttonHeight;
	button.width = buttonWidth;
	button.displayText = "[ Quit ]";
	button.screenPos = { startPos.x, startPos.y += offset };
	uiManager.WindowAddButton(window, button, "Quit");

	window.visible = true;
}

bool GameUI::MenuButtonPress(Menu menu, const std::string& name)
{
	UIWindow& window = uiManager.GetWindow(menu);

	bool& pressed = window.buttons[window.buttonIndex[name]].pressed;

	if (pressed)
	{
		pressed = false;
		return true;
	}
	else
	{
		return false;
	}
}

bool GameUI::MainMenu()  // returns true on quit
{
	if (MenuButtonPress(m_mainMenu, "Start"))
	{
		MenuVisible(m_mainMenu, false);
		game.playing = true;
	}
	else if (MenuButtonPress(m_mainMenu, "Settings"))
	{
		MenuVisible(m_mainMenu, false);
		MenuVisible(m_settingsMenu, true);
	}
	else if (MenuButtonPress(m_mainMenu, "Quit"))
	{
		return true;
	}
	else
	{
		SettingsMenu(game.GetSettings());
	}
	return false;
}

void GameUI::SettingsMenu(Settings& settings)
{
	if (MenuButtonPress(m_settingsMenu, "DisplayMode"))
	{
		static int displayMode = static_cast<int>(settings.displayMode);
		displayMode++;
		if (displayMode >= static_cast<int>(Settings::DisplayMode::DISPLAYCOUNT))
		{
			displayMode = 0;
		}

		settings.displayMode = static_cast<Settings::DisplayMode>(displayMode);

		UIWindow& window = uiManager.GetWindow(m_settingsMenu);
		UIButton& button = uiManager.WindowGetButton(window, "DisplayMode");

		switch (settings.displayMode)
		{
		case Settings::DisplayMode::WINDOWED:
			button.displayText = "[WINDOWED]";
			break;
		case Settings::DisplayMode::FULLSCREEN:
			button.displayText = "[FULLSCREEN]";
			break;
		}
	}

	if (MenuButtonPress(m_settingsMenu, "VSync"))
	{
		UIWindow& window = uiManager.GetWindow(m_settingsMenu);
		UIButton& button = uiManager.WindowGetButton(window, "VSync");

		if (settings.vsync)
		{
			button.displayText = "[OFF]";
			settings.vsync = false;
		}
		else if (!settings.vsync)
		{
			button.displayText = "[ON]";
			settings.vsync = true;
		}
	}

	if (MenuButtonPress(m_settingsMenu, "FrameLimit"))
	{
		static int choice = static_cast<int>(settings.frameLimit);

		choice++;
		if (choice >= static_cast<int>(Settings::FrameLimit::COUNT) || choice < 0)
		{
			choice = 0;
		}

		UIWindow& window = uiManager.GetWindow(m_settingsMenu);
		UIButton& button = uiManager.WindowGetButton(window, "FrameLimit");

		Settings::FrameLimit frameLimit = static_cast<Settings::FrameLimit>(choice);
		switch (frameLimit)
		{
		case Settings::FrameLimit::F30:
			button.displayText = "[30]";
			break;
		case Settings::FrameLimit::F60:
			button.displayText = "[60]";
			break;
		case Settings::FrameLimit::F90:
			button.displayText = "[90]";
			break;
		case Settings::FrameLimit::F120:
			button.displayText = "[120]";
			break;
		case Settings::FrameLimit::F144:
			button.displayText = "[144]";
			break;
		case Settings::FrameLimit::F200:
			button.displayText = "[200]";
			break;
		case Settings::FrameLimit::F240:
			button.displayText = "[240]";
			break;
		case Settings::FrameLimit::F360:
			button.displayText = "[360]";
			break;
		case Settings::FrameLimit::UNLIMITED:
			button.displayText = "[UNLIMITED]";
			break;
		}
		settings.frameLimit = frameLimit;
	}

	if (MenuButtonPress(m_settingsMenu, "MainSound"))
	{
		UIButton& button = uiManager.WindowGetButton(uiManager.GetWindow(m_settingsMenu), "MainSound");

		static int level;
		level++;

		if (level > 12)
			level = 0;

		button.displayText = "[";

		for (int i = 0; i < 12; i++)
		{
			if (i < level)
				button.displayText += '#';
			else
				button.displayText += '-';
		}

		button.displayText += "]";
	}

	if (MenuButtonPress(m_settingsMenu, "Debug"))
	{
		UIWindow& window = uiManager.GetWindow(m_settingsMenu);
		UIButton& button = uiManager.WindowGetButton(window, "Debug");

		settings.debugMode = !settings.debugMode;
		switch (settings.debugMode)
		{
		case false:
			button.displayText = "[OFF]";
			break;
		case true:
			button.displayText = "[ON]";
			break;
		}
	}

	if (MenuButtonPress(m_settingsMenu, "Volume"))
	{

	}

	if (MenuButtonPress(m_settingsMenu, "Back"))
	{
		MenuVisible(m_settingsMenu, false);
		MenuVisible(m_mainMenu, true);
	}
}