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
void SpawnRunningEffect(EntityManager& entityManager, Entity* effect, bool isGrounded, float deltaTime, Transform* transform, Animator* animator)
{
	if (!effect || !transform || !animator)
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

		if (animator->flippedX)
			vfxTransform->position = { transform->position.x + 10.0f, transform->position.y };
		else
			vfxTransform->position = { transform->position.x - 10.0f, transform->position.y };

		auto vfxAnimator = vfxObject.GetComponent<Animator>();
		if (!vfxAnimator) return;
		vfxAnimator->destroyOnFinish = true;
		vfxAnimator->update = true;
		vfxAnimator->finished = false;
		vfxAnimator->flippedX = animator->flippedX;
		vfxTimer = 0.0f;
	}
}

void PlayerMovement(InputSystem& inputSystem, EntityManager& entityManager, Entity* player, Entity* effect, float deltaTime)
{
	Transform* transform = player->GetComponent<Transform>();
	Physics2D* physics = player->GetComponent<Physics2D>();
	BoxCollider2D* boxCollider2D = player->GetComponent<BoxCollider2D>();
	Animator* animator = player->GetComponent<Animator>();
	if (!transform || !physics || !boxCollider2D || !animator) return;

	Vec2f accel = { 0, 0 };
	Vec2f force = { 0, 0 };

	bool isGrounded = (boxCollider2D->data.side == CollisionData::Side::Bottom);
	float movementSpeed = 3500.0f;

	if (!isGrounded)
	{
		movementSpeed = movementSpeed * 1.25f;
	}

	static float coyoteTimer = 0.0f;
	float coyoteTime = 0.18f;
	bool coyoteAvailable = false;
	IsCoyoteAvailable(deltaTime, coyoteTime, coyoteTimer, coyoteAvailable, isGrounded);

	float speed = physics->velocity.Magnitude();
	ChangePlayerAnimatorStates(animator, speed);

	if (animator->currentState == "Run")
	{
		SpawnRunningEffect(entityManager, effect, isGrounded, deltaTime, transform, animator);
	}

	if (inputSystem.GetButton(SDL_SCANCODE_W))
	{
		accel.y -= movementSpeed;
	}

	static float jumpBufferTimer = 0.0f;
	static bool gatherBuffer = false;
	float jumpBufferTime = 0.12f;
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
	playerTransform->position = Vec2f{ 200.0f, -100.0f };
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

		if (tag->name == "Spikes" && GetCollision(*playerBoxCollider, *entityBoxCollider).side != CollisionData::Side::None) {
			playerDeath = true;
		}
	}
	if (playerDeath)
		PlayerDeath(player, effect, entityManager);
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
	camera.pos.x = std::round(camera.pos.x);
	camera.pos.y = std::round(camera.pos.y);
}

