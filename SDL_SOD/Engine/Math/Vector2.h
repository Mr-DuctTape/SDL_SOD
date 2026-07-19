#pragma once
#include <iostream>

struct Vec2i
{
	int x = 0, y = 0;

	Vec2i()
	{
		x = 0;
		y = 0;
	}
	Vec2i(int posX, int posY)
	{
		x = posX;
		y = posY;
	}
	float Magnitude() const; //Returns length of vector

	static float Dot(const Vec2i& a, const Vec2i& b)
	{
		return (float)(a.x * b.x + a.y * b.y);
	}

	void operator= (const Vec2i& other); //No return type disallows chaining

	Vec2i operator+ (const Vec2i& other) const;
	Vec2i operator+ (const int scaler) const;
	void operator+= (const Vec2i& other);
	void operator+= (const int scaler);

	Vec2i operator- (const Vec2i& other) const;
	Vec2i operator- (const int scaler) const;
	Vec2i operator- () const;
	void operator-= (const Vec2i& other);
	void operator-= (const int scaler);

	Vec2i operator* (const Vec2i& other) const;
	Vec2i operator* (const int scaler) const;
	void operator*= (const Vec2i& other);
	void operator*= (const int scaler);

	Vec2i operator/ (const Vec2i& other) const;
	Vec2i operator/ (const int scaler) const;
	void operator/= (const Vec2i& other);
	void operator/= (const int scaler);

	bool operator == (const Vec2i& other) const;
	bool operator >= (const Vec2i& other) const;
	bool operator <= (const Vec2i& other) const;
	bool operator > (const Vec2i& other) const;
	bool operator < (const Vec2i& other) const;
};

struct Vec2f
{
	float x = 0.0f, y = 0.0f;

	Vec2f()
	{
		x = 0.0f;
		y = 0.0f;
	}
	Vec2f(float posX, float posY)
	{
		x = posX;
		y = posY;
	}
	Vec2f normalized() const; //Returns a normalized vector
	void Normalize(); //Normalizes the vector 
	float Magnitude() const; //Returns length of vector

	static float Dot(const Vec2f& a, const Vec2f& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	void operator= (const Vec2f& other); //No return type disallows chaining

	Vec2f operator+ (const Vec2f& other) const;
	Vec2f operator+ (const float scaler) const;
	void operator+= (const Vec2f& other);
	void operator+= (const float scaler);

	Vec2f operator- (const Vec2f& other) const;
	Vec2f operator- (const float scaler) const;
	Vec2f operator- () const;
	void operator-= (const Vec2f& other);
	void operator-= (const float scaler);

	Vec2f operator* (const Vec2f& other) const;
	Vec2f operator* (const float scaler) const;
	void operator*= (const Vec2f& other);
	void operator*= (const float scaler);

	Vec2f operator/ (const Vec2f& other) const;
	Vec2f operator/ (const float scaler) const;
	void operator/= (const Vec2f& other);
	void operator/= (const float scaler);

	bool operator== (const Vec2f& other) const;

	bool operator >= (const Vec2f& other) const;
	bool operator <= (const Vec2f& other) const;
	bool operator > (const Vec2f& other) const;
	bool operator < (const Vec2f& other) const;

	friend std::ostream& operator<<(std::ostream& os, const Vec2f& vec);
};

std::ostream& operator<<(std::ostream& os, const Vec2f& vec);
Vec2f operator* (const float scalar, Vec2f& v);