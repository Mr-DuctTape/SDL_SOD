#include "Debugger.h"
#include "../ECS/Components.h"
#include "../ECS/Entities.h"
#include "../Math/Vector2.h"
#include "../Graphics/Camera.h"
#include <format>

void Debugger::DrawBoxCollider2D(const BoxCollider2D& a, SDL_Color color)
{
	if (!camRef)
		return;

	SDL_FRect rect = WorldToScreen(a.rect, *camRef);
	Collider collider;
	collider.rect = rect;
	collider.color = color;
	boxColliders.push_back(collider);
}

void Debugger::DrawPerformanceStats(SDL_Renderer* renderer, EntityManager& entityManager)
{
	debugStats.entities = entityManager.entities.size();
	debugStats.components = 0;
	for (auto entity : entityManager.entities)
	{
		debugStats.components += entity->m_components.size();
	}
	Vec2f pos = { 20,20 };

	SDL_SetRenderDrawColor(
		renderer,
		255,
		255,
		255,
		255);

	if (!camRef)
	{
		std::string cameraPos("Camera Ref: nullptr");
		SDL_RenderDebugText(renderer, 20, 700, cameraPos.c_str());
	}
	else
	{
		std::string cameraPos("Camera position X: " + std::to_string(camRef->pos.x) + " Y: " + std::to_string(camRef->pos.y) + "\n");
		SDL_RenderDebugText(renderer, 20, 700, cameraPos.c_str());
	}

	//==========================================================
	// STYLE
	//==========================================================

	constexpr int LABEL_WIDTH = 18;

	constexpr int LINE_HEIGHT = 10;
	constexpr int SECTION_GAP = 30;
	constexpr int HEADER_GAP = 20;

	// VSCode-inspired colors
	constexpr SDL_Color PERFORMANCE_COLOR = { 220, 180,  80, 255 }; // Gold
	constexpr SDL_Color RENDERING_COLOR = { 80, 200, 230, 255 }; // Cyan
	constexpr SDL_Color PHYSICS_COLOR = { 100, 220, 130, 255 }; // Green
	constexpr SDL_Color ECS_COLOR = { 170, 120, 230, 255 }; // Purple


	//==========================================================
	// HEADERS
	//==========================================================

	std::string performanceHeader = "====== PERFORMANCE ======";
	std::string renderingHeader = "======= RENDERING =======";
	std::string physicsHeader = "======== PHYSICS ========";
	std::string ecsHeader = "========== ECS ==========";


	//==========================================================
	// PERFORMANCE
	//==========================================================

	if (debugStats.OtherMs < 0)
		debugStats.OtherMs = 0;

	std::string FPS = std::format(
		"{:<18}{:.3f}",
		"FPS (Avg):",
		debugStats.fps);

	std::string frameMs = std::format(
		"{:<18}{:.3f} ms",
		"Frame Time:",
		debugStats.frameMs);


	std::string updateMs = std::format(
		"{:<18}{:.3f} ms",
		"Engine Update:",
		debugStats.updateMs);

	std::string componentMs = std::format(
		"  |- Components:   {:.3f} ms",
		debugStats.componentMs);

	std::string physicsMs = std::format(
		"  `- Physics:      {:.3f} ms",
		debugStats.physicsMs);



	std::string renderMs = std::format(
		"{:<18}{:.3f} ms",
		"Render CPU:",
		debugStats.renderMs);

	std::string clearScreenMs = std::format(
		"  |- Clear:        {:.3f} ms",
		debugStats.clearScreenMs);

	std::string drawingMs = std::format(
		"  |- Drawing:      {:.3f} ms",
		debugStats.drawingMs);

	std::string debugMs = std::format(
		"  `- Debug:        {:.3f} ms",
		debugStats.debugMs);


	std::string presentScreenMs = std::format(
		"{:<18}{:.3f} ms",
		"Present:",
		debugStats.presentScreenMs);


	std::string otherMs = std::format(
		"{:<18}{:.3f} ms",
		"Other:",
		debugStats.OtherMs);


	//==========================================================
	// RENDERING
	//==========================================================

	SDL_PropertiesID props = SDL_GetRendererProperties(renderer);

	const char* name = SDL_GetStringProperty(
		props,
		SDL_PROP_RENDERER_NAME_STRING,
		"Unknown"
	);

	std::string graphicsAPI = std::format(
		"{:<18}{}",
		"Graphics API: ",
		name
		);

	std::string drawCalls = std::format(
		"{:<18}{}",
		"Draw Calls:",
		debugStats.drawCalls);

	std::string sprites = std::format(
		"{:<18}{}",
		"Sprites:",
		debugStats.spritesRendered);

	std::string tiles = std::format(
		"{:<18}{}",
		"Tiles:",
		debugStats.tilesRendered);


	//==========================================================
	// PHYSICS
	//==========================================================

	std::string collisionChecks = std::format(
		"{:<18}{}",
		"Collision Checks:",
		debugStats.collisionChecks);

	std::string resolved = std::format(
		"{:<18}{}",
		"Resolved:",
		debugStats.resolvedCollisions);


	//==========================================================
	// ECS
	//==========================================================

	std::string entities = std::format(
		"{:<18}{}",
		"Entities:",
		debugStats.entities);

	std::string components = std::format(
		"{:<18}{}",
		"Components:",
		debugStats.components);
	//==========================================================
	// DRAW
	//==========================================================

	int y = static_cast<int>(pos.y);

	 
	//==========================================================
	// PERFORMANCE
	//==========================================================

	SDL_SetRenderDrawColor(
		renderer,
		PERFORMANCE_COLOR.r,
		PERFORMANCE_COLOR.g,
		PERFORMANCE_COLOR.b,
		PERFORMANCE_COLOR.a);


	SDL_RenderDebugText(renderer, pos.x, y, performanceHeader.c_str());
	y += 20;


	SDL_RenderDebugText(renderer, pos.x, y, FPS.c_str());
	y += 10;

	SDL_RenderDebugText(renderer, pos.x, y, frameMs.c_str());
	y += 20;


	SDL_RenderDebugText(renderer, pos.x, y, updateMs.c_str());
	y += 10;

	SDL_RenderDebugText(renderer, pos.x, y, componentMs.c_str());
	y += 10;

	SDL_RenderDebugText(renderer, pos.x, y, physicsMs.c_str());
	y += 20;



	SDL_RenderDebugText(renderer, pos.x, y, renderMs.c_str());
	y += 10;

	SDL_RenderDebugText(renderer, pos.x, y, clearScreenMs.c_str());
	y += 10;

	SDL_RenderDebugText(renderer, pos.x, y, drawingMs.c_str());
	y += 10;

	SDL_RenderDebugText(renderer, pos.x, y, debugMs.c_str());
	y += 20;



	SDL_RenderDebugText(renderer, pos.x, y, presentScreenMs.c_str());
	y += 20;



	SDL_RenderDebugText(renderer, pos.x, y, otherMs.c_str());
	y += 30;


	//==========================================================
	// RENDERING STATS
	//==========================================================

	SDL_SetRenderDrawColor(
		renderer,
		RENDERING_COLOR.r,
		RENDERING_COLOR.g,
		RENDERING_COLOR.b,
		RENDERING_COLOR.a);

	SDL_RenderDebugText(renderer, pos.x, y, renderingHeader.c_str());
	y += 20;

	SDL_RenderDebugText(renderer, pos.x, y, graphicsAPI.c_str());
	y += 10;

	SDL_RenderDebugText(renderer, pos.x, y, drawCalls.c_str());
	y += 10;

	SDL_RenderDebugText(renderer, pos.x, y, sprites.c_str());
	y += 10;

	SDL_RenderDebugText(renderer, pos.x, y, tiles.c_str());
	y += 30;


	//==========================================================
	// PHYSICS
	//==========================================================

	SDL_SetRenderDrawColor(
		renderer,
		PHYSICS_COLOR.r,
		PHYSICS_COLOR.g,
		PHYSICS_COLOR.b,
		PHYSICS_COLOR.a);


	SDL_RenderDebugText(renderer, pos.x, y, physicsHeader.c_str());
	y += 20;

	SDL_RenderDebugText(renderer, pos.x, y, collisionChecks.c_str());
	y += 10;

	SDL_RenderDebugText(renderer, pos.x, y, resolved.c_str());
	y += 30;


	//==========================================================
	// ECS
	//==========================================================

	SDL_SetRenderDrawColor(
		renderer,
		ECS_COLOR.r,
		ECS_COLOR.g,
		ECS_COLOR.b,
		ECS_COLOR.a);


	SDL_RenderDebugText(renderer, pos.x, y, ecsHeader.c_str());
	y += 20;

	SDL_RenderDebugText(renderer, pos.x, y, entities.c_str());
	y += 10;

	SDL_RenderDebugText(renderer, pos.x, y, components.c_str());
}

void Debugger::DrawAllColliders(EntityManager& entityManager)
{
	boxColliders.clear();
	for (auto& e : entityManager.entities)
	{
		if (e->HasComponent<TileMap>())
		{
			TileMap& tileMap = e->GetComponent<TileMap>();
			auto& tiles = tileMap.GetTiles();
			for (int y = 0; y < tiles.size(); y++)
			{
				for (int x = 0; x < tiles[y].size(); x++)
				{
					if (tileMap.IsTileSolid(x, y))
						DrawBoxCollider2D(tileMap.GetTileBoxCollider2D(x, y), { 20,60,60, 255 });
				}
			}
		}
		else if (e->HasComponent<BoxCollider2D>())
		{
			DrawBoxCollider2D(e->GetComponent<BoxCollider2D>(), { 70,180,220, 255 });
		}
	}
}

void Debugger::DrawTrajectory(const Vec2f& v1, const Vec2f& v2)
{
	trajectories.push_back({ WorldToScreen(v1.x, v1.y, *camRef), WorldToScreen(v2.x, v2.y, *camRef)});
}