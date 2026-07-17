#include "System.h"
#include "../ECS/Entities.h"
#include "../Debug/Debugger.h"
#include <algorithm>

void Physics::TileMapCollision(Entity* entity, TileMap* tileMap)
{
	if (!entity || !tileMap) { std::cout << "Entity || tileMap == nullptr\n";  return; }

	BoxCollider2D* boxCollider = entity->GetComponent<BoxCollider2D>();
	Transform* transform = entity->GetComponent<Transform>();
	Physics2D* phyiscs = entity->GetComponent<Physics2D>();

	if (!boxCollider || !transform || !phyiscs)
	{
		return;
	}

	auto scale = tileMap->GetTileScale();

	int x = (int)(transform->position.x / scale.scaledX);
	int y = (int)(transform->position.y / scale.scaledY) - 1;

	Vec2i positions[] =
	{
		{x , y},
		{x - 1,y},
		{x + 1,y},
		{x,y - 1},
		{x,y + 1},
		{x - 1,y - 1},
		{x - 1,y + 1},
		{x + 1, y - 1},
		{x + 1,y + 1},
	};

	float moveX = 0.0f;
	float moveY = 0.0f;
	size_t size = sizeof(positions) / sizeof(Vec2i);

	for (int i = 0; i < size; i++)
	{
		int tX = positions[i].x;
		int tY = positions[i].y;

		if (!tileMap->isTileSolid(tX, tY)) continue;

		auto& a = *boxCollider;
		auto b = tileMap->GetTileBoxCollider2D(tX, tY);

		if(!CheckCollision(a, b)) continue;

		boxCollider->isColliding = true;

		// Get the amount of overlap
		SDL_FRect intersect;
		SDL_GetRectIntersectionFloat(&a.rect, &b.rect, &intersect);

		float overlapX = intersect.w;
		float overlapY = intersect.h;

		if (overlapX <= 0.0f || overlapY <= 0.0f) continue;

		if (fabs(overlapY) < 0.05f) continue;
		if (fabs(overlapX) < 0.05f) continue;

		auto physicsComponent = phyiscs;
		Vec2f objectCenter =
		{
			a.rect.x + a.rect.w * 0.5f,
			a.rect.y + a.rect.h * 0.5f
		};

		Vec2f tileCenter =
		{
			b.rect.x + b.rect.w * 0.5f,
			b.rect.y + b.rect.h * 0.5f
		};

		Vec2f collision_axis = objectCenter - tileCenter;

		if (overlapX < overlapY)
		{
			if (collision_axis.x < 0.0f)
				transform->position.x -= overlapX;
			else
				transform->position.x += overlapX;

			physicsComponent->velocity.x = 0.0f;
			boxCollider->Update(0);
			boxCollider->isColliding = true;
		}
		else
		{
			if (collision_axis.y < 0.0f)
				transform->position.y -= overlapY;
			else
				transform->position.y += overlapY;

			physicsComponent->velocity.y = 0.0f;
			boxCollider->Update(0);
			boxCollider->isColliding = true;
		}
	}
}

void Physics::Gravity(Entity* entity, float deltaTime)
{
	if (!entity) return;

	Physics2D* physics = entity->GetComponent<Physics2D>();
	if (!physics) return;

	float gravity = 9000.0f;

	physics->acceleration.y += gravity;
}

void Physics::Movement(Entity* entity, float deltaTime)
{
	if (!entity) return;

	Physics2D* physics = entity->GetComponent<Physics2D>();
	Transform* transform = entity->GetComponent<Transform>();

	if (!physics || !transform) return;

	physics->velocity += physics->acceleration * deltaTime;
	float drag = powf(0.90f, deltaTime * 60.0f);
	physics->velocity *= drag;

	transform->position += physics->velocity * deltaTime;
	physics->acceleration = { 0.0f, 0.0f };
}

void Physics::Update(EntityManager& entityManager, float deltaTime)
{
	deltaTime = (deltaTime > 0.033f) ? 0.033f : deltaTime; // Ensure that deltatime doesnt get too big

	// Calculate movement and gravity

	for (auto& entity : entityManager.entities)
	{
		Gravity(entity, deltaTime);
	}

	for (auto& entity : entityManager.entities)
	{
		Movement(entity, deltaTime);
	}

	// Update all colliders
	for (auto& entity : entityManager.entities)
	{
		if (!entity->HasComponent<BoxCollider2D>())
			continue;

		BoxCollider2D* boxCollider = entity->GetComponent<BoxCollider2D>();
		boxCollider->Update(0);
	}

	// Perform collisions
	for (auto& entity : entityManager.entities)
	{
		for (auto& mapEntity : entityManager.entities)
		{
			if (entity == mapEntity) continue;
			if (!mapEntity->HasComponent<TileMap>()) continue;

			TileMap* tileMap = mapEntity->GetComponent<TileMap>();
			TileMapCollision(entity, tileMap);
		}
	}
}