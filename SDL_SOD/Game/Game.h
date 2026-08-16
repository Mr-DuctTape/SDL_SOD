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

	std::unordered_map<std::string, Entity*> m_prefabs;
	Entity* GetPrefab(const std::string& name);

	void CreateDialogs(DialogSystem& dialogSystem);

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

	std::tuple<
		std::vector<Torch>,
		std::optional<Player>,
		std::optional<Amber>,
		std::optional<Bob>>
		m_gameEntities;

	bool playing = false;

	Game(Engine& engine) :
		m_engine(engine),
		m_gameUI(engine, *this)
	{
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