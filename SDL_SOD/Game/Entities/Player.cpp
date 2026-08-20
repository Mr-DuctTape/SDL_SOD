#include "Player.h"
#include "../../Engine/SOD_Engine.h"


// Animator
bool Player::AnimationKeyFrame(const std::string& animationName, Animator& animator, const std::vector<int>& frames)
{
	if (animationName != animator.currentState)
		return false;

	int currentFrame = animator.currentAnimation.currentFrame;

	for (int frame : frames)
	{
		if (frame == currentFrame)
		{
			return true;
		}
	}

	return false;
}

void Player::PlaySoundOnAnimation(
	AudioManager& audioManager,
	const std::string& audioClip,
	const std::string& animationName,
	float volume,
	Animator& animator,
	const std::vector<int>& framesToPlayOn)
{
	static int lastFrame = -1;

	if (animationName != animator.currentState)
		return;

	int currentFrame = animator.currentAnimation.currentFrame;

	if (currentFrame == lastFrame)
		return;

	for (int frame : framesToPlayOn)
	{
		if (frame == currentFrame)
		{
			audioManager.Play(audioClip, volume);
			break;
		}
	}

	lastFrame = currentFrame;
}
void Player::ChangeAnimatorStates(float playerMovingSpeed)
{
	if (m_animator.currentState == "WallIdle")
	{
		if (!m_boxCollider2D.wallCollision)
		{
			if (playerMovingSpeed > 70)
			{
				m_animator.SetAnimation("Run");
				m_animator.currentState = "Run";
			}
			else
			{
				m_animator.SetAnimation("Idle");
				m_animator.currentState = "Idle";
			}
		}

		m_animator.speed = 0.15f;
	}
	else if (m_animator.currentState == "Jump" && m_animator.finished)
	{
		if (playerMovingSpeed > 70)
		{
			m_animator.SetAnimation("Run");
			m_animator.currentState = "Run";
		}
		else
		{
			m_animator.SetAnimation("Idle");
			m_animator.currentState = "Idle";
		}
	}
	else if (m_animator.currentState == "Idle")
	{
		if (playerMovingSpeed > 70)
		{
			m_animator.SetAnimation("Run");
			m_animator.currentState = "Run";
		}

		m_animator.speed = 0.16f;
	}
	else if (m_animator.currentState == "Run")
	{
		if (playerMovingSpeed < 60)
		{
			m_animator.SetAnimation("Idle");
			m_animator.currentState = "Idle";
		}

		m_animator.speed = 0.1f;
	}
}

// Spawning effects
void Player::SpawnEffect(EntityManager& entityManager, Entity& effect, Vec2f pos, bool flippedX)
{
	Entity& fxObj = entityManager.CreateEntity(effect);

	Animator& fxAnimator = fxObj.GetComponent<Animator>();
	Transform& fxTransform = fxObj.GetComponent<Transform>();

	fxTransform.position = { pos.x, pos.y };

	fxAnimator.update = true;
	fxAnimator.destroyOnFinish = true;
	fxAnimator.finished = false;
	fxAnimator.flippedX = flippedX;
	fxAnimator.timer = 0.0f;
}
void Player::SpawnRunningEffect(EntityManager& entityManager, Entity& effect, bool isGrounded, bool flippedX, float deltaTime)
{
	static float vfxTimer = 0.0f;
	float vfxTime = 0.1f;

	// visual effect timer
	vfxTimer += deltaTime;
	if (vfxTimer >= vfxTime && isGrounded)
	{
		// Create visual effect
		Vec2f pos;
		if (flippedX)
			pos = { m_transform.position.x, m_transform.position.y };
		else
			pos = { m_transform.position.x, m_transform.position.y };

		SpawnEffect(entityManager, effect, pos, flippedX);
		vfxTimer = 0;
	}
}
void Player::SpawnJumpEffect(EntityManager& entityManager, Entity& prefab, Vec2f pos, bool flippedX)
{
	SpawnEffect(entityManager, prefab, pos, flippedX);
}

