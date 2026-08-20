#include "Game.h"
#include "GameUI.h"

Entity* Game::GetPrefab(const std::string& name)
{
	auto it = m_prefabs.find(name);
	if (it == m_prefabs.end())
	{
		std::cout << "could not find : " << name << "\n";
		return nullptr;
	}
	return it->second;
}

void Game::LoadAllTextures(AssetManager& assetManager)
{
	assetManager.CreateTexture("TileSet", "Assets/Textures/tiles.png");

	assetManager.CreateTexture("SpikeTexture", "Assets/Textures/Spike.png");

	// Torch Textures
	assetManager.CreateTexture("TorchOFF", "Assets/Textures/TorchOff.png");
	assetManager.CreateTexture("TorchLIT", "Assets/Textures/TorchLit.png");
	assetManager.CreateTexture("TorchON", "Assets/Textures/TorchFire.png");

	// Player Textures
	assetManager.CreateTexture("PlayerRun", "Assets/Textures/Run.png");
	assetManager.CreateTexture("PlayerIdle", "Assets/Textures/Idle.png");
	assetManager.CreateTexture("PlayerJump", "Assets/Textures/Jump.png");
	assetManager.CreateTexture("PlayerWallIdle", "Assets/Textures/WallIdle.png");

	// Bob Textures
	assetManager.CreateTexture("Bob", "Assets/Textures/Bob.png");

	// Amber Textures
	assetManager.CreateTexture("Amber", "Assets/Textures/Amber.png");

	// Effect Textures
	assetManager.CreateTexture("WallJumpEffect", "Assets/Textures/WallJumpEffect.png");
	assetManager.CreateTexture("RunningEffect", "Assets/Textures/Effect.png");
	assetManager.CreateTexture("JumpEffect", "Assets/Textures/JumpFX.png");
	assetManager.CreateTexture("DashEffect", "Assets/Textures/Dash.png");
	assetManager.CreateTexture("ExplosionEffect", "Assets/Textures/Explosion.png");
}

