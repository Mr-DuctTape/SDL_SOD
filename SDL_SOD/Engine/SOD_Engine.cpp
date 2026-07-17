#include "SOD_Engine.h"

void Engine::Initialize()
{
	// creates the window and renderer
	application.Initialize(renderingSystem.renderer);
	// sets up the renderer
	renderingSystem.Initialize();
	// Give debugger a camera ref
	debugger.Initialize(&renderingSystem.camera);
	// Give assetmanager a renderer
	assetManager.Initialize(renderingSystem.renderer);
	// Just gives pointer too the debugger
	physics.Intialize(&debugger);
	isRunning = true;
}

void Engine::Quit()
{
	isRunning = false;
}

void Engine::DeltaTimeUpdate() {
	Uint64 now = SDL_GetPerformanceCounter();
	deltaTime = (float)(now - previousFrame) / SDL_GetPerformanceFrequency();
	previousFrame = now;
}

void Engine::Physics()
{
	physics.Update(entityManager, deltaTime);
}

void Engine::Update()
{
	DeltaTimeUpdate(); // Before or after destroyable calculation idk???

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
}