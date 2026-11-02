#pragma once
#include <iostream>
#include "../Macros/DEBUGPRINT.h"
#include "../Debugger/Debugger.h"
#include "Camera.h"
#include <unordered_map>
#include <vector>

struct SDL_Renderer;
class EntityManager;
class Debugger;
class UIManager;
class DialogSystem;
class Entity;

class RenderingSystem
{
private:
	Debugger* debugger = nullptr;
	UIManager* uiManager = nullptr;
	DialogSystem* dialogSystem = nullptr;

	struct RenderingLayer
	{
		std::vector<Entity*> entities;
	};
	std::unordered_map<std::string, int> m_renderingLayerIndex;
	std::vector<RenderingLayer> m_renderingLayers;

	std::string StrToUpper(const std::string str)
	{
		std::string upper;
		upper.resize(str.size());
		for (size_t i = 0; i < str.size(); i++)
		{
			upper[i] = std::toupper(str[i]);
		}

		return upper;
	}

	void ClearRenderingLayers()
	{
		for (RenderingLayer& layer : m_renderingLayers)
		{
			layer.entities.clear();
		}
	}
	void PopulateRenderingLayers(EntityManager& entityManager);
	void DrawRenderingLayers();

public:
	Camera camera;

	SDL_Renderer* renderer = nullptr;
	SDL_Texture* renderTexture = nullptr;

	const int renderResX = 1280;
	const int renderResY = 720;

	Debugger* GetDebugger();

	std::vector<RenderingLayer>& GetRenderingLayers();
	RenderingLayer& GetRenderingLayer(const std::string& name);
	RenderingLayer* FindRenderingLayer(const std::string& name);
	void CreateRenderingLayer(const std::string& name);

	void PresentScreen();
	void RenderText(const std::string& text, Vec2f pos);
	void Initialize(Debugger& debugger, UIManager& uiManager, DialogSystem& dialogSystem);
	void RenderScreen(EntityManager& entityManager);
	void ClearScreen();

	RenderingSystem() = default;
	~RenderingSystem()
	{
		if constexpr (DEBUGPRINT) {
			std::cout << "[" << "\033[33m" << "RENDERINGSYSTEM" << "\033[37m" << "] " << " Destroying: " << this << "\n";
			std::cout << "[" << "\033[33m" << "RENDERINGSYSTEM" << "\033[37m" << "] " << " Destroying Renderer: " << renderer << "\n";
			std::cout << "[" << "\033[33m" << "RENDERINGSYSTEM" << "\033[37m" << "] " << " Destroying RenderTexture: " << renderTexture << "\n";
		}
		SDL_DestroyRenderer(renderer);
		SDL_DestroyTexture(renderTexture);
	}
};