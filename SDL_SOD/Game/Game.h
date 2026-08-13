#pragma once
#include "../Engine/SOD_Engine.h"
#include "Entities/Player.h"
#include "Entities/Torch.h"
#include "Entities/Characters.h"

#include "GameUI.h"
#include <string>

constexpr int SPRT_HEIGHT = 128;
constexpr int SPRT_WIDTH = 128;

class GameUI;
struct Settings;

class Game
{
	// All entities are managed/destroyed by the Engine's EntityManager
private:
	Engine& m_engine;
	GameUI m_gameUI;
	Settings m_settings;

	std::vector<Player> m_players;
	std::vector<Torch> m_torches;
	std::vector<NPC> m_npcs;

	std::unordered_map<std::string, Entity*> m_prefabs;
	Entity* GetPrefab(const std::string& name);


	void CreateDialogs(DialogSystem& dialogSystem)
	{
		auto CreateDialog = [&dialogSystem](const char* dialogName) -> DialogSystem::Dialog&
			{
				return dialogSystem.CreateDialog(dialogName);
			};

		// TODO: Probably need some file IO here instead

		{
			auto& dialog = CreateDialog("Amber_Intro");
			dialog.entireDialog.push_back("Oh!");
			dialog.entireDialog.push_back("You're awake!");
			dialog.entireDialog.push_back("Hi! I'm Amber.");
			dialog.entireDialog.push_back("I'm here to help you!");
			dialog.entireDialog.push_back("...");
			dialog.entireDialog.push_back("I think.");
			dialog.audioClip = m_engine.audioManager.GetAudio("Hover");
		}

		{
			auto& dialog = CreateDialog("Amber_Response");
			dialog.entireDialog.push_back("Amber is sorry about your sniffer Bob.");
			dialog.entireDialog.push_back("Don't worry, We will find it!");
			dialog.entireDialog.push_back("Just give us a moment.");
			dialog.entireDialog.push_back("Amber needs to teach!");
			dialog.entireDialog.push_back("...");
			dialog.entireDialog.push_back("Just... uhhh..");
			dialog.entireDialog.push_back("Amber says try moving around!");
			dialog.entireDialog.push_back("See? Easy!");
			dialog.entireDialog.push_back("Amber is amazed!");
			dialog.entireDialog.push_back("You're a natural!");
			dialog.audioClip = m_engine.audioManager.GetAudio("Hover");
		}

		{
			auto& dialog = CreateDialog("Bob_Intro");
			dialog.entireDialog.push_back("I am the mysterious Blob...");
			dialog.entireDialog.push_back("But friends call me BOB!");
			dialog.entireDialog.push_back("You seem quite new around here...");
			dialog.entireDialog.push_back("Anyway you're now BOB'S NEW FRIEND!");
			dialog.entireDialog.push_back("But Bob needs help with finding his missing sniffer.");
			dialog.entireDialog.push_back("Ever since he lost it..");
			dialog.entireDialog.push_back("Things haven't been smelling the same.");
			dialog.entireDialog.push_back("And Bob really misses the smells.");
			dialog.audioClip = m_engine.audioManager.GetAudio("Jump2");
		}

		{
			auto& dialog = CreateDialog("Bob_Response");
			dialog.entireDialog.push_back("Thank you Amber..");
			dialog.entireDialog.push_back("and you fire spirit.");
			dialog.entireDialog.push_back("I can't wait to smell again!");
			dialog.audioClip = m_engine.audioManager.GetAudio("Jump2");
		}
	}

	// Effect prefabs creation
	void CreateJumpingFXPrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreateRunningFXPrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreateDashFXPrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreateExplosionFXPrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreateWallJumpFXPrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreateAllEffectPrefabs(EntityManager& entityManager, AssetManager& assetManager);

	// Prefabs creation
	void CreateBobPrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreateAmberPrefab(EntityManager& entityManager, AssetManager& assetManager);

	void CreateSpikePrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreatePlayerPrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreateTorchPrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreateAllPrefabs(EntityManager& entityManager, AssetManager& assetManager);

	// Loading 
	void LoadAllTextures(AssetManager& assetManager);
	void LoadTilemap(EntityManager& entityManager, AssetManager& assetManager, const std::string& level);
	void LoadObjects(EntityManager& entityManager, const std::string& level);

	// Spawning
	void SpawnPlayer(EntityManager& entityManager, const Vec2f position);
	void SpawnTorch(EntityManager& entityManager, const Vec2f position);
	void SpawnBob(EntityManager& entityManager, const Vec2f position);
	void SpawnAmber(EntityManager& entityManager, const Vec2f position);

	void Update
	(
		RenderingSystem& renderingSystem,
		EntityManager& entityManager,
		AudioManager& audioManager,
		InputSystem& inputSystem,
		float deltaTime
	);

public:
	bool playing = false;

	Game(Engine& engine) :
		m_engine(engine),
		m_gameUI(engine, *this)
	{
		m_players.reserve(2);
		m_npcs.reserve(16);
		m_torches.reserve(16);

		LoadTexturesAndPrefabs(m_engine.entityManager, m_engine.assetManager);
		CreateDialogs(m_engine.dialogSystem);
	}

	[[nodiscard]] Settings& GetSettings()
	{
		return m_settings;
	}

	void LoadTexturesAndPrefabs(EntityManager& entityManager, AssetManager& assetManager);
	void LoadLevel(EntityManager& entityManager, AssetManager& assetManager, const std::string& level);
	void Update();
};