#pragma once
#include "../../Engine/Audio/AudioSystem.h"
#include "../../Engine/ECS/Entities.h""
#include "../../Engine/Dialog.h"
#include "Player.h"

class Engine;

class Character
{
protected:
	Entity& m_entity;
	DialogSystem& m_dialogSystem;
	Sprite& m_sprite;
	Transform& m_transform;
	Animator& m_animator;

public:
	using dialog = size_t;
	using activator = size_t;

	std::vector<std::pair<dialog, activator>> dependentDialogs;

	Character(Entity& entity, DialogSystem& dialogSystem) :
		m_entity(entity),
		m_dialogSystem(dialogSystem),
		m_sprite(m_entity.GetComponent<Sprite>()),
		m_animator(m_entity.GetComponent<Animator>()),
		m_transform(m_entity.GetComponent<Transform>())
	{
	}
};

class Bob : Character
{
public:
	Bob(Entity& entity, DialogSystem& dialogSystem) : 
		Character(entity, dialogSystem)
	{
		dependentDialogs =
		{
			{dialogSystem.GetDialogIndex("Bob_Intro"), dialogSystem.GetDialogIndex("Amber_Intro")},
			{dialogSystem.GetDialogIndex("Bob_Response"), dialogSystem.GetDialogIndex("Amber_Response")}
		};
	}

	void BobDialogs(Engine& engine, Vec2f playerDir);
	void Update(Engine& engine, Player& player, float deltaTime);

	friend class Game;
};

class Amber : Character
{
public:
	bool tutorialFinished = false;
	std::vector<std::pair<dialog, activator>> tutorialDialogs;

	Amber(Entity& entity, DialogSystem& dialogSystem) : 
		Character(entity, dialogSystem)
	{

		auto GetDialogIndex = [&dialogSystem](const char* name) -> size_t
			{
				return dialogSystem.GetDialogIndex(name);
			};

		dependentDialogs =
		{
			{GetDialogIndex("Amber_Response"), GetDialogIndex("Bob_Intro")},
			{GetDialogIndex("Amber_Tutorial_End"), GetDialogIndex("Bob_Response")}
		};

		tutorialDialogs =
		{
			{GetDialogIndex("Amber_Move_Start"),      GetDialogIndex("Amber_Intro")},
			{GetDialogIndex("Amber_Move_End"),        GetDialogIndex("Amber_Move_Start")},

			{GetDialogIndex("Amber_Jump_Start"),      GetDialogIndex("Amber_Move_End")},
			{GetDialogIndex("Amber_Jump_End"),        GetDialogIndex("Amber_Jump_Start")},

			{GetDialogIndex("Amber_Spikes_Start"),    GetDialogIndex("Amber_Jump_End")},
			{GetDialogIndex("Amber_Spikes_End"),      GetDialogIndex("Amber_Spikes_Start")},

			{GetDialogIndex("Amber_Torch_Start"),      GetDialogIndex("Amber_Spikes_End")},

			{GetDialogIndex("Amber_Dash_Start"),      GetDialogIndex("Amber_Torch_Start")},
			{GetDialogIndex("Amber_Dash_End"),        GetDialogIndex("Amber_Dash_Start")},

			{GetDialogIndex("Amber_Fall"),            GetDialogIndex("Amber_Dash_End")},

			{GetDialogIndex("Amber_WallJump_Start"),  GetDialogIndex("Amber_Dash_End")},
			{GetDialogIndex("Amber_WallJump_End"),    GetDialogIndex("Amber_WallJump_Start")},
		};
	}

	void AmberIntro(Engine& engine, Vec2f playerDir);
	void Update(Engine& engine, Player& player, float deltaTime);

	friend class Game;
};