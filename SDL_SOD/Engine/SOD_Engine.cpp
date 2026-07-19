#include "SOD_Engine.h"

void Engine::Initialize()
{
	// creates the window and renderer
	application.Initialize(renderingSystem.renderer);

	// Initialize rendering system, give references or pointers to others
	renderingSystem.Initialize(&debugger);
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
	// Before or after destroyable calculation idk???
	Uint64 start = SDL_GetPerformanceCounter();

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

	// Update physics
	Physics();

	Uint64 end = SDL_GetPerformanceCounter();
	if (debugger.enabled) {
		float ms = (end - start) * 1000.0f / SDL_GetPerformanceFrequency();
		debugger.debugStats.updateMs = ms;
	}
}