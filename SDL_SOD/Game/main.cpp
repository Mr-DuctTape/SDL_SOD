#include <iostream>
#include "../Engine/SOD_Engine.h"
#include "Components/GameComponents.h"

bool IsCoyoteAvailable(float dt, float cTime, float& cTimer, bool& coyote, bool grounded)
{
	if (!grounded)
	{
		cTimer += dt;
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

void SpawnRunningEffect(EntityManager& entityManager, Entity* effect, bool isGrounded, float dt, Transform* transform, Animator* animator)
{
	if (!effect || !transform || !animator)
		return;

	static float fxTimer = 0.0f;
	float fxTime = 0.30f;

	// FX timer
	fxTimer += dt;
	if (fxTimer >= fxTime && isGrounded)
	{
		auto& fx = entityManager.CreateEntity(*effect);
		auto fxTransform = fx.GetComponent<Transform>();
		if (!fxTransform) return;

		if (animator->flippedX)
			fxTransform->position = { transform->position.x + 2.0f, transform->position.y };
		else
			fxTransform->position = { transform->position.x - 2.0f, transform->position.y };

		auto fxAnim = fx.GetComponent<Animator>();
		if (!fxAnim) return;
		fxAnim->destroyOnFinish = true;
		fxAnim->update = true;
		fxAnim->finished = false;
		fxAnim->flippedX = animator->flippedX;
		fxTimer = 0.0f;
	}
}

void PlayerMovement(InputSystem& inputSystem, EntityManager& entityManager, Entity* player, Entity* effect, float dt)
{
	Transform* transform = player->GetComponent<Transform>();
	Physics2D* physics = player->GetComponent<Physics2D>();
	BoxCollider2D* boxCollider2D = player->GetComponent<BoxCollider2D>();
	Animator* animator = player->GetComponent<Animator>();
	if (!transform || !physics || !boxCollider2D || !animator) return;

	Vec2f accel = { 0, 0 };
	Vec2f force = { 0, 0 };

	bool isGrounded = boxCollider2D->isColliding;
	float movementSpeed = 3500.0f;

	if (!isGrounded)
	{
		movementSpeed = movementSpeed * 1.25f;
	}

	static float coyoteTimer = 0.0f;
	float coyoteTime = 0.1f;
	bool coyoteAvailable = false;
	IsCoyoteAvailable(dt, coyoteTime, coyoteTimer, coyoteAvailable, isGrounded);

	float speed = physics->velocity.Magnitude();
	ChangePlayerAnimatorStates(animator, speed);

	if (animator->currentState == "Run") 
	{
		SpawnRunningEffect(entityManager, effect, isGrounded, dt, transform, animator);
	}

	if (inputSystem.GetButton(SDL_SCANCODE_W))
	{
		accel.y -= movementSpeed;
	}

	if (inputSystem.GetButtonDown(SDL_SCANCODE_SPACE))
	{
		if (isGrounded || coyoteAvailable)
		{
			animator->SetAnimation("Jump");
			force.y -= 4000.0f;
			animator->speed = 0.16f;
		}
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

void CameraSplitWorld(Entity* player, RenderingSystem& renderingSystem, float deltaTime)
{
	auto transform = player->GetComponent<Transform>();
	if (!transform || transform->position.x <= 0.0f)
		return;

	deltaTime = (deltaTime > 0.033f) ? 0.033f : deltaTime; // Ensure that deltatime doesnt get too big

	static float cameraMovedX = 0.0f;
	static float oldMoved = cameraMovedX;
	static float positionToMove = renderingSystem.renderResX;
	const float cameraSpeed = 1000.0f;

	Camera& camera = renderingSystem.camera;

	// Moving the camera
	if (camera.pos.x < positionToMove)
	{
		camera.pos.x += cameraSpeed * deltaTime;

		if (camera.pos.x > positionToMove)
			camera.pos.x = positionToMove;
	}
	else if (camera.pos.x > positionToMove)
	{
		camera.pos.x -= cameraSpeed * deltaTime;

		if (camera.pos.x < positionToMove)
			camera.pos.x = positionToMove;
	}

	// Setting the next position
	if (transform->position.x >= cameraMovedX)
	{
		oldMoved = cameraMovedX;
		positionToMove = cameraMovedX;
		cameraMovedX += renderingSystem.renderResX * 2.0f;
	}
	else if (transform->position.x <= oldMoved)
	{
		oldMoved = cameraMovedX;
		positionToMove = cameraMovedX;
		cameraMovedX -= renderingSystem.renderResX * 2.0f;
	}
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

		if (tag->name == "Spikes" && CheckCollision(*playerBoxCollider, *entityBoxCollider)) {
			playerDeath = true;
		}
	}
	if (playerDeath)
		PlayerDeath(player, effect, entityManager);
}

#define SPRT_HEIGHT 128
#define SPRT_WIDTH 128

int main()
{
	Engine engine;
	engine.Initialize();

	engine.assetManager.CreateTexture("TileSet", "C:\\Test\\tiles.png");
	SDL_SetTextureScaleMode(engine.assetManager.GetTexture("TileSet"), SDL_SCALEMODE_NEAREST);
	engine.assetManager.CreateTexture("PlayerRun", "C:\\Test\\PlayerSheet.png");
	engine.assetManager.CreateTexture("PlayerIdle", "C:\\Test\\Idle.png");
	engine.assetManager.CreateTexture("PlayerJump", "C:\\Test\\Jump.png");

	auto& obj = engine.entityManager.CreateEntity();
	obj.AddComponent<Transform>()->position = { 100.0f, 200.0f };

	auto& tileMap = *obj.AddComponent<TileMap>();
	tileMap.LoadTileMap("C:\\Test\\Testing.sodmap");
	tileMap.SetTilePixelSize(16, 16);
	tileMap.SetTileScale(8.0f, 8.0f);
	tileMap.SetSpriteSheet(engine.assetManager.GetTexture("TileSet"));

	auto& player = engine.entityManager.CreateEntity();
	auto playerSprite = player.AddComponent<Sprite>();

	player.AddComponent<Transform>()->position = { 200.0f, -100.0f };
	player.AddComponent<Physics2D>();
	auto playerColl = player.AddComponent<BoxCollider2D>();
	auto animator = player.AddComponent<Animator>();
	animator->CreateAnimation("Run", 4, 16, 16, engine.assetManager.GetTexture("PlayerRun"));
	animator->CreateAnimation("Idle", 4, 16, 16, engine.assetManager.GetTexture("PlayerIdle"));
	animator->CreateAnimation("Jump", 3, 16, 16, engine.assetManager.GetTexture("PlayerJump"));

	animator->SetAnimation("Idle");
	animator->speed = 0.1f;
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

	auto scale = tileMap.GetTileScale();

	playerColl->width = 72;
	playerColl->height = 64;
	playerColl->offsetX = 28;
	playerColl->offsetY = 64;

	playerSprite->height = SPRT_HEIGHT;
	playerSprite->width = SPRT_WIDTH;
	bool debugger = false;

	auto& effectObj = engine.entityManager.CreateEntity();
	effectObj.AddComponent<Transform>();
	auto s = effectObj.AddComponent<Sprite>();
	s->height = SPRT_HEIGHT;
	s->width = SPRT_WIDTH;
	auto anim = effectObj.AddComponent<Animator>();
	auto texture = engine.assetManager.CreateTexture("EffectSheet", "C:\\Test\\Effect.png");
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

	engine.assetManager.CreateTexture("DoorTexture", "C:\\Test\\Door.png");
	auto& door = engine.entityManager.CreateEntity();
	auto doorTransform = door.AddComponent<Transform>();
	auto doorSprite = door.AddComponent<Sprite>();
	doorSprite->texture = engine.assetManager.GetTexture("DoorTexture");
	doorTransform->position.x = -700;

	engine.assetManager.CreateTexture("ExplosionFX", "C:\\Test\\Explosion.png");
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

	engine.assetManager.CreateTexture("SpikeTexture", "C:\\Test\\Spike.png");

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

	engine.entityManager.CreateEntitiesFromObjFile("C:\\Test\\Objects.sodobj", "Spikes", spike);

	while (engine.isRunning)
	{
		engine.inputSystem.Process();
		engine.renderingSystem.ClearScreen();

		if (debugger)
		{
			engine.debugger.visuals = true;
			engine.debugger.DrawAllColliders(engine.entityManager);
		}

		PlayerSpikesCollisions(player, &explosion, engine.entityManager);
		PlayerMovement(engine.inputSystem, engine.entityManager, &player, &effectObj, engine.deltaTime);
		PlayerBounds(player, &explosion, engine.renderingSystem, engine.entityManager);
		CameraSplitWorld(&player, engine.renderingSystem, engine.deltaTime);

		engine.Update();
		engine.renderingSystem.RenderFrame(engine.entityManager, engine.debugger);
		engine.renderingSystem.PresentScreen();

		if (engine.inputSystem.GetButtonDown(SDL_SCANCODE_0))
		{
			debugger = !debugger;
			engine.debugger.BoxColliders.clear();
		}

		if (engine.inputSystem.GetButtonDown(SDL_SCANCODE_ESCAPE))
			engine.Quit();
	}

	return 0;
}