void Game::CreateJumpingFXPrefab(EntityManager& entityManager, AssetManager& assetManager)
{
	Entity& jumpFxObj = entityManager.CreateEntity();
	jumpFxObj.AddComponent<Transform>()->position = { -999999.0f, -999999.0f };

	Sprite* jmpFxSprt = jumpFxObj.AddComponent<Sprite>();
	jmpFxSprt->height = SPRT_HEIGHT;
	jmpFxSprt->width = SPRT_WIDTH;

	Animator* jmpfxAnimator = jumpFxObj.AddComponent<Animator>();
	jmpfxAnimator->CreateAnimation("JumpFX", 6, 16, 16, assetManager.GetTexture("JumpEffect"));
	jmpfxAnimator->SetAnimation("JumpFX");
	jmpfxAnimator->update = false;
	jmpfxAnimator->effectBase = true;
	jmpfxAnimator->speed = 0.1f;
	jmpfxAnimator->scaleAnimationX = 1.0f;
	jmpfxAnimator->scaleAnimationY = 1.0f;

	m_prefabs.emplace("JumpFX", &jumpFxObj);
}
void Game::CreateWallJumpFXPrefab(EntityManager& entityManager, AssetManager& assetManager)
{
	Entity& jumpFxObj = entityManager.CreateEntity();
	jumpFxObj.AddComponent<Transform>()->position = { -999999.0f, -999999.0f };

	Sprite* jmpFxSprt = jumpFxObj.AddComponent<Sprite>();
	jmpFxSprt->height = SPRT_HEIGHT;
	jmpFxSprt->width = SPRT_WIDTH;

	Animator* jmpfxAnimator = jumpFxObj.AddComponent<Animator>();
	jmpfxAnimator->CreateAnimation("WallJumpFX", 6, 16, 16, assetManager.GetTexture("WallJumpEffect"));
	jmpfxAnimator->SetAnimation("WallJumpFX");
	jmpfxAnimator->update = false;
	jmpfxAnimator->effectBase = true;
	jmpfxAnimator->speed = 0.1f;
	jmpfxAnimator->scaleAnimationX = 1.0f;
	jmpfxAnimator->scaleAnimationY = 1.0f;

	m_prefabs.emplace("WallJumpFX", &jumpFxObj);
}
void Game::CreateRunningFXPrefab(EntityManager& entityManager, AssetManager& assetManager)
{
	Entity& runningEffectObj = entityManager.CreateEntity();
	runningEffectObj.AddComponent<EntityTag>()->name = "RunningFX";
	runningEffectObj.AddComponent<Transform>()->position = { -999999.0f, -999999.0f };

	Sprite* runningEffectSprite = runningEffectObj.AddComponent<Sprite>();
	runningEffectSprite->height = SPRT_HEIGHT;
	runningEffectSprite->width = SPRT_WIDTH;

	Animator* runningObjAnimator = runningEffectObj.AddComponent<Animator>();
	runningObjAnimator->CreateAnimation("RunningFX", 5, 16, 16, assetManager.GetTexture("RunningEffect"));
	runningObjAnimator->SetAnimation("RunningFX");
	runningObjAnimator->speed = 0.15f;
	runningObjAnimator->scaleAnimationX = 1.0f;
	runningObjAnimator->scaleAnimationY = 1.0f;
	runningObjAnimator->update = false;

	m_prefabs.emplace("RunFX", &runningEffectObj);
}
void Game::CreateDashFXPrefab(EntityManager& entityManager, AssetManager& assetManager)
{
	Entity& dashEffectObj = entityManager.CreateEntity();
	dashEffectObj.AddComponent<EntityTag>()->name = "RunningFX";
	dashEffectObj.AddComponent<Transform>()->position = { -999999.0f, -999999.0f };

	Sprite* dashEffectSprite = dashEffectObj.AddComponent<Sprite>();
	dashEffectSprite->height = SPRT_HEIGHT;
	dashEffectSprite->width = SPRT_WIDTH;

	Animator* dashEffectAnimator = dashEffectObj.AddComponent<Animator>();
	dashEffectAnimator->CreateAnimation("DashFX", 6, 16, 16, assetManager.GetTexture("DashEffect"));
	dashEffectAnimator->SetAnimation("DashFX");
	dashEffectAnimator->speed = 0.1f;
	dashEffectAnimator->scaleAnimationX = 1.0f;
	dashEffectAnimator->scaleAnimationY = 1.0f;
	dashEffectAnimator->update = false;

	m_prefabs.emplace("DashFX", &dashEffectObj);
}
void Game::CreateExplosionFXPrefab(EntityManager& entityManager, AssetManager& assetManager)
{
	Entity& explosionEffectObj = entityManager.CreateEntity();
	explosionEffectObj.AddComponent<EntityTag>()->name = "RunningFX";
	explosionEffectObj.AddComponent<Transform>()->position = { -999999.0f, -999999.0f };

	Sprite* explosionEffectSprite = explosionEffectObj.AddComponent<Sprite>();
	explosionEffectSprite->height = SPRT_HEIGHT;
	explosionEffectSprite->width = SPRT_WIDTH;

	Animator* explosionEffectAnimator = explosionEffectObj.AddComponent<Animator>();
	explosionEffectAnimator->CreateAnimation("ExplosionFX", 6, 16, 16, assetManager.GetTexture("ExplosionEffect"));
	explosionEffectAnimator->SetAnimation("ExplosionFX");
	explosionEffectAnimator->speed = 0.15f;
	explosionEffectAnimator->scaleAnimationX = 1.0f;
	explosionEffectAnimator->scaleAnimationY = 1.0f;
	explosionEffectAnimator->update = false;

	m_prefabs.emplace("DeathFX", &explosionEffectObj);
}
void Game::CreateAllEffectPrefabs(EntityManager& entityManager, AssetManager& assetManager)
{
	CreateWallJumpFXPrefab(entityManager, assetManager);
	CreateJumpingFXPrefab(entityManager, assetManager);
	CreateRunningFXPrefab(entityManager, assetManager);
	CreateDashFXPrefab(entityManager, assetManager);
	CreateExplosionFXPrefab(entityManager, assetManager);
}

