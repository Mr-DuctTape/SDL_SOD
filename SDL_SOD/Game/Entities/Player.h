#pragma once
#include "../../Engine/Audio/AudioSystem.h"
#include "../../Engine/ECS/Entities.h"

class Camera;

class RenderingSystem;
class InputSystem;

class Torch
{
private:
	Entity& m_entity;

	void ChangeAnimatorState() {};
	void CheckForPlayerTrigger() {};
public: 

	void Update() {};
	Entity& GetEntity()
	{
		return m_entity;
	}
	Torch(Entity& entity) : m_entity(entity) {};
	~Torch() = default;
};

class Player
{
private:
	Entity& m_entity;
	Sprite* m_sprite;
	Transform* m_transform;
	Animator* m_animator;
	Physics2D* m_physics2D;
	BoxCollider2D* m_boxCollider2D;

	// Animator
	void PlaySoundOnAnimation(AudioManager& audioManager, AudioManager::AudioClip* audioClip, const std::string& animationName, Animator* m_animator, const std::vector<int>& framesToPlayON);
	void ChangeAnimatorStates(float playerMovingSpeed);

	// Spawning effects
	void SpawnEffect(EntityManager& entityManager, Entity& effect, Vec2f transform, bool flippedX);
	void SpawnRunningEffect(EntityManager& entityManager, Entity& effect, bool isGrounded, bool flippedX, float deltaTime);
	void SpawnJumpEffect(EntityManager& entityManager, Entity& prefab, Vec2f pos, bool flippedX);

	// Movement 

	bool IsCoyoteAvailable(float deltaTime, float cTime, float& cTimer, bool& coyote, bool grounded);
	bool IsJumpBufferRunnning(float deltaTime, float jTime, float& jTimer, bool& jumpBuffer);

	void WallJump(EntityManager& entityManager, AudioManager& audioManager, InputSystem& inputSystem, Entity& effect, bool& gatherBuffer, float deltaTime);

	void Dash(EntityManager& entityManager, AudioManager& audioManager, InputSystem& inputSystem, Entity& dashEffect, Vec2f accel, float deltaTime);
	void Movement(EntityManager& entityManager, AudioManager& audioManager, InputSystem& inputSystem, Entity& runningEffect, Entity& jumpEffect, Entity& dashEffect, float deltaTime);
	void Death(Entity& effect, EntityManager& entityManager);
	void Bounds(RenderingSystem& renderingSystem, EntityManager& entityManager, Entity& effect);
	void SpikesCollisions(EntityManager& entityManager, Entity& effect);

	void CameraFollow(RenderingSystem& renderingSystem, float deltaTime);
public:
	Entity& GetEntity()
	{
		return m_entity;
	}
	void Update(RenderingSystem& renderingSystem, EntityManager& entityManager, AudioManager& audioManager, InputSystem& inputSystem,
		Entity& deathEffect, Entity& runningEffect, Entity& jumpingEffect, Entity& dashingEffect, float deltaTime);

	Player(Entity& entity) : m_entity(entity) 
	{
		m_sprite = m_entity.GetComponent<Sprite>();
		m_animator = m_entity.GetComponent<Animator>();
		m_transform = m_entity.GetComponent<Transform>();
		m_boxCollider2D = m_entity.GetComponent<BoxCollider2D>();
		m_physics2D = m_entity.GetComponent<Physics2D>();

		if (!m_sprite || !m_animator || !m_transform || !m_boxCollider2D || !m_physics2D)
		{
			std::cout << "(PLAYER) One or more components are nullptr!\n";
		}
	}
	~Player() = default;
};