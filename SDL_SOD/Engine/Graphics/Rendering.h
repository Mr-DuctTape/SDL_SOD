#pragma once
#include <iostream>
#include "../Macros/DEBUGPRINT.h"
#include "../Debugger/Debugger.h"
#include "Camera.h"

struct SDL_Renderer;
class EntityManager;
class Debugger;
class UIManager;
class DialogSystem;

class RenderingSystem
{
private:
	Debugger* debugger = nullptr;
	UIManager* uiManager = nullptr;
	DialogSystem* dialogSystem = nullptr;

public:
	Camera camera;

	SDL_Renderer* renderer = nullptr;
	SDL_Texture* renderTexture = nullptr;

	const int renderResX = 1280;
	const int renderResY = 720;

	Debugger* GetDebugger();
	void PresentScreen();
	void RenderText(const std::string& text, Vec2f pos);
	void Initialize(Debugger& debugger, UIManager& uiManager, DialogSystem& dialogSystem);
	void RenderScreen(EntityManager& entityManager);
	void ClearScreen();

	RenderingSystem() = default;
	~RenderingSystem()
	{
		if (DEBUGPRINT)
			std::cout << "[" << "\033[33m" << "RENDERINGSYSTEM" << "\033[37m" << "] " << " Destroying Renderer: " << renderer << "\n";
		SDL_DestroyRenderer(renderer);
	}
};