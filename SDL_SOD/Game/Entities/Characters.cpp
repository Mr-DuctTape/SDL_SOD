#include "Characters.h"
#include "../Game.h"

void Bob::Update(Engine& engine, Player& player, float deltaTime)
{
	Transform& playerTransform = player.GetEntity().GetComponent<Transform>();
	Vec2f playerDir = playerTransform.position - m_transform.position;

	for (auto& [dialog, activator] : dependentDialogs)
	{
		m_dialogSystem.GetDialog(dialog).position = WorldToScreen((int)m_transform.position.x, (int)m_transform.position.y, engine.renderingSystem.camera);

		if (m_dialogSystem.GetDialogIndex("Bob_Intro") == dialog)
		{
			if (playerDir.Magnitude() <= 200.0f)
			{
				engine.dialogSystem.ActivateDialog(dialog);
			}
			else
			{
				continue;
			}
		}

		if (m_dialogSystem.IsFinished(activator))
		{
			m_dialogSystem.ActivateDialog(dialog);
		}
	}

	BobDialogs(engine, playerDir);
}

void Amber::Update(Engine& engine, Player& player, float deltaTime)
{
	Transform& playerTransform = player.GetEntity().GetComponent<Transform>();
	Vec2f playerDir = playerTransform.position - m_transform.position;

	if (playerDir.Magnitude() >= 200.0f)
	{
		m_transform.position += playerDir * deltaTime;
	}

	Physics2D& playerPhys = player.GetEntity().GetComponent<Physics2D>();
	DialogSystem& dialogSystem = m_dialogSystem;

	auto ActivateDialog = [&dialogSystem](const size_t dialog) -> void
		{
			dialogSystem.ActivateDialog(dialog);
		};

	auto IsFinished = [&dialogSystem](auto name) -> bool
		{
			return dialogSystem.IsFinished(name);
		};

	auto IsActive = [&dialogSystem](const char* name) -> bool
		{
			return dialogSystem.IsActive(name);
		};

	if (!IsFinished("Amber_Intro"))
	{
		player.freezeInput = true;
	}

	if (IsActive("Amber_Intro") && engine.inputSystem.GetButton(SDL_SCANCODE_S))
	{
		player.freezeInput = false;
		m_dialogSystem.SetFinished("Amber_Intro");
	}

	for (auto& [dialog, activator] : tutorialDialogs)
	{
		if (!IsFinished(activator))
			continue;

		m_dialogSystem.GetDialog(dialog).position =
			WorldToScreen((int)m_transform.position.x, (int)m_transform.position.y, engine.renderingSystem.camera);

		auto IsDialog = [&dialogSystem, dialog](const std::string& name)
			{
				return dialogSystem.GetDialogIndex(name) == dialog;
			};

		if (engine.inputSystem.GetButton(SDL_SCANCODE_2))
		{
			dialogSystem.SetFinished(dialog);
		}

		if (IsDialog("Amber_Move_Start"))
		{
			if (playerDir.Magnitude() <= 300.0f) {
				dialogSystem.ActivateDialog(dialog);
			}

			if (!IsFinished("Amber_Move_Start"))
			{
				playerPhys.velocity = { 0,0 };
			}
			else
			{
				player.freezeInput = false;
			}
		}

		if (IsDialog("Amber_Move_End"))
		{
			if (playerPhys.velocity.x > 0.1f || playerPhys.velocity.x < -0.1f) {
				ActivateDialog(dialog);
			}
		}

		if (IsDialog("Amber_Jump_Start"))
		{
			ActivateDialog(dialog);
		}

		if (IsDialog("Amber_Jump_End") && engine.inputSystem.GetButton(SDL_SCANCODE_SPACE))
		{
			ActivateDialog(dialog);
		}

		if (IsDialog("Amber_Spikes_Start") && playerTransform.position.x >= 2400.0f)
		{
			ActivateDialog(dialog);
		}

		if (IsDialog("Amber_Spikes_End") && playerTransform.position.x >= 3500.0f)
		{
			ActivateDialog(dialog);
		}

		if (IsDialog("Amber_Dash_Start") && playerTransform.position.x >= 5500.0f)
		{
			ActivateDialog(dialog);
		}

		if (IsDialog("Amber_Dash_End") && playerTransform.position.x >= 6700.0f)
		{
			ActivateDialog(dialog);
		}

		if (IsDialog("Amber_Fall") && playerTransform.position.x >= 9890.0f)
		{
			ActivateDialog(dialog);
		}

		if (IsDialog("Amber_WallJump_Start") && playerTransform.position.x >= 10900.0f)
		{
			ActivateDialog(dialog);
		}

		if (IsDialog("Amber_Torch_Start") && playerTransform.position.x >= 4800.0f)
		{
			ActivateDialog(dialog);
		}

	}
	
	for (auto& [dialog, activator] : dependentDialogs)
	{
		m_dialogSystem.GetDialog(dialog).position = WorldToScreen((int)m_transform.position.x, (int)m_transform.position.y, engine.renderingSystem.camera);

		if (IsFinished(activator))
		{
			ActivateDialog(dialog);
		}
	}

	AmberIntro(engine, playerDir);
}

void Bob::BobDialogs(Engine& engine, Vec2f playerDir)
{
}

void Amber::AmberIntro(Engine& engine, Vec2f playerDir)
{
	auto& amber_intro = engine.dialogSystem.GetDialog("Amber_Intro");

	amber_intro.position =
		WorldToScreen((int)m_transform.position.x, (int)m_transform.position.y, engine.renderingSystem.camera);

	if (playerDir.Magnitude() <= 200.0f)
	{
		engine.dialogSystem.ActivateDialog(amber_intro);
	}
}