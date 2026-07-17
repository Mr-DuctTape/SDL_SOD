#pragma once
#include <iostream>
#include "../Macros/DEBUGPRINT.h"
#include "Camera.h"

struct SDL_Renderer;
class EntityManager;
class Debugger;

class RenderingSystem
{
public:
	Camera camera;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* renderTexture = nullptr;

	const int renderResX = 1280;
	const int renderResY = 720;

	void PresentScreen();
	void Initialize() {
		renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_TARGET, renderResX, renderResY);
		SDL_SetTextureScaleMode(renderTexture, SDL_SCALEMODE_NEAREST);
		SDL_SetRenderLogicalPresentation(
			renderer,
			renderResX,
			renderResY,
			SDL_LOGICAL_PRESENTATION_STRETCH
		);
	}
	void RenderFrame(EntityManager& entityManager, Debugger& debugger);
	void ClearScreen();

	RenderingSystem()
	{
	
	}

	~RenderingSystem()
	{
		if(DEBUGPRINT)
			std::cout << "(RenderingSystem) Destroying Renderer: " << renderer << "\n";
		SDL_DestroyRenderer(renderer);
	}
};