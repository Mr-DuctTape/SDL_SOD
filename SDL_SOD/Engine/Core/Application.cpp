#include "Application.h"
#include "../Macros/DEBUGPRINT.h"
#include <SDL3_image/SDL_image.h>

void Application::Initialize(SDL_Renderer*& renderer, int windowWidth, int windowHeight)
{
	window = SDL_CreateWindow("Window: Ignis", windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, "opengl");
	
	SDL_Surface* icon = IMG_Load("Assets/Textures/icon.png");
	if (!icon)
		std::cout << "Unable to create icon\n";
	SDL_SetWindowIcon(window, icon);
	SDL_DestroySurface(icon);
}