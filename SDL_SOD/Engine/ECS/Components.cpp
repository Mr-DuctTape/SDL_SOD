#include "Components.h"
#include "Entities.h"
#include "../Graphics/Rendering.h"
#include <fstream>
#include "../Macros/DEBUGPRINT.h"

/// ===== ANIMATOR COMPONENT =====
std::unordered_map<std::string, Animator::Animation> Animator::animations;

void Animator::Update(float dt)
{
	if (!update)
		return;

	SDL_FRect& frame = currentAnimation.src;
	frame.w = static_cast<float>(currentAnimation.pixelWidth);
	frame.h = static_cast<float>(currentAnimation.pixelHeight);

	timer += dt;
	if (timer >= speed)
	{
		currentAnimation.currentFrame++;
		timer = 0;
	}

	if (currentAnimation.currentFrame >= currentAnimation.frames)
	{
		currentAnimation.currentFrame = 0;
		finished = true;
	}

	frame.x = static_cast<float>(currentAnimation.currentFrame * currentAnimation.pixelWidth);
	frame.y = 0;

	SDL_FRect& dst = currentAnimation.dst;

	Transform* transform = parent->GetComponent<Transform>();
	Sprite* sprite = parent->GetComponent<Sprite>();
	if (!sprite || !transform)
	{
		std::cout << "Sprite or Transform not found (Animator)\n";
		return;
	}

	dst.x = transform->position.x;
	dst.y = transform->position.y;

	dst.w = static_cast<float>(sprite->width);
	dst.h = static_cast<float>(sprite->height);
}
Animator::Animation Animator::CreateAnimation(const std::string& name, int frames, int pixelWidth, int pixelHeight, SDL_Texture* spriteSheet)
{
	static int Increase = 0;
	Animation anim;
	anim.frames = frames;
	anim.pixelHeight = pixelHeight;
	anim.pixelWidth = pixelWidth;
	anim.spriteSheet = spriteSheet;
	anim.ID = Increase++;
	anim.AnimationName = name;
	animations.emplace(name, anim);
	return anim;
}
Animator::Animation Animator::GetAnimation(const std::string& anim)
{
	auto it = animations.find(anim);
	return it->second;
}
void Animator::SetAnimation(const Animation& animation)
{
	if (currentAnimation.ID != animation.ID)
		currentAnimation = animation;

	finished = false;
}
void Animator::SetAnimation(const std::string& name)
{
	auto anim = GetAnimation(name);
	if (currentAnimation.ID != anim.ID)
		currentAnimation = anim;

	currentState = name;
	finished = false;
}
void Animator::Print() const
{
	if (!DEBUGPRINT)
		return;

	std::cout << "=== Animator Debug ===\n";

	std::cout << "Parent Entity ID: " << parent->ID << "\n";

	std::cout << "Current State: " << currentState << "\n";

	std::cout << "Scale X: " << scaleAnimationX << "\n";
	std::cout << "Scale Y: " << scaleAnimationY << "\n";

	std::cout << "Timer: " << timer << "\n";
	std::cout << "Speed: " << speed << "\n";

	std::cout << "Flipped X: " << (flippedX ? "true" : "false") << "\n";

	std::cout << "--- Current Animation ---\n";
	std::cout << "ID: " << currentAnimation.ID << "\n";
	std::cout << "Frames: " << currentAnimation.frames << "\n";
	std::cout << "Current Frame: " << currentAnimation.currentFrame << "\n";
	std::cout << "Pixel Width: " << currentAnimation.pixelWidth << "\n";
	std::cout << "Pixel Height: " << currentAnimation.pixelHeight << "\n";

	std::cout << "Src: { "
		<< currentAnimation.src.x << ", "
		<< currentAnimation.src.y << ", "
		<< currentAnimation.src.w << ", "
		<< currentAnimation.src.h << " }\n";

	std::cout << "Dst: { "
		<< currentAnimation.dst.x << ", "
		<< currentAnimation.dst.y << ", "
		<< currentAnimation.dst.w << ", "
		<< currentAnimation.dst.h << " }\n";

	std::cout << "SpriteSheet: "
		<< (currentAnimation.spriteSheet ? "Loaded" : "NULL")
		<< "\n";

	std::cout << "========================\n";
}

// ==== PHYSICS2D COMPONENT ====
void Physics2D::Init()
{
	auto transform = parent->GetComponent<Transform>();
	if (!transform) return;
}
void Physics2D::Accelerate(Vec2f force)
{
	acceleration += force;
}
void Physics2D::AddForce(Vec2f force)
{
	velocity += force;
}

