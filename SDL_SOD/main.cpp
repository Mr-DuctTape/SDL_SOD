#include <iostream>
#include "Engine/SOD_Engine.h"

#define SPRT_HEIGHT 128
#define SPRT_WIDTH 128

bool IsCoyoteAvailable(float deltaTime, float cTime, float& cTimer, bool& coyote, bool grounded)
{
	if (!grounded)
	{
		cTimer += deltaTime;
		if (cTimer < cTime)
		{
			coyote = true;
		}
		else
		{
			coyote = false;
		}
	}
	if (grounded)
	{
		cTimer = 0.0f;
	}

	return coyote;
}
bool IsJumpBufferRunnning(float deltaTime, float jTime, float& jTimer, bool& jumpBuffer)
{
	if (jTimer < jTime)
	{
		jTimer += deltaTime;
		jumpBuffer = true;
	}

	if (jTimer >= jTime)
	{
		jumpBuffer = false;
		jTimer = 0;
	}

	return jumpBuffer;
}

void ChangePlayerAnimatorStates(Animator* animator, float speed)
{
	if (!animator) return;

	if (animator->currentState == "Jump" && animator->finished)
	{
		if (speed > 70)
		{
			animator->SetAnimation("Run");
			animator->currentState = "Run";
		}
		else
		{
			animator->SetAnimation("Idle");
			animator->currentState = "Idle";
		}

	}
	if (animator->currentState == "Idle")
	{
		if (speed > 70)
		{
			animator->SetAnimation("Run");
			animator->currentState = "Run";
		}

		animator->speed = 0.16f;
	}
	else if (animator->currentState == "Run")
	{
		if (speed < 60)
		{
			animator->SetAnimation("Idle");
			animator->currentState = "Idle";
		}

		animator->speed = 0.1f;
	}
}
void SpawnRunningEffect(EntityManager& entityManager, Entity* effect, bool isGrounded, float deltaTime, Transform* transform, bool flippedX)
{
	if (!effect || !transform)
		return;

	static float vfxTimer = 0.0f;
	float vfxTime = 0.30f;

	// visual effect timer
	vfxTimer += deltaTime;
	if (vfxTimer >= vfxTime && isGrounded)
	{
		// Create visual effect
		auto& vfxObject = entityManager.CreateEntity(*effect);
		auto vfxTransform = vfxObject.GetComponent<Transform>();

		if (!vfxTransform) return;

		if (flippedX)
			vfxTransform->position = { transform->position.x + 10.0f, transform->position.y };
		else
			vfxTransform->position = { transform->position.x - 10.0f, transform->position.y };

		auto vfxAnimator = vfxObject.GetComponent<Animator>();
		if (!vfxAnimator) return;
		vfxAnimator->destroyOnFinish = true;
		vfxAnimator->update = true;
		vfxAnimator->finished = false;
		vfxAnimator->flippedX = flippedX;
		vfxTimer = 0.0f;
	}
}
void SpawnEffect(EntityManager& entityManager, Entity* effect, Vec2f transform, bool flippedX)
{
	if (!effect)
		return;

	Entity& fxObj = entityManager.CreateEntity(effect);

	Animator* fxAnimator = fxObj.GetComponent<Animator>();
	Transform* fxTransform = fxObj.GetComponent<Transform>();

	if (!fxAnimator || !fxTransform)
		return;

	fxTransform->position = { transform.x - 10, transform.y };

	fxAnimator->update = true;
	fxAnimator->destroyOnFinish = true;
	fxAnimator->finished = false;
	fxAnimator->flippedX = flippedX;
	fxAnimator->timer = 0.0f;
}
void PlayerDash(EntityManager& entityManager, InputSystem& inputSystem, Entity* dashEffect, Transform* transform, Animator* animator, Physics2D* physics, Vec2f accel, float deltaTime)
{
	static bool dashing = false;
	static float dashCoolDownTimer = 0.0f;
	constexpr float dashCoolDownTime = 1.5f;

	if (dashing && dashCoolDownTimer < dashCoolDownTime)
	{
		dashCoolDownTimer += deltaTime;
		if (dashCoolDownTimer >= dashCoolDownTime)
		{
			dashing = false;
			dashCoolDownTimer = 0.0f;
		}
	}

	if (inputSystem.GetButtonDown(SDL_SCANCODE_LSHIFT) && !dashing)
	{
		float dirX = (accel.normalized().x > 0.0f) ? 1.0f : -1.0f;
		Vec2f force = { dirX * 4000.0f, 0.0f };
		physics->AddForce(force);
		dashing = true;
		Vec2f spawnPos = transform->position;
		spawnPos.y += 25.0f;
		if (dirX >= 1.0f)
			spawnPos.x += 100.0f;
		else
			spawnPos.x -= 100.0f;

		SpawnEffect(entityManager, dashEffect, spawnPos, animator->flippedX);
	}
}
void PlayerWallJump(EntityManager& entityManager, InputSystem& inputSystem, Entity* effect, BoxCollider2D* boxCollider, Transform* transform, bool& gatherBuffer, Physics2D* physics, float deltaTime);
void PlayerMovement(InputSystem& inputSystem, EntityManager& entityManager, Entity* player, Entity* runningEffect, Entity* jumpEffect, Entity* dashEffect, float deltaTime)
{
	Transform* transform = player->GetComponent<Transform>();
	Physics2D* physics = player->GetComponent<Physics2D>();
	BoxCollider2D* boxCollider2D = player->GetComponent<BoxCollider2D>();
	Animator* animator = player->GetComponent<Animator>();
	if (!transform || !physics || !boxCollider2D || !animator) return;

	Vec2f accel = { 0, 0 };
	Vec2f force = { 0, 0 };

	bool isGrounded = boxCollider2D->groundCollision;
	float movementSpeed = 3500.0f;

	if (!isGrounded)
	{
		movementSpeed = movementSpeed * 1.25f;
	}

	static float coyoteTimer = 0.0f;
	float coyoteTime = 0.10f;
	bool coyoteAvailable = false;
	IsCoyoteAvailable(deltaTime, coyoteTime, coyoteTimer, coyoteAvailable, isGrounded);

	float speed = physics->velocity.Magnitude();
	ChangePlayerAnimatorStates(animator, speed);

	if (animator->currentState == "Run")
	{
		SpawnRunningEffect(entityManager, runningEffect, isGrounded, deltaTime, transform, animator->flippedX);
	}

	if (inputSystem.GetButton(SDL_SCANCODE_W))
	{
		accel.y -= movementSpeed;
	}

	static float jumpBufferTimer = 0.0f;
	static bool gatherBuffer = false;
	float jumpBufferTime = 0.2f; 
	bool jumpBuffer = false;

	if (inputSystem.GetButtonDown(SDL_SCANCODE_SPACE) && gatherBuffer)
	{
		gatherBuffer = false;
	}
	else if (inputSystem.GetButtonDown(SDL_SCANCODE_SPACE))
	{
		gatherBuffer = true;
		jumpBufferTimer = 0.0f;
	}

	if (gatherBuffer)
	{
		gatherBuffer = IsJumpBufferRunnning(deltaTime, jumpBufferTime, jumpBufferTimer, jumpBuffer);
	}

	bool jumpAvailable = isGrounded || coyoteAvailable;
	if (gatherBuffer && jumpAvailable)
	{
		gatherBuffer = false;
		jumpBufferTimer = 0.0f;

		animator->SetAnimation("Jump");
		SpawnEffect(entityManager, jumpEffect, transform->position, animator->flippedX);
		force.y -= 4000.0f;
	}

	if (inputSystem.GetButton(SDL_SCANCODE_A))
	{
		animator->flippedX = true;
		accel.x -= movementSpeed;
	}

	if (inputSystem.GetButton(SDL_SCANCODE_D))
	{

		animator->flippedX = false;
		accel.x += movementSpeed;
	}

	PlayerDash(entityManager, inputSystem, dashEffect, transform, animator, physics, accel, deltaTime);;
	PlayerWallJump(entityManager, inputSystem, jumpEffect, boxCollider2D, transform, gatherBuffer, physics, deltaTime);

	physics->Accelerate(accel);
	physics->AddForce(force);
}
void PlayerDeath(Entity& player, Entity* effect, EntityManager& entityManager) {

	Transform* playerTransform = player.GetComponent<Transform>();
	if (!playerTransform) return;

	auto& deathFX = entityManager.CreateEntity(*effect);
	auto fxTransform = deathFX.GetComponent<Transform>();
	if (!fxTransform) return;

	*fxTransform = *playerTransform;

	Animator* fxAnim = deathFX.GetComponent<Animator>();
	if (!fxAnim) return;

	fxAnim->destroyOnFinish = true;
	fxAnim->update = true;
	fxAnim->finished = false;
	playerTransform->position = Vec2f{ 7500.0f, 700.0f };
}
void PlayerBounds(Entity& player, Entity* effect, RenderingSystem& renderingSystem, EntityManager& entityManager) {
	Transform* playerTransform = player.GetComponent<Transform>();
	if (!playerTransform) return;

	if (playerTransform->position.y >= renderingSystem.renderResX * 2)
		PlayerDeath(player, effect, entityManager);
}
void PlayerSpikesCollisions(Entity& player, Entity* effect, EntityManager& entityManager)
{
	BoxCollider2D* playerBoxCollider = player.GetComponent<BoxCollider2D>();
	if (!playerBoxCollider)
		return;

	bool playerDeath = false;

	for (auto entity : entityManager.entities)
	{
		if (!entity)
			continue;

		if (entity == &player)
			continue;

		BoxCollider2D* entityBoxCollider = entity->GetComponent<BoxCollider2D>();
		EntityTag* tag = entity->GetComponent<EntityTag>();
		if (!tag || !entityBoxCollider)
			continue;

		if (tag->name == "Spikes" && IsColliding(*playerBoxCollider, *entityBoxCollider))
		{
			playerDeath = true;
		}
	}
	if (playerDeath)
		PlayerDeath(player, effect, entityManager);
}

