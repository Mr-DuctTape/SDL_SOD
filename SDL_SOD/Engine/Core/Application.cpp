#include "Application.h"
#include "../Macros/DEBUGPRINT.h"
#include <SDL3_image/SDL_image.h>

void Application::Initialize(SDL_Renderer*& renderer, int windowWidth, int windowHeight)
{
	if(!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cout << "(CRITICAL ERROR) Unable to initailize SDL_Video: ";
		std::cout << SDL_GetError();
		return;
	}

	window = SDL_CreateWindow("Window: Ignis", windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, NULL);

	SDL_SetRenderVSync(renderer, 0);
	int vsync;
	SDL_GetRenderVSync(renderer, &vsync);
	std::cout << "VSync: " << vsync << '\n';
	
	SDL_Surface* icon = IMG_Load("Assets/Textures/icon.png");
	if (!icon)
		std::cout << "Unable to create icon\n";
	SDL_SetWindowIcon(window, icon);
	SDL_DestroySurface(icon);

	if constexpr (DEBUGPRINT)
		std::cout << "[" << "\033[35m" << "APPLICATION" << "\033[37m" << "] " << " Initialized: " << this << "\n";
}