#include "Camera.h"

inline Vec2f WorldToScreen(float x, float y, const Camera& camera)
{
    return Vec2f{ x - camera.pos.x, y - camera.pos.y } * camera.zoom;
}
inline SDL_FRect WorldToScreen(SDL_FRect rect, const Camera& camera)
{
    return
    {
        (rect.x - camera.pos.x) * camera.zoom,
        (rect.y - camera.pos.y) * camera.zoom,
        rect.w * camera.zoom,
        rect.h * camera.zoom
    };
}