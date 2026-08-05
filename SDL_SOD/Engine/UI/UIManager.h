#pragma once
#include <SDL3/SDL.h>
#include "../Math/Vector2.h"
#include <vector>
#include <unordered_map>

class InputSystem;
class AudioManager;
class RenderingSystem;
class Camera;

struct Button
{
	std::string text{};
	SDL_Texture* texture = nullptr;

	void(*binnedFunction)() = nullptr;

	Vec2f screenPosition;

	SDL_Color currentColor{};
	SDL_Color stationaryColor{};
	SDL_Color highlightedColor{};
	SDL_Color pressedColor{};

	float width = 0.0f;
	float height = 0.0f;
	bool pressed = false;
};

class UIManager
{
private: 
	InputSystem* inputSystem = nullptr;
	RenderingSystem* renderingSystem = nullptr;
	AudioManager* audioManager = nullptr;
	Camera* camera = nullptr;

private:
	std::unordered_map<std::string, Button> m_buttons;
	void RenderText(SDL_Renderer* renderer);

public:

	void Initialize(InputSystem& inputSystem, RenderingSystem& renderingSystem, AudioManager& audioManager, Camera& camera);

	bool IsButtonPressed(const std::string& name);
	bool MouseHoverOver(const std::string& name);
	
	void BindFunctionToButton(void(*func)(), const std::string& buttonName);
	void RenderButtons(SDL_Renderer* renderer);
	void AddButton(const std::string& name, const Button& button);
	void RemoveButton(const std::string& name);

	void Update();
};