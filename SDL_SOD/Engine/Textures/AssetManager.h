#pragma once
#include <unordered_map>
#include <string>
#include <SDL3/SDL.h>
#include <iostream>

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
	}

	SDL_Texture* CreateTexture(std::string name, const std::string& filePath);
	SDL_Texture* GetTexture(std::string name);

	~AssetManager();
};

