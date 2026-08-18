#pragma once
#include <SDL3/SDL.h>
#include "../Macros/DEBUGPRINT.h"
#include <iostream>

enum Mouse
{
	LEFT_BUTTON = 1,
	MIDDLE_BUTTON,
	RIGHT_BUTTON,
	EXTRA_BUTTON1,
	EXTRA_BUTTON2,
	BUTTON_COUNT = 5
};

struct Vec2f;
class RenderingSystem;

class InputSystem
{
private:
	// Possible improvement for less memory usage on stack, by using bits to represent booleans
	RenderingSystem* renderingSystem;
	SDL_Window* window;

	bool previousKeys[SDL_SCANCODE_COUNT] = { false };
	bool currentKeys[SDL_SCANCODE_COUNT] = { false };
	bool previousMouse[BUTTON_COUNT] = { false };
	bool currentMouse[BUTTON_COUNT] = { false };

public:
	void Initialize(SDL_Window& window, RenderingSystem& renderingSystem);
	const bool GetButton(SDL_Scancode key);
	const bool GetButtonDown(SDL_Scancode key);
	const bool GetButtonUp(SDL_Scancode key);
	const bool GetMouseButtonDown(Mouse button);
	const bool GetMouseButton(Mouse button);
	const bool GetMouseButtonUp(Mouse button);
	const Vec2f GetMousePosition() const;
	void Process();
	~InputSystem()
	{
		if constexpr (DEBUGPRINT)
			std::cout << "[" << "\033[35m" << "INPUTSYSTEM" << "\033[37m" << "] " << " Destroying: " << this << "\n";
	}
};
