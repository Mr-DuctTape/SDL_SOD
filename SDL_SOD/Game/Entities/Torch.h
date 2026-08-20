#pragma once
#include "../../Engine/Audio/AudioSystem.h"
#include "../../Engine/ECS/Entities.h"

class Player;
class Torch
{
private:
	Entity& m_entity;
	Sprite& m_sprite;
	Transform& m_transform;
	Animator& m_animator;

	enum class State
	{
		OFF,
		LIT,
		ON
	};

	State m_state = State::OFF;
	float m_turnOnRange = 350.0f;

	void ChangeAnimatorState();
	bool CheckPlayerTrigger(AudioManager& audioManager, Transform& player);

public:
	float m_lightVolume = 0.4f;
	void Update(AudioManager& audioManager, Player& player);

	Entity& GetEntity()
	{
		return m_entity;
	}
	Torch(Entity& entity) :
		m_entity(entity),
		m_sprite(m_entity.GetComponent<Sprite>()),
		m_animator(m_entity.GetComponent<Animator>()),
		m_transform(m_entity.GetComponent<Transform>())
	{

	}

	~Torch() = default;
};