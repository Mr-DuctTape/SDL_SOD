#include <SDL3/SDL.h>
#include <vector>
#include "Rendering.h"
#include "../ECS/Entities.h"
#include "../ECS/Components.h"
#include "../Debugger/Debugger.h"
#include "../UI/UIManager.h"
#include "../Dialog.h"

void RenderingSystem::Initialize(Debugger& debugger, UIManager& uiManager, DialogSystem& dialogSystem)
{
	renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, renderResX, renderResY);
	SDL_SetTextureScaleMode(renderTexture, SDL_SCALEMODE_PIXELART);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(renderTexture, SDL_BLENDMODE_BLEND);

	this->debugger = &debugger;
	this->uiManager = &uiManager;
	this->dialogSystem = &dialogSystem;

	if constexpr (DEBUGPRINT)
		std::cout << "[" << "\033[33m" << "RENDERINGSYSTEM" << "\033[37m" << "] " << " Initialized: " << this << "\n";
}
Debugger* RenderingSystem::GetDebugger()
{
	return debugger;
}

// Component rendering

void TileMap::Render(RenderingSystem& renderingSystem, Camera& camera)
{
	Transform& transform = parent->GetComponent<Transform>();

	for (int y = 0; y < tiles.size(); y++)
	{
		for (int x = 0; x < tiles[y].size(); x++)
		{
			// Get the position on screen
			SDL_FRect dst = WorldToScreen(GetTileBoxCollider2D(x, y).rect, camera);

			//TODO: Change tiles[y][x] == 3
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
		std::cout << "(Animator Rendering): Animator spritesheet not found " << currentAnimation.AnimationName << "\n";
		std::cout << "(Animator): Entity ID: " << parent->m_id << "\n";
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
	if (debugger->enabled)
	{
		if (destroyOnFinish)
		{
			SDL_SetRenderDrawColor(renderingSystem.renderer, 170, 140, 255, 255);
			SDL_RenderDebugText(renderingSystem.renderer, x, y + 20, "Destroyable");
		}
		else
		{
			SDL_SetRenderDrawColor(renderingSystem.renderer, 120, 180, 255, 255);
			SDL_RenderDebugText(renderingSystem.renderer, x - 10, y - 20, "Animator: ");
			SDL_RenderDebugText(renderingSystem.renderer, x - 10, y - 10, ("State: " + currentState).c_str());
			SDL_RenderDebugText(renderingSystem.renderer, x - 10, y, ("FlippedX: " + std::string(flippedX ? "true" : "false")).c_str());
		}
		debugger->debugStats.spritesRendered++;
		debugger->debugStats.drawCalls++;
	}
}

// UI Rendering
void UIElement::RenderButtonText(SDL_Renderer* renderer, UIElement& element)
{
	constexpr float charWidth = 8.0f;
	constexpr float charHeight = 8.0f;

	const float textWidth = static_cast<float>(element.displayText.size()) * charWidth;

	const float x = element.screenPos.x + (element.width - textWidth) * 0.5f;
	const float y = element.screenPos.y + (element.height - charHeight) * 0.5f;

	SDL_SetRenderDrawColor(renderer, 234, 204, 183, 255);

	if (element.displayName)
	{
		SDL_RenderDebugText(renderer, element.screenPos.x + 10, y, element.name.c_str());
	}
	SDL_RenderDebugText(renderer, x, y, element.displayText.c_str());
}

void UIElement::RenderButton(SDL_Renderer* renderer, UIElement& element)
{
	SDL_FRect rect{};
	rect.x = element.screenPos.x;
	rect.y = element.screenPos.y;
	rect.w = element.width;
	rect.h = element.height;

	if (element.texture)
	{
		SDL_RenderTexture(renderer, element.texture, NULL, &rect);
	}
	else
	{
		SDL_SetRenderDrawColor(renderer, element.currentColor.r, element.currentColor.g, element.currentColor.b, element.currentColor.a);
		SDL_RenderFillRect(renderer, &rect);
	}
}

// Global rendering
void RenderingSystem::PresentScreen()
{
	SDL_RenderTexture(renderer, renderTexture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

void RenderingSystem::RenderText(const std::string& text, Vec2f pos)
{
	SDL_RenderDebugText(renderer, pos.x, pos.y, text.c_str());
}

void RenderingSystem::RenderScreen(EntityManager& entityManager)
{
	Uint64 startDraw = SDL_GetPerformanceCounter();

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_SetRenderTarget(renderer, renderTexture);

	for (auto& entity : entityManager.entities)
	{
		if (entity->HasComponent<TileMap>())
		{
			TileMap& tileMap = entity->GetComponent<TileMap>();
			tileMap.Render(*this, camera);
			continue;
		}

		if (!entity->HasComponent<Transform>() ||
			!entity->HasComponent<Sprite>())
			continue;

		if (debugger && debugger->enabled && entity->HasComponent<Physics2D>())
		{
			Transform& transform = entity->GetComponent<Transform>();
			Physics2D& physics = entity->GetComponent<Physics2D>();

			Vec2f previous = transform.position;
			Vec2f velocity = physics.velocity;

			previous.y += 100.0f;
			previous.x += 50.0f;

			Vec2f position = previous;

			for (float t = 0; t < 0.5f; t += 0.05f)
			{
				velocity += physics.acceleration * 0.05f;
				position += velocity * 0.05f;

				debugger->DrawTrajectory(previous, position);

				previous = position;
			}
		}

		if (entity->HasComponent<Animator>())
		{
			Animator& animator = entity->GetComponent<Animator>();
			animator.Render(*this, camera);
			if (debugger && debugger->enabled && !animator.destroyOnFinish && animator.currentState != "AmberIdle")
			{
				Transform* transform = &entity->GetComponent<Transform>();
				std::string entityStr = "ID: " + std::to_string(entity->m_id);

				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				Vec2f screenPos = WorldToScreen(transform->position.x - 20, transform->position.y + 30, camera);
				SDL_RenderDebugText(renderer, screenPos.x, screenPos.y, entityStr.c_str());

				std::string posX("X: " + std::to_string(transform->position.x));
				std::string posY("Y: " + std::to_string(transform->position.y));
				SDL_RenderDebugText(renderer, screenPos.x, screenPos.y - 100, posX.c_str());
				SDL_RenderDebugText(renderer, screenPos.x, screenPos.y - 110, posY.c_str());
			}
		}
		else
		{
			Sprite& sprt = entity->GetComponent<Sprite>();
			Transform& transform = entity->GetComponent<Transform>();

			SDL_FRect rect{};
			rect.h = static_cast<float>(sprt.height);
			rect.w = static_cast<float>(sprt.width);
			rect.x = transform.position.x;
			rect.y = transform.position.y;

			SDL_FRect dst = WorldToScreen(rect, camera);

			if (dst.x + dst.w < 0.0f ||
				dst.x > renderResX ||
				dst.y + dst.h < 0.0f ||
				dst.y > renderResY)
			{
				continue;
			}

			if (!sprt.texture)
			{
				SDL_RenderFillRect(renderer, &dst);
			}
			else 
			{
				SDL_RenderTexture(renderer, sprt.texture, NULL, &dst);
			}
		}

		if (debugger && debugger->enabled)
		{
			Transform& transform = entity->GetComponent<Transform>();
			std::string entityStr = "ID: " + std::to_string(entity->m_id);

			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			Vec2f screenPos = WorldToScreen(transform.position.x - 20, transform.position.y + 30, camera);
			SDL_RenderDebugText(renderer, screenPos.x, screenPos.y, entityStr.c_str());
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
			SDL_RenderLine(renderer, v.p1.x, v.p1.y, v.p2.x, v.p2.y);
		}
		debugger->trajectories.clear();
		debugger->boxColliders.clear();

		debugger->DrawPerformanceStats(renderer, entityManager);
		Uint64 endDebugDraw = SDL_GetPerformanceCounter();
		debugger->debugStats.debugMs = (endDebugDraw - startDebugDraw) * 1000.0f / SDL_GetPerformanceFrequency();
	}

	// Render UI
	uiManager->RenderWindows(renderer);
	dialogSystem->RenderDialogs(renderer);

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
	SDL_Color color{};

	SDL_GetRenderDrawColor(renderer, &color.r, &color.g, &color.b, &color.a);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

	// clear renderTexture
	SDL_SetRenderTarget(renderer, renderTexture);
	SDL_RenderClear(renderer);

	// clear renderer
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