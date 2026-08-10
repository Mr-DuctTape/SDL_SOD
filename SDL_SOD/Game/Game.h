#pragma once
#include "../Engine/SOD_Engine.h"
#include "Entities/Player.h"
#include <string>

class AssetManager;
class EntityManager;

class RenderingSystem;
class InputSystem;

constexpr int SPRT_HEIGHT = 128;
constexpr int SPRT_WIDTH = 128;

class Game;

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

	float masterVolume = 0.0f;
	float musicVolume = 0.0f;
	float sfxVolume = 0.0f;

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

	FrameLimit frameLimit = FrameLimit::F144;

	bool vsync = true;
	bool debugMode = true;
};

class GameUI
{
private:
	Game& game;
	RenderingSystem& renderingSystem;
	AudioManager& audioManager;

	void CreateMainMenuButtons()
	{
		float buttonHeight = 100.0f;
		float buttonWidth = 300.0f;
		float offset = 200.0f;

		Vec2f startPos{ renderingSystem.renderResX * 0.5f - buttonWidth * 0.5f, 100.0f };

		UIButton button;
		button.height = buttonHeight;
		button.width = buttonWidth;
		button.displayText = "[ Start ]";
		button.screenPos = startPos;
		button.SetAudioClip(audioManager.m_audioClips["Hover"]);

		UIWindow& window = uiManager.GetWindow(mainMenu);
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
	void CreateSettingsButtons()
	{
		float buttonHeight = 100.0f;
		float buttonWidth = 300.0f;
		float offset = 50.0f;

		Vec2f startPos{ renderingSystem.renderResX * 0.5f - buttonWidth * 0.5f - 100.0f, 100.0f };

		UIWindow& window = uiManager.GetWindow(settingsMenu);

		UIButton button;
		button.displayName = true;
		button.height = 50.0f;
		button.width = 400.0f;
		button.name = "Display Mode:";
		button.displayText = "[Windowed]";
		button.screenPos = { 100.0f, startPos.y };
		button.SetAudioClip(audioManager.m_audioClips["Hover"]);

		uiManager.WindowAddButton(window, button, "DisplayMode");

		button.height = 50.0f;
		button.width = 400.0f;
		button.name = "V-Sync:";
		button.displayText = "[ON]";
		button.screenPos = { 100.0f, startPos.y += offset };

		uiManager.WindowAddButton(window, button, "VSync");

		button.height = 50.0f;
		button.width = 400.0f;
		button.name = "Debug Mode:";
		button.displayText = "[ON]";
		button.screenPos = { 100.0f, startPos.y += offset };
		uiManager.WindowAddButton(window, button, "Debug");

		button.height = 50.0f;
		button.width = 400.0f;
		button.name = "Frame Limit:";
		button.displayText = "[144]";
		button.screenPos = { 100.0f, startPos.y += offset };

		uiManager.WindowAddButton(window, button, "FrameLimit");

		button.height = 50.0f;
		button.width = 400.0f;
		button.name = "Main Volume:";
		button.displayText = "[";

		for (int i = 0; i < 12; i++)
		{
			button.displayText += '-';
		}

		button.displayText += "]";
		button.screenPos = { 100.0f, startPos.y += offset };

		uiManager.WindowAddButton(window, button, "MainSound");

		button.height = 50.0f;
		button.width = 400.0f;
		button.name = "Back";
		button.displayText = "[Back]";
		button.screenPos = { startPos.x, startPos.y + 300 };
		button.displayName = false;

		uiManager.WindowAddButton(window, button, "Back");
		window.visible = false;
	}

public:
	UIManager& uiManager;
	Menu mainMenu;
	Menu settingsMenu;
	Menu pausedMenu;

	void MenuVisible(Menu menu, bool visible)
	{
		uiManager.GetWindow(menu).visible = visible;
	}
	[[nodiscard]] bool MenuButtonPress(Menu menu, const std::string& name)
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

	void SettingsMenu(Settings& settings)
	{
		if (MenuButtonPress(settingsMenu, "DisplayMode"))
		{
			static int displayMode = 0;
			displayMode++;
			if (displayMode >= static_cast<int>(Settings::DisplayMode::DISPLAYCOUNT))
			{
				displayMode = 0;
			}

			settings.displayMode = static_cast<Settings::DisplayMode>(displayMode);

			UIWindow& window = uiManager.GetWindow(settingsMenu);
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

		if (MenuButtonPress(settingsMenu, "VSync"))
		{
			UIWindow& window = uiManager.GetWindow(settingsMenu);
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

		if (MenuButtonPress(settingsMenu, "FrameLimit"))
		{
			static int choice = 0;

			choice++;
			if (choice >= static_cast<int>(Settings::FrameLimit::COUNT) || choice < 0)
			{
				choice = 0;
			}

			UIWindow& window = uiManager.GetWindow(settingsMenu);
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

		if (MenuButtonPress(settingsMenu, "MainSound"))
		{
			UIButton& button = uiManager.WindowGetButton(uiManager.GetWindow(settingsMenu), "MainSound");

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

		if (MenuButtonPress(settingsMenu, "Debug"))
		{
			UIWindow& window = uiManager.GetWindow(settingsMenu);
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

		if (MenuButtonPress(settingsMenu, "Back"))
		{
			MenuVisible(settingsMenu, false);
			MenuVisible(mainMenu, true);
		}
		else if (MenuButtonPress(settingsMenu, "Volume"))
		{

		}
	}

	[[nodiscard]] bool MainMenu(); // returns true on quit

	GameUI(Engine& engine, Game& game)
		: 
		game(game),
		uiManager(engine.uiManager),
		renderingSystem(engine.renderingSystem),
		audioManager(engine.audioManager),
		mainMenu(uiManager.CreateWindow("MainMenu")),
		pausedMenu(uiManager.CreateWindow("PausedMenu")),
		settingsMenu(uiManager.CreateWindow("SettingsMenu"))
	{
		audioManager.CreateAudioClip("Hover", "Assets/Audio/Hover.wav");
		audioManager.CreateAudioClip("Click", "Assets/Audio/Hover.wav");
		CreateMainMenuButtons();
		CreateSettingsButtons();
	}
};

class Game
{
	// All entities are managed/destroyed by the Engine's EntityManager
private:
	Engine& m_engine;
	GameUI m_gameUI;
	Settings m_settings;

	std::vector<Player> m_players;
	std::vector<Torch> m_torches;

	std::unordered_map<std::string, Entity*> m_prefabs;
	Entity* GetPrefab(const std::string& name);

	// Effect prefabs creation
	void CreateJumpingFXPrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreateRunningFXPrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreateDashFXPrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreateExplosionFXPrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreateAllEffectPrefabs(EntityManager& entityManager, AssetManager& assetManager);

	// Prefabs creation
	void CreateSpikePrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreatePlayerPrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreateTorchPrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreateAllPrefabs(EntityManager& entityManager, AssetManager& assetManager);

	// Loading & spawning
	void LoadAllTextures(AssetManager& assetManager);
	void LoadTilemap(EntityManager& entityManager, AssetManager& assetManager, const std::string& level);
	void LoadObjects(EntityManager& entityManager, const std::string& level);
	void SpawnPlayer(EntityManager& entityManager, const Vec2f position);
	void SpawnTorch(EntityManager& entityManager, const Vec2f position);

	void Update(RenderingSystem& renderingSystem, EntityManager& entityManager,
		AudioManager& audioManager, InputSystem& inputSystem, float deltaTime);

public:
	bool playing = false;

	Game(Engine& engine) :
		m_engine(engine),
		m_gameUI(engine, *this)
	{

	}

	[[nodiscard]] Settings& GetSettings()
	{
		return m_settings;
	}

	void LoadTexturesAndPrefabs(EntityManager& entityManager, AssetManager& assetManager);
	void LoadLevel(EntityManager& entityManager, AssetManager& assetManager, const std::string& level);
	void Update();
};