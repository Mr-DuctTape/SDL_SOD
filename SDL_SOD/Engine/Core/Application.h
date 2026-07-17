#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include "../Macros/DEBUGPRINT.h"

class Application
{
private:
	SDL_Window* window = nullptr;
public:
	void Initialize(SDL_Renderer*& renderer, int windowWidth = 1280, int windowHeight = 720);

	~Application()
	{
		if(DEBUGPRINT)
			std::cout << "(Application) Destroying window: " << window << "\n";
		SDL_DestroyWindow(window);
	}
};