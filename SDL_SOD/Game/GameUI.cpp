#include "Game.h"
#include "GameUI.h"

// --- GAME UI ---

void GameUI::CreateMainMenuButtons()
{
	constexpr float buttonHeight = 100.0f;
	constexpr float buttonWidth = 500.0f;
	constexpr float offset = 200.0f;

	Vec2f startPos{ m_renderingSystem.renderResX * 0.5f - buttonWidth * 0.5f, 100.0f };

	UIButton button;
	button.height = buttonHeight;
	button.width = buttonWidth;
	button.displayText = "[ Start ]";
	button.screenPos = startPos;
	button.SetHoverAudio(m_audioManager.GetAudio("Hover"));
	button.SetClickAudio(m_audioManager.GetAudio("Click"));

	UIWindow& window = m_uiManager.GetWindow(m_mainMenu);
	m_uiManager.WindowAddButton(window, button, "Start");

	button.height = buttonHeight;
	button.width = buttonWidth;
	button.displayText = "[ Settings ]";
	button.screenPos = { startPos.x, startPos.y += offset };

	m_uiManager.WindowAddButton(window, button, "Settings");

	button.height = buttonHeight;
	button.width = buttonWidth;
	button.displayText = "[ Quit ]";
	button.screenPos = { startPos.x, startPos.y += offset };
	m_uiManager.WindowAddButton(window, button, "Quit");

	window.visible = true;
}

void GameUI::CreateSettingsButtons()
{
	constexpr float buttonHeight = 50.0f;
	constexpr float buttonWidth = 400.0f;
	constexpr float yOffset = 75.0f;

	Vec2f startPos{ m_renderingSystem.renderResX * 0.5f - buttonWidth * 0.5f - 100.0f, 100.0f };

	UIWindow& window = m_uiManager.GetWindow(m_settingsMenu);

	UIButton button;
	button.displayName = true;
	button.height = buttonHeight;
	button.width = buttonWidth;
	button.name = "Display Mode:";
	button.displayText = "[Windowed]";
	button.screenPos = { 100.0f, startPos.y };
	button.SetClickAudio(m_audioManager.GetAudio("Click"));
	button.SetHoverAudio(m_audioManager.GetAudio("Hover"));

	m_uiManager.WindowAddButton(window, button, "DisplayMode");

	button.height = buttonHeight;
	button.width = buttonWidth;
	button.name = "V-Sync:";
	button.displayText = "[ON]";
	button.screenPos = { 100.0f, startPos.y += yOffset };

	m_uiManager.WindowAddButton(window, button, "VSync");

	button.height = buttonHeight;
	button.width = buttonWidth;
	button.name = "Debug Mode:";
	button.displayText = "[OFF]";
	button.screenPos = { 100.0f, startPos.y += yOffset };
	m_uiManager.WindowAddButton(window, button, "Debug");

	button.height = buttonHeight;
	button.width = buttonWidth;
	button.name = "Frame Limit:";
	button.displayText = "[60]";
	button.screenPos = { 100.0f, startPos.y += yOffset };

	m_uiManager.WindowAddButton(window, button, "FrameLimit");

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

	m_uiManager.WindowAddButton(window, button, "MainSound");

	button.displayText = "[" + std::string(m_audioManager.GetDeviceName(m_audioManager.m_playBackDevice)) + "]";
	button.height = buttonHeight;
	button.width = buttonWidth * 2;
	button.name = "Audio device:";
	button.screenPos = { 100.0f, startPos.y += yOffset };

	m_uiManager.WindowAddButton(window, button, "AudioDevice");

	button.height = buttonHeight + 50;
	button.width = buttonWidth;
	button.name = "Back";
	button.displayText = "[Back]";
	button.screenPos = { m_renderingSystem.renderResX * 0.5f - buttonWidth * 0.5f, startPos.y + 100};
	button.displayName = false;

	m_uiManager.WindowAddButton(window, button, "Back");
	window.visible = false;
}

void GameUI::CreatePauseMenuButtons()
{
	constexpr float buttonHeight = 100.0f;
	constexpr float buttonWidth = 500.0f;
	constexpr float offset = 200.0f;

	UIWindow& window = m_uiManager.GetWindow(m_pausedMenu);

	Vec2f startPos{ m_renderingSystem.renderResX * 0.5f - buttonWidth * 0.5f, 100.0f };

	UIButton button;
	button.height = buttonHeight;
	button.width = buttonWidth;
	button.displayText = "[ Start ]";
	button.screenPos = startPos;
	button.SetHoverAudio(m_audioManager.GetAudio("Hover"));
	button.SetClickAudio(m_audioManager.GetAudio("Click"));

	m_uiManager.WindowAddButton(window, button, "Start");

	button.height = buttonHeight;
	button.width = buttonWidth;
	button.displayText = "[ Settings ]";
	button.screenPos = { startPos.x, startPos.y += offset };

	m_uiManager.WindowAddButton(window, button, "Settings");

	button.height = buttonHeight;
	button.width = buttonWidth;
	button.displayText = "[ Quit ]";
	button.screenPos = { startPos.x, startPos.y += offset };
	m_uiManager.WindowAddButton(window, button, "Quit");

	window.visible = true;
}

bool GameUI::MenuButtonPress(Menu menu, const std::string& name)
{
	UIWindow& window = m_uiManager.GetWindow(menu);

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
		m_game.playing = true;
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
		SettingsMenu(m_game.GetSettings());
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

		UIWindow& window = m_uiManager.GetWindow(m_settingsMenu);
		UIButton& button = m_uiManager.WindowGetButton(window, "DisplayMode");

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
		UIWindow& window = m_uiManager.GetWindow(m_settingsMenu);
		UIButton& button = m_uiManager.WindowGetButton(window, "VSync");

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

		UIWindow& window = m_uiManager.GetWindow(m_settingsMenu);
		UIButton& button = m_uiManager.WindowGetButton(window, "FrameLimit");

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
		UIButton& button = m_uiManager.WindowGetButton(m_uiManager.GetWindow(m_settingsMenu), "MainSound");

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
		UIWindow& window = m_uiManager.GetWindow(m_settingsMenu);
		UIButton& button = m_uiManager.WindowGetButton(window, "Debug");

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

	if (MenuButtonPress(m_settingsMenu, "AudioDevice"))
	{
		static int choice = m_audioManager.m_playBackDevice;
		static std::vector<float> savedWidths;

		std::vector<uint32_t> devices = m_audioManager.GetAvailableDevices();
		size_t count = devices.size();
		savedWidths.resize(count);

		choice++;
		if (choice >= count) {
			choice = 0;
		}

		UIButton& button = m_uiManager.WindowGetButton(m_uiManager.GetWindow(m_settingsMenu), "AudioDevice");
		button.displayText = "[" + std::string(m_audioManager.GetDeviceName(devices[choice])) + "]";
		m_audioManager.m_playBackDevice = devices[choice];

		m_audioManager.RebindAudioDevice();
	}
}