void PlayerWallJump(EntityManager& entityManager, InputSystem& inputSystem, Entity* effect, BoxCollider2D* boxCollider, Transform* transform, bool& gatherBuffer, Physics2D* physics, float deltaTime)
{
	if (!boxCollider->wallCollision || boxCollider->groundCollision)
		return;

	static bool leftWallJumped = false;
	static bool rightWallJumped = false;

	static float timer = 0.0f;
	constexpr float timeBetweenValidJumps = 0.1f;

	if (rightWallJumped || leftWallJumped)
	{
		timer += deltaTime;
		if (timer >= timeBetweenValidJumps)
		{
			rightWallJumped = false;
			leftWallJumped = false;
			timer = 0.0f;
		}
	}

	if (gatherBuffer)
	{
		Vec2f collisonVec = boxCollider->collisionVector.normalized();
		float direction = (collisonVec.x < 0.0f) ? -1.0f : 1.0f;

		if (direction > 0.0f)
		{
			if (leftWallJumped)
				return;
			leftWallJumped = true;
			rightWallJumped = false;
		}
		else
		{
			if (rightWallJumped)
				return;
			rightWallJumped = true;
			leftWallJumped = false;
		}

		physics->velocity = {
			direction * 1500.0f,
			-4000.0f
		};
		Vec2f pos = transform->position;
		pos.y -= 50.0f;
		SpawnEffect(entityManager, effect, pos, false);
	}
}

