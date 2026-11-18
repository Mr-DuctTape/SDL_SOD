#include "Input.h"
#include "../Math/Vector2.h"
#include "../Graphics/Rendering.h"
#include <cmath>

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

	if (!SDL_Init(SDL_INIT_GAMEPAD))
	{
		std::cout << "[ CRITICAL ERROR ] Unable to initailize SDL_INIT_GAMEPAD: ";
		std::cout << SDL_GetError();
		return;
	}

	m_gamePad = findGamepad();

	if constexpr (DEBUGPRINT)
		std::cout << "[" << "\033[35m" << "INPUTSYSTEM" << "\033[37m" << "] " << " Initialized: " << this << "\n";
}

void InputSystem::Process()
{
	memcpy(previousKeys, currentKeys, sizeof(currentKeys));
	memcpy(previousMouse, currentMouse, sizeof(currentMouse));
	memcpy(previousGamepadAxis, currentGamepadAxis, sizeof(currentGamepadAxis));
	memcpy(previousGamepadKeys, currentGamepadKeys, sizeof(currentGamepadKeys));

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// Find Gamepad
		case SDL_EVENT_GAMEPAD_ADDED:
			if (!m_gamePad) {
				m_gamePad = SDL_OpenGamepad(event.gdevice.which);
			}
			break;
		case SDL_EVENT_GAMEPAD_REMOVED:
			if (m_gamePad && event.gdevice.which == SDL_GetGamepadID(m_gamePad)) {
				SDL_CloseGamepad(m_gamePad);
				m_gamePad = findGamepad();
			}
			break;

			// Keyboard
		case SDL_EVENT_KEY_DOWN:
			currentKeys[event.key.scancode] = true;
			break;
		case SDL_EVENT_KEY_UP:
			currentKeys[event.key.scancode] = false;
			break;

			// Mouse 
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			currentMouse[event.button.button] = true;
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			currentMouse[event.button.button] = false;
			break;

			// Gamepad
		case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
			currentGamepadKeys[event.gbutton.button] = true;
			break;
		case SDL_EVENT_GAMEPAD_BUTTON_UP:
			currentGamepadKeys[event.gbutton.button] = false;
			break;
		case SDL_EVENT_GAMEPAD_AXIS_MOTION:
			constexpr int DEADZONE = 8000;

			int value = event.gaxis.value;

			currentGamepadAxis[event.gaxis.axis] =
				value > DEADZONE ? 1 :
				value < -DEADZONE ? -1 : 0;

			break;
		}
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

const bool InputSystem::GetGamepadButton(SDL_GamepadButton button)
{
	return currentGamepadKeys[button] && previousGamepadKeys[button];
}

const bool InputSystem::GetGamepadButtonUp(SDL_GamepadButton button)
{
	return !currentGamepadKeys[button] && previousGamepadKeys[button];
}

const bool InputSystem::GetGamepadButtonDown(SDL_GamepadButton button)
{
	return currentGamepadKeys[button] && !previousGamepadKeys[button];
}

const int InputSystem::GetGamepadAxis(SDL_GamepadAxis axis)
{
	return currentGamepadAxis[axis];
}

const bool InputSystem::GetKeyboardButtonDown(SDL_Scancode key)
{
	return currentKeys[key] && !previousKeys[key];
}

const bool InputSystem::GetKeyboardButtonUp(SDL_Scancode key)
{
	return !currentKeys[key] && previousKeys[key];
}

const bool InputSystem::GetKeyboardButton(SDL_Scancode key)
{
	return currentKeys[key] && previousKeys[key];
}