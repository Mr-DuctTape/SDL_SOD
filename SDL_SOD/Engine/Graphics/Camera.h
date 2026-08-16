#pragma once
#include "../Math/Vector2.h"
#include <SDL3/SDL.h>

class Camera
{
public:
	Vec2f pos;
	float zoom = 0.4f;
};

extern inline Vec2f WorldToScreen(float x, float y, const Camera& camera);
extern inline SDL_FRect WorldToScreen(SDL_FRect rect, const Camera& camera);