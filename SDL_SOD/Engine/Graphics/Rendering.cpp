#include <SDL3/SDL.h>
#include <vector>

#include "Rendering.h"
#include "../ECS/Entities.h"
#include "../ECS/Components.h"
#include "../Debugger/Debugger.h"
#include "../UI/UIManager.h"
#include "../Dialog/Dialog.h"

// ty chatbbpt

SDL_Texture* CreateCircleLightTexture(SDL_Renderer* renderer)
{
	constexpr int size = 64;

	SDL_Texture* texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		size,
		size
	);

	if (!texture)
		return nullptr;

	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD);

	void* pixels = nullptr;
	int pitch = 0;

	if (!SDL_LockTexture(texture, nullptr, &pixels, &pitch))
	{
		SDL_DestroyTexture(texture);
		return nullptr;
	}

	constexpr float center = size / 2.0f;
	constexpr float radius = size / 2.0f;

	auto* pixelData = static_cast<Uint32*>(pixels);

	const SDL_PixelFormatDetails* format =
		SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA8888);

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			float dx = x - center;
			float dy = y - center;

			float distance = sqrtf(dx * dx + dy * dy);
			float d = distance / radius;

			float glow = expf(-d * d * 3.0f);
			float core = expf(-d * d * 12.0f);

			float intensity = std::clamp(
				glow * 0.65f + core * 0.65f,
				0.0f,
				1.0f
			);

			Uint8 alpha = static_cast<Uint8>(intensity * 255.0f);

			pixelData[y * (pitch / 4) + x] =
				SDL_MapRGBA(format, nullptr, 255, 255, 255, alpha);
		}
	}

	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
	SDL_UnlockTexture(texture);

	return texture;
}

void RenderingSystem::Initialize(Debugger& debugger, UIManager& uiManager, DialogSystem& dialogSystem)
{
	renderLayer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, renderResX, renderResY);
	SDL_SetTextureScaleMode(renderLayer, SDL_SCALEMODE_PIXELART);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(renderLayer, SDL_BLENDMODE_BLEND);

	spriteLayer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, renderResX, renderResY);
	SDL_SetTextureScaleMode(renderLayer, SDL_SCALEMODE_PIXELART);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(renderLayer, SDL_BLENDMODE_BLEND);

	uiLayer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, renderResX, renderResY);
	SDL_SetTextureScaleMode(renderLayer, SDL_SCALEMODE_PIXELART);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(renderLayer, SDL_BLENDMODE_BLEND);

	lightLayer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, renderResX, renderResY);
	SDL_SetTextureScaleMode(renderLayer, SDL_SCALEMODE_PIXELART);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(renderLayer, SDL_BLENDMODE_BLEND);

	lightTexture = CreateCircleLightTexture(renderer);

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

// Global rendering
void RenderingSystem::PresentScreen()
{
	SDL_SetTextureBlendMode(spriteLayer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(lightLayer, SDL_BLENDMODE_MOD);
	SDL_SetTextureBlendMode(renderLayer, SDL_BLENDMODE_BLEND);

	SDL_RenderTexture(renderer, spriteLayer, nullptr, nullptr);

	if(enableLighting)
		SDL_RenderTexture(renderer, lightLayer, nullptr, nullptr);

	SDL_RenderTexture(renderer, renderLayer, nullptr, nullptr);

	SDL_RenderTexture(renderer, uiLayer, nullptr, nullptr);

	SDL_RenderPresent(renderer);
}

void RenderingSystem::RenderText(const std::string& text, Vec2f pos)
{
	SDL_RenderDebugText(renderer, pos.x, pos.y, text.c_str());
}

void RenderingSystem::RenderLights(EntityManager& entityManager)
{
	bool foundLight = false;

	std::vector<Entity*> lightEntities;
	lightEntities.reserve(32);
	for (auto& entity : entityManager.entities)
	{
		if (entity->HasComponent<Light2D>())
		{
			lightEntities.emplace_back(entity);
			foundLight = true;
		}
	}

	if (!foundLight)
		return;

	SDL_SetRenderTarget(renderer, lightLayer);
	for (auto& entity : lightEntities)
	{
		Light2D& light = entity->GetComponent<Light2D>();
		Transform& transform = entity->GetComponent<Transform>();

		SDL_FRect lightPosition;
		lightPosition.w = light.range;
		lightPosition.h = light.range;
		lightPosition.x = transform.position.x - lightPosition.w * 0.5f;
		lightPosition.y = transform.position.y - lightPosition.h * 0.5f;

		lightPosition = WorldToScreen(lightPosition, camera);

		SDL_RenderTexture(renderer, lightTexture, NULL, &lightPosition);
	}
}

void RenderingSystem::RenderScreen(EntityManager& entityManager)
{
	Uint64 startDraw = SDL_GetPerformanceCounter();

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_SetRenderTarget(renderer, spriteLayer);

	// Sprite & Component rendering
	for (Entity* entity : entityManager.entities)
	{
		if (entity->HasComponent<TileMap>())
		{
			TileMap& tileMap = entity->GetComponent<TileMap>();
			tileMap.Render(*this, camera);
			continue;
		}

		if (!entity->HasComponent<Transform>() || !entity->HasComponent<Sprite>())
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

			if (dst.x + dst.w < 0.0f || dst.x > renderResX ||
				dst.y + dst.h < 0.0f || dst.y > renderResY)
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

	// Lighting pass
	if(enableLighting)
		RenderLights(entityManager);

	Uint64 endDraw = SDL_GetPerformanceCounter();
	if (debugger)
		debugger->debugStats.drawingMs = (endDraw - startDraw) * 1000.0f / SDL_GetPerformanceFrequency();

	// --- Debugging ---
	SDL_SetRenderTarget(renderer, uiLayer);
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

	SDL_Renderer* r = renderer;
	auto ClearLayer = [&r](SDL_Texture* texture, SDL_Color color) -> void
		{
			SDL_SetRenderTarget(r, texture);
			SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
			SDL_RenderClear(r);
		};

	SDL_Color defColor = { 0,0,0,0 };

	ClearLayer(spriteLayer, defColor);
	ClearLayer(lightLayer, { 0, 0, 0, 255 });
	ClearLayer(renderLayer, defColor);
	ClearLayer(uiLayer, defColor);
	ClearLayer(nullptr, defColor);

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