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
	void CreateDashCoolDownFXPrefab(EntityManager& entityManager, AssetManager& assetManager);

	void CreateDashFXPrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreateExplosionFXPrefab(EntityManager& entityManager, AssetManager& assetManager);
	void CreateWallJumpFXPrefab(EntityManager& entityManager, AssetManager& assetManager);

	void CreateDashCoolDownDestroyFXPrefab(EntityManager& entityManager, AssetManager& assetManager);

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
	void LoadTilemap(EntityManager& entityManager, AssetManager& assetManager, const Vec2f offset, const std::string& level, const std::string& tileSet, bool collidable);
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
	// Wrappers referencing entities managed by EntityManager, 
	// Does not own anything

	Engine& GetEngine()
	{
		return m_engine;
	}

	struct GameEntities 
	{
		std::vector<Torch> torches;
		std::optional<Player> player;
		std::optional<Amber> amber;
		std::optional<Bob> bob;
	};

	GameEntities m_gameEntities;

	bool playing = false;

	Game(Engine& engine) :
		m_engine(engine),
		m_gameUI(engine, *this)
	{
		if constexpr (DEBUGPRINT)
			m_settings.debugMode = true;

		LoadTexturesAndPrefabs(m_engine.entityManager, m_engine.assetManager);
		CreateDialogs(m_engine.dialogSystem);

		// Edit dialogs to have correct controls

		DialogSystem::Dialog& AmberStart = m_engine.dialogSystem.GetDialog("Amber_Move_Start");
		DialogSystem::Dialog& AmberJump = m_engine.dialogSystem.GetDialog("Amber_Jump_Start");
		DialogSystem::Dialog& AmberDash = m_engine.dialogSystem.GetDialog("Amber_Dash_Start");
		DialogSystem::Dialog& AmberWallJump = m_engine.dialogSystem.GetDialog("Amber_WallJump_Start");

		auto ReplaceText = [](std::vector<std::string>& dialog,
			const std::string& from,
			const std::string& to)
			{
				for (std::string& line : dialog)
				{
					size_t pos = line.find(from);

					while (pos != std::string::npos)
					{
						line.replace(pos, from.length(), to);
						pos = line.find(from, pos + to.length());
					}
				}
			};


		SDL_GamepadType controller = m_engine.inputSystem.GetGamepadType();
		std::string jumpControl;
		std::string dashControl;
		std::string moveControl;

		switch (controller)
		{
		case SDL_GAMEPAD_TYPE_PS3:
		case SDL_GAMEPAD_TYPE_PS4:
		case SDL_GAMEPAD_TYPE_PS5:
			dashControl = "( R1 )";
			jumpControl = "( X )";
			moveControl = "( Left stick )";
			break;
		case SDL_GAMEPAD_TYPE_XBOX360:
		case SDL_GAMEPAD_TYPE_XBOXONE:
			dashControl = "( RB )";
			jumpControl = "( A )";
			moveControl = "( Left stick )";
			break;
		case SDL_GAMEPAD_TYPE_GAMECUBE:
			dashControl = "( R )";
			jumpControl = "( A )";
			moveControl = "( Control stick )";
			break;

		case SDL_GAMEPAD_TYPE_STANDARD:
		default:
			break;
		}

		if (!jumpControl.empty())
		{
			ReplaceText(AmberJump.entireDialog, "( SPACE )", jumpControl);
			ReplaceText(AmberWallJump.entireDialog, "( SPACE )", jumpControl);
		}

		if (!dashControl.empty())
			ReplaceText(AmberDash.entireDialog, "( SHIFT )", dashControl);

		if (!moveControl.empty())
		{
			ReplaceText(AmberStart.entireDialog, "( A, D )", moveControl);
			ReplaceText(AmberWallJump.entireDialog, "( A, D )", moveControl);
		}

		m_engine.renderingSystem.camera.pos = { -5000.0f, -5000.0f };
	}

	[[nodiscard]] Settings& GetSettings()
	{
		return m_settings;
	}

	void LoadTexturesAndPrefabs(EntityManager& entityManager, AssetManager& assetManager);
	void LoadLevel(EntityManager& entityManager, AssetManager& assetManager, const Vec2f offset, const std::string& level, const std::string& tileSet);
	void Update();
};