#pragma once
#include "../Math/Vector2.h"

class EntityManager;
class Entity;
class Debugger;
class TileMap;

class PhysicSystem
{
private:
	Debugger* debugger = nullptr;

	void TileMapCollision(Entity* entity, TileMap* tileMap);
	void Gravity(Entity* entity, float deltaTime);
	void Movement(Entity* entity, float deltaTime);
public:

	void Intialize(Debugger* debugger) 
	{
		this->debugger = debugger;
	}
	void Update(EntityManager& entityManager, float deltaTime);
};