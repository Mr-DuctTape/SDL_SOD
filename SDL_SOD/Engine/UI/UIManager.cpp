#include "UIManager.h"
#include "../Input/Input.h"
#include "../Audio/AudioSystem.h"
#include "../Graphics/Camera.h"
#include "../Graphics/Rendering.h"

void UIManager::Initialize(InputSystem& inputSystem, RenderingSystem& renderingSystem, AudioManager& audioManager)
{
	this->inputSystem = &inputSystem;
	this->audioManager = &audioManager;
	this->renderingSystem = &renderingSystem;
}

void UIElement::RenderButtonText(SDL_Renderer* renderer, UIElement& element)
{
	constexpr float charWidth = 8.0f;
	constexpr float charHeight = 8.0f;

	const float textWidth = static_cast<float>(element.displayText.size()) * charWidth;

	const float x = element.screenPos.x + (element.width - textWidth) * 0.5f;
	const float y = element.screenPos.y + (element.height - charHeight) * 0.5f;

	SDL_SetRenderDrawColor(renderer, 234, 204, 183, 255);

	if (element.displayName) 
	{
		SDL_RenderDebugText(renderer, element.screenPos.x + 10, y, element.name.c_str());
	}
	SDL_RenderDebugText(renderer, x, y, element.displayText.c_str());
}

void UIElement::RenderButton(SDL_Renderer* renderer, UIElement& element)
{
	SDL_FRect rect{};
	rect.x = element.screenPos.x;
	rect.y = element.screenPos.y;
	rect.w = element.width;
	rect.h = element.height;

	if (element.texture)
	{
		SDL_RenderTexture(renderer, element.texture, NULL, &rect);
	}
	else
	{
		SDL_SetRenderDrawColor(renderer, element.currentColor.r, element.currentColor.g, element.currentColor.b, element.currentColor.a);
		SDL_RenderFillRect(renderer, &rect);
	}
}

void UIManager::RenderButtons(std::vector<UIButton>& buttons, SDL_Renderer* renderer)
{
	for (UIButton& button : buttons)
	{
		UIElement::RenderButton(renderer, button);
		UIElement::RenderButtonText(renderer, button);
	}
}

bool UIElement::MouseHoverOver(InputSystem& inputSystem)
{
	SDL_FRect mouseRect{};
	Vec2f mousePos = inputSystem.GetMousePosition();
	SDL_FPoint point{ mousePos.x, mousePos.y };

	SDL_FRect buttonRect;
	buttonRect.x = screenPos.x;
	buttonRect.y = screenPos.y;
	buttonRect.h = height;
	buttonRect.w = width;

	if (SDL_PointInRectFloat(&point, &buttonRect))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool UIElement::IsButtonPressed()
{
	return pressed;
}

void SmoothSize(UIButton& button, float savedH, float savedW, const float deltaTime)
{
	float& height = button.height;
	float& width = button.width;

	constexpr float speed = 400.0f;

	constexpr float modified = 1.1f;

	if (height < savedH * modified)
	{
		const float increase = speed * deltaTime;
		height += increase;
		button.screenPos.y -= increase * 0.5f;
	}

	if (width < savedW * modified)
	{
		const float increase = speed * deltaTime;
		width += increase;
		button.screenPos.x -= increase * 0.5f;
	}
}

void UIManager::Update(const float deltaTime)
{
	for (UIWindow& window : m_windows)
	{
		if (!window.visible)
			continue;

		for (UIButton& button : window.buttons)
		{
			// Button press
			if (button.MouseHoverOver(*inputSystem) &&
				inputSystem->GetMouseButtonDown(Mouse::LEFT_BUTTON) && !button.pressed)
			{
				button.currentColor = button.pressedColor;
				button.PlayClickSound(*audioManager, 1.0f);
				button.pressed = true;
			}
			// Button hover
			else if (button.MouseHoverOver(*inputSystem))
			{
				if (!button.playedAudio)
				{
					button.savedHeight = button.height;
					button.savedWidth = button.width;
					button.savedScreenPos = button.screenPos;

					button.playedAudio = true;
					button.PlayHoverSound(*audioManager, 1.0f);
				}
				// Smoothly increase the size of the button
				SmoothSize(button, button.savedHeight, button.savedWidth, deltaTime);
				button.currentColor = button.highlightedColor;

			}
			else
			{
				if (button.savedHeight > 0 && button.savedWidth > 0) {
					button.screenPos = button.savedScreenPos;
					button.height = button.savedHeight;
					button.width = button.savedWidth;
				}

				button.playedAudio = false;
				button.currentColor = button.stationaryColor;
			}
		}
	}
}

void UIManager::RenderWindows(SDL_Renderer* renderer)
{
	for (UIWindow& window : m_windows)
	{
		if (!window.visible)
			continue;

		RenderButtons(window.buttons, renderer);
	}
}