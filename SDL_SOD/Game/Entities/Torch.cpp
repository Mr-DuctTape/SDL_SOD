#include "Torch.h"
#include "Player.h"

void Torch::Update(AudioManager& audioManager, Player& player)
{
	if (CheckPlayerTrigger(audioManager, player.GetEntity().GetComponent<Transform>())) {
		player.GetRespawnPosition() = m_transform.position;
	}
	ChangeAnimatorState();
}

bool Torch::CheckPlayerTrigger(AudioManager& audioManager, Transform& player)
{
	if ((player.position - m_transform.position).Magnitude()
		< m_turnOnRange && m_state == State::OFF)
	{
		audioManager.Play("TorchLight", m_lightVolume);
		m_state = State::LIT;
		return true;
	}
	return false;
}

void Torch::ChangeAnimatorState()
{
	switch (m_state)
	{
	case State::LIT:
		if (m_animator.currentState != "TorchLIT")
		{
			m_animator.SetAnimation("TorchLIT");
		}
		else if (m_animator.finished)
		{
			m_state = State::ON;
		}
		break;
	case State::ON:
		if (m_animator.currentState != "TorchON") {
			m_animator.SetAnimation("TorchON");
		}
		break;
	case State::OFF:
		if (m_animator.currentState != "TorchOFF") {
			m_animator.SetAnimation("TorchOFF");
		}
		break;
	}
};