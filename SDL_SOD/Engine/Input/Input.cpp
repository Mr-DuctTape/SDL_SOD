#include "Input.h"
#include "../Math/Vector2.h"
#include "../Graphics/Rendering.h"

const Vec2f InputSystem::GetMousePosition() const
{
	float mX, mY;
	SDL_GetMouseState(&mX, &mY);
	int wX, wY;
	SDL_GetWindowSize(window, &wX, &wY);
	float X = mX * (static_cast<float>(renderingSystem->renderResX) / wX);
	float Y = mY * (static_cast<float>(renderingSystem->renderResY) / wY);
	return { X,Y };
}

void InputSystem::Initialize(SDL_Window& window, RenderingSystem& renderingSystem)
{
	this->window = &window;
	this->renderingSystem = &renderingSystem;
	if constexpr (DEBUGPRINT)
		std::cout << "[" << "\033[35m" << "INPUTSYSTEM" << "\033[37m" << "] " << " Initialized: " << this << "\n";
}

void InputSystem::Process()
{
	memcpy(previousKeys, currentKeys, sizeof(currentKeys));
	memcpy(previousMouse, currentMouse, sizeof(currentMouse));

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_EVENT_KEY_DOWN)
			currentKeys[e.key.scancode] = true;
		if (e.type == SDL_EVENT_KEY_UP)
			currentKeys[e.key.scancode] = false;

		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
			currentMouse[e.button.button] = true;
		if (e.type == SDL_EVENT_MOUSE_BUTTON_UP)
			currentMouse[e.button.button] = false;
	}
}

const bool InputSystem::GetMouseButtonDown(Mouse button)
{
	return currentMouse[button] && !previousMouse[button];
}

const bool InputSystem::GetMouseButtonUp(Mouse button)
{
	return !currentMouse[button] && previousMouse[button];
}

const bool InputSystem::GetMouseButton(Mouse button)
{
	return currentMouse[button] && previousMouse[button];
}

const bool InputSystem::GetButtonDown(SDL_Scancode key)
{
	return currentKeys[key] && !previousKeys[key];
}

const bool InputSystem::GetButtonUp(SDL_Scancode key)
{
	return !currentKeys[key] && previousKeys[key];
}

const bool InputSystem::GetButton(SDL_Scancode key)
{
	return currentKeys[key] && previousKeys[key];
}