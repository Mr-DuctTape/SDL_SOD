#include "Application.h"
#include "../Macros/DEBUGPRINT.h"

void Application::Initialize(SDL_Renderer*& renderer, int windowWidth, int windowHeight)
{
	window = SDL_CreateWindow("Window", windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, "opengl");
}