void Game::CreateSpikePrefab(EntityManager& entityManager, AssetManager& assetManager)
{
	Entity& spikeObject = entityManager.CreateEntity();
	spikeObject.AddComponent<EntityTag>()->name = "Spikes";

	spikeObject.AddComponent<Transform>()->position = { -999999.0f, -999999.0f };
	BoxCollider2D* spikeCollider = spikeObject.AddComponent<BoxCollider2D>();

	Sprite* spikeSprite = spikeObject.AddComponent<Sprite>();
	spikeCollider->height = 32;
	spikeCollider->width = 70;
	spikeCollider->offsetY = 128;
	spikeCollider->offsetX = 45;
	spikeSprite->texture = assetManager.GetTexture("SpikeTexture");

	m_prefabs.emplace("Spikes", &spikeObject);
}
void Game::CreatePlayerPrefab(EntityManager& entityManager, AssetManager& assetManager)
{
	Entity& playerObject = entityManager.CreateEntity();

	playerObject.AddComponent<Physics2D>();
	playerObject.AddComponent<EntityTag>()->name = "Player";

	Sprite* playerSprite = playerObject.AddComponent<Sprite>();
	playerSprite->height = SPRT_HEIGHT;
	playerSprite->width = SPRT_WIDTH;

	Transform* playerTransform = playerObject.AddComponent<Transform>();
	playerTransform->position = { -999999.0f, -999999.0f };

	Animator* playerAnimator = playerObject.AddComponent<Animator>();
	playerAnimator->CreateAnimation("Run", 4, 16, 16, assetManager.GetTexture("PlayerRun"));
	playerAnimator->CreateAnimation("Idle", 4, 16, 16, assetManager.GetTexture("PlayerIdle"));
	playerAnimator->CreateAnimation("Jump", 3, 16, 16, assetManager.GetTexture("PlayerJump"));
	playerAnimator->CreateAnimation("WallIdle", 4, 16, 16, assetManager.GetTexture("PlayerWallIdle"));

	playerAnimator->SetAnimation("Idle");
	playerAnimator->speed = 0.15f;
	playerAnimator->scaleAnimationX = 1.0f;
	playerAnimator->scaleAnimationY = 1.0f;

	BoxCollider2D* playerCollider = playerObject.AddComponent<BoxCollider2D>();
	playerCollider->width = 72;
	playerCollider->height = 64;
	playerCollider->offsetX = 28;
	playerCollider->offsetY = 64;

	m_prefabs.emplace("Player", &playerObject);
}
void Game::CreateBobPrefab(EntityManager& entityManager, AssetManager& assetManager)
{
	Entity& torchObject = entityManager.CreateEntity();
	torchObject.AddComponent<EntityTag>()->name = "Bob";
	torchObject.AddComponent<Transform>()->position = { -99999.0f, -9999.0f };

	Sprite* torchSprite = torchObject.AddComponent<Sprite>();
	torchSprite->height = SPRT_HEIGHT;
	torchSprite->width = SPRT_WIDTH;

	Animator* torchAnimator = torchObject.AddComponent<Animator>();
	torchAnimator->effectBase = true;
	torchAnimator->update = true;
	torchAnimator->scaleAnimationX = 1.0f;
	torchAnimator->scaleAnimationY = 1.0f;
	torchAnimator->speed = 0.15f;

	torchAnimator->CreateAnimation("BobIdle", 4, 16, 16, assetManager.GetTexture("Bob"));
	torchAnimator->SetAnimation("BobIdle");

	m_prefabs.emplace("Bob", &torchObject);
}
void Game::CreateAmberPrefab(EntityManager& entityManager, AssetManager& assetManager)
{
	Entity& emberObj = entityManager.CreateEntity();
	emberObj.AddComponent<EntityTag>()->name = "Amber";
	emberObj.AddComponent<Transform>()->position = { -99999.0f, -9999.0f };

	Sprite* emberSprite = emberObj.AddComponent<Sprite>();
	emberSprite->height = SPRT_HEIGHT;
	emberSprite->width = SPRT_WIDTH;

	Animator* emberAnimator = emberObj.AddComponent<Animator>();
	emberAnimator->effectBase = true;
	emberAnimator->update = true;
	emberAnimator->scaleAnimationX = 1.0f;
	emberAnimator->scaleAnimationY = 1.0f;
	emberAnimator->speed = 0.15f;

	emberAnimator->CreateAnimation("AmberIdle", 4, 16, 16, assetManager.GetTexture("Amber"));
	emberAnimator->SetAnimation("AmberIdle");

	m_prefabs.emplace("Amber", &emberObj);
}
void Game::CreateTorchPrefab(EntityManager& entityManager, AssetManager& assetManager)
{
	Entity& torchObject = entityManager.CreateEntity();
	torchObject.AddComponent<EntityTag>()->name = "Torch";
	torchObject.AddComponent<Transform>()->position = { -99999.0f, -9999.0f };

	Sprite* torchSprite = torchObject.AddComponent<Sprite>();
	torchSprite->height = SPRT_HEIGHT;
	torchSprite->width = SPRT_WIDTH;

	Animator* torchAnimator = torchObject.AddComponent<Animator>();
	torchAnimator->effectBase = true;
	torchAnimator->update = true;
	torchAnimator->scaleAnimationX = 1.0f;
	torchAnimator->scaleAnimationY = 1.0f;
	torchAnimator->speed = 0.15f;

	torchAnimator->CreateAnimation("TorchOFF", 1, 16, 16, assetManager.GetTexture("TorchOFF"));
	torchAnimator->CreateAnimation("TorchLIT", 4, 16, 16, assetManager.GetTexture("TorchLIT"));
	torchAnimator->CreateAnimation("TorchON", 4, 16, 16, assetManager.GetTexture("TorchON"));
	torchAnimator->SetAnimation("TorchOFF");

	m_prefabs.emplace("Torch", &torchObject);
}

