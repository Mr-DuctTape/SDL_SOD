#pragma once
#include "../../Engine/Audio/AudioSystem.h"
#include "../../Engine/ECS/Entities.h"

class Torch
{
private:
	Entity& m_entity;
	Sprite* m_sprite = nullptr;
	Transform* m_transform = nullptr;
	Animator* m_animator = nullptr;

	enum State
	{
		OFF,
		LIT,
		ON
	};

	State m_state = OFF;

	void ChangeAnimatorState()
	{
		switch (m_state)
		{
		case LIT:
			if (m_animator->currentState != "TorchLIT")
			{
				m_animator->SetAnimation("TorchLIT");
			}
			else if (m_animator->finished)
			{
				m_state = ON;
			}
			break;
		case ON:
			if (m_animator->currentState != "TorchON") {
				m_animator->SetAnimation("TorchON");
			}
			break;
		case OFF:
			if (m_animator->currentState != "TorchOFF") {
				m_animator->SetAnimation("TorchOFF");
			}
			break;
		}
	};

	void CheckPlayerTrigger(Transform* player)
	{
		if ((player->position - m_transform->position).Magnitude() < 350.0f && m_state == OFF)
		{
			m_state = LIT;
		}
	};

public:

	void Update(Transform* player) 
	{
		CheckPlayerTrigger(player);
		ChangeAnimatorState();
	};

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