void CameraFollowPlayer(Entity& player, RenderingSystem& renderingSystem, float deltaTime)
{
	Transform* playerTransform = player.GetComponent<Transform>();
	if (!playerTransform)
		return;

	Vec2f target =
	{
		playerTransform->position.x - renderingSystem.renderResX,
		playerTransform->position.y - renderingSystem.renderResY
	};

	Camera& camera = renderingSystem.camera;

	constexpr float followSpeed = 3.5f;
	camera.pos += (target - camera.pos) * followSpeed * deltaTime;
}

void TorchChangeStates(Entity& torch, Animator* animator)
{
	if (animator->currentState == "TorchLit" && animator->finished)
	{
		animator->SetAnimation("TorchFire");
		animator->finished = false;
	}
	if (animator->currentState == "TorchFire" && animator->finished)
	{
		animator->SetAnimation("TorchLit");
		animator->finished = false;
	}
}

int main()
{
	Engine engine;
	engine.Initialize();

	bool enableDebugger = false;

#pragma region TileMap
	engine.assetManager.CreateTexture("TileSet", "Assets/Textures/tiles.png");
	
	Entity& tilemapObj = engine.entityManager.CreateEntity();
	tilemapObj.AddComponent<Transform>()->position = { 100.0f, 200.0f };

	auto& tileMap = *tilemapObj.AddComponent<TileMap>();
	tileMap.LoadTileMap("Assets/Maps/Testing.sodmap");
	tileMap.SetTilePixelSize(16, 16);
	tileMap.SetTileScale(8.0f, 8.0f);
	tileMap.SetSpriteSheet(engine.assetManager.GetTexture("TileSet"));

	TileMap::TileProperties properties;
	properties.solid = true;
	for (int i = 0; i < 26; i++)
	{
		if (i == 3 || i == 4 || i == 15)
			continue;

		tileMap.SetTileProperties(i, properties);
	}

	TileMap::TileScale tilemapScale = tileMap.GetTileScale();
#pragma endregion TileMap

#pragma region Torch
	Entity& torchObj = engine.entityManager.CreateEntity();
	
	torchObj.AddComponent<Transform>()->position = { 7200.0f, 700.0f };

	Sprite* torchSprite = torchObj.AddComponent<Sprite>();
	torchSprite->height = SPRT_HEIGHT;
	torchSprite->width = SPRT_WIDTH;

	Animator* torchAnimator = torchObj.AddComponent<Animator>();
	torchAnimator->effectBase = true;
	torchAnimator->update = true;
	torchAnimator->scaleAnimationX = 1.0f;
	torchAnimator->scaleAnimationY = 1.0f;
	torchAnimator->speed = 0.15f;

	SDL_Texture* torchLitTexture = engine.assetManager.CreateTexture("TorchLitFX", "Assets/Textures/TorchLit.png");
	SDL_Texture* torchFireTexture = engine.assetManager.CreateTexture("TorchFireFX", "Assets/Textures/TorchFire.png");

	torchAnimator->CreateAnimation("TorchLit", 4, 16, 16, torchLitTexture);
	torchAnimator->CreateAnimation("TorchFire", 4, 16, 16, torchFireTexture);
	torchAnimator->SetAnimation("TorchFire");

	Entity& otherTorch = engine.entityManager.CreateEntity(torchObj);
	otherTorch.GetComponent<Transform>()->position = { 100, 100 };

	Animator* otherTorchAnimator = otherTorch.GetComponent<Animator>();
	otherTorchAnimator->update = true;
	otherTorchAnimator->SetAnimation("TorchLit");

#pragma endregion Torch

#pragma region Player
	Entity& playerObject = engine.entityManager.CreateEntity();

	Sprite* playerSprite = playerObject.AddComponent<Sprite>();
	playerSprite->height = SPRT_HEIGHT;
	playerSprite->width = SPRT_WIDTH;

	Transform* playerTransform = playerObject.AddComponent<Transform>();
	playerTransform->position = {7500.0f, 700.0f};
	playerObject.AddComponent<Physics2D>();
	playerObject.AddComponent<EntityTag>()->name = "Player";

	Animator* playerAnimator = playerObject.AddComponent<Animator>();
	engine.assetManager.CreateTexture("PlayerRun", "Assets/Textures/Run.png");
	engine.assetManager.CreateTexture("PlayerIdle", "Assets/Textures/Idle.png");
	engine.assetManager.CreateTexture("PlayerJump", "Assets/Textures/Jump.png");

	playerAnimator->CreateAnimation("Run", 4, 16, 16, engine.assetManager.GetTexture("PlayerRun"));
	playerAnimator->CreateAnimation("Idle", 4, 16, 16, engine.assetManager.GetTexture("PlayerIdle"));
	playerAnimator->CreateAnimation("Jump", 3, 16, 16, engine.assetManager.GetTexture("PlayerJump"));

	playerAnimator->SetAnimation("Idle");
	playerAnimator->speed = 0.15f;
	playerAnimator->scaleAnimationX = 1.0f;
	playerAnimator->scaleAnimationY = 1.0f;

	BoxCollider2D* playerCollider = playerObject.AddComponent<BoxCollider2D>();
	playerCollider->width = 72;
	playerCollider->height = 64;
	playerCollider->offsetX = 28;
	playerCollider->offsetY = 64;
#pragma endregion Player

#pragma region Spikes
	engine.assetManager.CreateTexture("SpikeTexture", "Assets/Textures/Spike.png");

	Entity& spike = engine.entityManager.CreateEntity();
	spike.AddComponent<EntityTag>()->name = "Spikes";

	Transform* spikeTransform = spike.AddComponent<Transform>();
	BoxCollider2D* spikeCollider = spike.AddComponent<BoxCollider2D>();

	Sprite* spikeSprite = spike.AddComponent<Sprite>();
	spikeCollider->height = 32;
	spikeCollider->width = 70;
	spikeCollider->offsetY = 128;
	spikeCollider->offsetX = 45;
	spikeSprite->texture = engine.assetManager.GetTexture("SpikeTexture");
	spikeTransform->position.x = -700;

	engine.entityManager.CreateEntitiesFromObjFile("Assets/Maps/Testing.sodobj", "Spikes", spike);
#pragma endregion Spikes

#pragma region EFFECTS

#pragma region RUNNING_FX
	Entity& runningEffectObj = engine.entityManager.CreateEntity();
	runningEffectObj.AddComponent<Transform>();

	Sprite* runningEffectSprite = runningEffectObj.AddComponent<Sprite>();
	runningEffectSprite->height = SPRT_HEIGHT;
	runningEffectSprite->width = SPRT_WIDTH;

	SDL_Texture* texture = engine.assetManager.CreateTexture("EffectSheet", "Assets/Textures/Effect.png");

	Animator* runningObjAnimator = runningEffectObj.AddComponent<Animator>();
	runningObjAnimator->CreateAnimation("Effect", 5, 16, 16, texture);
	runningObjAnimator->SetAnimation(runningObjAnimator->GetAnimation("Effect"));
	runningObjAnimator->speed = 0.15f;
	runningObjAnimator->scaleAnimationX = 1.0f;
	runningObjAnimator->scaleAnimationY = 1.0f;
	runningObjAnimator->update = false;

#pragma endregion RUNNING_FX

#pragma region EXPLOSION_FX
	engine.assetManager.CreateTexture("ExplosionFX", "Assets/Textures/Explosion.png");
	auto& explosion = engine.entityManager.CreateEntity();
	auto explosionTransform = explosion.AddComponent<Transform>();
	auto explosionSprite = explosion.AddComponent<Sprite>();
	explosionSprite->height = SPRT_HEIGHT;
	explosionSprite->width = SPRT_WIDTH;
	auto explosionAnim = explosion.AddComponent<Animator>();
	explosionAnim->CreateAnimation("Explosion", 4, 16, 16, engine.assetManager.GetTexture("ExplosionFX"));
	explosionAnim->SetAnimation(explosionAnim->GetAnimation("Explosion"));
	explosionAnim->speed = 0.1f;
	explosionAnim->update = false;
	explosionAnim->scaleAnimationX = 1.0f;
	explosionAnim->scaleAnimationY = 1.0f;
#pragma endregion EXPLOSION_FX

#pragma region JUMP_FX
	SDL_Texture* jumpfxTexture = engine.assetManager.CreateTexture("JumpFxTexture", "Assets/Textures/JumpFX.png");

	Entity& jumpFxObj = engine.entityManager.CreateEntity();
	jumpFxObj.AddComponent<Transform>()->position = { 7500.0f, 700.0f };

	Sprite* jmpFxSprt = jumpFxObj.AddComponent<Sprite>();
	jmpFxSprt->height = SPRT_HEIGHT;
	jmpFxSprt->width = SPRT_WIDTH;

	Animator* jmpfxAnimator = jumpFxObj.AddComponent<Animator>();
	jmpfxAnimator->CreateAnimation("JumpFX", 6, 16, 16, jumpfxTexture);
	jmpfxAnimator->SetAnimation("JumpFX");
	jmpfxAnimator->update = false;
	jmpfxAnimator->effectBase = true;
	jmpfxAnimator->speed = 0.1f;
	jmpfxAnimator->scaleAnimationX = 1.0f;
	jmpfxAnimator->scaleAnimationY = 1.0f;

#pragma endregion JUMP_FX

#pragma region DASH_FX

	Entity& dashFxObj = engine.entityManager.CreateEntity();
	dashFxObj.AddComponent<Transform>()->position = { 7200.0f, 800.0f };

	Sprite* dashFxSprite = dashFxObj.AddComponent<Sprite>();
	dashFxSprite->height = SPRT_HEIGHT;
	dashFxSprite->width = SPRT_WIDTH;

	SDL_Texture* dashFxSpriteSheet = engine.assetManager.CreateTexture("DashFX", "Assets/Textures/Dash.png");
	Animator* dashFxAnimator = dashFxObj.AddComponent<Animator>();
	dashFxAnimator->CreateAnimation("Dash", 6, 16, 16, dashFxSpriteSheet);
	dashFxAnimator->SetAnimation("Dash");
	dashFxAnimator->update = false;
	dashFxAnimator->effectBase = true;
	dashFxAnimator->speed = 0.1f;
	dashFxAnimator->scaleAnimationX = 1.0f;
	dashFxAnimator->scaleAnimationY = 1.0f;

#pragma endregion DASH_FX

#pragma endregion EFFECTS

	float fps = 0.0f;
	Uint64 lastTime = SDL_GetTicksNS();
	int frameCount = 0;

	constexpr float targetFrameTime = 1.0f / 6000.0f;


	auto clip = engine.audioManager.CreateAudioClip("Sample", "Assets/Audio/sample.wav");
	engine.audioManager.m_audioClips.find("Sample");
	engine.audioManager.PlayAudioClip(*clip);

	// Game loop
	while (engine.isRunning)
	{
		engine.debugger.DebuggerStartTime();

		engine.DeltaTimeUpdate();
		engine.inputSystem.Process();
		engine.renderingSystem.ClearScreen();

		// Debugger stuff, get rid off on release
		if (engine.inputSystem.GetButtonDown(SDL_SCANCODE_0))
		{
			enableDebugger = !enableDebugger;
			engine.debugger.enabled = enableDebugger;
			engine.debugger.boxColliders.clear();
		}
		if (enableDebugger)
		{
			engine.debugger.enabled = true;
			engine.debugger.DrawAllColliders(engine.entityManager);
		}

		PlayerSpikesCollisions(playerObject, &explosion, engine.entityManager);
		PlayerBounds(playerObject, &explosion, engine.renderingSystem, engine.entityManager);
		CameraFollowPlayer(playerObject, engine.renderingSystem, engine.deltaTime);
		PlayerMovement(engine.inputSystem, engine.entityManager, &playerObject, &runningEffectObj, &jumpFxObj, &dashFxObj, engine.deltaTime);

		if (engine.inputSystem.GetButtonDown(SDL_SCANCODE_1))
		{
			PlayerDeath(playerObject, &explosion, engine.entityManager);
		}

		engine.Update();
		engine.renderingSystem.RenderScreen(engine.entityManager);

		// IGNORE BELOW THIS IS NOT IMPORTANT ONLY FPS LIMITER
		float frameTime =
			(float)(SDL_GetPerformanceCounter() - engine.debugger.GetStartTime()) /
			SDL_GetPerformanceFrequency();

		if (frameTime < targetFrameTime)
		{
			float delay = (targetFrameTime - frameTime) * 1000.0f;
			SDL_Delay((Uint32)delay);
		}
		if (engine.inputSystem.GetButtonDown(SDL_SCANCODE_ESCAPE))
			engine.Quit();

		engine.debugger.DebuggerEndTime();
	}

	return 0;
}