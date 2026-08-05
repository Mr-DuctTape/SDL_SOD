#include "SOD_Engine.h"
#include <chrono>
#include <thread>

void Sleep(int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void Engine::PrintCLIProgress()
{
	char empty = 177, filled = 219;

	printf("Loading: ");

	// Print initial loading bar
	for (int i = 0; i < 26; i++)
		printf("%c", empty);

	// Return to the start of the bar
	printf("\rLoading: ");

	// Fill the bar
	for (int i = 0; i < 26; i++)
	{
		printf("%c", filled);
		fflush(stdout); // Force it to update immediately
		Sleep(250);
	}

	printf("\n");
}

void Engine::Initialize()
{
	// creates the window and renderer
	application.Initialize(renderingSystem.renderer);
	inputSystem.Initialize(*application.GetWindow(), renderingSystem);

	// Initialize rendering system, give references/pointers to others
	renderingSystem.Initialize(debugger, uiManager);
	uiManager.Initialize(inputSystem, renderingSystem, audioManager, renderingSystem.camera);
	debugger.Initialize(&renderingSystem.camera);
	assetManager.Initialize(renderingSystem.renderer);
	physics.Intialize(&debugger);

	isRunning = true;
}

void Engine::Quit()
{
	isRunning = false;
}

void Engine::DeltaTimeUpdate() 
{
	Uint64 now = SDL_GetPerformanceCounter();
	deltaTime = (float)(now - previousFrame) / SDL_GetPerformanceFrequency();
	previousFrame = now;

	constexpr float deltaTimeMAX = 0.033f;
	deltaTime = (deltaTime > deltaTimeMAX) ? deltaTimeMAX : deltaTime;
}

void Engine::Physics()
{
	physics.Update(entityManager, deltaTime);
}

void Engine::Update()
{
	audioManager.Update();
	//uiManager.Update();

	// Update all components 
	Uint64 componentMsStart = SDL_GetPerformanceCounter();
	std::vector<Entity*> destroyables;
	for (auto& entity : entityManager.entities)
	{
		for (auto& component : entity->components)
		{
			// Update which entities are destroyables
			if (auto p = dynamic_cast<Animator*>(component))
			{
				if (p->destroyOnFinish && p->finished) 
				{
					destroyables.push_back(entity);
				}
			}
			// Update component
			component->Update(deltaTime);
		}
	}

	// Destroy the destroyable entities
	for (size_t i = 0; i < destroyables.size(); i++)
	{
		entityManager.DestroyEntity(*destroyables[i]);
	}

	Uint64 componentMsEnd = SDL_GetPerformanceCounter();
	if (debugger.enabled)
	{
		float ms = (componentMsEnd - componentMsStart) * 1000.0f / SDL_GetPerformanceFrequency();
		debugger.debugStats.componentMs = ms;
	}

	// Update physics
	Uint64 physicsStart = SDL_GetPerformanceCounter();
	Physics();
	Uint64 physicsEnd = SDL_GetPerformanceCounter();
	if (debugger.enabled)
	{
		float ms = (physicsEnd - physicsStart) * 1000.0f / SDL_GetPerformanceFrequency();
		debugger.debugStats.physicsMs = ms;
		debugger.debugStats.updateMs = (SDL_GetPerformanceCounter() - componentMsStart) * 1000.0f / SDL_GetPerformanceFrequency();
	}
}