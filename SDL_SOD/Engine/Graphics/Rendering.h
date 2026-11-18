#pragma once
#include <iostream>
#include "../Macros/DEBUGPRINT.h"
#include "../Debugger/Debugger.h"
#include "Camera.h"
#include <unordered_map>
#include <vector>

struct SDL_Renderer;
class EntityManager;
class Debugger;
class UIManager;
class DialogSystem;
class Entity;

class RenderingSystem
{
private:
	Debugger* debugger = nullptr;
	UIManager* uiManager = nullptr;
	DialogSystem* dialogSystem = nullptr;

	SDL_Texture* lightTexture = nullptr;

public:
	Camera camera;

	SDL_Renderer* renderer = nullptr;

	SDL_Texture* spriteLayer = nullptr;
	SDL_Texture* lightLayer = nullptr;
	SDL_Texture* uiLayer = nullptr;
	SDL_Texture* renderLayer = nullptr;

	bool enableLighting = true;

	const int renderResX = 1280;
	const int renderResY = 720;

	Debugger* GetDebugger();

	void PresentScreen();
	void RenderText(const std::string& text, Vec2f pos);
	void Initialize(Debugger& debugger, UIManager& uiManager, DialogSystem& dialogSystem);
	void RenderLights(EntityManager& entityManager);
	void RenderScreen(EntityManager& entityManager);
	void ClearScreen();

	RenderingSystem() = default;
	~RenderingSystem()
	{
		if constexpr (DEBUGPRINT) {
			std::cout << "[" << "\033[33m" << "RENDERINGSYSTEM" << "\033[37m" << "] " << " Destroying: " << this << "\n";
			std::cout << "[" << "\033[33m" << "RENDERINGSYSTEM" << "\033[37m" << "] " << " Destroying Renderer: " << renderer << "\n";
			std::cout << "[" << "\033[33m" << "RENDERINGSYSTEM" << "\033[37m" << "] " << " Destroying RenderTexture: " << renderLayer << "\n";
		}
		SDL_DestroyRenderer(renderer);
		SDL_DestroyTexture(renderLayer);
		SDL_DestroyTexture(spriteLayer);
		SDL_DestroyTexture(lightLayer);
		SDL_DestroyTexture(uiLayer);
	}
};