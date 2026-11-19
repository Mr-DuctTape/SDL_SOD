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

enum Axis
{
	left = -1,
	neutral,
	right
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

	bool previousGamepadKeys[SDL_GAMEPAD_BUTTON_COUNT] = { false };
	bool currentGamepadKeys[SDL_GAMEPAD_BUTTON_COUNT] = { false };

	int previousGamepadAxis[SDL_GAMEPAD_AXIS_COUNT] = { false };
	int currentGamepadAxis[SDL_GAMEPAD_AXIS_COUNT] = { false };

	SDL_Gamepad* findGamepad() {
		int count = 0;
		SDL_JoystickID* ids = SDL_GetGamepads(&count);
		SDL_Gamepad* gamepad = count > 0 ? SDL_OpenGamepad(ids[0]) : nullptr;
		SDL_free(ids);
		return gamepad;
	}
public:
	SDL_Gamepad* m_gamePad = nullptr;

	SDL_GamepadType GetGamepadType()
	{
		return SDL_GetGamepadType(m_gamePad);
	}

	void Initialize(SDL_Window& window, RenderingSystem& renderingSystem);

	const bool GetKeyboardButton(SDL_Scancode key);
	const bool GetKeyboardButtonDown(SDL_Scancode key);
	const bool GetKeyboardButtonUp(SDL_Scancode key);

	const bool GetGamepadButtonDown(SDL_GamepadButton button);
	const bool GetGamepadButton(SDL_GamepadButton button);
	const bool GetGamepadButtonUp(SDL_GamepadButton button);

	[[nodiscard]] const int GetGamepadAxis(SDL_GamepadAxis axis); // -1 for right and 1 for left
	const bool HasGamepad()
	{
		return (m_gamePad) ? true : false;
	}

	const bool GetMouseButtonDown(Mouse button);
	const bool GetMouseButton(Mouse button);
	const bool GetMouseButtonUp(Mouse button);
	const Vec2f GetMousePosition() const;

	void Process();

	~InputSystem()
	{
		if constexpr (DEBUGPRINT) {
			std::cout << "[" << "\033[35m" << "INPUTSYSTEM" << "\033[37m" << "] " << " Destroying Gamepad: " << m_gamePad << "\n";
			SDL_free(m_gamePad);
			std::cout << "[" << "\033[35m" << "INPUTSYSTEM" << "\033[37m" << "] " << " Destroying: " << this << "\n";
		}
	}
};
