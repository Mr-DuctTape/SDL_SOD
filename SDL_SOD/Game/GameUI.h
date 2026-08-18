#pragma once
#include "../Engine/SOD_Engine.h"

using Menu = size_t;

struct Settings
{
	enum class DisplayMode
	{
		WINDOWED,
		FULLSCREEN,
		DISPLAYCOUNT
	};

	DisplayMode displayMode = DisplayMode::WINDOWED;

	enum class FrameLimit
	{
		F30,
		F60,
		F90,
		F120,
		F144,
		F200,
		F240,
		F360,
		UNLIMITED,
		COUNT
	};

	FrameLimit frameLimit = FrameLimit::F60;

	int FrameLimitToInt()
	{
		switch (frameLimit)
		{
		case Settings::FrameLimit::F30:
			return 30;
		case Settings::FrameLimit::F60:
			return 60;
		case Settings::FrameLimit::F90:
			return 90;
		case Settings::FrameLimit::F120:
			return 120;
		case Settings::FrameLimit::F144:
			return 144;
		case Settings::FrameLimit::F200:
			return 200;
		case Settings::FrameLimit::F240:
			return 240;
		case Settings::FrameLimit::F360:
			return 360;
		case Settings::FrameLimit::UNLIMITED:
			return 999999;
		}
	}

	float masterVolume = 1.0f;
	float musicVolume = 0.0f;
	float sfxVolume = 0.0f;

	bool vsync = true;
	bool debugMode = false;
};

class Game;

class GameUI
{
private:
	Game& m_game;
	RenderingSystem& m_renderingSystem;
	AudioManager& m_audioManager;

	void CreateMainMenuButtons();
	void CreateSettingsButtons();
	void CreatePauseMenuButtons();

public:
	UIManager& m_uiManager;
	
	Menu m_mainMenu;
	Menu m_settingsMenu;
	Menu m_pausedMenu;

	inline void MenuVisible(Menu menu, bool visible)
	{
		m_uiManager.GetWindow(menu).visible = visible;
	}

	//[[nodiscard]] bool MenuButtonHover(Menu menu, const std::string& name);
	[[nodiscard]] bool MenuButtonPress(Menu menu, const std::string& name);
	[[nodiscard]] bool MainMenu(); // returns true on quit

	void SettingsMenu(Settings& settings);

	GameUI(Engine& engine, Game& game)
		:
		m_game(game),
		m_uiManager(engine.uiManager),
		m_renderingSystem(engine.renderingSystem),
		m_audioManager(engine.audioManager),
		m_mainMenu(m_uiManager.CreateWindow("MainMenu")),
		m_pausedMenu(m_uiManager.CreateWindow("PauseMenu")),
		m_settingsMenu(m_uiManager.CreateWindow("SettingsMenu"))
	{
		CreateMainMenuButtons();
		CreateSettingsButtons();
	}
};
