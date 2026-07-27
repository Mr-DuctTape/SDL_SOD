#pragma once
#include "../Math/Vector2.h"
#include <SDL3/SDL.h>

class Camera
{
public:
	Vec2f pos;
	float zoom = 0.4f;
};

static Vec2f WorldToScreen(float x, float y, const Camera& camera)
{
    return Vec2f{ x - camera.pos.x, y - camera.pos.y } * camera.zoom;
}
static SDL_FRect WorldToScreen(SDL_FRect rect, const Camera& camera) 
{
    return
    {
        (rect.x - camera.pos.x) * camera.zoom,
        (rect.y - camera.pos.y) * camera.zoom,
        rect.w * camera.zoom,
        rect.h * camera.zoom
    };
}