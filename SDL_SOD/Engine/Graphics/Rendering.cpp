#include <SDL3/SDL.h>
#include <vector>
#include "Rendering.h"
#include "../ECS/Entities.h"
#include "../ECS/Components.h"
#include "../Debug/Debugger.h"

// Component rendering

void TileMap::Render(RenderingSystem& renderingSystem, Camera& camera)
{
	Transform* transform = parent->GetComponent<Transform>();
	if (!transform)
	{
		std::cout << "Could not find Transform component (Tilemap rendering)\n";
		return;
	}

	for (int y = 0; y < tiles.size(); y++)
	{
		for (int x = 0; x < tiles[y].size(); x++)
		{
			// Get the position on screen
			SDL_FRect dst = WorldToScreen(GetTileBoxCollider2D(x, y).rect, camera);

			dst.x = std::round(dst.x);
			dst.y = std::round(dst.y);

			if (dst.x + dst.w < 0.0f ||
				dst.x > renderingSystem.renderResX ||
				dst.y + dst.h < 0.0f ||
				dst.y > renderingSystem.renderResY)
			{
				continue;
			}

			// Get the correct tile on texture
			SDL_FRect src{};
			int ID = GetTileID(x, y); 
			src.h = tilePixelHeight;
			src.w = tilePixelWidth;
			src.x = ID * tilePixelWidth;
			src.y = 0;

			// Draw tile onto screen
			SDL_RenderTexture(renderingSystem.renderer, spriteSheet, &src, &dst);
		}
	}
}

void Animator::Render(RenderingSystem& renderingSystem, Camera& camera)
{
	if (!currentAnimation.spriteSheet && !errorDisplayed)
	{
		std::cout << "=== Animator Error ===\n";
		std::cout << "(Animator): Animator spritesheet not found\n";
		std::cout << "(Animator): Entity ID: " << parent->ID << "\n";
		errorDisplayed = true;
		return;
	}

	SDL_FlipMode flip = flippedX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	SDL_FRect dst = WorldToScreen(currentAnimation.dst, camera);

	if (dst.x + dst.w < 0.0f ||
		dst.x > renderingSystem.renderResX ||
		dst.y + dst.h < 0.0f ||
		dst.y > renderingSystem.renderResY)
	{
		return;
	}

	SDL_RenderTextureRotated(
		renderingSystem.renderer,
		currentAnimation.spriteSheet,
		&currentAnimation.src,
		&dst,
		0.0f,
		nullptr,
		flip
	);
}


// Global rendering
void RenderingSystem::PresentScreen()
{
	SDL_RenderTexture(renderer, renderTexture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

void RenderingSystem::RenderFrame(EntityManager& entityManager, Debugger& debugger)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_SetRenderTarget(renderer, renderTexture);

	for (auto& e : entityManager.entities)
	{
		if (e->HasComponent<TileMap>())
		{
			TileMap* tileMap = e->GetComponent<TileMap>();
			tileMap->Render(*this, camera);
			continue;
		}

		if (!e->HasComponent<Transform>() || 
			!e->HasComponent<Sprite>())
			continue;

		if (e->HasComponent<Animator>())
		{
			Animator* animator = e->GetComponent<Animator>();
			animator->Render(*this, camera); // Renders pretty much everything needed
		}
		else 
		{
			auto sprt = e->GetComponent<Sprite>();
			auto transform = e->GetComponent<Transform>();

			SDL_FRect rect{};
			rect.h = sprt->height;
			rect.w = sprt->width;
			rect.x = transform->position.x;
			rect.y = transform->position.y;

			SDL_FRect dst = WorldToScreen(rect, camera);

			if (dst.x + dst.w < 0.0f ||
				dst.x > renderResX ||
				dst.y + dst.h < 0.0f ||
				dst.y > renderResY)
			{
				continue;
			}

			if (!sprt->texture)
			{
				SDL_RenderFillRect(renderer, &dst);
				continue;
			}
			SDL_RenderTexture(renderer, sprt->texture, NULL, &dst);
		}
	}


	// --- Debugging ---
	for (auto& r : debugger.BoxColliders)
	{
		SDL_RenderRect(renderer, &r);
	}

	for (auto& v : debugger.trajectories)
	{
		SDL_RenderLine(renderer, v.x1, v.y1, v.x2, v.y2);
	}

	debugger.trajectories.clear();
	debugger.BoxColliders.clear();
	SDL_SetRenderTarget(renderer, nullptr);
}

void RenderingSystem::ClearScreen()
{
	SDL_Color color;
	SDL_GetRenderDrawColor(renderer, &color.r, &color.g, &color.b, &color.a);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_SetRenderTarget(renderer, renderTexture);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, nullptr);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}