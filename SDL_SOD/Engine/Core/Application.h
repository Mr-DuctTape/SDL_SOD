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

	SDL_Window* GetWindow()
	{
		return window;
	}
	~Application()
	{
		if(DEBUGPRINT)
				std::cout << "[" << "\033[35m" << "APPLICATION" << "\033[37m" << "] " << " Destroying window: " << window << "\n";
		SDL_DestroyWindow(window);
	}
};