#pragma once
#include "../../Engine/Audio/AudioSystem.h"
#include "../../Engine/ECS/Entities.h""

class Character
{
private:
	Entity& m_entity;
	Sprite* m_sprite = nullptr;
	Transform* m_transform = nullptr;
	Animator* m_animator = nullptr;
public:
	Character(Entity& entity) :
		m_entity(entity),
		m_sprite(entity.GetComponent<Sprite>()),
		m_transform(entity.GetComponent<Transform>()),
		m_animator(entity.GetComponent<Animator>())
	{
		
	}

	void Update(float deltaTime);
};