int main()
{
	Engine engine;
	engine.Initialize();

	engine.assetManager.CreateTexture("TileSet", "Assets/Textures/tiles.png");
	engine.assetManager.CreateTexture("PlayerRun", "Assets/Textures/Run.png");
	engine.assetManager.CreateTexture("PlayerIdle", "Assets/Textures/Idle.png");
	engine.assetManager.CreateTexture("PlayerJump", "Assets/Textures/Jump.png");


	auto& obj = engine.entityManager.CreateEntity();
	obj.AddComponent<Transform>()->position = { 100.0f, 200.0f };

	auto& tileMap = *obj.AddComponent<TileMap>();
	tileMap.LoadTileMap("Assets/Maps/Testing.sodmap");
	tileMap.SetTilePixelSize(16, 16);
	tileMap.SetTileScale(8.0f, 8.0f);
	tileMap.SetSpriteSheet(engine.assetManager.GetTexture("TileSet"));

	auto& player = engine.entityManager.CreateEntity();
	auto playerSprite = player.AddComponent<Sprite>();

	player.AddComponent<Transform>()->position = { 200.0f, -100.0f };
	player.AddComponent<Physics2D>();
	player.AddComponent<EntityTag>()->name = "Player";
	auto playerColl = player.AddComponent<BoxCollider2D>();
	auto animator = player.AddComponent<Animator>();
	animator->CreateAnimation("Run", 4, 16, 16, engine.assetManager.GetTexture("PlayerRun"));
	animator->CreateAnimation("Idle", 4, 16, 16, engine.assetManager.GetTexture("PlayerIdle"));
	animator->CreateAnimation("Jump", 3, 16, 16, engine.assetManager.GetTexture("PlayerJump"));

	animator->SetAnimation("Idle");
	animator->speed = 0.15f;
	animator->scaleAnimationX = 1.0f;
	animator->scaleAnimationY = 1.0f;
	animator->Print();

	auto& entity2 = engine.entityManager.CreateEntity();
	auto& t = *entity2.AddComponent<Transform>();
	entity2.AddComponent<BoxCollider2D>();

	TileMap::TileProperties properties;
	properties.solid = true;
	tileMap.SetTileProperties(0, properties);
	tileMap.SetTileProperties(1, properties);
	tileMap.SetTileProperties(2, properties);
	tileMap.SetTileProperties(5, properties);
	tileMap.SetTileProperties(6, properties);
	tileMap.SetTileProperties(7, properties);
	tileMap.SetTileProperties(8, properties);
	tileMap.SetTileProperties(9, properties);
	tileMap.SetTileProperties(10, properties);
	tileMap.SetTileProperties(11, properties);
	tileMap.SetTileProperties(12, properties);
	tileMap.SetTileProperties(13, properties);
	tileMap.SetTileProperties(14, properties);

	tileMap.SetTileProperties(16, properties);
	tileMap.SetTileProperties(17, properties);
	tileMap.SetTileProperties(18, properties);
	tileMap.SetTileProperties(19, properties);
	tileMap.SetTileProperties(20, properties);
	tileMap.SetTileProperties(21, properties);
	tileMap.SetTileProperties(22, properties);
	tileMap.SetTileProperties(23, properties);
	tileMap.SetTileProperties(24, properties);
	tileMap.SetTileProperties(25, properties);
	tileMap.SetTileProperties(26, properties);

	auto scale = tileMap.GetTileScale();

	playerColl->width = 72;
	playerColl->height = 64;
	playerColl->offsetX = 28;
	playerColl->offsetY = 64;

	playerSprite->height = SPRT_HEIGHT;
	playerSprite->width = SPRT_WIDTH;
	bool enableDebugger = false;

	auto& effectObj = engine.entityManager.CreateEntity();
	effectObj.AddComponent<Transform>();
	auto s = effectObj.AddComponent<Sprite>();
	s->height = SPRT_HEIGHT;
	s->width = SPRT_WIDTH;
	auto anim = effectObj.AddComponent<Animator>();
	auto texture = engine.assetManager.CreateTexture("EffectSheet", "Assets/Textures/Effect.png");
	anim->CreateAnimation("Effect", 5, 16, 16, texture);
	anim->SetAnimation(anim->GetAnimation("Effect"));
	anim->speed = 0.15f;
	anim->scaleAnimationX = 1.0f;
	anim->scaleAnimationY = 1.0f;
	anim->update = false;
	anim->Print();

	float fps = 0.0f;
	Uint64 lastTime = SDL_GetTicksNS();
	int frameCount = 0;

	engine.assetManager.CreateTexture("DoorTexture", "Assets/Textures/Door.png");
	auto& door = engine.entityManager.CreateEntity();
	auto doorTransform = door.AddComponent<Transform>();
	auto doorSprite = door.AddComponent<Sprite>();
	doorSprite->texture = engine.assetManager.GetTexture("DoorTexture");
	doorTransform->position.x = -700;

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

	engine.assetManager.CreateTexture("SpikeTexture", "Assets/Textures/Spike.png");

	auto& spike = engine.entityManager.CreateEntity();
	auto spikeTransform = spike.AddComponent<Transform>();
	auto spikeCollider = spike.AddComponent<BoxCollider2D>();
	spike.AddComponent<EntityTag>()->name = "Spikes";
	auto spikeSprite = spike.AddComponent<Sprite>();
	spikeCollider->height = 32;
	spikeCollider->width = 70;
	spikeCollider->offsetY = 128;
	spikeCollider->offsetX = 45;
	spikeSprite->texture = engine.assetManager.GetTexture("SpikeTexture");
	spikeTransform->position.x = -700;
	engine.entityManager.CreateEntitiesFromObjFile("Assets/Maps/Testing.sodobj", "Spikes", spike);

	constexpr float targetFrameTime = 1.0f / 100000.0f;

	// Game loop
	while (engine.isRunning)
	{
		Uint64 frameStart = SDL_GetPerformanceCounter();

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
			static float fpsTimer = 0.0f;
			static int frameCount = 0;

			fpsTimer += engine.deltaTime;
			frameCount++;

			if (fpsTimer >= 0.5f)
			{
				engine.debugger.debugStats.fps = frameCount / fpsTimer;
				engine.debugger.debugStats.ms = (fpsTimer / frameCount) * 1000.0f;

				frameCount = 0;
				fpsTimer = 0.0f;
			}

			engine.debugger.enabled = true;
			engine.debugger.DrawAllColliders(engine.entityManager);
		}

		PlayerSpikesCollisions(player, &explosion, engine.entityManager);
		PlayerMovement(engine.inputSystem, engine.entityManager, &player, &effectObj, engine.deltaTime);
		PlayerBounds(player, &explosion, engine.renderingSystem, engine.entityManager);

		engine.Update();
		CameraFollowPlayer(player, engine.renderingSystem, engine.deltaTime);

		engine.renderingSystem.RenderFrame(engine.entityManager);
		engine.renderingSystem.PresentScreen();

		if (engine.inputSystem.GetButtonDown(SDL_SCANCODE_ESCAPE))
			engine.Quit();

		float frameTime =
			(float)(SDL_GetPerformanceCounter() - frameStart) /
			SDL_GetPerformanceFrequency();

		if (frameTime < targetFrameTime)
		{
			float delay = (targetFrameTime - frameTime) * 1000.0f;
			SDL_Delay((Uint32)delay);
		}
	}

	return 0;
}