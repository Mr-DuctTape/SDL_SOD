#pragma once
#include <unordered_map>
#include <string>
#include <SDL3/SDL.h>
#include <iostream>
#include "../Macros/DEBUGPRINT.h"

class AssetManager
{
private:
	std::unordered_map<std::string, SDL_Texture*> textures;
	SDL_Renderer* renderer = nullptr; 
public:
	AssetManager() {};

	void Initialize(SDL_Renderer* renderer)
	{
		this->renderer = renderer;

		if constexpr (DEBUGPRINT)
			std::cout << "[" << "\033[36m" << "ASSETMANAGER" << "\033[37m" << "] " << " Initialized: " << this << "\n";
	}

	SDL_Texture* CreateTexture(std::string name, const std::string& filePath);
	SDL_Texture* GetTexture(std::string name);

	~AssetManager();
};

