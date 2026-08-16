#include "Torch.h"
#include "Player.h"

void Torch::Update(Player& player)
{
	CheckPlayerTrigger(player.GetEntity().GetComponent<Transform>());
	ChangeAnimatorState();
	player.GetRespawnPosition() = m_transform->position;
}