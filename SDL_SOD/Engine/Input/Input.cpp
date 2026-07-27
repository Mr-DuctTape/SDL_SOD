#include "Input.h"
#include "../Math/Vector2.h"

const Vec2f InputSystem::GetMousePosition() const
{
	float x, y;
	SDL_GetMouseState(&x, &y);
	return { x,y };
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