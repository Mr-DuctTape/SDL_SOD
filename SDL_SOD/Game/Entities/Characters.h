#pragma once
#include "../../Engine/Audio/AudioSystem.h"
#include "../../Engine/ECS/Entities.h""
#include "../../Engine/Dialog.h"

class Engine;

class NPC
{
protected:
	Entity& m_entity;
	Sprite* m_sprite = nullptr;
	Transform* m_transform = nullptr;
	Animator* m_animator = nullptr;

public:
	enum class CharacterType
	{
		Default,
		Bob,
		Amber
	};
	CharacterType character = CharacterType::Default;

	NPC(Entity& entity) :
		m_entity(entity)
	{
		m_sprite = m_entity.GetComponent<Sprite>();
		m_animator = m_entity.GetComponent<Animator>();
		m_transform = m_entity.GetComponent<Transform>();

		if (!m_sprite || !m_animator || !m_transform)
		{
			std::cout << "(Amber) One or more components are nullptr!\n";
		}
	}
	void Update(Engine& engine, Entity& player, float deltaTime);

	friend void AmberDialogs(Engine& engine, Vec2f playerDir, NPC& npc);
	friend void BobDialogs(Engine& engine, Vec2f playerDir, NPC& npc);
};