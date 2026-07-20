#pragma once
#include <iostream>
#include "../Macros/DEBUGPRINT.h"
#include "../Debug/Debugger.h"
#include "Camera.h"

struct SDL_Renderer;
class EntityManager;
class Debugger;

class RenderingSystem
{
private:
	Debugger* debugger = nullptr;

public:
	Camera camera;

	SDL_Renderer* renderer = nullptr;
	SDL_Texture* renderTexture = nullptr;

	const int renderResX = 1280;
	const int renderResY = 720;

	Debugger* GetDebugger();
	void PresentScreen();
	void Initialize(Debugger* debugger);
	void RenderScreen(EntityManager& entityManager);
	void ClearScreen();

	RenderingSystem() {};
	~RenderingSystem()
	{
		if(DEBUGPRINT)
			std::cout << "(RenderingSystem) Destroying Renderer: " << renderer << "\n";
		SDL_DestroyRenderer(renderer);
	}
};