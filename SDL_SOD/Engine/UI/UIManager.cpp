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

	const float textWidth = static_cast<float>(element.text.size()) * charWidth;

	const float x = element.screenPos.x + (element.width - textWidth) * 0.5f;

	const float y = element.screenPos.y + (element.height - charHeight) * 0.5f;


	SDL_SetRenderDrawColor(renderer, 105, 255, 105, 255);

	SDL_RenderDebugText(renderer, x, y, element.text.c_str());
}

void UIManager::RenderDropdowns(std::vector<UIDropDown>& dropDowns, SDL_Renderer* renderer)
{
	for (UIDropDown& dropDown : dropDowns)
	{
		UIElement::RenderButton(renderer, dropDown);

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
			if (button.MouseHoverOver(*inputSystem) &&
				inputSystem->GetMouseButtonDown(Mouse::LEFT_BUTTON) && !button.pressed)
			{
				button.currentColor = button.pressedColor;
				button.pressed = true;
			}
			else if (button.MouseHoverOver(*inputSystem))
			{
				button.currentColor = button.highlightedColor;
			}
			else
			{
				button.currentColor = button.stationaryColor;
			}
		}

		for (UIDropDown& dropDown : window.dropdownButtons)
		{
			if (dropDown.MouseHoverOver(*inputSystem))
			{
				dropDown.currentColor = dropDown.highlightedColor;
			}

			if (dropDown.MouseHoverOver(*inputSystem) &&
				inputSystem->GetMouseButtonDown(Mouse::LEFT_BUTTON) && !dropDown.pressed)
			{
				dropDown.currentColor = dropDown.pressedColor;
				dropDown.pressed = true;
			}
		}

		for (UIToggle& toggle : window.toggles)
		{
			if (toggle.MouseHoverOver(*inputSystem))
			{
				toggle.currentColor = toggle.highlightedColor;
			}

			if (toggle.MouseHoverOver(*inputSystem) &&
				inputSystem->GetMouseButtonDown(Mouse::LEFT_BUTTON) && !toggle.pressed)
			{
				toggle.currentColor = toggle.pressedColor;
				toggle.pressed = true;
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
		RenderDropdowns(window.dropdownButtons, renderer);
	}
}