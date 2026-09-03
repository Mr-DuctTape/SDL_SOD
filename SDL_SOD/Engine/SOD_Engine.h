#pragma once
#include "Core/Application.h"
#include "Graphics/Rendering.h"
#include "Input/Input.h"
#include "ECS/Entities.h"
#include "Physics/System.h"
#include "Textures/AssetManager.h"
#include "Debugger/Debugger.h"
#include "Audio/AudioSystem.h"
#include "UI/UIManager.h"
#include "Dialog/Dialog.h"

class Engine
{
private:
	Uint64 previousFrame = 0;

public:
	Engine() {};

	Application application;

	InputSystem inputSystem;
	AudioManager audioManager;

	RenderingSystem renderingSystem;
	DialogSystem dialogSystem;

	UIManager uiManager;
	Debugger debugger;

	EntityManager entityManager;
	AssetManager assetManager;
	PhysicSystem physics;

	float deltaTime = 0.0;
	bool isRunning = true;

	void DeltaTimeUpdate();
	void Update();
	void Physics();
	void Initialize();
	void Quit();
};