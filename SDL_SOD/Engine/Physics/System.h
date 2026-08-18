#pragma once
#include "../Math/Vector2.h"
#include "../Macros/DEBUGPRINT.h"

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
		if constexpr (DEBUGPRINT)
			std::cout << "[" << "\033[36m" << "PHYSICSYSTEM" << "\033[37m" << "] " << " Initialized: " << this << "\n";
	}
	void Update(EntityManager& entityManager, float deltaTime);
	~PhysicSystem()
	{
		if constexpr (DEBUGPRINT)
			std::cout << "[" << "\033[36m" << "PHYSICSYSTEM" << "\033[37m" << "] " << " Destroying: " << this << "\n";
	}
};