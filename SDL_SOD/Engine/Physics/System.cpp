#include "System.h"
#include "../ECS/Entities.h"
#include "../Debug/Debugger.h"
#include <algorithm>

void PhysicSystem::TileMapCollision(Entity* entity, TileMap* tileMap)
{
	if (!entity || !tileMap) { std::cout << "Entity || tileMap == nullptr\n";  return; }

	BoxCollider2D* boxCollider = entity->GetComponent<BoxCollider2D>();
	Transform* transform = entity->GetComponent<Transform>();
	Physics2D* physics = entity->GetComponent<Physics2D>();
	if (!boxCollider || !transform || !physics)
		return;

	auto scale = tileMap->GetTileScale();

	if (debugger && debugger->enabled) {
		debugger->debugStats.collisionChecks = 0;
		debugger->debugStats.resolvedCollisions = 0;
	}

#pragma region RESOLVE_Y
	{
		int x = (int)(transform->position.x / scale.scaledX);
		int y = (int)(transform->position.y / scale.scaledY);

		Vec2i positions[] =
		{
			{x, y},
			{x + 1, y},
			{x - 1, y},
			{x, y + 1},
			{x, y - 1},
			{x + 1, y + 1},
			{x - 1, y + 1},
			{x + 1, y - 1},
			{x - 1, y - 1}
		};
		size_t size = sizeof(positions) / sizeof(Vec2i);

		for (int i = 0; i < size; i++)
		{
			int tX = positions[i].x;
			int tY = positions[i].y;

			if (!tileMap->IsTileSolid(tX, tY))
				continue;

			auto& a = *boxCollider;
			auto b = tileMap->GetTileBoxCollider2D(tX, tY);

			// Check for collision
			if (debugger && debugger->enabled)
				debugger->debugStats.collisionChecks++;

			if (!IsColliding(a, b))
				continue;

			boxCollider->isColliding = true;

			// Get the amount of overlap
			SDL_FRect intersect;
			SDL_GetRectIntersectionFloat(&a.rect, &b.rect, &intersect);

			float overlapX = intersect.w;
			float overlapY = intersect.h;

			if (overlapX <= 0.0f || overlapY <= 0.0f)
				continue;

			auto physicsComponent = physics;
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
			Vec2f& velocity = physicsComponent->velocity;

			// Moving downward -> floor
			if (velocity.y > 0 && collision_axis.y < 0 && overlapY <= overlapX)
			{
				transform->position.y -= overlapY;
				physicsComponent->velocity.y = 0;
				boxCollider->groundCollision = true;
				boxCollider->collisionVector = collision_axis;
			}
			// Moving upward -> ceiling
			else if (velocity.y < 0 && collision_axis.y > 0 && overlapY <= overlapX)
			{
				transform->position.y += overlapY;
				physicsComponent->velocity.y = 0;
				boxCollider->collisionVector = collision_axis;
			}

			if (debugger && debugger->enabled)
				debugger->debugStats.resolvedCollisions++;

			boxCollider->UpdatePosition();
		}
	}
#pragma endregion RESOLVE_Y

#pragma region RESOLVE_X
	{
		int x = (int)(transform->position.x / scale.scaledX);
		int y = (int)(transform->position.y / scale.scaledY);

		Vec2i positions[] =
		{
			{x, y},
			{x + 1, y},
			{x - 1, y},
			{x, y + 1},
			{x, y - 1},
			{x + 1, y + 1},
			{x - 1, y + 1},
			{x + 1, y - 1},
			{x - 1, y - 1}
		};
		size_t size = sizeof(positions) / sizeof(Vec2i);

		for (int i = 0; i < size; i++)
		{
			int tX = positions[i].x;
			int tY = positions[i].y;

			if (!tileMap->IsTileSolid(tX, tY))
				continue;

			auto& a = *boxCollider;
			auto b = tileMap->GetTileBoxCollider2D(tX, tY);

			// Check for collision
			if (debugger && debugger->enabled)
				debugger->debugStats.collisionChecks++;

			if (!IsColliding(a, b))
				continue;

			boxCollider->isColliding = true;

			// Get the amount of overlap
			SDL_FRect intersect;
			SDL_GetRectIntersectionFloat(&a.rect, &b.rect, &intersect);

			float overlapX = intersect.w;
			float overlapY = intersect.h;

			if (overlapX <= 0.0f || overlapY <= 0.0f)
				continue;

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
			Vec2f& velocity = physics->velocity;

			// Moving right -> wall
			if (velocity.x > 0 && collision_axis.x < 0 && overlapX <= overlapY)
			{
				transform->position.x -= overlapX;
				physics->velocity.x = 0;
				boxCollider->wallCollision = true;
				boxCollider->collisionVector = collision_axis;
			}

			// Moving left -> wall
			else if (velocity.x < 0 && collision_axis.x > 0 && overlapX <= overlapY)
			{
				transform->position.x += overlapX;
				physics->velocity.x = 0;
				boxCollider->wallCollision = true;
				boxCollider->collisionVector = collision_axis;
			}

			if (debugger && debugger->enabled)
				debugger->debugStats.resolvedCollisions++;

			boxCollider->UpdatePosition();
		}
	}
#pragma endregion RESOLVE_X
}


void PhysicSystem::Gravity(Entity* entity, float deltaTime)
{
	if (!entity) return;

	Physics2D* physics = entity->GetComponent<Physics2D>();
	if (!physics) return;

	float gravity = 9000.0f;

	physics->acceleration.y += gravity;
}

void PhysicSystem::Movement(Entity* entity, float deltaTime)
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

void PhysicSystem::Update(EntityManager& entityManager, float deltaTime)
{
	// Calculate movement and gravity
	for (auto& entity : entityManager.entities)
	{
		Gravity(entity, deltaTime);
	}

	for (auto& entity : entityManager.entities)
	{
		Movement(entity, deltaTime);
	}

	// Update all collider positions
	for (auto& entity : entityManager.entities)
	{
		if (!entity->HasComponent<BoxCollider2D>())
			continue;

		BoxCollider2D* boxCollider = entity->GetComponent<BoxCollider2D>();
		boxCollider->UpdatePosition();
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