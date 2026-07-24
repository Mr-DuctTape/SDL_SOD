#include "AssetManager.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include "../Macros/DEBUGPRINT.h"

SDL_Texture* LoadIMG(const char* filePath, SDL_Renderer* renderer)
{
    SDL_Surface* surface = IMG_Load(filePath);
    if (!surface)
    {
        std::cout << "IMG_Load failed: " << "\n";
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        std::cout << "CreateTextureFromSurface failed: " << SDL_GetError() << "\n";
        return nullptr;
    }

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    SDL_DestroySurface(surface);
    return texture;
}

SDL_Texture* AssetManager::CreateTexture(std::string name, const std::string& filePath)
{
    SDL_Texture* texture = LoadIMG(filePath.c_str(), renderer);
    if (!texture)
    {
        std::cout << "Unable to create texture " << name << "\n";
        std::cout << "Path: " << filePath << "\n";
        return nullptr;
    }
    textures.emplace(name, texture);
    return texture;
}

SDL_Texture* AssetManager::GetTexture(std::string name)
{
    auto it = textures.find(name);
    if (it == textures.end())
        return nullptr;
    return it->second;
}

AssetManager::~AssetManager() 
{
    for (const auto& [name, texture] : textures)
    {
        if (!texture)
            continue;

        if(DEBUGPRINT)
            std::cout << "(AssetManager) Destroying texture " << name << ": " << texture << "\n";

        SDL_DestroyTexture(texture);
    }
}