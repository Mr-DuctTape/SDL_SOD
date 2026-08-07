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

class GameUI
{
private:
	UIManager& uiManager;
	RenderingSystem& renderingSystem;

	struct Menu
	{
		size_t windowIndex;
	};

	void CreateMainMenuButtons()
	{
		float buttonHeight = 100.0f;
		float buttonWidth = 300.0f;
		float offset = 200.0f;

		Vec2f startPos{ renderingSystem.renderResX * 0.5f - buttonWidth * 0.5f, 100.0f };

		UIButton button;
		button.height = buttonHeight;
		button.width = buttonWidth;
		button.text = "Start";
		button.screenPos = startPos;

		UIWindow& window = uiManager.GetWindow(mainMenu.windowIndex);
		uiManager.WindowAddButton(window, button, "Start");

		button.height = buttonHeight;
		button.width = buttonWidth;
		button.text = "Settings";
		button.screenPos = { startPos.x, startPos.y += offset };
		uiManager.WindowAddButton(window, button, "Settings");

		button.height = buttonHeight;
		button.width = buttonWidth;
		button.text = "Quit";
		button.screenPos = { startPos.x, startPos.y += offset };
		uiManager.WindowAddButton(window, button, "Quit");

		UIDropDown dropDown;
		dropDown.width = buttonWidth;
		dropDown.height = buttonHeight;
		dropDown.screenPos = { 100, 0 };

		UIElement element;
		element.text = "TESTINGGGGGGGGGG1";
		dropDown.elements.push_back(element);

		uiManager.WindowAddDropdown(window, dropDown, "Dropdown");

		window.visible = true;
	}

	void CreateSettingsButtons()
	{
		float buttonHeight = 100.0f;
		float buttonWidth = 300.0f;
		float offset = 200.0f;

		Vec2f startPos{ renderingSystem.renderResX * 0.5f - buttonWidth - 400.0f * 0.5f, 100.0f };

		UIButton button;
		button.height = buttonHeight;
		button.width = buttonWidth;
		button.text = "Resolution";
		button.screenPos = startPos;

		UIWindow& window = uiManager.GetWindow(settingsMenu.windowIndex);
		uiManager.WindowAddButton(window, button, "Resolution");

		button.height = buttonHeight;
		button.width = buttonWidth;
		button.text = "Volume";
		button.screenPos = { startPos.x, startPos.y += offset };
		uiManager.WindowAddButton(window, button, "Volume");

		button.height = buttonHeight;
		button.width = buttonWidth;
		button.text = "Back";
		button.screenPos = { startPos.x, startPos.y += offset };
		uiManager.WindowAddButton(window, button, "Back");

		window.visible = false;
	}

public:
	Menu mainMenu;
	Menu settingsMenu;
	Menu pausedMenu;

	void MenuOn(Menu& menu, bool visible)
	{
		uiManager.GetWindow(menu.windowIndex).visible = visible;
	}

	bool MenuButtonPress(Menu& menu, const std::string& name)
	{
		UIWindow& window = uiManager.GetWindow(menu.windowIndex);
		
		for (auto& button : window.buttons)
		{
			if (button.text == name && button.pressed)
			{
				button.pressed = false;
				return true;
			}
		}
		return false;
	}

	GameUI(Engine& engine)
		: uiManager(engine.uiManager),
		renderingSystem(engine.renderingSystem),
		mainMenu(uiManager.CreateWindow("MainMenu")),
		pausedMenu(uiManager.CreateWindow("PausedMenu")),
		settingsMenu(uiManager.CreateWindow("SettingsMenu"))
	{
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
		m_gameUI(engine)
	{

	}

	void Menu()
	{
		if (m_gameUI.MenuButtonPress(m_gameUI.settingsMenu, "Back"))
		{
			std::cout << "Back pressed\n";
			m_gameUI.MenuOn(m_gameUI.settingsMenu, false);
			m_gameUI.MenuOn(m_gameUI.mainMenu, true);
		}
		else if (m_gameUI.MenuButtonPress(m_gameUI.settingsMenu, "Volume"))
		{
			std::cout << "Volume\n";
		}
		
		if (m_gameUI.MenuButtonPress(m_gameUI.mainMenu, "Start"))
		{
			LoadTexturesAndPrefabs(m_engine.entityManager, m_engine.assetManager);
			LoadLevel(m_engine.entityManager, m_engine.assetManager, "Assets/Levels/Tutorial");
			m_gameUI.MenuOn(m_gameUI.mainMenu, false);
			playing = true;
		}
		else if (m_gameUI.MenuButtonPress(m_gameUI.mainMenu, "Settings"))
		{
			std::cout << "Settings pressed\n";
			m_gameUI.MenuOn(m_gameUI.mainMenu, false);
			m_gameUI.MenuOn(m_gameUI.settingsMenu, true);
		}
		else if (m_gameUI.MenuButtonPress(m_gameUI.mainMenu, "Quit"))
		{
			m_engine.Quit();
		}
	}

	void LoadTexturesAndPrefabs(EntityManager& entityManager, AssetManager& assetManager);
	void LoadLevel(EntityManager& entityManager, AssetManager& assetManager, const std::string& level);
	void Update();
};