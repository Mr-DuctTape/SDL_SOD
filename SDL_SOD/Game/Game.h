#pragma once
#include "../Engine/SOD_Engine.h"
#include "Entities/Player.h"
#include <string>

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

class GameUI
{
private:
	Game& game;
	RenderingSystem& renderingSystem;
	AudioManager& audioManager;

	void CreateMainMenuButtons();
	void CreateSettingsButtons();

public:
	UIManager& uiManager;
	Menu mainMenu;
	Menu settingsMenu;
	Menu pausedMenu;

	void MenuVisible(Menu menu, bool visible)
	{
		uiManager.GetWindow(menu).visible = visible;
	}

	[[nodiscard]] bool MenuButtonHover(Menu menu, const std::string& name);
	[[nodiscard]] bool MenuButtonPress(Menu menu, const std::string& name);

	void SettingsMenu(Settings& settings);

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
	void CreateWallJumpFXPrefab(EntityManager& entityManager, AssetManager& assetManager);
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
		LoadTexturesAndPrefabs(m_engine.entityManager, m_engine.assetManager);
	}

	[[nodiscard]] Settings& GetSettings()
	{
		return m_settings;
	}

	void LoadTexturesAndPrefabs(EntityManager& entityManager, AssetManager& assetManager);
	void LoadLevel(EntityManager& entityManager, AssetManager& assetManager, const std::string& level);
	void Update();
};