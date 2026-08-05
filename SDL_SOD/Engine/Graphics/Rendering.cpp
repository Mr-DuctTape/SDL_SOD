#include <SDL3/SDL.h>
#include <vector>
#include "Rendering.h"
#include "../ECS/Entities.h"
#include "../ECS/Components.h"
#include "../Debugger/Debugger.h"
#include "../UI/UIManager.h"

void RenderingSystem::Initialize(Debugger& debugger, UIManager& uiManager)
{
	renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, renderResX, renderResY);
	SDL_SetTextureScaleMode(renderTexture, SDL_SCALEMODE_PIXELART);
	//SDL_SetRenderLogicalPresentation(renderer, 1280, 720, SDL_LOGICAL_PRESENTATION_STRETCH);
	this->debugger = &debugger;
	this->uiManager = &uiManager;
}
Debugger* RenderingSystem::GetDebugger()
{
	return debugger;
}

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

			if (dst.x + dst.w < 0.0f ||
				dst.x > renderingSystem.renderResX ||
				dst.y + dst.h < 0.0f ||
				dst.y > renderingSystem.renderResY || tiles[y][x] == 3)
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
			Debugger* debugger = renderingSystem.GetDebugger();
			if (debugger && debugger->enabled)
			{
				debugger->debugStats.tilesRendered++;
				debugger->debugStats.drawCalls++;
			}
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

	int x = static_cast<int>(std::round(dst.x));
	int y = static_cast<int>(std::round(dst.y));

	Debugger* debugger = renderingSystem.GetDebugger();

	// some debugging for the animator
	if (debugger && debugger->enabled)
	{
		if (destroyOnFinish)
		{
			SDL_SetRenderDrawColor(renderingSystem.renderer, 170, 140, 255, 255);
			SDL_RenderDebugText(renderingSystem.renderer, x, y + 20,
				("Destroyable"));
		}
		else
		{
			SDL_SetRenderDrawColor(renderingSystem.renderer, 120, 180, 255, 255);
			SDL_RenderDebugText(renderingSystem.renderer, x - 10, y - 20,
				("Animator: "));
			SDL_RenderDebugText(renderingSystem.renderer, x - 10, y - 10,
				("State: " + currentState).c_str());

			SDL_RenderDebugText(renderingSystem.renderer, x - 10, y,
				("FlippedX: " + std::string(flippedX ? "true" : "false")).c_str());
		}
		debugger->debugStats.spritesRendered++;
		debugger->debugStats.drawCalls++;
	}
}

// Global rendering
void RenderingSystem::PresentScreen()
{
	SDL_RenderTexture(renderer, renderTexture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

void RenderingSystem::RenderScreen(EntityManager& entityManager)
{
	Uint64 startDraw = SDL_GetPerformanceCounter();

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
			animator->Render(*this, camera);
			continue;
		}
		else
		{
			auto sprt = e->GetComponent<Sprite>();
			auto transform = e->GetComponent<Transform>();

			SDL_FRect rect{};
			rect.h = static_cast<float>(sprt->height);
			rect.w = static_cast<float>(sprt->width);
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

		if (debugger && debugger->enabled)
		{
			Transform* transform = e->GetComponent<Transform>();
			std::string entity = "ID: " + std::to_string(e->ID);

			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			Vec2f screenPos = WorldToScreen(transform->position.x - 20, transform->position.y + 30, camera);
			SDL_RenderDebugText(renderer, screenPos.x, screenPos.y, entity.c_str());
		}
	}

	Uint64 endDraw = SDL_GetPerformanceCounter();
	if (debugger)
		debugger->debugStats.drawingMs = (endDraw - startDraw) * 1000.0f / SDL_GetPerformanceFrequency();

	// --- Debugging ---
	if (debugger && debugger->enabled)
	{
		Uint64 startDebugDraw = SDL_GetPerformanceCounter();
		for (auto& r : debugger->boxColliders)
		{
			SDL_SetRenderDrawColor(renderer, r.color.r, r.color.g, r.color.b, r.color.a);
			SDL_RenderRect(renderer, &r.rect);
		}

		for (auto& v : debugger->trajectories)
		{
			SDL_RenderLine(renderer, v.x1, v.y1, v.x2, v.y2);
		}
		debugger->trajectories.clear();
		debugger->boxColliders.clear();

		debugger->DrawPerformanceStats(renderer, entityManager);
		Uint64 endDebugDraw = SDL_GetPerformanceCounter();
		debugger->debugStats.debugMs = (endDebugDraw - startDebugDraw) * 1000.0f / SDL_GetPerformanceFrequency();
	}

	// Render UI

	Uint64 presentScreenStart = SDL_GetPerformanceCounter();
	SDL_SetRenderTarget(renderer, nullptr);
	PresentScreen();
	Uint64 presentScreenEnd = SDL_GetPerformanceCounter();
	if (debugger && debugger->enabled) 
	{
		debugger->debugStats.presentScreenMs = (presentScreenEnd - presentScreenStart) * 1000.0f / SDL_GetPerformanceFrequency();
	}
}

void RenderingSystem::ClearScreen()
{
	Uint64 start = SDL_GetPerformanceCounter();
	SDL_Color color;

	SDL_GetRenderDrawColor(renderer, &color.r, &color.g, &color.b, &color.a);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_SetRenderTarget(renderer, renderTexture);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, nullptr);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	if (debugger)
	{
		debugger->debugStats.drawCalls = 0;
		debugger->debugStats.spritesRendered = 0;
		debugger->debugStats.tilesRendered = 0;

		Uint64 end = SDL_GetPerformanceCounter();
		debugger->debugStats.clearScreenMs = 
			(end - start) * 1000.0f / SDL_GetPerformanceFrequency();

		debugger->debugStats.renderMs = debugger->debugStats.clearScreenMs + debugger->debugStats.drawingMs + debugger->debugStats.debugMs;
	}
}