#pragma once
#include "../../Engine/Audio/AudioSystem.h"
#include "../../Engine/ECS/Entities.h"

class Player;
class Torch
{
private:
	Entity& m_entity;
	Sprite* m_sprite = nullptr;
	Transform* m_transform = nullptr;
	Animator* m_animator = nullptr;

	enum class State
	{
		OFF,
		LIT,
		ON
	};

	State m_state = State::OFF;
	float m_turnOnRange = 350.0f;

	void ChangeAnimatorState();
	bool CheckPlayerTrigger(Transform* player);

public:

	void Update(Player& player);

	Entity& GetEntity()
	{
		return m_entity;
	}
	Torch(Entity& entity) : m_entity(entity)
	{
		m_sprite = m_entity.GetComponent<Sprite>();
		m_animator = m_entity.GetComponent<Animator>();
		m_transform = m_entity.GetComponent<Transform>();

		if (!m_sprite || !m_animator || !m_transform)
		{
			std::cout << "(TORCH) One or more components are nullptr!\n";
		}
	};
	~Torch() = default;
};