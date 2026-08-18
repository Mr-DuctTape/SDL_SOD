#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include <vector>
#include <string>
#include "../Math/Vector2.h"
#include "../Macros/DEBUGPRINT.h"

class Entity;

struct Component // Base component any component needs to inherits from this
{
	Entity* parent = nullptr;

	virtual void Update(float dt) {};
	virtual void Init() {};
	virtual size_t Size() {
		return sizeof(*this);
	}
	virtual Component* Clone() { return nullptr; };

	virtual ~Component() = default;
};

struct EntityTag : Component
{
	std::string name;

	EntityTag* Clone() override
	{
		return new EntityTag(*this);
	}
	size_t Size() override
	{
		return sizeof(*this);
	}
};

struct Physics2D : Component
{
	Vec2f acceleration = { 0.0f, 0.0f };
	Vec2f velocity = { 0.0f, 0.0f };
	Vec2f last_velocity = { 0.0f, 0.0f };

	Vec2f last_position = { 0.0f, 0.0f };

	bool useGravity = false;

	void Accelerate(Vec2f force);
	void AddForce(Vec2f force);
	void Init() override;

	Physics2D* Clone() override
	{
		return new Physics2D(*this);
	}
	size_t Size() override
	{
		return sizeof(*this);
	}
};

struct BoxCollider2D : Component
{
	SDL_FRect rect = {};
	SDL_FRect edited = {};

	float offsetX = 0.0f;
	float offsetY = 0.0f;

	float width = 150.0f;
	float height = 150.0f;

	bool isColliding = false;
	bool wasColliding = false;

	bool wallCollision = false;
	bool groundCollision = false;

	Vec2f collisionVector;
private:
	bool errorDisplayed = false;
public:

	void Update(float dt) override;
	void UpdatePosition();
	BoxCollider2D* Clone() override
	{
		return new BoxCollider2D(*this);
	}
	size_t Size() override
	{
		return sizeof(*this);
	}
};

inline bool IsColliding(BoxCollider2D& boxA, BoxCollider2D& boxB)
{
	return SDL_HasRectIntersectionFloat(&boxA.rect, &boxB.rect);
}

struct Transform : Component
{
	Vec2f position;
	float rotation = 0.0f;
	void RotateTowards(const Vec2f& pos) {};

	Transform* Clone() override
	{
		return new Transform(*this);
	}
	size_t Size() override
	{
		return sizeof(*this);
	}
};

struct Sprite : Component
{
	SDL_Texture* texture = nullptr;
	SDL_FColor color{};
	int width = 150, height = 150;
	Sprite* Clone() override
	{
		return new Sprite(*this);
	}
	size_t Size() override
	{
		return sizeof(*this);
	}
};

// This is the number of different tiles that can be loaded into a tilemap since each tile has its own properties
constexpr int MAX_TILE_PROPERTIES = 64;

class Camera;
class RenderingSystem;

class TileMap : public Component
{
private:
	SDL_Texture* spriteSheet = nullptr; // Do not touch the texture or handle it
	std::vector<std::vector<int>> tiles; //TODO:  Performance improvement, make into 1D array

	inline bool CheckBounds(int x, int y)
	{
		return (x >= 0 && y >= 0 && y < tiles.size() && x < tiles[y].size());
	}

	int tilePixelHeight = 0;
	int tilePixelWidth = 0;
	float tileScaleX = 0.0f;
	float tileScaleY = 0.0f;

public:
	struct TileProperties
	{
		bool solid = false;
	};
private:
	TileProperties tileProperties[MAX_TILE_PROPERTIES];
public:
	TileMap() { SDL_SetTextureScaleMode(spriteSheet, SDL_SCALEMODE_NEAREST); };
	TileMap(const TileMap& other);

	void SetTilePixelSize(int width, int height);
	void SetTileScale(float scaleX, float scaleY);
	void SetTileProperties(int ID, TileProperties properties);
	void SetSpriteSheet(SDL_Texture* spriteSheet);

	struct TileScale
	{
		int tilePixelHeight = 0;
		int tilePixelWidth = 0;
		float tileScaleX = 0.0f;
		float tileScaleY = 0.0f;

		float scaledX = 0.0f;
		float scaledY = 0.0f;
	};
	TileScale GetTileScale();

	std::vector<std::vector<int>>& GetTiles();
	SDL_FRect GetTileFRect(int x, int y);
	BoxCollider2D GetTileBoxCollider2D(int x, int y);
	inline TileProperties& GetTileProperties(int ID)
	{
		if (ID < 0 || ID >= MAX_TILE_PROPERTIES)
			return tileProperties[0];
		return tileProperties[ID];
	}
	inline int GetTileID(int x, int y) {
		if (!CheckBounds(x, y))
			return 0;
		return tiles[y][x];
	}
	inline int* GetTile(int x, int y) {
		if (!CheckBounds(x, y))
			return nullptr;
		return &tiles[y][x];
	}
	inline bool IsTileSolid(int x, int y)
	{
		if (!CheckBounds(x, y))
			return false;
		return GetTileProperties(GetTileID(x, y)).solid;
	}

private:
	struct TileMapData
	{
		int width{};
		int height{};
		int pixelHeight{};
		int pixelWidth{};
		std::vector<std::vector<int>> grid;
	};
	TileMapData GetMapData(const char* file);
public:
	void LoadTileMap(const std::string& filePath);
	void Render(RenderingSystem& renderingSystem, Camera& camera); // Defined in Rendering.cpp
	void TestPrint();

	TileMap* Clone() override
	{
		return new TileMap(*this);
	}
	size_t Size() override
	{
		size_t s = 0;
		s += sizeof(*this);

		s += tiles.size() * sizeof(std::vector<int>);
		for (size_t i = 0; i < tiles.size(); i++)
		{
			s += tiles[i].size() * sizeof(int);
		}

		return s;
	}
};

class Animator : public Component
{
public:
	struct Animation
	{
		SDL_Texture* spriteSheet = nullptr;
		std::string AnimationName;

		SDL_FRect src = {};
		SDL_FRect dst = {};

		int pixelWidth = 0;
		int pixelHeight = 0;
		int frames = 0;
		int currentFrame = 0;

		int ID = 0;
	};
	Animation currentAnimation;
private:
	static std::unordered_map<std::string, Animator::Animation> animations;
	bool errorDisplayed = false;

public:
	std::string currentState;

	float scaleAnimationX = 0.0f;
	float scaleAnimationY = 0.0f;

	float timer = 0.0f;
	float speed = 1.0f;

	bool finished = false;
	bool effectBase = false;

	bool update = true;
	bool destroyOnFinish = false;
	bool flippedX = false;

	Animation CreateAnimation(const std::string& name, int frames, int pixelWidth, int pixelHeight, SDL_Texture* spriteSheet);
	Animation GetAnimation(const std::string& anim);
	void SetAnimation(const Animation& animation);
	void SetAnimation(const std::string& name);

	void Print() const;
	void Update(float dt) override;
	void Render(RenderingSystem& renderingSystem, Camera& camera); // Defined in Rendering.cpp

	size_t Size() override
	{
		return sizeof(*this);
	}
	Animator* Clone() override
	{
		return new Animator(*this);
	}
};