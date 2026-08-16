#pragma once
#include <vector>
#include <SDL3/SDL.h>
#include "../Macros/DEBUGPRINT.h"
#include "../Math/Vector2.h"

struct Line
{
	Vec2f p1;
	Vec2f p2;
};

struct BoxCollider2D;
struct Vec2f;

class EntityManager;
class Camera;

struct DebugStats
{
	// OVERALL
	float fps = 0;
	float frameMs = 0;

	// Update frametime
	float updateMs = 0;
	float componentMs = 0;
	float physicsMs = 0;

	// Rendering frametime
	float renderMs = 0;
	float drawingMs = 0;
	float debugMs = 0;
	float presentScreenMs = 0;
	float clearScreenMs = 0;

	// Other misc, scripts, game etc
	float OtherMs = 0;

	// ECS
	size_t entities = 0;
	size_t components = 0;

	// Rendering
	int drawCalls = 0;
	int tilesRendered = 0;
	int spritesRendered = 0;

	// Physics
	int collisionChecks = 0;
	int resolvedCollisions = 0;
};

struct Collider
{
	SDL_FRect rect{};
	SDL_Color color{};
};

class Debugger
{
private:
	const Camera* camRef = nullptr;
	Uint64 startTime = 0;
	Uint64 endTime = 0;
public:
	std::vector<Collider> boxColliders;
	std::vector<Line> trajectories;
	DebugStats debugStats;

	SDL_Color drawColor = { 255, 255, 255, 255 };

	bool enabled = false;

	void DebuggerStartTime()
	{
		startTime = SDL_GetPerformanceCounter();

		debugStats.OtherMs = debugStats.frameMs - debugStats.updateMs - debugStats.renderMs - debugStats.presentScreenMs;
	}
	void DebuggerEndTime()
	{
		endTime = SDL_GetPerformanceCounter();

		float frameTime =
			(float)(endTime - startTime) /
			SDL_GetPerformanceFrequency();


		static float fpsTimer = 0.0f;
		static int frameCount = 0;

		fpsTimer += frameTime;
		frameCount++;

		if (fpsTimer >= 0.5f)
		{
			debugStats.fps = frameCount / fpsTimer;

			frameCount = 0;
			fpsTimer = 0.0f;
		}

		debugStats.frameMs = frameTime * 1000.0f;
	}

	Uint64 GetStartTime()
	{
		return startTime;
	}
	Uint64 GetEndTime()
	{
		return endTime;
	}
	void DrawPerformanceStats(SDL_Renderer* renderer, EntityManager& entityManager);
	void DrawAllColliders(EntityManager& entityManager);
	void DrawBoxCollider2D(const BoxCollider2D& a, SDL_Color color);
	void DrawTrajectory(const Vec2f& v1, const Vec2f& v2);

	void Initialize(const Camera* camera)
	{
		camRef = camera;
	}
	Debugger() {};
	Debugger(Camera* ref)
	{
		camRef = ref;
	}
};