// Movement 

bool Player::IsCoyoteAvailable(float deltaTime, float cTime, float& cTimer, bool& coyote, bool grounded)
{
	if (!grounded)
	{
		cTimer += deltaTime;
		if (cTimer < cTime)
		{
			coyote = true;
		}
		else
		{
			coyote = false;
		}
	}
	if (grounded)
	{
		cTimer = 0.0f;
	}

	return coyote;
}
bool Player::IsJumpBufferRunnning(float deltaTime, float jTime, float& jTimer, bool& jumpBuffer)
{
	if (jTimer < jTime)
	{
		jTimer += deltaTime;
		jumpBuffer = true;
	}

	if (jTimer >= jTime)
	{
		jumpBuffer = false;
		jTimer = 0;
	}

	return jumpBuffer;
}
void Player::SlideDownWall(
	BoxCollider2D& m_boxCollider2D,
	InputSystem& inputSystem,
	Vec2f& accel,
	Vec2f& velocity,
	float deltaTime)
{
	constexpr float slideDownTime = 0.8f;

	static float slideDownTimer = 0.0f;
	static bool slideExhausted = false;

	bool validCollision =
		!m_boxCollider2D.groundCollision &&
		m_boxCollider2D.wallCollision;

	bool validInput =
		inputSystem.GetButton(SDL_SCANCODE_A) ||
		inputSystem.GetButton(SDL_SCANCODE_D);

	// Not touching a wall.
	if (!validCollision)
	{
		slideDownTimer = 0.0f;
		slideExhausted = false;
		return;
	}

	// Don't do anything if we're not pressing toward the wall.
	if (!validInput)
	{
		slideDownTimer = 0.0f;
		slideExhausted = false;
		return;
	}

	// Just entered the wall.
	if (m_animator.currentState != "WallIdle")
	{
		slideDownTimer = 0.0f;
		slideExhausted = false;

		m_animator.SetAnimation("WallIdle");
		m_animator.currentState = "WallIdle";

		if (inputSystem.GetButton(SDL_SCANCODE_A))
			m_animator.flippedX = true;
		else if (inputSystem.GetButton(SDL_SCANCODE_D))
			m_animator.flippedX = false;

		// Stick to wall.
		velocity.y = 0.0f;

		return;
	}

	// Slide has already expired.
	if (slideExhausted)
		return;

	// Wall sliding.
	slideDownTimer += deltaTime;

	if (slideDownTimer >= slideDownTime)
	{
		slideExhausted = true;
		return;
	}

	accel.y -= 9000.0f;
}

void Player::WallJump(EntityManager& entityManager, AudioManager& audioManager, InputSystem& inputSystem, Entity& effect, bool& gatherBuffer, float deltaTime)
{
	if (!m_boxCollider2D.wallCollision || m_boxCollider2D.groundCollision)
		return;

	static bool leftWallJumped = false;
	static bool rightWallJumped = false;

	static float timer = 0.0f;
	constexpr float timeBetweenValidJumps = 0.1f;

	if (rightWallJumped || leftWallJumped)
	{
		timer += deltaTime;
		if (timer >= timeBetweenValidJumps)
		{
			rightWallJumped = false;
			leftWallJumped = false;
			timer = 0.0f;
		}
	}

	if (gatherBuffer)
	{
		Vec2f collisonVec = m_boxCollider2D.collisionVector.normalized();
		float direction = (collisonVec.x < 0.0f) ? -1.0f : 1.0f;

		if (direction > 0.0f)
		{
			if (leftWallJumped)
				return;
			leftWallJumped = true;
			rightWallJumped = false;
		}
		else
		{
			if (rightWallJumped)
				return;
			rightWallJumped = true;
			leftWallJumped = false;
		}

		m_physics2D.velocity = {
			direction * 1500.0f,
			-4000.0f
		};

		Vec2f pos = m_transform.position;
		pos.y += 0.0f;
		if (m_animator.flippedX)
			pos.x += 30.0f;
		else
			pos.x -= 30.0f;

		SpawnJumpEffect(entityManager, effect, pos, !m_animator.flippedX);
		audioManager.Play("Jump", m_jumpVolume);
		audioManager.Play("Step", m_stepVolume);
	}
}

