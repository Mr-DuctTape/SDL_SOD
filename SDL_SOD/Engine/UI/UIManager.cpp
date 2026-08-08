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

	if (element.displayName) {
		SDL_RenderDebugText(renderer, element.screenPos.x, y, element.name.c_str());
	}
	SDL_RenderDebugText(renderer, x, y, element.displayText.c_str());
}

void UIManager::RenderDropdowns(std::vector<UIDropDown>& dropDowns, SDL_Renderer* renderer)
{
	for (UIDropDown& dropDown : dropDowns)
	{
		UIElement::RenderButton(renderer, dropDown);
		UIElement::RenderButtonText(renderer, dropDown);

		if (dropDown.pressed) 
		{
			size_t elementIndex = 0;
			for (auto& element : dropDown.elements)
			{
				float offsetY = dropDown.height * (elementIndex + 1);

				element.height = dropDown.height;
				element.width = dropDown.width;
				element.screenPos = { dropDown.screenPos.x, dropDown.screenPos.y + offsetY };

				UIElement::RenderButton(renderer, element);
				UIElement::RenderButtonText(renderer, element);

				elementIndex++;
			}
		}
	}
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

void UIManager::Update()
{
	for (UIWindow& window : m_windows)
	{
		if (!window.visible || window.destroyed)
			continue;

		for (UIButton& button : window.buttons)
		{
			// Button press
			if (button.MouseHoverOver(*inputSystem) &&
				inputSystem->GetMouseButtonDown(Mouse::LEFT_BUTTON) && !button.pressed)
			{
				button.currentColor = button.pressedColor;
				audioManager->Play("Click", 1.0f);
				button.pressed = true;
			}
			// Button hover
			else if (button.MouseHoverOver(*inputSystem))
			{
				if (!button.playedAudio) {
					button.playedAudio = true;
					audioManager->Play("Hover", 1.0f);
				}
				button.currentColor = button.highlightedColor;
			}
			else
			{
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