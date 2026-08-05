#include "UIManager.h"
#include "../Input/Input.h"
#include "../Audio/AudioSystem.h"
#include "../Graphics/Camera.h"
#include "../Graphics/Rendering.h"

void UIManager::Initialize(InputSystem& inputSystem, RenderingSystem& renderingSystem, AudioManager& audioManager, Camera& camera)
{
	this->inputSystem = &inputSystem;
	this->audioManager = &audioManager;
	this->camera = &camera;
	this->renderingSystem = &renderingSystem;
}

void UIManager::RenderText(SDL_Renderer* renderer)
{
	for (auto& [key, button] : m_buttons)
	{
		SDL_FRect rect{};
		rect.w = button.width;
		rect.h = button.height;

		int offset = button.text.size();
		Vec2f scaledPosition{ button.screenPosition.x, button.screenPosition.y};
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDebugText(renderer, scaledPosition.x, scaledPosition.y, button.text.c_str());
	}
}

void UIManager::BindFunctionToButton(void(*func)(), const std::string& name)
{
	auto it = m_buttons.find(name);
	if (it == m_buttons.end())
		return;

	it->second.binnedFunction = func;
}

void UIManager::RenderButtons(SDL_Renderer* renderer)
{
	for (auto& [key, button] : m_buttons)
	{
		SDL_FRect rect{};
		rect.x = button.screenPosition.x;
		rect.y = button.screenPosition.y;
		rect.w = button.width;
		rect.h = button.height;

		if (button.texture)
		{
			SDL_RenderTexture(renderer, button.texture, NULL, &rect);
		}
		else
		{
			SDL_SetRenderDrawColor(renderer, button.currentColor.r, button.currentColor.g, button.currentColor.b, button.currentColor.a);
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
	buttonRect.x = button.screenPosition.x;
	buttonRect.y = button.screenPosition.y;
	buttonRect.w = button.width;
	buttonRect.h = button.height;

	SDL_FRect mouseRect{};
	Vec2f mousePos = inputSystem->GetMousePosition();
	SDL_FPoint point{ mousePos.x, mousePos.y };

	if (SDL_PointInRectFloat(&point, &buttonRect))
	{
		button.currentColor = button.highlightedColor;
		return true;
	}
	else
	{
		button.currentColor = button.stationaryColor;
		return false;
	}
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
		if (MouseHoverOver(key) && inputSystem->GetMouseButtonDown(Mouse::LEFT_BUTTON))
		{
			val.currentColor = val.pressedColor;
			val.pressed = true;

			if (val.binnedFunction) {
				val.binnedFunction();
				val.pressed = false;
			}
		}
		else
		{
			val.pressed = false;
			continue;
		}
	}
}