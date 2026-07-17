#pragma once
#include <SDL3/SDL.h>

enum Mouse
{
	LEFT_BUTTON = 1,
	MIDDLE_BUTTON,
	RIGHT_BUTTON,
	EXTRA_BUTTON1,
	EXTRA_BUTTON2,
	BUTTON_COUNT = 5
};

class InputSystem
{
private:
	bool previousKeys[SDL_SCANCODE_COUNT] = { false };
	bool currentKeys[SDL_SCANCODE_COUNT] = { false };
	bool previousMouse[BUTTON_COUNT] = { false };
	bool currentMouse[BUTTON_COUNT] = { false };

public:
	const bool GetButton(SDL_Scancode key);
	const bool GetButtonDown(SDL_Scancode key);
	const bool GetButtonUp(SDL_Scancode key);
	const bool GetMouseButtonDown(Mouse button);
	const bool GetMouseButton(Mouse button);
	const bool GetMouseButtonUp(Mouse button);
	void Process();
};
