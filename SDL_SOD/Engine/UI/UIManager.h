#pragma once
#include <SDL3/SDL.h>
#include "../Math/Vector2.h"
#include <vector>
#include <unordered_map>

class InputSystem;
class AudioManager;
class Camera;

class UIManager
{
private: 
	InputSystem* inputSystem = nullptr;
	const AudioManager* audioManager = nullptr;
	const Camera* camera = nullptr;

	struct Button
	{
		std::string text{};
		SDL_Texture* texture = nullptr;
		Vec2f screenPosition;
		SDL_Color color{};
		float width = 0.0f;
		float height = 0.0f;
		bool pressed = false;
	};

	std::unordered_map<std::string, Button> m_buttons;
	void RenderText(SDL_Renderer* renderer);

public:
	void Initiailze(InputSystem& inputSystem, AudioManager& audioManager, Camera& camera);

	bool IsButtonPressed(const std::string& name);
	bool MouseHoverOver(const std::string& name);
	

	void RenderButtons(SDL_Renderer* renderer);
	void AddButton(const std::string& name, const Button& button);
	void RemoveButton(const std::string& name);

	void Update();
};