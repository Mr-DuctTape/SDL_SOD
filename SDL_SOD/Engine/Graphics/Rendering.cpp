#include <SDL3/SDL.h>
#include <vector>
#include "Rendering.h"
#include "../ECS/Entities.h"
#include "../ECS/Components.h"
#include "../Debugger/Debugger.h"
#include "../UI/UIManager.h"
#include "../Dialog/Dialog.h"

void RenderingSystem::Initialize(Debugger& debugger, UIManager& uiManager, DialogSystem& dialogSystem)
{
	renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, renderResX, renderResY);
	SDL_SetTextureScaleMode(renderTexture, SDL_SCALEMODE_PIXELART);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(renderTexture, SDL_BLENDMODE_BLEND);

	this->debugger = &debugger;
	this->uiManager = &uiManager;
	this->dialogSystem = &dialogSystem;

	CreateRenderingLayer("Default");

	if constexpr (DEBUGPRINT)
		std::cout << "[" << "\033[33m" << "RENDERINGSYSTEM" << "\033[37m" << "] " << " Initialized: " << this << "\n";
}

void RenderingSystem::CreateRenderingLayer(const std::string& name)
{
	int index = m_renderingLayers.size();

	std::string upperCaseName = StrToUpper(name);

	m_renderingLayerIndex.emplace(upperCaseName, index);
	m_renderingLayers.emplace_back();
}

std::vector<RenderingSystem::RenderingLayer>& RenderingSystem::GetRenderingLayers()
{
	return m_renderingLayers;
}

RenderingSystem::RenderingLayer& RenderingSystem::GetRenderingLayer(const std::string& name)
{
	std::string upperCaseName = StrToUpper(name);

	auto it = m_renderingLayerIndex.find(upperCaseName);
	if (it == m_renderingLayerIndex.end()) {
		throw std::runtime_error("Error, could not get rendering layer");
	}

	return m_renderingLayers[it->second];
}

RenderingSystem::RenderingLayer* RenderingSystem::FindRenderingLayer(const std::string& name)
{
	std::string upperCaseName = StrToUpper(name);

	auto it = m_renderingLayerIndex.find(upperCaseName);
	if (it == m_renderingLayerIndex.end()) {
		return nullptr;
	}

	return &m_renderingLayers[it->second];
}

Debugger* RenderingSystem::GetDebugger()
{
	return debugger;
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

void RenderingSystem::PopulateRenderingLayers(EntityManager& entityManager)
{
	for (Entity* entity : entityManager.entities)
	{
		auto& entityLayer = entity->GetRenderingLayer();
		auto& renderingLayer = GetRenderingLayer(entityLayer);
		renderingLayer.entities.push_back(entity);
	}
}

void RenderingSystem::DrawRenderingLayers()
{
	for (auto& layer : m_renderingLayers)
	{
		for (Entity* entity : layer.entities)
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
	}
}

void RenderingSystem::RenderScreen(EntityManager& entityManager)
{
	Uint64 startDraw = SDL_GetPerformanceCounter();

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_SetRenderTarget(renderer, renderTexture);

	ClearRenderingLayers();
	PopulateRenderingLayers(entityManager);
	DrawRenderingLayers();

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