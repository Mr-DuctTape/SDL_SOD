#include "Characters.h"
#include "../Game.h"

void BobDialogs(Engine& engine, Vec2f playerDir, NPC& npc)
{
	auto& bob_intro = engine.dialogSystem.GetDialog("Bob_Intro");
	auto& bob_response = engine.dialogSystem.GetDialog("Bob_Response");

	auto& amber_intro = engine.dialogSystem.GetDialog("Amber_Intro");
	auto& amber_response = engine.dialogSystem.GetDialog("Amber_Response");

	bob_intro.position = WorldToScreen((int)npc.m_transform->position.x, (int)npc.m_transform->position.y, engine.renderingSystem.camera);
	bob_response.position = WorldToScreen((int)npc.m_transform->position.x, (int)npc.m_transform->position.y, engine.renderingSystem.camera);

	if (playerDir.Magnitude() <= 150.0f && amber_intro.finished)
	{
		engine.dialogSystem.ActivateDialog(bob_intro);
	}

	if (amber_response.finished)
	{
		engine.dialogSystem.ActivateDialog(bob_response);
	}
}

void AmberDialogs(Engine& engine, Vec2f playerDir, NPC& npc)
{
	auto& bob_intro = engine.dialogSystem.GetDialog("Bob_Intro");
	auto& bob_response = engine.dialogSystem.GetDialog("Bob_Response");

	auto& amber_intro = engine.dialogSystem.GetDialog("Amber_Intro");
	auto& amber_response = engine.dialogSystem.GetDialog("Amber_Response");

	amber_intro.position = WorldToScreen((int)npc.m_transform->position.x, (int)npc.m_transform->position.y, engine.renderingSystem.camera);
	amber_response.position = WorldToScreen((int)npc.m_transform->position.x, (int)npc.m_transform->position.y, engine.renderingSystem.camera);

	if (playerDir.Magnitude() <= 200.0f)
	{
		engine.dialogSystem.ActivateDialog(amber_intro);
	}

	if (bob_intro.finished)
	{
		engine.dialogSystem.ActivateDialog(amber_response);
	}
}

void NPC::Update(Engine& engine, Entity& player, float deltaTime)
{
	Transform* playerTransform = player.GetComponent<Transform>();
	Vec2f playerDirection = playerTransform->position - m_transform->position;

	switch (character)
	{
	case CharacterType::Default:
		break;
	case CharacterType::Bob:
		BobDialogs(engine, playerDirection, *this);
		break;
	case CharacterType::Amber:
		AmberDialogs(engine, playerDirection, *this);
		if (playerDirection.Magnitude() >= 100.0f) {
			m_transform->position += playerDirection * deltaTime;
		}
		break;

	}
}