#pragma once
#include "../../Engine/Audio/AudioSystem.h"
#include "../../Engine/ECS/Entities.h"

class Camera;
class RenderingSystem;
class InputSystem;

class Player
{
private:
	Vec2f m_respawnPosition{290.0f, 960.0f};

	float m_cameraFollowSpeed = 4.0f;
	float m_movementSpeed = 3600.0f;
	float m_jumpForce = 3800.0f;

	float m_dashVolume = 0.1f;
	float m_jumpVolume = 0.5f;
	float m_stepVolume = 0.02f;
	float m_deathVolume = 0.2f;

	float m_coyoteTimer = 0.0f;
	float m_coyoteTime = 0.10f;

	float m_jumpBufferTimer = 0.0f;
	float m_jumpBufferTime = 0.12f;

	// Sounds
	void PlaySoundOnAnimation
	(  
		AudioManager& audioManager, const std::string& audioClip, 
		const std::string& animationName, float volume, 
		Animator& animator, const std::vector<int>& framesToPlayON
	);

	bool AnimationKeyFrame
	(
		const std::string& animationName,
		Animator& animator, 
		const std::vector<int>& frames
	);

	// Animator
	void ChangeAnimatorStates(float playerMovingSpeed);

	// Spawning effects
	void SpawnEffect
	(
		EntityManager& entityManager, 
		Entity& effect, 
		Vec2f pos, 
		bool flippedX
	);
	void SpawnRunningEffect
	(
		EntityManager& entityManager, 
		Entity& effect, 
		bool isGrounded,
		bool flippedX,
		float deltaTime
	);
	void SpawnJumpEffect
	(
		EntityManager& entityManager, 
		Entity& prefab, 
		Vec2f pos,
		bool flippedX
	);

	// Movement 
	bool IsCoyoteAvailable
	(
		float deltaTime,
		float cTime, 
		float& cTimer, 
		bool& coyote, 
		bool grounded
	);
	bool IsJumpBufferRunnning
	(
		float deltaTime, 
		float jTime, 
		float& jTimer, 
		bool& jumpBuffer
	);

	void SlideDownWall
	(
		BoxCollider2D& m_boxCollider2D,
		InputSystem& inputSystem,

		Vec2f& accel,
		Vec2f& velocity,

		float deltaTime
	);

	void WallJump
	(   
		EntityManager& entityManager, 
		AudioManager& audioManager,
		InputSystem& inputSystem,
		Entity& effect, 

		bool& gatherBuffer, 
		float deltaTime
	);
	void Dash
	(   
		EntityManager& entityManager, 
		AudioManager& audioManager,
		InputSystem& inputSystem,

		Entity& dashEffect, 
		Entity& dashCooldownEffect,
		Entity& dashCoolDownEffectDestroy,
		
		Vec2f accel, 
		float deltaTime
	);
	void AllMovement
	(   
		EntityManager& entityManager, 
		AudioManager& audioManager, 
		InputSystem& inputSystem,

		Entity& runningEffect,
		Entity& wallJumpEffect, 
		Entity& jumpEffect, 
		Entity& dashEffect,
		Entity& dashCooldownEffect,
		Entity& dashCoolDownEffectDestroy,

		float deltaTime
	);

	void Death
	(
		Entity& effect, 
		AudioManager& audioManager, 
		EntityManager& entityManager
	);

	void Bounds
	(
		RenderingSystem& renderingSystem,
		AudioManager& audioManager, 
		EntityManager& entityManager, 
		Entity& effect
	);
	void SpikeCollision
	(
		AudioManager& audioManager, 
		EntityManager& entityManager, 
		Entity& effect
	);

	void CameraFollow(RenderingSystem& renderingSystem, float deltaTime);

public:
	Entity& m_playerEntity;

	Sprite& m_sprite;
	Transform& m_transform;
	Animator& m_animator;
	Physics2D& m_physics2D;
	BoxCollider2D& m_boxCollider2D;

	bool freezeInput = false;

	Entity& GetEntity()
	{
		return m_playerEntity;
	}
	Vec2f& GetRespawnPosition()
	{
		return m_respawnPosition;
	}

	void Update
	(
		RenderingSystem& renderingSystem, 
		EntityManager& entityManager, 
		AudioManager& audioManager, 
		InputSystem& inputSystem,

		Entity& deathEffect, 
		Entity& runningEffect,
		Entity& wallJumpEffect, 
		Entity& jumpingEffect,
		Entity& dashingEffect, 
		Entity& dashCooldownEffect,
		Entity& dashCoolDownEffectDestroy,

		float masterVolume,
		float deltaTime
	);

	Player(Entity& entity) : 
		m_playerEntity(entity),
		m_sprite(m_playerEntity.GetComponent<Sprite>()),
		m_animator(m_playerEntity.GetComponent<Animator>()),
		m_transform(m_playerEntity.GetComponent<Transform>()),
		m_boxCollider2D(m_playerEntity.GetComponent<BoxCollider2D>()),
		m_physics2D(m_playerEntity.GetComponent<Physics2D>())
	{

	}
	~Player() = default;

	friend class Game;
};