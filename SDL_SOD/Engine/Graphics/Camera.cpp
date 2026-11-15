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
inline SDL_FRect ScaledWorldToScreen(SDL_FRect rect, const Camera& camera, float parallax, float scale)
{
    return
    {
        (rect.x - camera.pos.x * parallax) * camera.zoom * scale,
        (rect.y - camera.pos.y * parallax) * camera.zoom * scale,
        rect.w * camera.zoom * scale,
        rect.h * camera.zoom * scale
    };
}