void Game::CreateAllPrefabs(EntityManager& entityManager, AssetManager& assetManager)
{
	CreateSpikePrefab(entityManager, assetManager);
	CreatePlayerPrefab(entityManager, assetManager);
	CreateTorchPrefab(entityManager, assetManager);

	CreateAmberPrefab(entityManager, assetManager);
	CreateBobPrefab(entityManager, assetManager);
}

void Game::SpawnPlayer(EntityManager& entityManager, const Vec2f position)
{
	auto& player = m_gameEntities.player;
	if (player){
		player.value().m_transform.position = position;
		return;
	}

	Entity& entity = entityManager.CreateEntity(GetPrefab("Player"));
	entity.GetComponent<Transform>().position = position;
	player.emplace(Player(entity));
}

void Game::SpawnBob(EntityManager& entityManager, const Vec2f position)
{
	auto& bob = m_gameEntities.bob;
	if (bob){
		bob.value().m_transform.position = position;
		return;
	}

	Entity& entity = entityManager.CreateEntity(GetPrefab("Bob"));
	entity.GetComponent<Transform>().position = position;

	bob.emplace(Bob(entity, m_engine.dialogSystem));
}

void Game::SpawnAmber(EntityManager& entityManager, const Vec2f position)
{
	auto& amber = m_gameEntities.amber;
	if (amber){
		amber.value().m_transform.position = position;
		return;
	}

	Entity& entity = entityManager.CreateEntity(GetPrefab("Amber"));
	entity.GetComponent<Transform>().position = position;

	amber.emplace(Amber(entity, m_engine.dialogSystem));
}

void Game::SpawnTorch(EntityManager& entityManager, const Vec2f position)
{
	Entity& entity = entityManager.CreateEntity(GetPrefab("Torch"));
	entity.GetComponent<Transform>().position = position;

	auto& torches = m_gameEntities.torches;
	torches.emplace_back(Torch(entity));
}

void Game::LoadTexturesAndPrefabs(EntityManager& entityManager, AssetManager& assetManager)
{
	LoadAllTextures(assetManager);
	CreateAllPrefabs(entityManager, assetManager);
	CreateAllEffectPrefabs(entityManager, assetManager);
}

void Game::LoadLevel(EntityManager& entityManager, AssetManager& assetManager, const std::string& level)
{
	LoadTilemap(entityManager, assetManager, level);
	LoadObjects(entityManager, level);
}

void Game::Update(RenderingSystem& renderingSystem, EntityManager& entityManager, AudioManager& audioManager, InputSystem& inputSystem, float deltaTime)
{
	Entity* deathFX = GetPrefab("DeathFX");
	Entity* jumpFX = GetPrefab("JumpFX");
	Entity* runFX = GetPrefab("RunFX");
	Entity* dashFX = GetPrefab("DashFX");
	Entity* wallJumpFX = GetPrefab("WallJumpFX");

	if (!deathFX || !jumpFX || !runFX || !dashFX || !wallJumpFX)
	{
		std::cout << "(ERROR): Unable to find effect prefabs\n";
		return;
	}

	auto& player = m_gameEntities.player;
	if (!player.has_value())
	{
		std::cout << "player doesn't exist!\n";
		return;
	}

	player.value().Update(renderingSystem, entityManager, audioManager,
		inputSystem, *deathFX, *runFX, *wallJumpFX, *jumpFX, *dashFX,
		m_settings.masterVolume, deltaTime);

	for (Torch& torch : m_gameEntities.torches)
	{
		torch.Update(m_engine.audioManager, player.value());
	}

	auto& bob = m_gameEntities.bob;
	if (bob){
		bob.value().Update(m_engine, player.value(), deltaTime);
	}

	auto& amber = m_gameEntities.amber;
	if (amber){
		amber.value().Update(m_engine, player.value(), deltaTime);
	}
}

