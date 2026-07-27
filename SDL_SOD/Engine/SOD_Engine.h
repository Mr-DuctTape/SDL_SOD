#pragma once
#include "../Engine/Core/Application.h"
#include "../Engine/Graphics/Rendering.h"
#include "../Engine/Input/Input.h"
#include "../Engine/ECS/Entities.h"
#include "../Engine/Physics/System.h"
#include "../Engine/Textures/AssetManager.h"
#include "../Engine/Debug/Debugger.h"
#include "../Engine/Audio/AudioSystem.h"

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
	Debugger debugger;

	EntityManager entityManager;
	AssetManager assetManager;
	PhysicSystem physics;

	float deltaTime = 0.0;
	bool isRunning = true;

	void PrintCLIProgress();
	void DeltaTimeUpdate();
	void Update();
	void Physics();
	void Initialize();
	void Quit();
};