void Player::Dash(EntityManager& entityManager, AudioManager& audioManager, InputSystem& inputSystem, Entity& dashEffect, Vec2f accel, float deltaTime)
{
	static bool dashing = false;
	static float dashCoolDownTimer = 0.0f;
	constexpr float dashCoolDownTime = 1.5f;

	if (dashing && dashCoolDownTimer < dashCoolDownTime)
	{
		dashCoolDownTimer += deltaTime;
		if (dashCoolDownTimer >= dashCoolDownTime)
		{
			dashing = false;
			dashCoolDownTimer = 0.0f;
		}
	}

	if (inputSystem.GetButtonDown(SDL_SCANCODE_LSHIFT) && !dashing)
	{
		float dirX = (accel.normalized().x > 0.0f) ? 1.0f : -1.0f;
		Vec2f force = { dirX * 4000.0f, 0.0f };
		m_physics2D.AddForce(force);
		dashing = true;
		Vec2f spawnPos = m_transform.position;
		spawnPos.y += 25.0f;
		if (dirX >= 1.0f)
			spawnPos.x += 100.0f;
		else
			spawnPos.x -= 100.0f;

		audioManager.Play("Dash", m_dashVolume);
		SpawnEffect(entityManager, dashEffect, spawnPos, m_animator.flippedX);
	}
}
void Player::AllMovement(EntityManager& entityManager, AudioManager& audioManager, InputSystem& inputSystem, Entity& runningEffect, Entity& wallJumpEffect, Entity& jumpEffect, Entity& dashEffect, float deltaTime)
{
	if (freezeInput)
	{
		m_animator.SetAnimation("Idle");
		return;
	}

	Vec2f accel = { 0, 0 };
	Vec2f force = { 0, 0 };

	bool isGrounded = m_boxCollider2D.groundCollision;
	float m_movementSpeed = this->m_movementSpeed;

	if (!isGrounded)
	{
		m_movementSpeed = m_movementSpeed * 1.25f;
	}

	float speed = m_physics2D.velocity.Magnitude();
	ChangeAnimatorStates(speed);

	if (m_animator.currentState == "Run")
	{
		if (AnimationKeyFrame("Run", m_animator, { 1 }))
		{
			SpawnRunningEffect(entityManager, runningEffect, isGrounded, m_animator.flippedX, deltaTime);
		}
	}

	static float jumpBufferTimer = 0.0f;
	static bool gatherBuffer = false;
	float jumpBufferTime = 0.2f;
	bool jumpBuffer = false;

	if (inputSystem.GetButtonDown(SDL_SCANCODE_SPACE) && gatherBuffer)
	{
		gatherBuffer = false;
	}
	else if (inputSystem.GetButtonDown(SDL_SCANCODE_SPACE))
	{
		gatherBuffer = true;
		jumpBufferTimer = 0.0f;
	}

	if (gatherBuffer)
	{
		gatherBuffer = IsJumpBufferRunnning(deltaTime, jumpBufferTime, jumpBufferTimer, jumpBuffer);
	}


	static float coyoteTimer = 0.0f;
	float coyoteTime = 0.10f;
	bool coyoteAvailable = false;
	IsCoyoteAvailable(deltaTime, coyoteTime, coyoteTimer, coyoteAvailable, isGrounded);

	bool jumpAvailable = isGrounded || coyoteAvailable;
	if (gatherBuffer && jumpAvailable)
	{
		gatherBuffer = false;
		jumpBufferTimer = 0.0f;

		m_animator.SetAnimation("Jump");
		SpawnJumpEffect(entityManager, jumpEffect, m_transform.position, m_animator.flippedX);
		audioManager.Play("Jump", m_jumpVolume);
		audioManager.Play("Step", m_stepVolume);
		force.y -= m_jumpForce;
	}

	if (inputSystem.GetButton(SDL_SCANCODE_A))
	{
		m_animator.flippedX = true;
		accel.x -= m_movementSpeed;
	}

	if (inputSystem.GetButton(SDL_SCANCODE_D))
	{

		m_animator.flippedX = false;
		accel.x += m_movementSpeed;
	}

	if (isGrounded) {
		PlaySoundOnAnimation(audioManager, "Step", "Run", m_stepVolume, m_animator, { 1 });
	}

	Dash(entityManager, audioManager, inputSystem, dashEffect, accel, deltaTime);
	SlideDownWall(m_boxCollider2D, inputSystem, accel, m_physics2D.velocity, deltaTime);
	WallJump(entityManager, audioManager, inputSystem, wallJumpEffect, gatherBuffer, deltaTime);

	m_physics2D.Accelerate(accel);
	m_physics2D.AddForce(force);
}
void Player::Death(Entity& effect, AudioManager& audioManager, EntityManager& entityManager)
{
	auto& deathFX = entityManager.CreateEntity(effect);
	auto& fxTransform = deathFX.GetComponent<Transform>();
	fxTransform = m_transform;

	Animator& fxAnim = deathFX.GetComponent<Animator>();

	fxAnim.destroyOnFinish = true;
	fxAnim.update = true;
	fxAnim.finished = false;
	m_transform.position = m_respawnPosition;

	audioManager.Play("Death", m_deathVolume);
}
void Player::Bounds(RenderingSystem& renderingSystem, AudioManager& audioManager, EntityManager& entityManager, Entity& effect)
{
	if (m_transform.position.y >= renderingSystem.renderResX * 4) {
		Death(effect, audioManager, entityManager);
	}
}
void Player::SpikeCollision(AudioManager& audioManager, EntityManager& entityManager, Entity& effect)
{
	bool playerDeath = false;

	for (auto entity : entityManager.entities)
	{
		if (!entity)
			continue;

		if (entity == &m_entity)
			continue;

		BoxCollider2D* entityBoxCollider = entity->FindComponent<BoxCollider2D>();
		EntityTag* tag = entity->FindComponent<EntityTag>();

		if (!tag || !entityBoxCollider)
			continue;

		if (tag->name == "Spikes" && IsColliding(*entityBoxCollider, m_boxCollider2D))
		{
			playerDeath = true;
		}
	}
	if (playerDeath) {
		Death(effect, audioManager, entityManager);
	}
}

void Player::CameraFollow(RenderingSystem& renderingSystem, float deltaTime)
{
	Vec2f target =
	{
		m_transform.position.x - renderingSystem.renderResX,
		m_transform.position.y - renderingSystem.renderResY
	};

	Camera& camera = renderingSystem.camera;

	camera.pos += (target - camera.pos) * m_cameraFollowSpeed * deltaTime;
}
void Player::Update(RenderingSystem& renderingSystem, EntityManager& entityManager, AudioManager& audioManager, InputSystem& inputSystem,
	Entity& deathEffect, Entity& runningEffect, Entity& wallJumpEffect, Entity& jumpingEffect, Entity& dashingEffect, float masterVolume, float deltaTime)
{
	m_jumpVolume *= masterVolume;
	m_dashVolume *= masterVolume;
	m_stepVolume *= masterVolume;

	AllMovement
	(
		entityManager, 
		audioManager, 
		inputSystem, 
		runningEffect,
		wallJumpEffect,
		jumpingEffect,
		dashingEffect, 
		deltaTime
	);

	SpikeCollision(audioManager, entityManager, deathEffect);
	Bounds(renderingSystem, audioManager, entityManager, deathEffect);
	CameraFollow(renderingSystem, deltaTime);
}