// ==== BOXCOLLIDER2D COMPONENT ====
void BoxCollider2D::UpdatePosition() 
{
	if (!parent)
		return;

	Transform* transform = parent->GetComponent<Transform>();
	if (!transform)
	{
		if (!errorDisplayed)
		{
			std::cout << "=== BoxCollider2D Error ===\n";
			std::cout << "(BoxCollider2D): Transform component not found\n";
			std::cout << "(BoxCollider2D): Entity ID: " << parent->ID << "\n";
			errorDisplayed = true;
		}
		return;
	}

	Sprite* sprite = parent->GetComponent<Sprite>();
	if (!sprite)
	{
		if (!errorDisplayed)
		{
			std::cout << "=== BoxCollider2D Error ===\n";
			std::cout << "(BoxCollider2D): Sprite component not found\n";
			std::cout << "(BoxCollider2D): Entity ID: " << parent->ID << "\n";
			errorDisplayed = true;
		}
		return;
	}

	if (sprite->width <= 0 || sprite->height <= 0)
	{
		if (!errorDisplayed)
		{
			std::cout << "=== BoxCollider2D Error ===\n";
			std::cout << "(BoxCollider2D): Sprite width & height too small!" << "\n";
			std::cout << "(BoxCollider2D): Entity ID: " << parent->ID << "\n";
			errorDisplayed = true;
		}
		return;
	}

	rect.x = transform->position.x + offsetX;
	rect.y = transform->position.y + offsetY;
 
	rect.w = width;
	rect.h = height;

	isColliding = false;
}

void BoxCollider2D::Update(float dt)
{
	wasColliding = isColliding;
	groundCollision = false;
	wallCollision = false;
}

// ==== TILEMAP COMPONENT ====
TileMap::TileMap(const TileMap& other)
{
	spriteSheet = other.spriteSheet;
	SDL_SetTextureScaleMode(spriteSheet, SDL_SCALEMODE_NEAREST);

	tiles = other.tiles;
	tilePixelHeight = other.tilePixelHeight;
	tilePixelWidth = other.tilePixelWidth;
	tileScaleX = other.tileScaleX;
	tileScaleY = other.tileScaleY;

	// Copies raw bytes, very fast for C style arrays, Plain old data structs etc
	// Ignores constructors
	memcpy(tileProperties, other.tileProperties, sizeof(other.tileProperties));
}

void TileMap::SetTilePixelSize(int width, int height) 
{
	tilePixelWidth = width;
	tilePixelHeight = height;
}
void TileMap::SetTileScale(float scaleX, float scaleY)
{
	tileScaleX = scaleX;
	tileScaleY = scaleY;
}
void TileMap::SetSpriteSheet(SDL_Texture* spriteSheet)
{
	this->spriteSheet = spriteSheet;
	SDL_SetTextureScaleMode(spriteSheet, SDL_SCALEMODE_NEAREST);
}
void TileMap::SetTileProperties(int ID, TileProperties properties)
{
	GetTileProperties(ID) = properties;
}
TileMap::TileScale TileMap::GetTileScale()
{
	TileScale scale;
	scale.tilePixelHeight = tilePixelHeight;
	scale.tilePixelWidth = tilePixelWidth;
	scale.tileScaleX = tileScaleX;
	scale.tileScaleY = tileScaleY;

	scale.scaledX = tilePixelWidth * tileScaleX;
	scale.scaledY = tilePixelHeight * tileScaleY;

	return scale;
}

std::vector<std::vector<int>>& TileMap::GetTiles()
{
	return tiles;
}
SDL_FRect TileMap::GetTileFRect(int x, int y)
{
	SDL_FRect rect = {};
	// Scale from tilespace too screen space
	rect.x = x * tilePixelWidth * tileScaleX;
	rect.y = y * tilePixelHeight * tileScaleY;
	rect.w = tilePixelWidth * tileScaleX;
	rect.h = tilePixelHeight * tileScaleY;
	return rect;
}
BoxCollider2D TileMap::GetTileBoxCollider2D(int x, int y)
{
	Transform* transform = parent->GetComponent<Transform>();
	if (!transform)
		return BoxCollider2D{};

	BoxCollider2D box;
	SDL_FRect& rect = box.rect;

	// Scale from tilespace too screen space
	rect.x = (x * tilePixelWidth * tileScaleX) + transform->position.x;
	rect.y = (y * tilePixelHeight * tileScaleY) + transform->position.y;
	rect.w = (tilePixelWidth * tileScaleX);
	rect.h = (tilePixelHeight * tileScaleY);

	return box;
}

TileMap::TileMapData TileMap::GetMapData(const char* file)
{
	TileMapData data;
	std::ifstream filep(file);
	if (!filep.is_open())
	{
		std::cout << "Failed to open tilemap file: " << file << "\n";
		return {};
	}

	filep >> data.width >> data.height;
	filep >> data.pixelWidth >> data.pixelHeight;

	data.grid.resize(data.height);
	for (size_t i = 0; i < data.grid.size(); i++)
	{
		data.grid[i].resize(data.width);
	}

	for (int y = 0; y < data.height; y++)
		for (int x = 0; x < data.width; x++)
			filep >> data.grid[y][x];

	if (DEBUGPRINT) {
		std::cout
			<< "width=" << data.width
			<< " height=" << data.height
			<< " tileW=" << data.pixelWidth
			<< " tileH=" << data.pixelHeight << "\n";
	}

	return data;
}

void TileMap::LoadTileMap(const std::string& filePath)
{
	TileMapData data = GetMapData(filePath.c_str());
	tiles = data.grid;
	tilePixelWidth = data.pixelWidth;
	tilePixelHeight = data.pixelHeight;
}

void TileMap::TestPrint()
{
	if (!DEBUGPRINT)
		return;

	for (size_t y = 0; y < tiles.size(); y++)
	{
		for (size_t x = 0; x < tiles[y].size(); x++)
		{
			std::cout << tiles[y][x];
		}
		std::cout << "\n";
	}
}