void Game::Update()
{
	static bool loaded = false;
	if (playing)
	{
		if (m_engine.inputSystem.GetButtonDown(SDL_SCANCODE_ESCAPE))
		{
			m_gameUI.MenuVisible(m_gameUI.m_mainMenu, true);
		}

		if (!loaded) // TODO: in the future there will be different levels loaded so keep that in mind.
		{
			LoadLevel(m_engine.entityManager, m_engine.assetManager, "Assets/Levels/Tutorial");
			SpawnPlayer(m_engine.entityManager, Vec2f{ 290.3f, 300.0f });

			SpawnTorch(m_engine.entityManager, Vec2f{ 5197.52f, 800.0f });
			SpawnTorch(m_engine.entityManager, Vec2f{ 11852.8f, 3650.0f });

			SpawnBob(m_engine.entityManager, Vec2f{ 14206.9f, 2145.0f });
			SpawnAmber(m_engine.entityManager, Vec2f{ 8000.0f, -600.0f });
			loaded = true;
		}

		Update(m_engine.renderingSystem, m_engine.entityManager,
			m_engine.audioManager, m_engine.inputSystem, m_engine.deltaTime);
	}
	else if (m_gameUI.MainMenu()){
		m_engine.Quit();
	}
}

void Game::LoadTilemap(EntityManager& entityManager, AssetManager& assetManager, const std::string& level)
{
	std::filesystem::path levelPath = level;
	if (!std::filesystem::is_directory(levelPath))
	{
		std::cout << "(ERROR) Level path is not a directory: " << level << "\n";
		return;
	}
	std::string tilemapFile;
	for (auto const& dir_entry : std::filesystem::directory_iterator{ levelPath })
	{
		if (dir_entry.path().extension() == ".sodmap")
		{
			tilemapFile = dir_entry.path().string();
			break;
		}
	}
	if (tilemapFile.empty())
	{
		std::cout << "Could not find .sodobj file : " << level << "\n";
		return;
	}


	Entity& tilemapObj = entityManager.CreateEntity();
	tilemapObj.AddComponent<Transform>()->position = { 100.0f, 200.0f };

	auto& tileMap = *tilemapObj.AddComponent<TileMap>();
	tileMap.LoadTileMap(tilemapFile);
	tileMap.SetTilePixelSize(16, 16);
	tileMap.SetTileScale(8.0f, 8.0f);
	tileMap.SetSpriteSheet(assetManager.GetTexture("TileSet"));

	TileMap::TileProperties properties;
	properties.solid = true;
	for (int i = 0; i < 26; i++)
	{
		if (i == 3 || i == 4 || i == 15)
			continue;

		tileMap.SetTileProperties(i, properties);
	}

	TileMap::TileScale tilemapScale = tileMap.GetTileScale();

}
void Game::LoadObjects(EntityManager& entityManager, const std::string& level)
{
	std::filesystem::path levelPath = level;
	if (!std::filesystem::is_directory(levelPath))
	{
		std::cout << "(ERROR) Level path is not a directory of the level: " << level << "\n";
		return;
	}
	std::string objFile;
	for (auto const& dir_entry : std::filesystem::directory_iterator{ levelPath })
	{
		if (dir_entry.path().extension() == ".sodobj")
		{
			objFile = dir_entry.path().string();
			break;
		}
	}
	if (objFile.empty())
	{
		std::cout << "Could not find .sodobj file\n";
		return;
	}

	Entity* prefab = GetPrefab("Spikes");
	if (!prefab)
	{
		std::cout << "Spikes Prefab not found! : " << level << "\n";
		return;
	}
	entityManager.CreateEntitiesFromObj(objFile, "Spikes", *prefab);
}
void Game::CreateDialogs(DialogSystem& dialogSystem)
{
	auto CreateDialog = [&dialogSystem](const char* dialogName) -> DialogSystem::Dialog&
		{
			return dialogSystem.CreateDialog(dialogName);
		};

	DialogSystem::Dialog* dialog = nullptr;
	auto AddLine = [&dialog](const char* str) -> void
		{
			dialog->entireDialog.emplace_back(str);
		};

	AudioManager& audioManager = m_engine.audioManager;
	auto SetAudio = [&dialog, &audioManager](const char* name) -> void
		{
			dialog->audioClip = audioManager.GetAudio(name);
		};


	const char* AmberVoiceLine = "Click";
	const char* BobVoiceLine = "Jump2";

	dialog = &CreateDialog("Amber_Intro");
	AddLine("You're awake!"); 
	AddLine("Hi! I'm Amber.");
	AddLine("Amber will help you learn!");
	SetAudio(AmberVoiceLine);

	dialog = &CreateDialog("Amber_Move_Start");
	AddLine("Try to move!");
	AddLine("Use your ( A,D ) keys.");
	SetAudio(AmberVoiceLine);

	dialog = &CreateDialog("Amber_Move_End");
	AddLine("That's it!");
	AddLine("Amber is proud!");
	SetAudio(AmberVoiceLine);

	dialog = &CreateDialog("Amber_Jump_Start");
	AddLine("Amber thinks you should jump!");
	AddLine("Press your ( SPACE ) key to jump!");
	SetAudio(AmberVoiceLine);

	dialog = &CreateDialog("Amber_Jump_End");
	AddLine("Perfect!");
	AddLine("Now put these two together!");
	SetAudio(AmberVoiceLine);

	dialog = &CreateDialog("Amber_Spikes_Start");
	AddLine("SPIKES are bad..");
	AddLine("Jump over them!");
	SetAudio(AmberVoiceLine);

	dialog = &CreateDialog("Amber_Spikes_End");
	SetAudio(AmberVoiceLine);

	dialog = &CreateDialog("Amber_Torch_Start");
	AddLine("Torches light up when you get close!");
	AddLine("They allow you to respawn at them!");
	SetAudio(AmberVoiceLine);

	dialog = &CreateDialog("Amber_Dash_Start");
	AddLine("You can't jump over this one.");
	AddLine("Jump and pair it with a dash!");
	AddLine("To dash press ( SHIFT )");
	SetAudio(AmberVoiceLine);

	dialog = &CreateDialog("Amber_Dash_End");
	AddLine("Amazing!");
	SetAudio(AmberVoiceLine);

	dialog = &CreateDialog("Amber_Fall");
	AddLine("Jump down.");
	AddLine("Amber will catch you!");
	AddLine("Probably.");
	SetAudio(AmberVoiceLine);

	dialog = &CreateDialog("Amber_WallJump_Start");
	AddLine("Now for Amber's favorite.");
	AddLine("Wall jumping!");
	AddLine("You can cling to walls with ( A,D )");
	AddLine("Then you can jump off them with ( SPACE )");
	SetAudio(AmberVoiceLine);

	dialog = &CreateDialog("Amber_WallJump_End");
	AddLine("You did it!");
	AddLine("Amber is not needed anymore...");
	AddLine("But don't worry!");
	AddLine("Amber will always be here!");
	SetAudio(AmberVoiceLine);

	dialog = &CreateDialog("Amber_Response");
	AddLine("Yes Amber is teaching!");
	AddLine("Amber will teach everything!");
	SetAudio(AmberVoiceLine);

	dialog = &CreateDialog("Amber_Tutorial_End");
	AddLine("If you got this as a playtest there is sadly nothing more.");
	AddLine("Feel free to move around and test out everything you learned.");
	AddLine("Amber is looking forward to seeing you again!");
	SetAudio(AmberVoiceLine);


	// Bob voice lines

	dialog = &CreateDialog("Bob_Intro");
	AddLine("I am the mysterious Blob...");
	AddLine("But friends call me BOB!");
	AddLine("You seem quite new around here..");
	AddLine("No matter! You're now Bob's friend!");
	AddLine("Amber are you teaching my friend?");
	SetAudio(BobVoiceLine);

	dialog = &CreateDialog("Bob_Response");
	AddLine("Whenever Amber is done teaching you.");
	AddLine("Come meet me here!");
	AddLine("I need help with something important.");
	SetAudio(BobVoiceLine);
}