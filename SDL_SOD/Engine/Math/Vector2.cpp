#include "Vector2.h"
#include <iostream>

// Vec2i
float Vec2i::Magnitude() const
{
	return sqrt(x * x + y * y);
}

void Vec2i::operator=(const Vec2i& other)
{
	x = other.x;
	y = other.y;
}

//Additions
Vec2i Vec2i::operator+ (const Vec2i& other) const
{
	return Vec2i{ x + other.x, y + other.y };
}
Vec2i Vec2i::operator+ (const int scaler) const
{
	return Vec2i{ x + scaler, y + scaler };
}
void Vec2i::operator+= (const Vec2i& other)
{
	x += other.x;
	y += other.y;
}
void Vec2i::operator+= (const int scaler)
{
	x += scaler;
	y += scaler;
}

//Subtractions
Vec2i Vec2i::operator- (const Vec2i& other) const
{
	return Vec2i{ x - other.x, y - other.y };
}
Vec2i Vec2i::operator- (const int scaler) const
{
	return Vec2i{ x - scaler, y - scaler };
}
Vec2i Vec2i::operator- () const
{
	return Vec2i{ -x, -y };
}
void Vec2i::operator-= (const Vec2i& other)
{
	x -= other.x;
	y -= other.y;
}
void Vec2i::operator-= (const int scaler)
{
	x -= scaler;
	y -= scaler;
}

//Multiplications
Vec2i Vec2i::operator* (const Vec2i& other) const
{
	return Vec2i{ x * other.x, y * other.y };
}
Vec2i Vec2i::operator* (const int scalar) const
{
	return Vec2i{ x * scalar, y * scalar };
}
void Vec2i::operator*= (const Vec2i& other)
{
	x *= other.x;
	y *= other.y;
}
void Vec2i::operator*= (const int scaler)
{
	x *= scaler;
	y *= scaler;
}

//Divisions
Vec2i Vec2i::operator/ (const Vec2i& other) const
{
	return Vec2i{ x / other.x, y / other.y };
}
Vec2i Vec2i::operator/ (const int scaler) const
{
	return Vec2i{ x / scaler, y / scaler };
}
void Vec2i::operator/= (const Vec2i& other)
{
	x /= other.x;
	y /= other.y;
}
void Vec2i::operator/= (const int scaler)
{
	x /= scaler;
	y /= scaler;
}

Vec2i operator* (const int scalar, const Vec2i& v)
{
	return { v.x * scalar, v.y * scalar };
}

bool Vec2i::operator== (const Vec2i& other) const
{
	return (x == other.x && y == other.y);
}
bool Vec2i::operator >= (const Vec2i& other) const {
	return (x >= other.x && y >= other.y);
}
bool Vec2i::operator <= (const Vec2i& other) const {
	return (x <= other.x && y <= other.y);
}
bool Vec2i::operator > (const Vec2i& other) const
{
	return (x > other.x && y > other.y);
}
bool Vec2i::operator < (const Vec2i& other) const {
	return (x < other.x && y < other.y);
}

// Vec2f

Vec2f Vec2f::normalized() const
{
	const float len = Magnitude();
	if (len < 0.0001f)
		return Vec2f{ 0,0 };
	const float inv = 1.0f / len;
	return Vec2f{ x * inv, y * inv };
}

void Vec2f::Normalize()
{
	const float len = Magnitude();
	if (len == 0)
		return;
	const float inv = 1.0f / len;
	x *= inv;
	y *= inv;
}

float Vec2f::Magnitude() const
{
	return sqrt(x * x + y * y);
}

void Vec2f::operator=(const Vec2f& other)
{
	x = other.x;
	y = other.y;
}

//Additions
Vec2f Vec2f::operator+ (const Vec2f& other) const
{
	return Vec2f{ x + other.x, y + other.y };
}
Vec2f Vec2f::operator+ (const float scaler) const
{
	return Vec2f{ x + scaler, y + scaler };
}
void Vec2f::operator+= (const Vec2f& other)
{
	x += other.x;
	y += other.y;
}
void Vec2f::operator+= (const float scaler)
{
	x += scaler;
	y += scaler;
}

//Subtractions
Vec2f Vec2f::operator- (const Vec2f& other) const
{
	return Vec2f{ x - other.x, y - other.y };
}
Vec2f Vec2f::operator- (const float scaler) const
{
	return Vec2f{ x - scaler, y - scaler };
}
Vec2f Vec2f::operator- () const
{
	return Vec2f{ -x, -y };
}
void Vec2f::operator-= (const Vec2f& other)
{
	x -= other.x;
	y -= other.y;
}
void Vec2f::operator-= (const float scaler)
{
	x -= scaler;
	y -= scaler;
}

//Multiplications
Vec2f Vec2f::operator* (const Vec2f& other) const
{
	return Vec2f{ x * other.x, y * other.y };
}
Vec2f Vec2f::operator* (const float scalar) const
{
	return Vec2f{ x * scalar, y * scalar };
}
void Vec2f::operator*= (const Vec2f& other)
{
	x *= other.x;
	y *= other.y;
}
void Vec2f::operator*= (const float scaler)
{
	x *= scaler;
	y *= scaler;
}

//Divisions
Vec2f Vec2f::operator/ (const Vec2f& other) const
{
	return Vec2f{ x / other.x, y / other.y };
}
Vec2f Vec2f::operator/ (const float scaler) const
{
	return Vec2f{ x / scaler, y / scaler };
}
void Vec2f::operator/= (const Vec2f& other)
{
	x /= other.x;
	y /= other.y;
}
void Vec2f::operator/= (const float scaler)
{
	x /= scaler;
	y /= scaler;
}

Vec2f operator* (const float scalar, const Vec2f& v)
{
	return { v.x * scalar, v.y * scalar };
}

// comparisons
bool Vec2f::operator== (const Vec2f& other) const
{
	return (x == other.x && y == other.y);
}
bool Vec2f::operator >= (const Vec2f& other) const {
	return (x >= other.x && y >= other.y);
}
bool Vec2f::operator <= (const Vec2f& other) const {
	return (x <= other.x && y <= other.y);
}
bool Vec2f::operator > (const Vec2f& other) const
{
	return (x > other.x && y > other.y);
}
bool Vec2f::operator < (const Vec2f& other) const {
	return (x < other.x && y < other.y);
}

std::ostream& operator<<(std::ostream& os, const Vec2f& vec)
{
	os << "X: " << vec.x << " Y: " << vec.y;
	return os;
}
