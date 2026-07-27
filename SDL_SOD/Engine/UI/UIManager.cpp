#include "UIManager.h"
#include "../Input/Input.h"
#include "../Audio/AudioSystem.h"
#include "../Graphics/Camera.h"

void UIManager::Initiailze(InputSystem& inputSystem, AudioManager& audioManager, Camera& camera)
{
	this->inputSystem = &inputSystem;
	this->audioManager = &audioManager;
	this->camera = &camera;
}

void UIManager::RenderText(SDL_Renderer* renderer)
{
	for (auto& [key, val] : m_buttons)
	{
		Vec2f scaledPosition = WorldToScreen(val.screenPosition.x, val.screenPosition.y, *camera);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDebugText(renderer, scaledPosition.x, scaledPosition.y, val.text.c_str());
	}
}

void UIManager::RenderButtons(SDL_Renderer* renderer)
{
	for (auto& [key, val] : m_buttons)
	{
		SDL_FRect rect{};
		Vec2f scaledPosition = WorldToScreen(val.screenPosition.x, val.screenPosition.y, *camera);
		rect.x = scaledPosition.x;
		rect.y = scaledPosition.y;
		rect.w = val.width;
		rect.h = val.height;

		if (val.texture)
		{
			SDL_RenderTexture(renderer, val.texture, NULL, &rect);
		}
		else
		{
			SDL_SetRenderDrawColor(renderer, val.color.r, val.color.g, val.color.b, val.color.a);
			SDL_RenderFillRect(renderer, &rect);
		}
	}
	RenderText(renderer);
}

bool UIManager::MouseHoverOver(const std::string& name)
{
	if (m_buttons.find(name) == m_buttons.end())
		return false;

	Button& button = m_buttons.at(name);
	SDL_FRect buttonRect{};
	Vec2f scaledPosition = WorldToScreen(button.screenPosition.x, button.screenPosition.y, *camera);
	buttonRect.x = scaledPosition.x;
	buttonRect.y = scaledPosition.y;
	buttonRect.w = button.width;
	buttonRect.h = button.height;

	SDL_FRect mouseRect{};
	const Vec2f mousePos = inputSystem->GetMousePosition();
	mouseRect.x = mousePos.x;
	mouseRect.y = mousePos.y;
	mouseRect.h = 50;
	mouseRect.w = 50;

	return SDL_HasRectIntersectionFloat(&buttonRect, &mouseRect);
}

bool UIManager::IsButtonPressed(const std::string& name)
{
	if (m_buttons.find(name) == m_buttons.end())
		return false;

	if (m_buttons[name].pressed)
		return true;

	return false;
}

void UIManager::AddButton(const std::string& name, const Button& button)
{
	m_buttons.emplace(name, button);
}

void UIManager::RemoveButton(const std::string& name)
{
	if (m_buttons.find(name) == m_buttons.end())
		return;

	m_buttons.erase(name);
}

void UIManager::Update()
{
	for (auto& [key, val] : m_buttons)
	{
		if (MouseHoverOver(key) && inputSystem->GetMouseButton(Mouse::LEFT_BUTTON))
		{
			val.pressed = true;
		}
		else
		{
			val.pressed = false;
		}
	}
}